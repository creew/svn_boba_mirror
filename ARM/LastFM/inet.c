#include "..\inc\swilib.h"
#include "inet.h"
#include "urlwork.h"
#include "md5.h"

extern char *strtok(char *s1, const char *s2);
extern unsigned long  strtoul (const char *nptr,char **endptr,int base);

extern const char TEMP_FILE[];
extern const char USERNAME[];
extern const char PASSWORD[];

GBSTMR reconnect_tmr;

#define TMR_SECOND 216

static int enable_connect=0;

static int is_gprs_online=0;

static int is_handshaked=0;

static int DNR_ID=0;
static int DNR_TRIES=3;

static int connect_state=0;

static int sock=-1;

static int sendq_l=0; //������ ������� ��� send
static char *sendq_p=NULL; //��������� �������

static int recvq_l=0;
static char *recvq_p=NULL;

static const char HANDSHAKE_HOST[]="post.audioscrobbler.com";
static const int HANDSHAKE_PORT=80;
static char POST_HOST[256]="";
static unsigned short POST_PORT=0;

static char CHALLENGE[64]="";
static char POST_URL[256]="";

static void INETLOG(int do_mfree, char *s)
{
  unsigned int ul;
  int f=fopen("4:\\LastFM_INET.log",A_ReadWrite+A_Create+A_Append+A_BIN,P_READ+P_WRITE,&ul);
  if (f!=-1)
  {
    fwrite(f,s,strlen(s),&ul);
    fclose(f,&ul);
  }
  if (do_mfree) mfree(s);
}

static void do_reconnect(void)
{
  void create_connect(void);
  SUBPROC((void*)INETLOG,0,"do_reconnect()\r\n");
  if (is_gprs_online&&enable_connect)
  {
    DNR_TRIES=3;
    SUBPROC((void*)create_connect);
  }
}

static void create_connect(void)
{
  int ***p_res=NULL;
  void do_reconnect(void);
  const char *hostname;
  unsigned int ip;
  int err;

  SOCK_ADDR sa;
  //������������� ����������
  connect_state = 0;
  GBS_DelTimer(&reconnect_tmr);
  if (!IsGPRSEnabled())
  {
    is_gprs_online=0;
    return;
  }
  DNR_ID=0;
  *socklasterr()=0;
  hostname=is_handshaked?POST_HOST:HANDSHAKE_HOST;
  ip=str2ip(hostname);
  if (ip!=0xFFFFFFFF)  
  {
    sa.ip=ip;
    INETLOG(0,"IP_CONNECT\r\n");
    goto L_CONNECT;
  }  
  INETLOG(0,"gethostbyname\r\n");
  err=async_gethostbyname(hostname,&p_res,&DNR_ID); //03461351 3<70<19<81
  if (err)
  {
    if ((err==0xC9)||(err==0xD6))
    {
      if (DNR_ID)
      {
	INETLOG(0,"wait dnr...\r\n");
	return; //���� ���������� DNR
      }
    }
    else
    {
      INETLOG(0,"DNR fault!\r\n");
      GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
      return;
    }
  }
  if (p_res)
  {
    if (p_res[3])
    {
      INETLOG(0,"DNR ok!\r\n");
      DNR_TRIES=0;
      sa.ip=p_res[3][0][0];
    L_CONNECT:
      INETLOG(0,"Start socket...\r\n");
      sock=socket(1,1,0);
      if (sock!=-1)
      {
	sa.family=1;
	sa.port=htons(is_handshaked?POST_PORT:HANDSHAKE_PORT);
	if (connect(sock,&sa,sizeof(sa))!=-1)
	{
	  connect_state=1;
	}
	else
	{
	  closesocket(sock);
	  sock=-1;
	  GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
	}
      }
      else
      {
	//�� ������� �������� ������, ��������� GPRS-������
	GPRS_OnOff(0,1);
      }
    }	
  }
  else
  {
    DNR_TRIES--;
  }
}

void ClearSendQ(void)
{
  mfree(sendq_p);
  sendq_p=NULL;
  sendq_l=NULL;
}

void ClearRecvQ(void)
{
  mfree(recvq_p);
  recvq_p=NULL;
  recvq_l=NULL;
}

void end_socket(void)
{
  if (sock>=0)
  {
    shutdown(sock,2);
    closesocket(sock);
  }
}

