#include "..\inc\swilib.h"
#include "conf_loader.h"

#include "inet.h"

#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])
extern unsigned long  strtoul (const char *nptr,char **endptr,int base);

CSM_DESC icsmd;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

WSHDR *ws1;

WSHDR *player_file; //������� �������������� ����

typedef struct
{
  union
  {
    struct
    {
      WSHDR *track;
      WSHDR *artist;
      WSHDR *album;
    };
    WSHDR *ws[3];
  };
  unsigned long timedate;
  int length;
}TSONG;

TSONG *cursong;

extern const unsigned int cfgX;
extern const unsigned int cfgY;

extern const unsigned int cfgW;
extern const unsigned int cfgH;
extern const char CSMADR[];
extern const char TEMP_FILE[];

extern const int TIMEZONESIGN;
extern const unsigned int TIMEZONE;

extern const char USERNAME[];
extern const char PASSWORD[];

unsigned int uiWidth, uiHeight;

unsigned int playercsmid=0;
void *playercsmadr=NULL;

GBSTMR mytmr;
int showstate=0;
volatile int readytochangestate=0;

const char msg_pa[]="Player active!";
const char percent_t[]="%t";

void RereadSettings()
{
  InitConfig();
  playercsmadr=(void *)strtoul(CSMADR,NULL,16);
  //========================
  uiWidth  = cfgW-1;
  uiHeight = cfgH-1;
}

void tmrproc_readychange(void)
{
  readytochangestate=1;
  GBS_StartTimerProc(&mytmr,3*216,tmrproc_readychange);
}

void FreeSONG(TSONG *song)
{
  if (song)
  {
    FreeWS(song->track);
    FreeWS(song->artist);
    FreeWS(song->album);
    mfree(song);
  }
}

void urlescape(char *out, const char *source)
{
  const char * h = "0123456789abcdef";
  int c;
  int i=0;
  while((c =*source++)!=0)
  {
    if ('a' <= c && c <= 'z'
	|| 'A' <= c && c <= 'Z'
	  || '0' <= c && c <= '9'
	    || c == '-' || c == '_' || c == '.')
      out[i++]= c;
    else 
      if( c == ' ' )
	out[i++]= '+';
      else {
	out[i++]= '%';
	out[i++]= h[c >> 4];
	out[i++]= h[c & 0x0f];
      }
  }
  out[i]=0;
}

//��� ������� ������� ����� �������,
//�� ��������� �������� ����� ������� �����/����<->���������� ������ ;)
// ���������� ���� �� ������ ������
const int DMonth[]={0,31,59,90,120,151,181,212,243,273,304,334,365};
// �������������� ������� � ���� � ���������� ������ ����� 01-01-2000 00:00
unsigned long TimeDate2Long(void)
{
  unsigned long iday;
  TTime tt;
  TDate dd;
  GetDateTime(&dd,&tt);
  dd.year=dd.year%100;
  iday=365*dd.year+DMonth[dd.month-1]+(dd.day - 1);
  iday=iday+(dd.year>>2);
  if (dd.month>2||(dd.year&3)>0)
    iday++;
  return(tt.param+60*(tt.min+60*(tt.hour+24* iday)));
}

// ������ ���� � ������� �� ���������� ������ � 01-01-2000 00:00
void PrintTimeDate(char *p, unsigned long x)
{
  unsigned int sec,min,hrs,mon,yrs;
  unsigned int day,iday,day4,yrs4;
  
  sec=x%60;
  min=(x/60)%60;
  hrs=(x/3600)%24;
  iday=x/86400;
  yrs4=x/((4*365+1)*86400);
  day4=iday%(4*365+1);
  iday=(day4==(31+28));
  if (day4>=(31+28)) day4--;
  yrs=(yrs4<<2)+day4/365;
  day=day4%365;
  mon=0;
  while (DMonth[++mon]<=day);
  day-=DMonth[mon-1];
  if (iday) day++;
  day++;
  if (yrs>99) yrs=0;
  sprintf(p,"%04d-%02d-%02d %02d:%02d:%02d",yrs+2000,mon,day,hrs,min,sec);
}

void SaveLog(int dummy, TSONG *song)
{
  unsigned int ul;
  char buf[1024];
  char utf8[256];
  int playedsec;
  if ((song->track)&&(song->artist)&&(song->album))
  {
    if (song->length>=30)
    {
      playedsec=TimeDate2Long()-song->timedate;
      if ((playedsec>240)||(playedsec>(song->length>>1)))
      {
	int f=fopen(TEMP_FILE,A_ReadWrite+A_Append+A_Create+A_BIN,P_READ+P_WRITE,&ul);
	if (f!=-1)
	{
	  int i;
	  fwrite(f,"&a[]=",5,&ul);
	  ws_2utf8(song->artist,utf8,&i,255);
	  fwrite(f,utf8,strlen(utf8),&ul);

	  fwrite(f,"&b[]=",5,&ul);
	  ws_2utf8(song->album,utf8,&i,255);
	  fwrite(f,utf8,strlen(utf8),&ul);

	  fwrite(f,"&t[]=",5,&ul);
	  ws_2utf8(song->track,utf8,&i,255);
	  fwrite(f,utf8,strlen(utf8),&ul);
	  
	  fwrite(f,"&i[]=",5,&ul);
	  PrintTimeDate(utf8,TIMEZONESIGN?song->timedate-3600*TIMEZONE:song->timedate+3600*TIMEZONE);
	  fwrite(f,utf8,strlen(utf8),&ul);
	  
	  fwrite(f,"&l[]=",5,&ul);
	  sprintf(utf8,"%d",song->length);
	  fwrite(f,utf8,strlen(utf8),&ul);
	  
	  fwrite(f,"&m[]=\r\n",7,&ul);
	  fclose(f,&ul);
	}
      }
    }
  }
  FreeSONG(song);
}

