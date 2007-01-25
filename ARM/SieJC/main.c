#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "SieJC.h"
#include "history.h"
#include "conf_loader.h"
#include "main.h"


/*
============== �������� Jabber-������ ==============
����������� ������� ����-������ ( :) ):
1. ���� � ����
2. ��������� �� ���� �������-����� :)
3. ��������� ������� ������: ������� iq, ������, ������ �����������
4. ������ ������ ������ �/�� ������� (���� 4:\jlog.txt)

����������: ���������� � ������� ������ ����� �������������,
��������� ������� ������ - � ���������� ����.
�����, ����� ������� �����������:
������� ��:
1 - �������� � ������� �����������,
2 - ������� ������� �� ������,
3 - ������� ������ ����������
0 - ���������� (����� ����� ����� ����� ����� Welcome)

��� ����� ������� � ������ �������:
 - �������� XML-������, ����� ��� �������� �������.
=====================================================
(c) Kibab
(r) Rst7, AD

*/

// ============= ������� ������ ============= 
const char NATICQ_HOST[] = "jabber.ru";
const unsigned int NATICQ_PORT = 5222;
const char USERNAME[] = "________";   // ������ ���� ������!!!!
const char PASSWORD[] = "________";   
const char RESOURCE[] = "Exp_SNJ";
const char VERSION_NAME[]= "Sie natJabber by Kibab";
const char VERSION_VERS[] = "0.1";


// ���� ���������� �������� ���
#define LOG_ALL

#define TMR_SECOND 216

char Is_Vibra_Enabled = 1;

#define EOP -10


const char percent_t[]="%t";
const char empty_str[]="";
const char I_str[]="I";

WSHDR *ews;

typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  GUI gui;
  WSHDR *ws1;
  WSHDR *ws2;
  int i1;
}MAIN_GUI;
int RXstate=EOP; //-sizeof(RXpkt)..-1 - receive header, 0..RXpkt.data_len - receive data

char *msg_buf;
TPKT RXbuf;
TPKT TXbuf;
char logmsg[256];

int connect_state=0;

int sock=-1;

volatile int is_gprs_online=1;

GBSTMR reconnect_tmr;

extern void kill_data(void *p, void (*func_p)(void *));

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

int total_unread;

#pragma inline
void patch_rect(RECT*rc,int x,int y, int x2, int y2)
{
  rc->x=x;
  rc->y=y;
  rc->x2=x2;
  rc->y2=y2;
}

//===============================================================================================
// ELKA Compatibility
#pragma inline
void patch_header(HEADER_DESC* head)
{
  head->rc.x=0;
  head->rc.y=YDISP;
  head->rc.x2=ScreenW()-1;
  head->rc.y2=HeaderH()+YDISP;
}
#pragma inline
void patch_input(INPUTDIA_DESC* inp)
{
  inp->rc.x=0;
  inp->rc.y=HeaderH()+1+YDISP;
  inp->rc.x2=ScreenW()-1;
  inp->rc.y2=ScreenH()-SoftkeyH()-1;
}
//===============================================================================================

volatile CLIST *cltop;

volatile int contactlist_menu_id;
volatile int request_close_clmenu;
volatile int request_remake_clmenu;

GBSTMR tmr_vibra;
volatile int vibra_count;

volatile int edchat_id;
volatile int request_remake_edchat;
volatile int request_close_edchat;
volatile int edchat_toitem;
volatile int edchat_answeritem;
CLIST *edcontact;

//MUTEX contactlist_mtx;

