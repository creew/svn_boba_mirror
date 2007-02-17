#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "history.h"
#include "conf_loader.h"
#include "main.h"
#include "clist_util.h"
#include "xml_parser.h"
#include "jabber.h"
#include "jabber_util.h"

/*
============== �������� Jabber-������ ==============
����������� ������� ����-������ ( :) ):
1. ���� � ����
2. ��������� �� ���� �������-����� :)
3. ��������� ������� ������: ������� iq, ������, ������ �����������
4. ������ ������ ������ �/�� ������� (���� 4:\jlog.txt)
5. ������ ����������� ������ XML-������ (���� 4:\xml_packet.txt)

����������: ���������� � ������� ������ ����� �������������,
��������� ������� ������ - � ���������� ����.

������� �������:
1 - �������� � ������� �����������,
2 - ������� ������� �� ������,
3 - ������� ������ ����������
0 - ����������

��� ����� ������� � ������ �������:
 - ���������� �� ������� �������
 - ���������, ��� ���������� � ���������� �� �� ������ ���������
 - ��������� ����� �������
 - ������ ���������
 - ��������� � ������� ���������
=====================================================
(c) Kibab
(r) Rst7, MasterMind� AD

*/

// ============= ������� ������ ============= 
extern const char JABBER_HOST[];
extern const unsigned int JABBER_PORT;
extern const char USERNAME[];  
extern const char PASSWORD[];   
const char RESOURCE[] = "SieJC";
const char VERSION_NAME[]= "Sie natJabber Client";
const char VERSION_VERS[] = "0.1";

#ifdef NEWSGOLD
const char OS[] = "NewSGOLD_ELF-Platform";
#else
const char OS[] = "SGOLD_ELF-Platform";
#endif


#define TMR_SECOND 216

char Is_Vibra_Enabled = 1;

const char percent_t[]="%t";
const char empty_str[]="";
char logmsg[512];

JABBER_STATE Jabber_state = JS_NOT_CONNECTED;

// ���� ������������� ��������� ������
char Quit_Required = 0;

int connect_state=0;
/*
0 = OffLine
1 = Socket Connected
2 = ���������� ����� ������, ������� ����� �� �������
*/

int sock=-1;

volatile int is_gprs_online=1;

GBSTMR reconnect_tmr;

extern void kill_data(void *p, void (*func_p)(void *));

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}
//===============================================================================================
// ELKA Compatibility

#pragma inline
void patch_rect(RECT*rc,int x,int y, int x2, int y2)
{
  rc->x=x;
  rc->y=y;
  rc->x2=x2;
  rc->y2=y2;
}


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

GBSTMR tmr_vibra;
volatile int vibra_count;

char My_JID[128];
char My_JID_full[128];


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
  int err=async_gethostbyname(JABBER_HOST,&p_res,&DNR_ID); //03461351 3<70<19<81
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
	sa.port=htons(JABBER_PORT);
	sa.ip=p_res[3][0][0];
	//    sa.ip=htonl(IP_ADDR(82,207,89,182));
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

void end_socket(void)
{
  if (sock>=0)
  {
    shutdown(sock,2);
    closesocket(sock);
  }
}

char* XMLBuffer;
unsigned int XMLBufferCurPos = 0; // ������� � ��������� ������
unsigned int virt_buffer_len = 0; // ����������� ����� ��������� ������
unsigned int processed_pos   = 0; // �� ������ ����� ����������



void get_buf_part(char* inp_buffer, unsigned int req_len)
{
  // ���������, ��� � ������ ��������� ������� ������� ������������ ������
  int xz = sdiv(XML_BUFFER_SIZE,processed_pos);
  unsigned int virt_processed_len = processed_pos - XML_BUFFER_SIZE * xz;

  if(req_len+virt_processed_len < XML_BUFFER_SIZE)
  {
    memcpy(inp_buffer, XMLBuffer+virt_processed_len, req_len);
  }
  else
  {
    memcpy(inp_buffer, XMLBuffer+virt_processed_len, (XML_BUFFER_SIZE-virt_processed_len));
    memcpy(inp_buffer + (XML_BUFFER_SIZE-virt_processed_len), XMLBuffer, req_len - (XML_BUFFER_SIZE-virt_processed_len));
  }
}