//��������������� ������� � �����, c ����������� ������������� ���������
static void bsend(int len, void *p)
{
  int i;
  int j;
  if (connect_state<2)
  {
    mfree(p);
    return;
  }
  if (p)
  {
    //���������, �� ���� �� �������� � �������
    if (sendq_p)
    {
      //���� �������, ��������� � ���
      memcpy((sendq_p=realloc(sendq_p,sendq_l+len))+sendq_l,p,len);
      mfree(p);
      sendq_l+=len;
      return;
    }
    sendq_p=p;
    sendq_l=len;
  }
  //���������� ��� ������������ � �������
  while((i=sendq_l)!=0)
  {
    if (i>0x400) i=0x400;
    j=send(sock,sendq_p,i,0);
    if (j<0)
    {
      j=*socklasterr();
      if ((j==0xC9)||(j==0xD6))
      {
	return; //������, ���� ����� ��������� ENIP_BUFFER_FREE
      }
      else
      {
	//������
	LockSched();
	ShowMSG(1,(int)"Send error!");
	UnlockSched();
	end_socket();
	return;
      }
    }
    memcpy(sendq_p,sendq_p+j,sendq_l-=j); //������� ����������
    if (j<i)
    {
      //�������� ������ ��� ����������
      return; //���� ��������� ENIP_BUFFER_FREE1
    }
  }
  mfree(sendq_p);
  sendq_p=NULL;
}

static void get_answer(void)
{
  char rb[1024];
  int i=recv(sock,rb,sizeof(rb),0);
  if (i<=0) return;
  memcpy((recvq_p=realloc(recvq_p,recvq_l+i+1))+recvq_l,rb,i);
  recvq_l+=i;
}

static void SendHandShake()
{
  char *hst=malloc(1024);
  strcpy(hst,"GET /?hs=true&p=1.1&c=tst&v=1.0&u=");
  urlcat(hst,USERNAME);
  strcat(hst,"\r\nHost: post.audioscrobbler.com\r\n\r\n");
  SUBPROC((void*)INETLOG,0,hst);
  SUBPROC((void*)bsend,strlen(hst),hst);
}

int stricmp(const char *s, const char *d)
{
  int cs;
  int ds;
  do
  {
    cs=*s++;
    if (cs&0x40) cs&=0xDF;
    ds=*d++;
    if (ds&0x40) ds&=0xDF;
    cs-=ds;
    if (cs) break;
  }
  while(ds);
  return(cs);
}

static void ParseHandShake()
{
  const char seps[] = " \n\r";
  char *s;
  char *response;
  char *submiturl;
  if (!recvq_l)
  {
    SUBPROC((void*)INETLOG,0,"No bytes on handshake recived!");
    return;
  }
  recvq_p[recvq_l]=0;
  s=malloc(recvq_l+1);
  strcpy(s,recvq_p);
  SUBPROC((void*)INETLOG,1,s);
  response=strstr(recvq_p,"\r\n\r\n");
  if (!response)
  {
    response=strtok(recvq_p,seps);
  }
  else
  {
    response=strtok(response,seps);
  }
  if (stricmp("UPTODATE",response)!=0) return;
  strncpy(CHALLENGE,strtok(NULL,seps),63);
  submiturl=strtok(NULL,seps);
  if (strncmp(submiturl,"http://",7)==0) submiturl+=7;
  strncpy(POST_HOST,strtok(submiturl,":"),255);
  POST_PORT=strtoul(strtok(NULL,"/"),NULL,10);
  strncpy(POST_URL,strtok(NULL,""),255);
  is_handshaked=1;
}

static unsigned int STRIP_SIZE;

static char SESSIONKEY[33];
static char PROCESSED_PWD[33];

void genSessionKey(void)
{
  md5_state_t md5state;
  unsigned char md5pword[16];
  char clear[256];
  char *key=SESSIONKEY;
  int j;
  strcpy(clear,PROCESSED_PWD);
  strcat(clear,CHALLENGE);
  md5_init(&md5state);
  md5_append(&md5state, (unsigned const char *)clear, (int)strlen(clear));
  md5_finish(&md5state, md5pword);
  memset(key,0,33);
  for (j = 0;j < 16;j++)
  {
    char a[3];
    sprintf(a, "%02x", md5pword[j]);
    key[2*j] = a[0];
    key[2*j+1] = a[1];
  }
}

