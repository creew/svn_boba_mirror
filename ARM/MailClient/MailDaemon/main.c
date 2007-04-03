#include "..\..\inc\swilib.h"
#include "..\conf_loader.h"
#include "..\mailclient.h"

#define TMR_SECOND 216


#pragma inline
void patch_header(HEADER_DESC* head)
{
  head->rc.x=0;
  head->rc.y=YDISP;
  head->rc.x2=ScreenW()-1;
  head->rc.y2=HeaderH()+YDISP;
}

#pragma inline
void patch_rect(RECT* rc)
{
  rc->x=0;
  rc->y=YDISP;
  rc->x2=ScreenW()-1;
  rc->y2=ScreenH()-1;
}

#pragma inline
int is_digit(int c)
{
  if (c>=0&&c<=9) return (1);
  return (0);
}

#pragma inline=forced
int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}


char *recived_line;

int strcmp_nocase(const char *s1,const char *s2)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))) if (!c) break;
  return(i);
}

char logmsg[256];
unsigned short maincsm_name_body[140];
const char percent_t[]="%t";
const char empty_str[]="";
const char eol[]="\r\n";


//������� ���������
int connect_state=0;

//������� ��������� ��������� �����
int pop_state=0;

//����� � �����
int in_pop3=0;

//���� ��� ������� ���������
int mes_rec=0;

GBSTMR reconnect_tmr;
volatile int is_gprs_online=1;
volatile ML_DAEMON *mails;

ML_DAEMON *cur_ml;
typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  GUI gui;
  WSHDR *ws1;
  int i1;
}MAIN_GUI;

const int minus11=-11;
int sock=-1;

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

void FreeMailDB()
{
  ML_DAEMON *ml_cur=(ML_DAEMON *)mails;
  ML_DAEMON *ml_prev;
  while (ml_cur)
  {
    if (ml_cur->uidl) mfree(ml_cur->uidl);
    ml_prev=ml_cur;
    ml_cur=ml_cur->next;
    mfree(ml_prev);
  }
  mails=0;
}

void InitMailDB()
{
  char fname[256];
  int f;
  MAIL_DB mail_db;
  char *uidl;
  ML_DAEMON *ml_cur=(ML_DAEMON *)&mails;
  unsigned int err;
  strcpy(fname,EML_PATH);
  strcat(fname,"mails.db");
  FreeMailDB();
  if ((f=fopen(fname,A_BIN+A_ReadOnly,P_READ,&err))==-1)
  {
    mails=0;
    return;
  }
  while (fread(f,&mail_db,sizeof(MAIL_DB),&err)==sizeof(MAIL_DB))
  {
    if(mail_db.magic!=MDB_MAGIC) break;
    ml_cur=ml_cur->next=malloc(sizeof(ML_DAEMON));
    ml_cur->next=0;
    ml_cur->state=mail_db.state;
    ml_cur->is_read=mail_db.is_read;
    ml_cur->mail_size=mail_db.mail_size;
    if (mail_db.uidl_len)
    {
      uidl=malloc(mail_db.uidl_len+1);
      uidl[mail_db.uidl_len]=0;
      fread(f,uidl,mail_db.uidl_len,&err);
      ml_cur->uidl=uidl;
    }
    else ml_cur->uidl=0;
    ml_cur->num_in_list=-1;
  }
  fclose(f,&err);
}

void write_mail_DB()
{
  int f;
  unsigned int err;
  char fname[128];
  ML_DAEMON *ml_list=(ML_DAEMON *)&mails;
  MAIL_DB mail_db;
  strcpy(fname,EML_PATH);
  strcat(fname,"mails.db");
  if ((f=fopen(fname,A_WriteOnly+A_BIN+A_Create+A_Truncate,P_WRITE,&err))==-1)
    return;
  while((ml_list=ml_list->next))
  {
    mail_db.magic=MDB_MAGIC;
    (ml_list->uidl)?(mail_db.uidl_len=strlen(ml_list->uidl)):(mail_db.uidl_len=0);
    mail_db.state=ml_list->state;
    mail_db.is_read=ml_list->is_read;
    mail_db.mail_size=ml_list->mail_size;
    fwrite(f,&mail_db,sizeof(MAIL_DB),&err);
    if (mail_db.uidl_len) fwrite(f,ml_list->uidl,mail_db.uidl_len,&err);
  }
  fclose(f,&err);
}