/*
����� ������ �� ������ � ��������� HELPER
�� ��������� ����� ����������� ������ ������
��� ������������ � MMI ��� �������� :)
*/
void get_answer(void)
{
  char* buf = malloc(REC_BUFFER_SIZE);    // �������� ����� ��� ����
  zeromem(buf,REC_BUFFER_SIZE);           // ��������
  int rec_bytes = 0;          // �� ��������� :)
  rec_bytes = recv(sock, buf, REC_BUFFER_SIZE, 0);
#ifdef LOG_ALL  
  char mess[15];
  sprintf(mess,"RECV:%d",rec_bytes);
  Log(mess, buf);
#endif
  
  // ������ � �����
  if(XMLBufferCurPos+rec_bytes < XML_BUFFER_SIZE) // ���� ����� ���-�� � ������
  {
    memcpy(XMLBuffer+XMLBufferCurPos, buf, rec_bytes);
    XMLBufferCurPos+=rec_bytes;
  }
  else // ������ ��������� ������� ������
  {
    // ���������� ����, �� �������� ���������� ������ ����� ���������� ��� ��������
    // � XML-�����
    unsigned int max_byte = (XML_BUFFER_SIZE-XMLBufferCurPos);
    memcpy(XMLBuffer+XMLBufferCurPos, buf, max_byte);
    memcpy(XMLBuffer, buf+max_byte, rec_bytes - max_byte);
    XMLBufferCurPos=rec_bytes - max_byte;
  }
  
  virt_buffer_len = virt_buffer_len + rec_bytes;  // ����������� ����� ������ �����������
  
  char lastchar = *(buf + rec_bytes - 1);
  
  if((rec_bytes<REC_BUFFER_SIZE)&&(lastchar=='>'))   // ������� ������ ������� ������ ����� - ��������� ����� ��������
  {
    int bytecount = virt_buffer_len - processed_pos;
    
    // ���� ���������� � MMI!
    IPC_BUFFER* tmp_buffer = malloc(sizeof(IPC_BUFFER)); // ���� ���������
    
    tmp_buffer->xml_buffer = malloc(bytecount);          // ����� � ���������
    get_buf_part(tmp_buffer->xml_buffer, bytecount);

    // ������� ��������������� UTF8->ANSI
    char* conv_buf=malloc(bytecount);
    int conv_size;
    conv_buf = convUTF8_to_ANSI(conv_buf,tmp_buffer->xml_buffer, bytecount, &conv_size);
    mfree(tmp_buffer->xml_buffer);
    tmp_buffer->buf_size = conv_size;
    tmp_buffer->xml_buffer = conv_buf;

    processed_pos = virt_buffer_len;

    // �������� � MMI ��������� � �������
    GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,tmp_buffer,sock);
  }  
  mfree(buf);
}

void SendAnswer(char *str)
{
  int i = strlen(str);
  send(sock,str,i,0);
#ifdef LOG_ALL
  Log("SEND",str);
#endif
}


void Process_Decoded_XML(XMLNode* node)
{
  XMLNode* nodeEx = node;
  while(nodeEx)
  {
//----------------
    if(!strcmp(nodeEx->name,"message"))
    {
      Process_Incoming_Message(nodeEx);
    }
//----------------
//----------------
    if(!strcmp(nodeEx->name,"iq"))
    {
      Process_Iq_Request(nodeEx);
    }

    if(!strcmp(nodeEx->name,"stream:stream"))
    {
      connect_state = 2;
      Jabber_state = JS_CONNECTED_STATE;
      SUBPROC((void*)Send_Auth);
    }   
//----------------

//----------------
    if(!strcmp(nodeEx->name,"presence"))
    {
      Process_Presence_Change(nodeEx);
    }
//----------------
    
    if(nodeEx->subnode) Process_Decoded_XML(nodeEx->subnode);
    nodeEx = nodeEx->next;
  }
}

void __log(char* buffer, int size)
{
  char mess[20];
  sprintf(mess,"RECV:%d",size);
  Log(mess,buffer);
  mfree(buffer);
}

void Process_XML_Packet(IPC_BUFFER* xmlbuf)
{
  // ���� ��������, ���� �� ����������� ������ ��������� �� ������ ������   
  LockSched();
  XMLNode *data=XMLDecode(xmlbuf->xml_buffer,xmlbuf->buf_size);
  UnlockSched();
  if(data)
  {
//#ifdef LOG_ALL
//    SaveTree(data);
//#endif
    Process_Decoded_XML(data);
    DestroyTree(data);
  }
/*  
    char* tmp_buf=malloc(xmlbuf->buf_size+1);
    zeromem(tmp_buf,xmlbuf->buf_size+1);
    memcpy(tmp_buf,xmlbuf->xml_buffer,xmlbuf->buf_size);
    SUBPROC((void*)__log,tmp_buf, xmlbuf->buf_size);
*/
  // ����������� ������ :)
    mfree(xmlbuf->xml_buffer);
    mfree(xmlbuf);    
    REDRAW();
}


//===============================================================================================
// ������ ����� � GUI
void onRedraw(MAIN_GUI *data)
{ 
  int scr_w=ScreenW();
  int scr_h=ScreenH();
  
  int font_color, bgr_color;
  if(connect_state<2)
  {
    font_color=0;
    bgr_color = 20;
  }
  if(connect_state==2)
  {
    font_color=1;
    bgr_color = 13;
  }

  if(Jabber_state==JS_AUTH_ERROR || Jabber_state==JS_ERROR)
  {
    font_color = 7;
    bgr_color  = 2;
  }  
  DrawRoundedFrame(0,0,scr_w-1,scr_h-1,0,0,0,
		   GetPaletteAdrByColorIndex(0),
		   GetPaletteAdrByColorIndex(bgr_color));
  
  CList_RedrawCList();
  
  LockSched();
  wsprintf(data->ws1,"CS: %d RECV: %d\n%t",connect_state,virt_buffer_len,logmsg);
  UnlockSched();
  DrawString(data->ws1,3,3,scr_w-4,scr_h-4-16,SMALL_FONT,0,GetPaletteAdrByColorIndex(font_color),GetPaletteAdrByColorIndex(23));
  DrawString(data->ws2,3,13,scr_w-4,scr_h-4-16,SMALL_FONT,0,GetPaletteAdrByColorIndex(font_color),GetPaletteAdrByColorIndex(23));
}