HEADER_DESC contactlist_menuhdr={0,0,0,0,NULL,(int)"Contacts...",LGP_NULL};
int menusoftkeys[]={0,1,2};
SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Options"},
  {0x0001,0x0000,(int)"Close"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

void contactlist_menu_ghook(void *data, int cmd);
int contactlist_menu_onkey(void *data, GUI_MSG *msg);
void contactlist_menu_iconhndl(void *data, int curitem, int *unk);

MENU_DESC contactlist_menu=
{
  8,(void *)contactlist_menu_onkey,(void*)contactlist_menu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  0x11,
  (void *)contactlist_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};

GBSTMR tmr_ping;

char My_JID[128];
char My_JID_full[128];

//���������� ������
void FreeCLIST(void)
{
  CLIST *cl=(CLIST*)cltop;
  cltop=0;
  while(cl)
  {
    CLIST *p;
    if (cl->log) mfree(cl->log);
    if (cl->answer) mfree(cl->answer);
    p=cl;
    cl=(CLIST*)(cl->next);
    mfree(p);
  }
}

//��������� ���� � offline
void FillAllOffline(void)
{
  CLIST *cl=(CLIST*)cltop;
  while(cl)
  {
    cl->state=0xFFFF;
    cl=(CLIST*)(cl->next);
  }
}

int GetIconIndex(CLIST *t)
{
  unsigned short s;
  if (t)
  {
    s=t->state;
    if (t->isunread)
      return(IS_MSG);
    else
    {
      if (s==0xFFFF) return(IS_OFFLINE);
      if (s & 0x0020) return(IS_FFC);
      if (s & 0x0001) return(IS_AWAY);
      if (s & 0x0005) return(IS_NA);
      if (s & 0x0011) return(IS_OCCUPIED);
      if (s & 0x0013) return(IS_DND);      
      if (s & 0x0002) return(IS_FFC);
      if (s & 0x0100) return(IS_INVISIBLE);
    }
  }
  else
  {
    return(IS_UNKNOWN);
  }
  return(IS_ONLINE);
}

void create_contactlist_menu(void)
{
  CLIST *t;
  int i;
  
  t=(CLIST *)cltop;
  i=0;
  while(t)
  {
    t=t->next;
    i++;
  }
  if (!i) return; //������ ���������
  patch_rect(&contactlist_menuhdr.rc,0,0,ScreenW()-1,HeaderH());
  contactlist_menu_id=CreateMenu(0,0,&contactlist_menu,&contactlist_menuhdr,0,i,0,0);
}

int need_jump_to_top_cl;

void contactlist_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
    if (need_jump_to_top_cl) 
    {
      SetCursorToMenuItem(data,0);
      need_jump_to_top_cl=0;
    }
    if (request_close_clmenu)
    {
      request_close_clmenu=0;
      GeneralFunc_flag1(contactlist_menu_id,1);
    }
  }
}

int contactlist_menu_onkey(void *data, GUI_MSG *msg)
{
//  CLIST *t;
//  int i;
  if (msg->keys==0x18)
  {
    //    GeneralFunc_F1(1);
    //ShowMSG(1,(int)"Under construction!");
//    ShowMainMenu();
    return(-1);
  }
  if (msg->keys==0x3D)
  {
//    void CreateEditChat(CLIST *t);
//    i=GetCurMenuItem(data);
//    t=FindContactByN(i);
//    if (t) CreateEditChat(t);
    //    GeneralFunc_F1(1);
    return(-1);
  }
  return(0);
}

void contactlist_menu_iconhndl(void *data, int curitem, int *unk)
{
/*
  CLIST *t;
  WSHDR *ws;
  void *item=AllocMenuItem(data);
  
//  t=FindContactByN(curitem);
  if (t)
  {
    ws=AllocMenuWS(data,strlen(t->name));
//    wsprintf(ws,percent_t,t->name);
  }
  else
  {
    ws=AllocMenuWS(data,10);
    wsprintf(ws,"error");
  }
//  SetMenuItemIconArray(data,item,S_ICONS);
  SetMenuItemText(data,item,ws,curitem);
  SetMenuItemIcon(data,curitem,GetIconIndex(t));
*/
}

void remake_clmenu(void)
{
  if (contactlist_menu_id)
  {
    request_remake_clmenu=1;
    if (IsGuiOnTop(contactlist_menu_id))
    {
      GeneralFunc_flag1(contactlist_menu_id,1);
    }
    else
    {
      request_close_clmenu=1;
    }
  }
  else
  {
    create_contactlist_menu();
  }
}

CLIST *AddContact(unsigned int uin, char *name)
{
  CLIST *p=malloc(sizeof(CLIST));
  CLIST *t;
  CLIST *pr;
  zeromem(p,sizeof(CLIST));
  p->uin=uin;
  strncpy(p->name,name,sizeof(p->name)-1);
  p->state=0xFFFF;
  t=(CLIST *)cltop;
  if (t)
  {
    //�� ������
    pr=(CLIST *)&cltop;
    while(strcmp(t->name,p->name)<0)
    {
      pr=t;
      t=t->next;
      if (!t) break;
    }
    p->next=t;
    pr->next=p;
  }
  else
  {
    //������
    cltop=p;
  }
  //  GBS_StartTimerProc(&tmr_contactlist_update,1000,remake_clmenu);
  return(p);
}

//===============================================================================================
int DNR_ID=0;
int DNR_TRIES=3;