void add_uidl_if_exist(char *recv_line)
{
  int num_in_list=strtoul(recv_line,0,10);
  recv_line=strchr(recv_line,' ')+1;
  *strrchr(recv_line,'\r')=0;
  ML_DAEMON *ml_list=(ML_DAEMON *)&mails;
  ML_DAEMON *ml_cur;
  while(ml_list->next)
  {
    ml_list=ml_list->next;
    if (ml_list->uidl)
    {
      if (!strcmp(recv_line,ml_list->uidl))
      {
        ml_list->num_in_list=num_in_list;
        return;
      }
    }
  }
  ml_cur=ml_list->next=malloc(sizeof(ML_DAEMON));
  ml_cur->num_in_list=num_in_list;
  ml_cur->next=0;
  ml_cur->uidl=malloc(strlen(recv_line)+1);
  strcpy(ml_cur->uidl,recv_line);
  ml_cur->state=DEFAULT_ACTION;
  ml_cur->is_read=0;
  ml_cur->mail_size=0;
}
  
void set_mes_size(char *recv_line)
{
  int num_in_list;
  int mes_size;
  num_in_list=strtoul(recv_line,0,10);
  mes_size=strtoul(strchr(recv_line,' ')+1,0,10);

  ML_DAEMON *ml_list=(ML_DAEMON *)mails;
  while(ml_list)
  {
    if (ml_list->num_in_list==num_in_list)
    {
      ml_list->mail_size=mes_size;
      return;
    }
    ml_list=ml_list->next;
  }
}

void remove_cur_from_list()
{
  char fname[128];
  unsigned int err;
  ML_DAEMON *ml_list=(ML_DAEMON *)&mails;
  ML_DAEMON *ml_prev=ml_list;
  while ((ml_list=ml_list->next))
  {
    if (!strcmp(ml_list->uidl,cur_ml->uidl))
    {
      sprintf(fname,"%s%s.eml",EML_PATH,cur_ml->uidl);
      unlink(fname,&err);
      ml_prev->next=cur_ml->next;
      if (cur_ml->uidl) mfree(cur_ml->uidl);
      mfree(cur_ml);
      cur_ml=ml_prev;
      return;
    }
    ml_prev=ml_list;
  }
}
  
void do_losted_mes()
{
  if (IF_DEL_AUX==NULL) remove_cur_from_list();
}  

void send_str(char* str)
{
  send(sock,str,strlen(str),0);
}

void send_login()
{
  char login[128];
  sprintf(login,"USER %s\r\n",MAIL_LOGIN);
  send_str(login);
}

void send_pass()
{
  char pass[128];
  sprintf(pass,"PASS %s\r\n",MAIL_PASS);
  send_str(pass);
}

void send_get_stat()
{send_str("STAT\r\n");}

void send_get_uidl_list()
{send_str("UIDL\r\n");}

void get_list()
{send_str("LIST\r\n");}

void send_get_mes(int i)
{
  char buf[64];
  sprintf(buf,"RETR %u\r\n",i);
  send_str(buf);
}

void send_get_headers(int i)
{
  char buf[64];
  sprintf(buf,"TOP %u 0\r\n",i);
  send_str(buf);
}

void send_del_mes(int i)
{
  char buf[64];
  sprintf(buf,"DELE %u\r\n",i);
  send_str(buf);
}

void send_quit()
{send_str("QUIT\r\n");}


int resp_ok(char *buf)
{
  if (strncmp(buf,"+OK",3)) return -1;
  return (0);
}

void end_socket();



void end_connect(char *err)
{
  strcpy(logmsg,err);
  REDRAW();
  send_quit();
  end_socket();
  if (recived_line)
  {
    mfree(recived_line);
    recived_line=0;
  }
}
  