int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  int csm_result;
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"LastFMD config updated!");
      RereadSettings();
    }
  }
  if (playercsmid)
  {
    //���������, ���� �� ������
    CSM_RAM *csmp=FindCSMbyID(playercsmid);
    if (csmp)
    {
      WSHDR *fn=((WSHDR **)csmp)[0x2C/4];
      if (!fn) goto L_ACTIVE;
      if (!fn->wsbody) goto L_ACTIVE;
      if (fn->wsbody[0])
      {
	if (!player_file) goto L_UPDATE; //��� ����������� �����
	if (memcmp(fn->wsbody,player_file->wsbody,player_file->wsbody[0]*2))
	{
	  //���������� ����� � ������, ���������� �������� � ��� ������ � ��������
	  SUBPROC((void *)SaveLog,0,cursong);
	L_UPDATE:
	  zeromem(cursong=malloc(sizeof(TSONG)),sizeof(TSONG));
	  cursong->timedate=TimeDate2Long();
	  FreeWS(player_file);
	  wstrcpy(player_file=AllocWS(fn->wsbody[0]),fn);
	}
	if (readytochangestate)
	{
	  readytochangestate=0;
	  if (showstate)
	  {
#ifdef ELKA
	    WSHDR **wws=((WSHDR ***)csmp)[(0x18C/4)]-1;
#else
	    WSHDR **wws=((WSHDR ***)csmp)[(0x170/4)]-1;
#endif
	    if (wws)
	    {
	      WSHDR *tws;
	      while(1)
	      {
		if (showstate>4) goto ZEROSTATE;
		if (showstate==4)
		{
		  showstate++;
		  wsprintf(ws1,"Song length %ds",cursong->length=(int)wws[6]);
		  break;
		}
		if (!(tws=wws[showstate++])) continue;
		if (!tws->wsbody) continue;
		if (tws->wsbody[0])
		{
		  wstrcpy(ws1,tws);
		  break;
		}
	      }
	      {
		//������ ������ �� ������� win1251
		int i=1;
		int c;
		while(i<=ws1->wsbody[0])
		{
		  c=ws1->wsbody[i];
		  if (c==0xA8) c=0x401;
		  if (c==0xAA) c=0x404;
		  if (c==0xAF) c=0x407;
		  if (c==0xB8) c=0x451;
		  if (c==0xBA) c=0x454;
		  if (c==0xBF) c=0x457;
		  if ((c>=0xC0)&&(c<0x100)) c+=0x350;
		  ws1->wsbody[i++]=c;
		}
	      }
	      if (!cursong->ws[showstate-2])
	      {
		wstrcpy(cursong->ws[showstate-2]=AllocWS(ws1->wsbody[0]),ws1);
	      }
	    }
	    else
	      goto ZEROSTATE;
	  }
	  else
	  {
	  ZEROSTATE:
	    showstate=0;
	    wstrcpy(ws1,fn);
	    showstate++;
	  }
	}
      }
      else
      {
      L_ACTIVE:
	wsprintf(ws1,percent_t,msg_pa);
	showstate=0;
      }
    }
    else
    {
      playercsmid=0;
      wsprintf(ws1,"Player not active");
      //����� �� �������, ��������� ��������� ����
      if (player_file)
      {
	FreeWS(player_file);
	player_file=NULL;
	if (cursong) SUBPROC((void *)SaveLog,0,cursong);
	cursong=NULL;
      }
    }
  }
  else
  {
    CSM_RAM *p=((void **)CSM_root()->csm_q)[2];
    while(p)
    {
      if (p->constr==playercsmadr) break;
      p=p->next;
    }
    if (p)
    {
      playercsmid=p->id;
      wsprintf(ws1,percent_t,msg_pa);
      showstate=0;
    }
  }
  ParseSocketMsg(msg);
  csm_result = old_icsm_onMessage(data, msg); //�������� ������ ���������� �������
  if (IsGuiOnTop(idlegui_id)) //���� IdleGui �� ����� �����
  {
    GUI *igui = GetTopGUI();
    if (igui) //� �� ����������
    {
#ifdef ELKA
      {
	void *canvasdata = BuildCanvas();
#else
	void *idata = GetDataOfItemByID(igui, 2);
	if (idata)
	{
	  void *canvasdata = ((void **)idata)[DISPLACE_OF_IDLECANVAS / 4];
#endif
	  DrawCanvas(canvasdata, cfgX, cfgY, cfgX + uiWidth, cfgY + uiHeight, 1);
	  DrawString(ws1, cfgX, cfgY, cfgX + uiWidth, cfgY + uiHeight, SMALL_FONT, 0x20,
		     GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(1));
      }
    }
  }
  return(csm_result);
}

void FreeAll(void)
{
  extern GBSTMR reconnect_tmr;
  GBS_DelTimer(&mytmr);
  GBS_DelTimer(&reconnect_tmr);
  ClearSendQ();
  ClearRecvQ();
  FreeWS(ws1);
  FreeWS(player_file);
  FreeSONG(cursong);
}

void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  FreeAll();
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

int main(void)
{
  RereadSettings();
  LockSched();
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  old_icsm_onClose=icsmd.onClose;
  icsmd.onMessage=MyIDLECSM_onMessage;
  icsmd.onClose=MyIDLECSM_onClose;
  icsm->constr=&icsmd;
  UnlockSched();
  ws1=AllocWS(128);
  wsprintf(ws1,"%t","LastFMD(C)Rst7");
  GBS_StartTimerProc(&mytmr,3*216,tmrproc_readychange);
  return 0;
}