void onCreate(MAIN_GUI *data, void *(*malloc_adr)(int))
{
  data->ws1=AllocWS(256);
  data->ws2=AllocWS(256);
  data->gui.state=1;
}

void onClose(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  FreeWS(data->ws1);
  FreeWS(data->ws2);
  data->gui.state=0;
}

void onFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  DisableIDLETMR();
  data->gui.state=2;
}

void onUnfocus(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}

void Test_UTF()
{
}

void QuitCallbackProc(int decision)
{
  if(!decision)Quit_Required = 1;
}

void DisplayQuitQuery()
{
  ShowDialog_YesNo(1,(int)"�������� SieJC?",QuitCallbackProc);  
}

int onKey(MAIN_GUI *data, GUI_MSG *msg)
{
  if(Quit_Required)return 1; //���������� ����� GeneralFunc ��� ���. GUI -> �������� GUI

  //DirectRedrawGUI();
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    switch(msg->gbsmsg->submess)
    {
    case LEFT_SOFT:
      //      if (cltop) remake_clmenu();
      break;
    case RIGHT_SOFT:
      DisplayQuitQuery();
      //return(1);
    case GREEN_BUTTON:
      if ((connect_state==0)&&(sock==-1))
      {
        GBS_DelTimer(&reconnect_tmr);
	DNR_TRIES=3;
        SUBPROC((void *)create_connect);
      }
      break;
    case '3':
      {
        SUBPROC((void*)Send_Presence);
        break;
      }      
    case '4':
      {
        CList_RedrawCList();
        break;
      }

    case '5':
      {
        Test_UTF();
      }
    
    case DOWN_BUTTON:
    case '8':
      {
        CList_MoveCursorDown();
        break;
      }

    case UP_BUTTON:
    case '2':
      {
        CList_MoveCursorUp();
        break;
      }

      
    case '0':
      {
        SUBPROC((void*)Send_Disconnect);
        break;
      }        
      
    case '*':
      {
        Is_Vibra_Enabled = !(Is_Vibra_Enabled);
        break;
      }
    case '9':
      {
        REDRAW();
        break;
      }    
    }    
  }
  //  onRedraw(data);
  return(0);
}

int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)onRedraw,	//Redraw
  (void *)onCreate,	//Create
  (void *)onClose,	//Close
  (void *)onFocus,	//Focus
  (void *)onUnfocus,	//Unfocus
  (void *)onKey,	//OnKey
  0,
  (void *)kill_data, //onDestroy,	//Destroy
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
  DNR_TRIES=3;
  XMLBuffer = malloc(XML_BUFFER_SIZE);
  zeromem(XMLBuffer, XML_BUFFER_SIZE);
 
  SUBPROC((void *)create_connect);
  
  // ��������� ������ ��������� ��������, �� ������,
  // ���� ������ ����� ������ - ��� ����� ���� ������...
  void* Process_XML_Packet_ADR = (void*)Process_XML_Packet;
  void* Process_Decoded_XML_ADR = (void*) Process_Decoded_XML;
  char msg[80];
  sprintf(msg,"@Process_XML_Packet=0x%X, @Process_Decoded_XML=0x%X\r\n",Process_XML_Packet_ADR, Process_Decoded_XML_ADR);
  Log("SYSTEM", msg);
}

void maincsm_onclose(CSM_RAM *csm)
{
  GBS_DelTimer(&tmr_vibra);
  GBS_DelTimer(&reconnect_tmr);
  SetVibration(0);
  CList_Destroy();
  mfree(XMLBuffer);

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
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if(Quit_Required)
  {
    csm->csm.state=-3;
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
        //����� XML-������ ����� � ��������� � ��������� �� ��������� � ��������� MMI
        Process_XML_Packet((IPC_BUFFER*)msg->data0);
        return(0);
      }
      switch((int)msg->data0)
      {
      case ENIP_SOCK_CONNECTED:
        if (connect_state==1)
        {
          //���������� �����������, �������� ����� Welcome
          SUBPROC((void*)Send_Welcome_Packet);
          REDRAW();
        }
        else
        {
          ShowMSG(1,(int)"Illegal message ENIP_SOCK_CONNECTED!");
        }
        break;
      case ENIP_SOCK_DATA_READ:
        if (connect_state>=1)
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
        Jabber_state = JS_NOT_CONNECTED;
        sock=-1;
        vibra_count=4;
        start_vibra();
        REDRAW();
        //GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
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
  
  InitConfig();
  if(!strlen(USERNAME))
  {
    ShowMSG(1,(int)"������� �����/������!");
    ShowMSG(1,(int)USERNAME);
    return 0;
  }
  UpdateCSMname();
  LockSched();
  CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
  return 0;
}