int fhandler;
void process_line(char *rec_line)
{
  unsigned int err;
  char fname[128];
  switch(pop_state)
  {
  case POP_GREETING:
    if (resp_ok(rec_line))
    {
      end_connect(rec_line);
      return;
    }
    send_login();
    pop_state=POP_VER_LOGIN;
    REDRAW();
    return;
    
  case POP_VER_LOGIN:
    if (resp_ok(rec_line))
    {
      end_connect(rec_line);
      return;
    }
    send_pass();
    pop_state=POP_VER_PASS;
    REDRAW();
    return;
    
  case POP_VER_PASS:
    if (resp_ok(rec_line))
    {
      end_connect(rec_line);
      return;
    }
    send_get_stat();
    pop_state=POP_GET_STAT;
    REDRAW();
    return;
    
  case POP_GET_STAT:
    if (resp_ok(rec_line))
    {
      end_connect(rec_line);
      return;
    }
    in_pop3=strtoul(strchr(rec_line,' ')+1,0,10);
    send_get_uidl_list();
    pop_state=POP_GET_UIDL_LIST;
    REDRAW();
    return;
    
  case POP_GET_UIDL_LIST:
    if (!mes_rec)
    {
      if (resp_ok(rec_line))
      {
        end_connect(rec_line);
        return;
      }
      mes_rec=1;
      return;
    }
    else
    {
      if (strcmp(rec_line,".\r\n"))
      {
        add_uidl_if_exist(rec_line);
        return;
      }
      else
      {
        mes_rec=0;
        get_list();
        pop_state=POP_GET_LIST;
        REDRAW();
        return;
      }
    }
    
  case POP_GET_LIST:
    if (!mes_rec)
    {
      if (resp_ok(rec_line))
      {
        end_connect(rec_line);
        return;
      }
      mes_rec=1;
      break;
    }
    else
    {
      if (strcmp(rec_line,".\r\n"))
      {
        set_mes_size(rec_line);
        return;
      }
      else
      {
        mes_rec=0;
        cur_ml=(ML_DAEMON*)&mails;
        write_mail_DB();
        pop_state=POP_PROCESS_LIST;
        REDRAW();
      }
    }
    
  case POP_PROCESS_LIST:
  case POP_RECEIVE_HEADERS:
  case POP_RECEIVE_MESSAGE:
  case POP_DELETE_MESSAGE:
    if (pop_state==POP_RECEIVE_HEADERS||pop_state==POP_RECEIVE_MESSAGE)
    {
      if (!mes_rec)
      {
        if (resp_ok(rec_line))
        {
          end_connect(rec_line);
          return;
        }
        sprintf(fname,"%s%s.eml",EML_PATH,cur_ml->uidl);
        if ((fhandler=fopen(fname,A_WriteOnly+A_Create+A_Truncate+A_BIN,P_WRITE,&err))==-1)
        {
          end_connect(pop_state==POP_RECEIVE_HEADERS?"Can't write headers!":"Can't write message!");
          return;
        }
        mes_rec=1;
        return;
      }
      else
      {
        if (strcmp(rec_line,".\r\n"))
        {
          fwrite(fhandler,rec_line,strlen(rec_line),&err);
          return;
        }
        else
        {
          cur_ml->state=(pop_state==POP_RECEIVE_HEADERS)?M_HEADERS_LOADED:M_FULL_LOADED;
          fclose(fhandler,&err);
          mes_rec=0;
          write_mail_DB();
          pop_state=POP_PROCESS_LIST;
          REDRAW();
        }
      }
    }
    else if (pop_state==POP_DELETE_MESSAGE)
    {
      if (resp_ok(rec_line))
      {
        end_connect("Can't delete!");
        return;
      }
      pop_state=POP_PROCESS_LIST;
      REDRAW();
    }
    
    while ((cur_ml=cur_ml->next))
    {
      if (cur_ml->num_in_list==-1)
      {
        do_losted_mes();
        continue;
      }
      switch(cur_ml->state)
      {
      case M_LOAD_HEADER:
        send_get_headers(cur_ml->num_in_list);
        pop_state=POP_RECEIVE_HEADERS;
        REDRAW();
        return;
      case M_LOAD_FULL:
        send_get_mes(cur_ml->num_in_list);
        pop_state=POP_RECEIVE_MESSAGE;
        REDRAW();
        return;
      case M_HEADERS_LOADED:
      case M_FULL_LOADED:
        continue;
        
      case M_DELETE:
        send_del_mes(cur_ml->num_in_list);
        remove_cur_from_list();
        pop_state=POP_DELETE_MESSAGE;
        REDRAW();
        return;
      }
    }
    pop_state=0;
    write_mail_DB();
    end_connect("Done!");
    return;
  }
}






#define BUF_SIZE 1024
void get_answer(void)
{
  char msg_buf[BUF_SIZE];
  char *s=msg_buf;
  char *d; //����
  int c;
  int i;
  i=recv(sock,msg_buf,BUF_SIZE-1,0);
  if (i>0)
  {
    d=recived_line;
    if (d)
    {
      //��� ���� ������
      d=recived_line=realloc(recived_line,strlen(d)+i+1); //��������, �� ������� ���� ����� � \0
      d+=strlen(d);
    }
    else
    {
      //������
     L_NEWLINE:
       d=recived_line=malloc(i+1); //��������, ��� ����� ����� ����� � \0
    }
    while(i)
    {
      i--;
      c=*d++=*s++; //�������� ������
      if (c=='\n')
      {
        //����� ������
        *d=0;
        process_line(recived_line);
        mfree(recived_line);
        goto L_NEWLINE;
      }
    }
    *d=0; //�������� ��������� ������
  }
}