static void setPassword(const char *pass)
{
  char *tmp;
  int j;
  md5_state_t md5state;
  unsigned char md5pword[16];
  md5_init(&md5state);
  md5_append(&md5state, (unsigned const char *)pass, (int)strlen(pass));
  md5_finish(&md5state, md5pword);
  if (!strlen(pass)) return;
  tmp=PROCESSED_PWD;
  memset(tmp,0,33);
  for (j = 0;j < 16;j++)
  {
    char a[3];
    sprintf(a, "%02x", md5pword[j]);
    tmp[2*j] = a[0];
    tmp[2*j+1] = a[1];
  }
  genSessionKey();
}

static int SendSubmit()
{
  char s2[32];

  int f;
  unsigned int err;
  int fsize;
  char *buf;
  char *outp;
  char *req;
  FSTATS stat;
  int c;

  int sn=0;
  char *PLOG;
  char *SUBMIT_DATA;

  setPassword(PASSWORD);
  
  if (GetFileStats(TEMP_FILE,&stat,&err)==-1)
    return 0;

  if ((fsize=stat.size)<=0)
    return 0;
 
  if ((f=fopen(TEMP_FILE,A_ReadOnly+A_BIN,P_READ,&err))==-1)
  return 0;
  
  SUBMIT_DATA=outp=malloc(512+fsize);
  buf=PLOG=malloc(fsize+1);
  
  buf[fread(f,buf,fsize,&err)]=0;
  fclose(f,&err);
  
  strcpy(outp,"u=");
  urlcat(outp,USERNAME);
  strcat(outp,"&s=");
  strcat(outp,SESSIONKEY);
  outp+=strlen(outp);
  //������ ��� ��������� � �����
  while(sn<1)
  {
    while((c=*buf++)>=32)
    {
      *outp++=c;
      if (c=='[')
      {
	*outp++=sn+'0';
      }
    }
    sn++;
    if (!c) break;
  }
  *outp=0;
  STRIP_SIZE=buf-PLOG;
  mfree(PLOG);
  
  if (!sn)
  {
    mfree(SUBMIT_DATA);
    return 0;
  }
  req=malloc(strlen(POST_URL)+strlen(POST_HOST)+1024);
  strcpy(req,"POST /");
  strcat(req,POST_URL);
  strcat(req," HTTP/1.1\r\nContent-type: application/x-www-form-urlencoded\r\nHost: ");
  strcat(req,POST_HOST);
//  sprintf(req+strlen(req),":%d",POST_PORT);
  strcat(req,"\r\nUser-Agent: LastFMD_Sie_v0");
  strcat(req,"\r\nContent-Length: ");
  sprintf(s2,"%d",strlen(SUBMIT_DATA));
  strcat(req,s2);
//  strcat(req,"\r\nAccept: */*\r\nConnection: Close\r\n\r\n");
  strcat(req,"\r\nCache-Control: no-cache\r\n\r\n");
  SUBPROC((void*)INETLOG,0,req);
  SUBPROC((void*)bsend,strlen(req),req);
  SUBPROC((void*)INETLOG,0,SUBMIT_DATA);
  SUBPROC((void*)bsend,strlen(SUBMIT_DATA),SUBMIT_DATA);
  SUBPROC((void*)INETLOG,0,"\r\n");
  return 1;
}

static void StripLog()
{
  //�������� ���� ����
  int f;
  unsigned int err;
  int fsize=0;
  char *buf=NULL;
  FSTATS stat;
  if (GetFileStats(TEMP_FILE,&stat,&err)==-1) goto L_REWRITE;
  if (stat.size<=0) goto L_REWRITE;
  fsize=stat.size;
  if ((f=fopen(TEMP_FILE,A_ReadOnly+A_BIN,P_READ,&err))==-1) goto L_REWRITE;
  buf=malloc(fsize);
  fread(f,buf,fsize,&err);
  fclose(f,&err);
L_REWRITE:
  f=fopen(TEMP_FILE,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&err);
  if (f!=-1)
  {
    if (buf)
    {
      if ((fsize-=STRIP_SIZE)>0)
      {
	fwrite(f,buf+STRIP_SIZE,fsize,&err);
      }
    }
    fclose(f,&err);
  }
  mfree(buf);
  if (fsize>0)
  {
    enable_connect=1;
    GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);    
  }
}