void create_connect(void)
{
  int ***p_res=NULL;
  void do_reconnect(void);
  SOCK_ADDR sa;
  //������������� ����������
  connect_state=0;
  GBS_DelTimer(&reconnect_tmr);
  DNR_ID=0;
  snprintf(logmsg,255,"Send DNR...");
  REDRAW();
  *socklasterr()=0;
  int err=async_gethostbyname(NATICQ_HOST,&p_res,&DNR_ID); //03461351 3<70<19<81
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
      sock=socket(1,1,0);
      if (sock!=-1)
      {
	sa.family=1;
	sa.port=htons(NATICQ_PORT);
	sa.ip=p_res[3][0][0];
	//    sa.ip=htonl(IP_ADDR(82,207,89,182));
	if (connect(sock,&sa,sizeof(sa))!=-1)
	{
	  ShowMSG(1,(int)"OK!");
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

void do_ping(void)
{
  static PKT pingp;
//  pingp.uin=UIN;
  pingp.type=0;
  pingp.data_len=0;
  send(sock,&pingp,sizeof(PKT),0);
}


void call_ping(void)
{
  if (connect_state>2) SUBPROC((void *)do_ping);
  GBS_StartTimerProc(&tmr_ping,120*TMR_SECOND,call_ping);
}

void end_socket(void)
{
  if (sock>=0)
  {
    shutdown(sock,2);
    closesocket(sock);
  }
}

/*
GBSTMR tmr_dorecv;

void dorecv(void)
{
void get_answer(void);
if (connect_state>1)
{
SUBPROC((void *)get_answer);
  }
}*/

void get_answer(void)
{
//int i=RXstate;
char* buf = malloc(512);
zeromem(buf,512);
int rec_bytes = 0;
rec_bytes = recv(sock, buf, 512, 0);
char *mess = malloc(10);
sprintf(mess,"RECV:%d",rec_bytes);
Log(mess, buf);
mfree(buf);
mfree(mess);
/*
  void *p;
  int j;
  if (connect_state<2) return;
  if (i==EOP) return;
  if (i<0)
  {
    j=recv(sock,RXbuf.data+i,-i,0);
    if (j>0) i+=j;
    if (i==0)
    {
      if (RXbuf.pkt.data_len==0) goto LPKT;
    }
  }
  else
  {
    if (RXbuf.pkt.data_len>16383)
    {
      snprintf(logmsg,255,"Bad packet");
      end_socket();
      RXstate=EOP;
      return;
    }
    j=recv(sock,RXbuf.data+i,RXbuf.pkt.data_len-i,0);
    if (j>0) i+=j;
  LPKT:
    if (i==RXbuf.pkt.data_len)
    {
      //����� ������ ������, ����� ���������...
      RXbuf.data[RXbuf.pkt.data_len]=0; //����� ������
      switch(RXbuf.pkt.type)
      {
      case T_LOGIN:
        //������ ������������
        GBS_StartTimerProc(&tmr_ping,120*TMR_SECOND,call_ping);
        snprintf(logmsg,255,"%s",RXbuf.data);
        connect_state=3;
        REDRAW();
        break;
      case T_CLENTRY:
        j=i+sizeof(PKT)+1;
        p=malloc(j);
        memcpy(p,&RXbuf,j);
        GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,p,sock);
        //snprintf(logmsg,255,"CL: %s",RXbuf.data);
        break;
      case T_STATUSCHANGE:
        j=i+sizeof(PKT);
        p=malloc(j);
        memcpy(p,&RXbuf,j);
        snprintf(logmsg,255,"SC%d: %04X",RXbuf.pkt.uin,*((unsigned short *)(RXbuf.data)));
        GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,p,sock);
        break;
      case T_ERROR:
        snprintf(logmsg,255,"ERR: %s",RXbuf.data);
        REDRAW();
        break;
      case T_RECVMSG:
        j=i+sizeof(PKT)+1;
        p=malloc(j);
        memcpy(p,&RXbuf,j);
        snprintf(logmsg,255,"MSG%d: %s",RXbuf.pkt.uin,RXbuf.data);
        GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,p,sock);
        REDRAW();
        break;
      case T_SSLRESP:
        LockSched();
        ShowMSG(1,(int)RXbuf.data);
        UnlockSched();
        break;
      }
      i=-(int)sizeof(PKT); //� ����� ��� ���� ������
    }
  }
  RXstate=i;
  //  GBS_StartTimerProc(&tmr_dorecv,3000,dorecv);
  //  REDRAW();
*/  
}

void SendAnswer(char *str)
{
  int i = strlen(str);
  send(sock,str,i,0);
#ifdef LOG_ALL
  Log("SEND",str);
#endif
  //mfree(str);
}


/*
  ������� ������������ Jabber Iq
*/
void SendIq(char* to, char* type, char* id, char* xmlns, char* payload)
{
  char* xmlq=malloc(1024);
  char *xmlq2=malloc(1024);
  char s_to[40];
  sprintf(xmlq, "<iq type='%s' id='%s'", type, id);
  if(to)
  {
    sprintf(s_to, " to='%s'", to);
    strcat(xmlq, s_to);
  }
  if(payload)
  {
    strcat(xmlq, ">\r\n<query xmlns='%s'>\r\n%s\r\n</query>\r\n</iq>");
    sprintf(xmlq2, xmlq, xmlns, payload);
  }
  else
  {
    strcat(xmlq, ">\r\n<query xmlns='%s'/>\r\n</iq>");
    sprintf(xmlq2, xmlq, xmlns);
  }
  sprintf(xmlq2, xmlq, xmlns, payload);
  SendAnswer(xmlq2);
  mfree(xmlq2);
  mfree(xmlq);
}

/*
  ������� �����������, �� ���� ������ �������� �������� stream.
  ����� ����� ����� �������� � ��������
*/
void Send_Welcome_Packet()
{
  connect_state = 2;
  char streamheader[]="<?xml version='1.0' encoding='UTF-8'?>\n<stream:stream to='%s' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' xml:lang='en'>";
  char* buf=malloc(256);  
  sprintf(buf,streamheader,NATICQ_HOST);
  SendAnswer(buf);
  mfree(buf);
  strcpy(logmsg,"Send Welcome");
#ifdef LOG_ALL
  Log("CONN",logmsg);
#endif 
}

/*
  ������� ����������
*/
void Send_Disconnect()
{
//  connect_state = 0;
  char streamend[]="</stream:stream>";
  SendAnswer(streamend);
  strcpy(logmsg,"Send Disconnect");
#ifdef LOG_ALL
  Log("CONN",logmsg);
#endif 
}

// ��� ��������� ���� ����� �������� �� ������������
char auth_id[] = "auth_422iup";
char rost_id[] = "rost_2iup";

/*
  ����������� �� Jabber-�������
  ����� �����, ��� ����������.
*/
void Send_Auth()
{
  sprintf(My_JID, "%s@%s",USERNAME, NATICQ_HOST);
  sprintf(My_JID_full,"%s/%s",My_JID, RESOURCE);
  char* payload = malloc(256);
  sprintf(payload,"<username>%s</username>\n<password>%s</password>\n<resource>%s</resource>",USERNAME, PASSWORD, RESOURCE);
  SendIq(NULL, "set", auth_id, "jabber:iq:auth", payload);  
  strcpy(logmsg,"Send auth");
#ifdef LOG_ALL
  Log("USER->", logmsg);
#endif 
}

/*
  ������� ��� ����������� (� ���������, ����� ����� �� ��� ������������ 
  ����������� ���������, � �� ���������� � �������� ������ ��������
*/

void Send_Presence()
{
  char presence[]="<presence><priority>100</priority><status></status></presence>";
  SendAnswer(presence);
  strcpy(logmsg,"Send presence");
#ifdef LOG_ALL
  Log("USER->", logmsg);
#endif 
}

/*
  ������� ������ �������
*/
void Send_Roster_Query()
{
  SendIq(NULL, "get", rost_id, "jabber:iq:roster", NULL); 
  strcpy(logmsg,"Send roster Q");
#ifdef LOG_ALL
  Log("USER->", logmsg);
#endif  
}

void start_vibra(void)
{
  void stop_vibra(void);
  if(Is_Vibra_Enabled)
  {
    SetVibration(100);
    GBS_StartTimerProc(&tmr_vibra,TMR_SECOND>>1,stop_vibra);
  }  
}

void stop_vibra(void)
{
  SetVibration(0);
  if (--vibra_count)
  {
    GBS_StartTimerProc(&tmr_vibra,TMR_SECOND>>1,start_vibra);
  }
}

ProcessPacket(TPKT *p)
{
// ��������� ������� � ��������� MMI... �� ���� ��� �� ����� ����������
// � ����� ������� ��������� �� �������. �� ��� � ��������� ������� ��������
// � ������ ���� ������!
  mfree(p);
}


//===============================================================================================
// ������ ����� � GUI
void method0(MAIN_GUI *data)
{
  int scr_w=ScreenW();
  int scr_h=ScreenH();
  DrawRoundedFrame(0,0,scr_w-1,scr_h-1,0,0,0,
		   GetPaletteAdrByColorIndex(0),
		   GetPaletteAdrByColorIndex(20));
  wsprintf(data->ws1,"State: %d, RXstate: %d\n%t",connect_state,RXstate,logmsg);
  DrawString(data->ws1,3,3,scr_w-4,scr_h-4-16,SMALL_FONT,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
  DrawString(data->ws2,(scr_w>>2)*3,scr_h-4-14,scr_w-4,scr_h-4,MIDDLE_FONT,2,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
  DrawString(data->ws2,3,scr_h-4-14,(scr_w>>2),scr_h-4,MIDDLE_FONT,2,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
}

void method1(MAIN_GUI *data, void *(*malloc_adr)(int))
{
  data->ws1=AllocWS(256);
  data->ws2=AllocWS(256);
  data->gui.state=1;
}

void method2(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  FreeWS(data->ws1);
  FreeWS(data->ws2);
  data->gui.state=0;
}

void method3(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  DisableIDLETMR();
  data->gui.state=2;
}

void method4(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}

int method5(MAIN_GUI *data, GUI_MSG *msg)
{
  DirectRedrawGUI();
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    switch(msg->gbsmsg->submess)
    {
    case LEFT_SOFT:
      if (cltop) create_contactlist_menu();
      //      if (cltop) remake_clmenu();
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
    case '1':
      {
        Send_Auth();
        break;
      }
    case '2':
      {
        Send_Roster_Query();
        break;
      }      
    case '3':
      {
        Send_Presence();
        break;
      }      

    case '0':
      {
        Send_Disconnect();
        break;
      }        
      
    case '*':
      {
        Is_Vibra_Enabled = !(Is_Vibra_Enabled);
        break;
      }
    }
    
  }
  //  method0(data);
  return(0);
}
//void method7(MAIN_GUI *data, void (*mfree_adr)(void *))
//{
//  mfree_adr(data);
//}

int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)method0,	//Redraw
  (void *)method1,	//Create
  (void *)method2,	//Close
  (void *)method3,	//Focus
  (void *)method4,	//Unfocus
  (void *)method5,	//OnKey
  0,
  (void *)kill_data, //method7,	//Destroy
  (void *)method8,
  (void *)method9,
  0
};

const RECT Canvas={0,0,0,0};

void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  patch_rect((RECT*)&Canvas,0,0,ScreenW()-1,ScreenH()-1);
  main_gui->gui.canvas=(void *)(&Canvas);
  main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=CreateGUI(main_gui);
  ews=AllocWS(16384);
  msg_buf=malloc(16384);
  //  MutexCreate(&contactlist_mtx);
  DNR_TRIES=3;
  SUBPROC((void *)create_connect);
}

void maincsm_onclose(CSM_RAM *csm)
{
  //  GBS_DelTimer(&tmr_dorecv);
  GBS_DelTimer(&tmr_ping);
  GBS_DelTimer(&tmr_vibra);
  GBS_DelTimer(&reconnect_tmr);
  SetVibration(0);
  FreeCLIST();
  mfree(msg_buf);
  FreeWS(ews);
  //  MutexDestroy(&contactlist_mtx);
  SUBPROC((void *)end_socket);
  SUBPROC((void *)ElfKiller);
}

void do_reconnect(void)
{
  if (is_gprs_online)
  {
    DNR_TRIES=3;
    SUBPROC((void*)create_connect);
  }
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  //  char ss[100];
  MAIN_CSM *csm=(MAIN_CSM*)data;
 
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      csm->csm.state=-3;
    }
    if ((int)msg->data0==contactlist_menu_id)
    {
      contactlist_menu_id=0;
      if (request_remake_clmenu)
      {
        request_remake_clmenu=0;
        create_contactlist_menu();
      }
    }
    if ((int)msg->data0==edchat_id)
    {
      edchat_id=0;
      if (request_remake_edchat)
      {
//        void CreateEditChat(CLIST *t);
        request_remake_edchat=0;
//        CreateEditChat(edcontact);
      }
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
      vibra_count=3;
      start_vibra();
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
      if ((((unsigned int)msg->data0)>>28)==0xA)
      {
        //������ �����
        //////ProcessPacket((TPKT *)msg->data0);
        return(0);
      }
      switch((int)msg->data0)
      {
      case ENIP_SOCK_CONNECTED:
        if (connect_state==1)
        {
          vibra_count=2;
          start_vibra();
          //���������� �����������, �������� ����� Welcome
          Send_Welcome_Packet();
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
          //���� �������� Welcome, ������� �� �������� � �������� HELPER
          SUBPROC((void *)get_answer);
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
        //        strcpy(logmsg,"No connection");
        connect_state=0;
        sock=-1;
        vibra_count=4;
        start_vibra();
        REDRAW();
        GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
        break;
      }
    }
  }
  return(1);
}


const int minus11=-11;

unsigned short maincsm_name_body[140];

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
  WSHDR *ws=AllocWS(256);
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"SieJC",ws);
  FreeWS(ws);
}


