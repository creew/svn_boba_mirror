#include "..\inc\swilib.h"
#include "..\inc\gpslogger_ipc.h"
#include "minigps_logger.h"
#include "actions.h"
#include "conf_loader.h"

CSM_DESC icsmd;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

WSHDR *Out_WS;
GBSTMR mytmr;

#define UPDATE_TIME 216
#define EDLEGUI_ID (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])
#define COLOR(x) (char *)(&(x))

// ������ ���������� �� �������
extern const          int Req_Clear_Cache;
extern const unsigned int TXT_X;
extern const unsigned int TXT_Y;
extern const unsigned int TXT_FONT;
extern const          int CENTER_TEXT;
extern const          char TXT_COLOR[];
extern const unsigned int TXT_ATTR;


void TimerProc()
{
    GBS_SendMessage(MMI_CEPID, MSG_IPC, IPC_GPSL_REFRESH);
    GBS_StartTimerProc(&mytmr,UPDATE_TIME,TimerProc);
}

void UpdateSett()
{
  InitConfig();
  if(Req_Clear_Cache)
  {
    KillRamNetCache();
    InitRamNetCache();
  }
}


int Get_String_Width(WSHDR *str)
{
  int i,wlen, pic_len=0;
  unsigned short *wschar;
  unsigned short symb;
  wlen = wstrlen(str);
  for(i=0;i<wlen;i++)
  {
    wschar = str->wsbody + i;
    symb = *wschar;
    pic_len += GetSymbolWidth(symb, TXT_FONT)+1;
  }
  return pic_len;
}


char IPC_me[]="GPSLogger";


// ����������� ������
void RedrawScreen(CSM_RAM* data)
{
  int x_pos;
  
  if(IsGuiOnTop(EDLEGUI_ID)) //���� IdleGui �� ����� �����
  {
    GUI *igui=GetTopGUI();
    if (igui) //� �� ����������
    {
      void *canvasdata;

#ifdef ELKA
      {
#else      
      void *idata;
      idata=GetDataOfItemByID(igui,2);
      if (idata)
      {
#endif
        char *xz;
        char action;
        xz= Get_Current_Location(&action);
        if(!xz)return;
        utf8_2ws(Out_WS, xz, 35);
        mfree(xz);
        int len;
        len = Get_WS_width(Out_WS, TXT_FONT);     //Get_String_Width(Out_WS);
        if(CENTER_TEXT)
        {
          x_pos = sdiv(2, ScreenW() - len);
        }else x_pos = TXT_X;
       
#ifdef ELKA
	canvasdata=BuildCanvas();
#else
        canvasdata=((void **)idata)[DISPLACE_OF_IDLECANVAS/4];
#endif      
        DrawCanvas(canvasdata,x_pos,TXT_Y,x_pos + len ,TXT_Y + GetFontYSIZE(TXT_FONT),1);
        DrawString(Out_WS,x_pos,TXT_Y,x_pos + len+1 ,TXT_Y + GetFontYSIZE(TXT_FONT)+1,TXT_FONT,TXT_ATTR,COLOR(TXT_COLOR),GetPaletteAdrByColorIndex(1));
        if(action)DoAction(action);       
      }
    }
  }  
}


int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  GPSL_IPC_MSG_UPD_TMO *updmsg;

  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (stricmp((char*)successed_config_filename,(char *)msg->data0)==0)
    {
      UpdateSett();
      ShowMSG(1,(int)"GPSLogger config updated!");
    }
  }
  
  if(msg->msg == MSG_IPC) 
  {
    if(msg->submess==IPC_GPSL_REFRESH)
    {
      RedrawScreen(data);
      return old_icsm_onMessage(data,msg);
    }
    IPC_REQ *ipc;
    if(ipc=(IPC_REQ*)msg->data0)
    {
      if(stricmp(ipc->name_to,IPC_me)==0)
      {
        switch(msg->submess)
        {
        case IPC_GPSL_UPD_TMO:
          
          updmsg = (GPSL_IPC_MSG_UPD_TMO*) ipc->data;
          //ShowMSG(1,(int)"TMO update received");
          Del_From_Cache(updmsg->cid, updmsg->lac);
          break;
        }
      }
    }
  }
  return old_icsm_onMessage(data,msg);
}


void MyIDLECSM_onClose(CSM_RAM *data)
{
  KillRamNetCache();
  GBS_DelTimer(&mytmr);
  FreeWS(Out_WS);
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

int main(void)
{
  InitConfig();
  InitRamNetCache();
  Out_WS = AllocWS(256);
  LockSched();
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  
  old_icsm_onMessage=icsmd.onMessage;
  icsmd.onMessage=MyIDLECSM_onMessage;
  
  old_icsm_onClose=icsmd.onClose;
  icsmd.onClose=MyIDLECSM_onClose;
  
  icsm->constr=&icsmd;

  UnlockSched();
  GBS_StartTimerProc(&mytmr,UPDATE_TIME*10,TimerProc);
  return 0;
}