static void ParseSubmit()
{
  char *s;
  char *response;
  const char seps[] = " \n\r";
  if (!recvq_l)
  {
    SUBPROC((void*)INETLOG,0,"No bytes on submit recived!");
    return;
  }
  recvq_p[recvq_l]=0;
  s=malloc(recvq_l+1);
  strcpy(s,recvq_p);
  SUBPROC((void*)INETLOG,1,s);
  response=strstr(recvq_p,"\r\n\r\n");
  if (!response)
  {
    SUBPROC((void*)INETLOG,0,"Incorrect HTTP answer!");
    return;
  }
  response+=4;
  response=strtok(response,seps);
  if (!stricmp("OK",response))
  {
    enable_connect=0;
    SUBPROC((void*)StripLog);
  }
}

int ParseSocketMsg(GBS_MSG *msg)
{
  if (msg->msg==MSG_HELPER_TRANSLATOR)
  {
    switch((int)msg->data0)
    {
    case LMAN_DISCONNECT_IND:
      SUBPROC((void*)INETLOG,0,"LMAN_DISCONNECT_IND\r\n");
      is_handshaked=0;
      is_gprs_online=0;
      return(1);
    case LMAN_CONNECT_CNF:
      SUBPROC((void*)INETLOG,0,"LMAN_CONNECT_CNF\r\n");
      is_handshaked=0;
      is_gprs_online=1;
      GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*10,do_reconnect);
      return(1);
    case ENIP_DNR_HOST_BY_NAME:
      if ((int)msg->data1==DNR_ID)
      {
	SUBPROC((void*)INETLOG,0,"ENIP_DNR_HOST_BY_NAME\r\n");
	if (DNR_TRIES) SUBPROC((void *)create_connect);
      }
      return(1);
    }
    if ((int)msg->data1==sock)
    {
      //���� ��� �����
//      if ((((unsigned int)msg->data0)>>28)==0xA)
//      {
//	//������ �����
//	ProcessPacket((TPKT *)msg->data0);
//	return(0);
//      }
      switch((int)msg->data0)
      {
      case ENIP_SOCK_CONNECTED:
	SUBPROC((void*)INETLOG,0,"ENIP_SOCK_CONNECTED\r\n");
	if (connect_state==1)
	{
	  connect_state=2;
	  //���������� ������������, �������� ����� login
	  if (is_handshaked)
	  {
	    if (!SendSubmit())
	    {
	      enable_connect=0;
	      SUBPROC((void*)end_socket);
	    }
	  }
	  else
	  {
	    SendHandShake();
	  }
	}
	else
	{
	}
	break;
      case ENIP_SOCK_DATA_READ:
	SUBPROC((void*)INETLOG,0,"ENIP_SOCK_DATA_READ\r\n");
	if (connect_state>=2)
	{
	  //���� �������� send
	  SUBPROC((void *)get_answer);
	  //REDRAW();
	}
	else
	{
	}
	break;
      case ENIP_BUFFER_FREE:
	if (!sendq_p)
	{
	  ShowMSG(1,(int)"Illegal ENIP_BUFFER_FREE!");
	  SUBPROC((void *)end_socket);
	}
	else
	{
	  //�������� �������
	  SUBPROC((void *)bsend,0,0);
	}
	break;
      case ENIP_BUFFER_FREE1:
	if (!sendq_p)
	{
	  ShowMSG(1,(int)"Illegal ENIP_BUFFER_FREE1!");
	  SUBPROC((void *)end_socket);
	}
	else
	{
	  //�������� �������
	  SUBPROC((void *)bsend,0,0);
	}
	break;
      case ENIP_SOCK_REMOTE_CLOSED:
	SUBPROC((void*)INETLOG,0,"ENIP_SOCK_REMOTE_CLOSED\r\n");
	//������ �� ������� �������
	if (connect_state)
	  SUBPROC((void *)end_socket);
	break;
      case ENIP_SOCK_CLOSED:
	SUBPROC((void*)INETLOG,0,"ENIP_SOCK_CLOSED\r\n");
	connect_state=0;
	sock=-1;
	if (is_handshaked)
	{
	  ParseSubmit();
	  is_handshaked=0;
	}
	else
	{
	  ParseHandShake();
	}
	SUBPROC((void *)ClearSendQ);
	SUBPROC((void *)ClearRecvQ);
	if (enable_connect) GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*(is_handshaked?2:120),do_reconnect);
	break;
      }
    }
  }
  return(1);
}

void StartINET(void)
{
  if ((!strlen(USERNAME))||(!strlen(PASSWORD))) return;
  if (!connect_state)
  {
    enable_connect=1;
    do_reconnect();
  }
}