int main()
{
  char dummy[sizeof(MAIN_CSM)];
  
//  InitConfig();
  UpdateCSMname();
  LockSched();
  CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
  return 0;
}

// �� ���� ���� ������� � ������, �������� �����������, �������� ���.

//===========================================================
// Edit chat
//===========================================================
typedef struct
{
  unsigned short u;
  char dos;
  char win;
  char koi8;
} TUNICODE2CHAR;

const TUNICODE2CHAR unicode2char[]=
{
  // CAPITAL Cyrillic letters (base)
  0x410,0x80,0xC0,0xE1, // �
  0x411,0x81,0xC1,0xE2, // �
  0x412,0x82,0xC2,0xF7, // �
  0x413,0x83,0xC3,0xE7, // �
  0x414,0x84,0xC4,0xE4, // �
  0x415,0x85,0xC5,0xE5, // �
  0x416,0x86,0xC6,0xF6, // �
  0x417,0x87,0xC7,0xFA, // �
  0x418,0x88,0xC8,0xE9, // �
  0x419,0x89,0xC9,0xEA, // �
  0x41A,0x8A,0xCA,0xEB, // �
  0x41B,0x8B,0xCB,0xEC, // �
  0x41C,0x8C,0xCC,0xED, // �
  0x41D,0x8D,0xCD,0xEE, // �
  0x41E,0x8E,0xCE,0xEF, // �
  0x41F,0x8F,0xCF,0xF0, // �
  0x420,0x90,0xD0,0xF2, // �
  0x421,0x91,0xD1,0xF3, // �
  0x422,0x92,0xD2,0xF4, // �
  0x423,0x93,0xD3,0xF5, // �
  0x424,0x94,0xD4,0xE6, // �
  0x425,0x95,0xD5,0xE8, // �
  0x426,0x96,0xD6,0xE3, // �
  0x427,0x97,0xD7,0xFE, // �
  0x428,0x98,0xD8,0xFB, // �
  0x429,0x99,0xD9,0xFD, // �
  0x42A,0x9A,0xDA,0xFF, // �
  0x42B,0x9B,0xDB,0xF9, // �
  0x42C,0x9C,0xDC,0xF8, // �
  0x42D,0x9D,0xDD,0xFC, // �
  0x42E,0x9E,0xDE,0xE0, // �
  0x42F,0x9F,0xDF,0xF1, // �
  // CAPITAL Cyrillic letters (additional)
  0x402,'_',0x80,'_', // _ .*.*
  0x403,'_',0x81,'_', // _ .*.*
  0x409,'_',0x8A,'_', // _ .*.*
  0x40A,'_',0x8C,'_', // _ .*.*
  0x40C,'_',0x8D,'_', // _ .*.*
  0x40B,'_',0x8E,'_', // _ .*.*
  0x40F,'_',0x8F,'_', // _ .*.*
  0x40E,0xF6,0xA1,'_', // � ...*
  0x408,0x4A,0xA3,0x4A, // _ .*.*
  0x409,0x83,0xA5,0xBD, // _ .*..
  0x401,0xF0,0xA8,0xB3, // �
  0x404,0xF2,0xAA,0xB4, // �
  0x407,0xF4,0xAF,0xB7, // �
  0x406,0x49,0xB2,0xB6, // _ .*..
  0x405,0x53,0xBD,0x53, // _ .*.*
  // SMALL Cyrillic letters (base)
  0x430,0xA0,0xE0,0xC1, // �
  0x431,0xA1,0xE1,0xC2, // �
  0x432,0xA2,0xE2,0xD7, // �
  0x433,0xA3,0xE3,0xC7, // �
  0x434,0xA4,0xE4,0xC4, // �
  0x435,0xA5,0xE5,0xC5, // �
  0x436,0xA6,0xE6,0xD6, // �
  0x437,0xA7,0xE7,0xDA, // �
  0x438,0xA8,0xE8,0xC9, // �
  0x439,0xA9,0xE9,0xCA, // �
  0x43A,0xAA,0xEA,0xCB, // �
  0x43B,0xAB,0xEB,0xCC, // �
  0x43C,0xAC,0xEC,0xCD, // �
  0x43D,0xAD,0xED,0xCE, // �
  0x43E,0xAE,0xEE,0xCF, // �
  0x43F,0xAF,0xEF,0xD0, // �
  0x440,0xE0,0xF0,0xD2, // �
  0x441,0xE1,0xF1,0xD3, // �
  0x442,0xE2,0xF2,0xD4, // �
  0x443,0xE3,0xF3,0xD5, // �
  0x444,0xE4,0xF4,0xC6, // �
  0x445,0xE5,0xF5,0xC8, // �
  0x446,0xE6,0xF6,0xC3, // �
  0x447,0xE7,0xF7,0xDE, // �
  0x448,0xE8,0xF8,0xDB, // �
  0x449,0xE9,0xF9,0xDD, // �
  0x44A,0xEA,0xFA,0xDF, // �
  0x44B,0xEB,0xFB,0xD9, // �
  0x44C,0xEC,0xFC,0xD8, // �
  0x44D,0xED,0xFD,0xDC, // �
  0x44E,0xEE,0xFE,0xC0, // �
  0x44F,0xEF,0xFF,0xD1, // �
  // SMALL Cyrillic letters (additional)
  0x452,'_',0x90,'_', // _ .*.*
  0x453,'_',0x83,'_', // _ .*.*
  0x459,'_',0x9A,'_', // _ .*.*
  0x45A,'_',0x9C,'_', // _ .*.*
  0x45C,'_',0x9D,'_', // _ .*.*
  0x45B,'_',0x9E,'_', // _ .*.*
  0x45F,'_',0x9F,'_', // _ .*.*
  0x45E,0xF7,0xA2,'_', // � ...*
  0x458,0x6A,0xBC,0x6A, // _ .*.*
  0x491,0xA3,0xB4,0xAD, // _ .*..
  0x451,0xF1,0xB8,0xA3, // �
  0x454,0xF3,0xBA,0xA4, // �
  0x457,0xF5,0xBF,0xA7, // �
  0x456,0x69,0xB3,0xA6, // _ .*..
  0x455,0x73,0xBE,0x73, // _ .*.*
  0x0A0,'_',0xA0,0x20, // space .*..
  0x0A4,'_',0xA4,0xFD, // �   .*..
  0x0A6,'_',0xA6,'_', // �   .*.*
  0x0B0,0xF8,0xB0,0x9C, // �
  0x0B7,0xFA,0xB7,0x9E, // �
  // 0x2022,,0x95,0x95, //    .*..
  // 0x2116,0xFC,0xB9,0x23, // �   ...*
  // 0x2219,,0xF9,0x9E, //    .*..
  // 0x221A,0xFB,,0x96, // v   ..*.
  // 0x25A0,0xFE,,0x94, // �
  0x0000,0,0,0
};