// ==============================================================================================
int DNR_ID=0;
int DNR_TRIES=3;


void create_connect(void)
{
  char ip_adr[64];
  char *end_ip;
  unsigned int ip;
  int err;
  int ***p_res=NULL;
  
  void do_reconnect(void);
  SOCK_ADDR sa;
  //������������� ����������
  connect_state=0;
  GBS_DelTimer(&reconnect_tmr);
  DNR_ID=0;
  strcpy(ip_adr,POP3_ADRESS);
  end_ip=strrchr(ip_adr,':');
  if (end_ip) *end_ip=0;
  ip=str2ip(ip_adr);
  if (ip!=0xFFFFFFFF)  
  {
    snprintf(logmsg,255,"Connect via IP...");
    REDRAW();
    sa.ip=ip;
    goto L_CONNECT;
  }  
  snprintf(logmsg,255,"Send DNR...");
  REDRAW();
  *socklasterr()=0;
  err=async_gethostbyname(POP3_ADRESS,&p_res,&DNR_ID); //03461351 3<70<19<81
  if (err)
  {
    if ((err==0xC9)||(err==0xD6))
    {
      if (DNR_ID)
      {
	return; //���� ���������� DNR
      }
    }
    else
    {
      snprintf(logmsg,255,"DNR ERROR %d!",err);
      REDRAW();
      GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
      return;
    }
  }
  if (p_res)
  {
    if (p_res[3])
    {
      snprintf(logmsg,255,"DNR Ok, connecting...");
      REDRAW();
      DNR_TRIES=0;
      sa.ip=p_res[3][0][0];

    L_CONNECT:
      sock=socket(1,1,0);
      if (sock!=-1)
      {
	sa.family=1;
	sa.port=htons(POP3_PORT);
	if (connect(sock,&sa,sizeof(sa))!=-1)
	{
	  connect_state=1;
	  REDRAW();
	}
	else
	{
	  closesocket(sock);
	  sock=-1;
	  LockSched();
	  ShowMSG(1,(int)"Can't connect!");
	  UnlockSched();
	  GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
	}
      }
      else
      {
	LockSched();
	ShowMSG(1,(int)"Can't create socket, GPRS restarted!");
	UnlockSched();
	//�� ������� �������� ������, ��������� GPRS-������
	GPRS_OnOff(0,1);
      }
    }	
  }
  else
  {
    DNR_TRIES--;
    LockSched();
    ShowMSG(1,(int)"Host not found!");
    UnlockSched();
  }
}

void do_reconnect(void)
{
  if (is_gprs_online)
  {
    DNR_TRIES=3;
    SUBPROC((void*)create_connect);
  }
}

void end_socket(void)
{
  if (sock>=0)
  {
    shutdown(sock,2);
    closesocket(sock);
  }
}

// ==============================================================================================

void OnRedraw(MAIN_GUI *data)
{ 
  int scr_w=ScreenW();
  int scr_h=ScreenH();
  DrawRoundedFrame(0,YDISP,scr_w-1,scr_h-1,0,0,0,
		   GetPaletteAdrByColorIndex(0),
		   GetPaletteAdrByColorIndex(20));
  wsprintf(data->ws1,"State: %d POPState: %d\n%t\n%t",connect_state,pop_state,c_states[pop_state],logmsg);
  DrawString(data->ws1,3,3+YDISP,scr_w-4,scr_h-4-GetFontYSIZE(SMALL_FONT),SMALL_FONT,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));

  wsprintf(data->ws1,percent_t,"Exit");
  DrawString(data->ws1,(scr_w>>1),scr_h-4-GetFontYSIZE(MIDDLE_FONT),scr_w-4,scr_h-4,MIDDLE_FONT,2,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));

  wsprintf(data->ws1,percent_t,connect_state<=0?"Mails":empty_str);
  DrawString(data->ws1,3,scr_h-4-GetFontYSIZE(MIDDLE_FONT),scr_w>>1,scr_h-4,MIDDLE_FONT,2,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
  
  
}


void OnCreate(MAIN_GUI *data, void *(*malloc_adr)(int))
{
  data->ws1=AllocWS(256);
  data->gui.state=1;
}

