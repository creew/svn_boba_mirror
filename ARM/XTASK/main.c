#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

//#define USE_ONE_KEY

CSM_DESC icsmd;
int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

WSHDR *ws_nogui;

CSM_RAM *under_idle;

extern const int RED_BUT_MODE;
extern const int ENA_LONG_PRESS;
extern const int ENA_LOCK;

extern void kill_data(void *p, void (*func_p)(void *));

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  FreeWS(ws_nogui);
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

extern void do_gui(int);

// -1 - XTask GUI present
// 0 - XTask GUI absent
// 1 - IBUT longpressed, ready for exit
int mode;

// 0 - no press
// 1 - long press REDBUT

int mode_red;

int my_keyhook(int submsg, int msg)
{
#ifdef NEWSGOLD
  void *icsm=FindCSMbyID(CSM_root()->idle_id);
  if ((submsg==RED_BUTTON)&&(RED_BUT_MODE))
  {
    if (IsGuiOnTop(((int *)icsm)[DISPLACE_OF_IDLEGUI_ID/4]))
    {
      if (msg==KEY_UP)
      {
	if (mode_red)
	{
	  mode_red=0;
	  return(2);
	}
      }
      mode_red=0;
    }
    else
    {
      if (msg==KEY_DOWN)
      {
	if (mode_red==1)
	{
	  mode_red=0;
	  return(0); //Long press, continue with REDB PRESS
	}
      }
      if (msg==KEY_UP)
      {
	if (mode_red==1)
	{
	  mode_red=0; //Release after longpress
	  return(0);
	}
	//Release after short press
	if (mode==0)
	{
          if (RED_BUT_MODE==1)
          {
            GBS_SendMessage(MMI_CEPID,KEY_DOWN,RIGHT_SOFT);
          }
          else
          {
            do_gui(1);
          }
	}
      }
      if (msg==LONG_PRESS)
      {
	mode_red=1;
	GBS_SendMessage(MMI_CEPID,KEY_DOWN,RED_BUTTON);
      }
      return(2);
    }
  }
#endif
  
#ifdef USE_ONE_KEY
#ifdef ELKA
  if (submsg!=POC_BUTTON) return(0);
#else
  if (submsg!=INTERNET_BUTTON) return(0);
#endif
  if (mode==-1)
  {
    if (msg==KEY_UP)
    {
      GBS_SendMessage(MMI_CEPID,KEY_DOWN,ENTER_BUTTON);
    }
    return(2);
  }
  switch(msg)
  {
  case KEY_DOWN:
    break;
  case KEY_UP:
    if (mode==1)
    {
      mode=0;
      //RemoveKeybMsgHook((void *)my_keyhook);
      //ShowMSG(1,(int)"XTask �������!");
      //SUBPROC((void *)ElfKiller);
      break;
    }
    {
      if (IsUnlocked()||ENA_LOCK)
      do_gui(0);
      else mode=0;
    }
    break;
  case LONG_PRESS:
    if (ENA_LONG_PRESS) mode=1;
  }
  return(2);
#else
  if (mode==-1) return (0);
  {
    if (msg==KEY_UP)
    {
      mode=0;
      return (0);
    }
    if (msg==KEY_DOWN)
    {
      switch (submsg)
      {
      case '*':
        mode=1;
        return (0);
        
      case '#':
        if (mode==1)
        {
          if (IsUnlocked()||ENA_LOCK)
            do_gui(0);
          else mode=0;
        }
        else return (0);
      }
    }
  }
  return (0);
#endif
}

volatile int callhide_mode=0;

#pragma inline=forced
int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}

int strcmp_nocase(const char *s1,const char *s2)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))) if (!c) break;
  return(i);
}

int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  int csm_result;
  int icgui_id;
  csm_result = old_icsm_onMessage(data, msg); //�������� ������ ���������� �������
  icgui_id=((int *)data)[DISPLACE_OF_INCOMMINGGUI/4];
  if (!icgui_id) callhide_mode=0;
  
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"XTask config updated!");
      InitConfig();
    }
  }
  if (msg->msg==MSG_INCOMMING_CALL)
  {
    callhide_mode=1;
  }
  if (callhide_mode)
  {
    if (IsGuiOnTop(icgui_id))
    {
      extern void RotateCSMs(CSM_RAM *ucsm, void *selcsm);
      RotateCSMs(CSM_root()->csm_q->csm.last,FindCSMbyID(CSM_root()->idle_id));
      callhide_mode=0;
    }
  }
  return csm_result;  
}

void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  FreeWS(ws_nogui);
  RemoveKeybMsgHook((void *)my_keyhook);
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

void main(void)
{
  mode=0;
  InitConfig();
  LockSched();
  if (!AddKeybMsgHook_end((void *)my_keyhook))
  {
    ShowMSG(1,(int)"���������� ���������������� ����������!");
    SUBPROC((void *)ElfKiller);
  }
  else
  {
    extern const int ENA_HELLO_MSG;
    if (ENA_HELLO_MSG) ShowMSG(1,(int)"XTask ����������!");
    {
      CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
      memcpy(&icsmd,icsm->constr,sizeof(icsmd));
      old_icsm_onClose=icsmd.onClose;
      old_icsm_onMessage=icsmd.onMessage;
      icsmd.onClose=MyIDLECSM_onClose;
      icsmd.onMessage=MyIDLECSM_onMessage;
      icsm->constr=&icsmd;
    }
    ws_nogui=AllocWS(256);
    wsprintf(ws_nogui,"%t","��� GUI!");
    under_idle=(FindCSMbyID(CSM_root()->idle_id))->prev; //���� idle_dialog
  }
  UnlockSched();
}