const char wintranslation[128]=
{
  0x5F,0x5F,0x27,0x5F,0x22,0x3A,0xC5,0xD8,0x5F,0x25,0x5F,0x3C,0x5F,0x5F,0x5F,0x5F,
  0x5F,0x27,0x27,0x22,0x22,0x07,0x2D,0x2D,0x5F,0x54,0x5F,0x3E,0x5F,0x5F,0x5F,0x5F,
  0xFF,0xF6,0xF7,0x5F,0xFD,0x83,0xB3,0x15,0xF0,0x63,0xF2,0x3C,0xBF,0x2D,0x52,0xF4,
  0xF8,0x2B,'I' ,'i' ,0xA3,0xE7,0x14,0xFA,0xF1,0xFC,0xF3,0x3E,0x5F,0x5F,0x5F,0xF5,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
  0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF
};
const unsigned short dos2unicode[128]=
{
  0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,
  0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
  0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,
  0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
  0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,
  0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
  0x002D,0x002D,0x002D,0x00A6,0x002B,0x00A6,0x00A6,0x00AC,
  0x00AC,0x00A6,0x00A6,0x00AC,0x002D,0x002D,0x002D,0x00AC,
  0x004C,0x002B,0x0054,0x002B,0x002D,0x002B,0x00A6,0x00A6,
  0x004C,0x0433,0x00A6,0x0054,0x00A6,0x003D,0x002B,0x00A6,
  0x00A6,0x0054,0x0054,0x004C,0x004C,0x002D,0x0433,0x002B,
  0x002B,0x002D,0x002D,0x002D,0x002D,0x00A6,0x00A6,0x002D,
  0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,
  0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F,
  0x0401,0x0451,0x0404,0x0454,0x0407,0x0457,0x040E,0x045E,
  0x00B0,0x2022,0x00B7,0x0076,0x2116,0x00A4,0x00A6,0x00A0
};