void OnClose(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  FreeWS(data->ws1);
  data->gui.state=0;
}

void OnFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  DisableIDLETMR();
  data->gui.state=2;
}

void OnUnfocus(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}


int OnKey(MAIN_GUI *data, GUI_MSG *msg)
{
  DirectRedrawGUI();
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    switch(msg->gbsmsg->submess)
    {
    case LEFT_SOFT:
      if (connect_state<=0)
        //create_maillist_menu();
      break;

    case RIGHT_SOFT:
      return(1); //���������� ����� GeneralFunc ��� ���. GUI -> �������� GUI
      
    case GREEN_BUTTON:
      if ((connect_state==0)&&(sock==-1))
      {
        GBS_DelTimer(&reconnect_tmr);
        DNR_TRIES=3;
        SUBPROC((void *)create_connect);
      }
      break;
    }
  }
  return(0);
}


int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)OnRedraw,	//Redraw
  (void *)OnCreate,	//Create
  (void *)OnClose,	//Close
  (void *)OnFocus,	//Focus
  (void *)OnUnfocus,	//Unfocus
  (void *)OnKey,	//OnKey
  0,
  (void *)kill_data, //method7,	//Destroy
  (void *)method8,
  (void *)method9,
  0
};

const RECT Canvas={0,0,0,0};

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  
  if (msg->msg==MSG_RECONFIGURE_REQ)
  {
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"MailDaemon config updated!");
      InitConfig();
    }
  }
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      csm->csm.state=-3;
    }
  }
  if (msg->msg==MSG_HELPER_TRANSLATOR)
  {
    switch((int)msg->data0)
    {
    case LMAN_DISCONNECT_IND:
      is_gprs_online=0;
      return(1);
    case LMAN_CONNECT_CNF:
      is_gprs_online=1;
      GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
      return(1);
    case ENIP_DNR_HOST_BY_NAME:
      if ((int)msg->data1==DNR_ID)
      {
	if (DNR_TRIES) SUBPROC((void *)create_connect);
      }
      return(1);
    }
    if ((int)msg->data1==sock)
    {
      //���� ��� �����
      switch((int)msg->data0)
      {
      case ENIP_SOCK_CONNECTED:
	if (connect_state==1)
	{
	  //���������� ������������, �������� ����� login
          InitMailDB();
	  strcpy(logmsg,"Try to login...");
          connect_state=2;
	  REDRAW();
	}
	else
	{
	  ShowMSG(1,(int)"Illegal message ENIP_SOCK_CONNECTED!");
	}
	break;
      case ENIP_SOCK_DATA_READ:
	if (connect_state>=2)
	{
	  //���� �������� send
	  SUBPROC((void *)get_answer);
	  //    REDRAW();
	}
	else
	{
	  ShowMSG(1,(int)"Illegal message ENIP_DATA_READ");
	}
	break;
      case ENIP_SOCK_REMOTE_CLOSED:
	//������ �� ������� �������
	if (connect_state) SUBPROC((void *)end_socket);
	break;
        
      case ENIP_SOCK_CLOSED:
	connect_state=0;
	sock=-1;
	REDRAW();
	break;
      }
    }
  }
  return(1);
}


void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  patch_rect((void *)(&Canvas));
  main_gui->gui.canvas=(void *)(&Canvas);
  main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=CreateGUI(main_gui);
}

void maincsm_onclose(CSM_RAM *csm)
{
  GBS_DelTimer(&reconnect_tmr);
  if (recived_line)
  {
    mfree(recived_line); 
    recived_line=NULL;
  }
  FreeMailDB();
  SUBPROC((void *)end_socket);
  SUBPROC((void *)ElfKiller);
}


const struct
{
  CSM_DESC maincsm;
  WSHDR maincsm_name;
}MAINCSM =
{
  {
  maincsm_onmessage,
  maincsm_oncreate,
#ifdef NEWSGOLD
  0,
  0,
  0,
  0,
#endif
  maincsm_onclose,
  sizeof(MAIN_CSM),
  1,
  &minus11
  },
  {
    maincsm_name_body,
    NAMECSM_MAGIC1,
    NAMECSM_MAGIC2,
    0x0,
    139
  }
};

void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"Mail Client");
}

int main(char *exename, char *fname)
{
  MAIN_CSM main_csm;
  InitConfig();
  LockSched();
  UpdateCSMname();
  CreateCSM(&MAINCSM.maincsm,&main_csm,0);
  UnlockSched();
  return (0);
}