unsigned int char8to16(int c)
{
  if (c>=128)
  {
    //Win->Dos
    c=wintranslation[c-128];
    if (c<128) return(c);
    return(dos2unicode[c-128]);
  }
  return(c);
}

unsigned int char16to8(unsigned int c)
{
  const TUNICODE2CHAR *p=unicode2char;
  unsigned int i;
  if (c<128) return(c);
  while((i=p->u))
  {
    if (c==i)
    {
      return(p->win);
    }
    p++;
  }
  c&=0xFF;
  if (c<32) return(' ');
  return(c);
}

void ascii2ws(WSHDR *ws, const char *s)
{
  char c;
  CutWSTR(ws,0);
  while((c=*s++))
  {
    wsAppendChar(ws,char8to16(c));
  }
}

void edchat_locret(void){}

void ExtractAnswer(WSHDR *ws)
{
  int i=0;
  int c;
  do
  {
    if (i>=ws->wsbody[0]) break;
    c=ws->wsbody[i+1];
    if (c==10) c=13;
    msg_buf[i]=char16to8(c);
    i++;
  }
  while(i<16383);
  msg_buf[i]=0;
  return;
}

int edchat_onkey(GUI *data, GUI_MSG *msg)
{
  //-1 - do redraw
  CLIST *t;
  TPKT *p;
  char *s;
  int l;
  if (msg->keys==0xFFF)
  {
//    void ec_menu(void);
//    ec_menu();
    return(-1);
  }
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    l=msg->gbsmsg->submess;
    if ((l>='0')&&(l<='9'))
    {
      if (EDIT_GetFocus(data)!=edchat_answeritem)
        EDIT_SetFocus(data,edchat_answeritem);
    }
    if (l==GREEN_BUTTON)
    {
      if (connect_state==3)
      {
        if ((t=edcontact))
        {
          if ((s=t->answer))
          {
            if (strlen(s))
            {
              p=malloc(sizeof(PKT)+(l=strlen(s))+1);
              p->pkt.uin=t->uin;
              p->pkt.type=T_SENDMSG;
              p->pkt.data_len=l;
              strcpy(p->data,s);
//              AddStringToLog(t,0x01,p->data,I_str);
              SUBPROC((void *)SendAnswer,0,p);
              mfree(s);
              t->answer=0;
              request_remake_edchat=1;
              EDIT_SetFocus(data,edchat_answeritem);
              CutWSTR(ews,0);
              EDIT_SetTextToFocused(data,ews);
              return(1);
            }
          }
        }
      }
    }
  }
  return(0); //Do standart keys
  //1: close
}

void edchat_ghook(GUI *data, int cmd)
{
  static SOFTKEY_DESC sk={0x0FFF,0x0000,(int)"Menu"};
  //  static SOFTKEY_DESC sk={0x0018,0x0000,(int)"Menu"};
  char *s;
  EDITCONTROL ec;
  if (cmd==3)
  {
    //    EDIT_CURSOR_POS(data)=0x7FFF;
  }
  if (cmd==0x0A)
  {
    DisableIDLETMR();
    if (request_close_edchat)
    {
      request_close_edchat=0;
      GeneralFunc_flag1(edchat_id,1);
    }
  }
  if (cmd==7)
  {
    SetSoftKey(data,&sk,SET_SOFT_KEY_N);
    if (edchat_toitem)
    {
      EDIT_SetFocus(data,edchat_toitem);
      edchat_toitem=0;
    }
    ExtractEditControl(data,edchat_answeritem,&ec);
    ExtractAnswer(ec.pWS);
    if (edcontact)
    {
      if ((s=edcontact->answer)) mfree(s);
      s=malloc(strlen(msg_buf)+1);
      strcpy(s,msg_buf);
      edcontact->answer=s;
    }
  }
}

HEADER_DESC edchat_hdr={0,0,NULL,NULL,NULL,0,LGP_NULL};

INPUTDIA_DESC edchat_desc=
{
  1,
  edchat_onkey,
  edchat_ghook,
  (void *)edchat_locret,
  0,
  &menu_skt,
  {0,NULL,NULL,NULL},
  4,
  100,
  101,
  0,
  //  0x00000001 - ��������� �� ������� ����
  //  0x00000002 - ��������� �� ������
  //  0x00000004 - �������� ���������
  //  0x00000008 - UnderLine
  //  0x00000020 - �� ���������� �����
  //  0x00000200 - bold
  0,
  //  0x00000002 - ReadOnly
  //  0x00000004 - �� ��������� ������
  //  0x40000000 - �������� ������� ����-������
  0x40000000
};
