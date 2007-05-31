#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"
#include "TalkPhone.h"

CSM_DESC icsmd;
int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

unsigned int tmp;

char pic_path[];
unsigned int vibra_count1;

TTime CurTime;
TDate date;


void Check();

GBSTMR UPDATE_TMR;

//=============================������������ �����===============================
int PLAY_ID;
unsigned int NEXT_PLAY_FUNK=0;
/*
0 - ������
//1 - SayTime
1 - SayHourDig
2 - SayHourVoice
3 - SayMinAllDig
4 - SayMinDig
5 - SayMinVoice
*/

void Play(const char *fpath, const char *fname)
{
      WSHDR* sndPath=AllocWS(128);
      WSHDR* sndFName=AllocWS(128);

      wsprintf(sndPath, fpath);
      wsprintf(sndFName, fname);
  

      PLAYFILE_OPT _sfo1;
      zeromem(&_sfo1,sizeof(PLAYFILE_OPT));
      _sfo1.repeat_num=1;
      _sfo1.time_between_play=0;
      _sfo1.play_first=0;
      _sfo1.volume=volume;
    
      #ifdef NEWSGOLD
        _sfo1.unk6=1;
        _sfo1.unk7=1;
        _sfo1.unk9=2;
        PLAY_ID=PlayFile(0x10, sndPath, sndFName, MMI_CEPID, MSG_PLAYFILE_REPORT, &_sfo1);
      #else
          #ifdef X75
            _sfo1.unk4=0x80000000;
            _sfo1.unk5=1;
            PLAY_ID=PlayFile(0xC, sndPath, sndFName, 0, MMI_CEPID, MSG_PLAYFILE_REPORT, &_sfo1);
          #else
            _sfo1.unk5=1;
            PLAY_ID=PlayFile(0xC, sndPath, sndFName, MMI_CEPID, MSG_PLAYFILE_REPORT, &_sfo1);
          #endif
      #endif 

      FreeWS(sndPath);
      FreeWS(sndFName);
}

//============================================================================== 
void RereadSettings()
{
   LockSched();
    InitConfig();  
    strcpy(pic_path, folder_path);
    strcat(pic_path, "TalkPhone.png");
   UnlockSched();   
}

//++++++++++++++++++++++++++++++�������������� �������+++++++++++++++++++++++++++++++++

//-----------------------------------------------------------
GBSTMR tmr_vibra;

void start_vibra(void)
{
  void stop_vibra(void);
  if ((Is_Vibra_Enabled)&&(!IsCalling()))
  {
    SetVibration(vibra_power);
    GBS_StartTimerProc(&tmr_vibra,216>>1,stop_vibra);
  }  
}

void stop_vibra(void)
{
  SetVibration(0);
  if (--vibra_count1)
  {
    GBS_StartTimerProc(&tmr_vibra,216>>1,start_vibra);
  }
}
//----------------------------------------------------------


void SayMinVoice()
{
    if (CurTime.min>=10 && CurTime.min<=19) Play(folder_path, "min3.wav");
      else
        {
          switch (CurTime.min%10)
            {
              case 1:
                Play(folder_path, "min2.wav");
              break;
              case 2: case 3: case 4:
                Play(folder_path, "min1.wav");
              break;
              case 0: case 5: case 6: case 7: case 8: case 9: 
                Play(folder_path, "min3.wav");
              break;  
            }
        }
    NEXT_PLAY_FUNK=0;
}

//-------------------------------- ������ -------------------------------------
void SayMinDig()
{
    if (CurTime.min!=0)
      {
        if (CurTime.min%10==1) 
          {
            Play(folder_path, "1_2.wav");
            NEXT_PLAY_FUNK=5;
          }
          else
        if (CurTime.min%10==2) 
          {
            Play(folder_path, "2_1.wav");          
            NEXT_PLAY_FUNK=5;
          }
          else
            {
              char s[5];
              sprintf(s, "%d.wav", CurTime.min%10);            
              Play(folder_path, s);  
              NEXT_PLAY_FUNK=5;
            }
      }
    else
      {
        Play(folder_path, "xxx.wav");
        NEXT_PLAY_FUNK=0;
      }
}

//////////////////////////////////////////////////////////////////////////////
void SayMinAllDig()
{
      switch (CurTime.min)
        {
          case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
            SayMinDig();
          break;
          
          case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19:
          case 20: case 30: case 40: case 50:   
            { 
              char s[6];
              sprintf(s, "%d.wav", CurTime.min);            
              Play(folder_path, s);
              NEXT_PLAY_FUNK=5;
            }
          break;  
          
          case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29:
            Play(folder_path, "20.wav");
            NEXT_PLAY_FUNK=4;
          break;

          case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39:
            Play(folder_path, "30.wav");
            NEXT_PLAY_FUNK=4;
          break;

          case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: 
            Play(folder_path, "40.wav");
            NEXT_PLAY_FUNK=4;
          break;

          case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59: 
            Play(folder_path, "50.wav");
            NEXT_PLAY_FUNK=4;
          break;
        }      
}
//------------------------------ /*������*/ -----------------------------------

void SayHourVoice()
{
        switch (CurTime.hour)
          {
            case 1: case 21:
              Play(folder_path, "hour2.wav");
              NEXT_PLAY_FUNK=3;
            break;
          
            case 2: case 3: case 4: case 22: case 23:
              Play(folder_path, "hour3.wav");
              NEXT_PLAY_FUNK=3;
            break;
          
            case 0: case 5: case 6: case 7: case 8: case 9: case 10: 
            case 11: case 12: case 13: case 14: case 15: case 16: 
            case 17: case 18: case 19: case 20:
              Play(folder_path, "hour1.wav");
              NEXT_PLAY_FUNK=3;
            break;  
          }    
}

void SayHourDig()
{
      if (CurTime.hour==1) 
        {
          Play(folder_path, "1_1.wav");
          NEXT_PLAY_FUNK=2;
        }
        else
      if (CurTime.hour==2) 
        {
          Play(folder_path, "2_2.wav");          
          NEXT_PLAY_FUNK=2;
        }
        else
          {
            char s[6];
            sprintf(s, "%d.wav", CurTime.hour);            
            Play(folder_path, s);
            NEXT_PLAY_FUNK=2;
          }
}

int IsMediaActive(void)
{
  char s[40];
  sprintf(s,RamMediaIsPlaying());
#ifdef NEWSGOLD
  if (s[0]==1) return 1;
#else
  if (s[0]==2) return 1;// ��� SGOLD s[0]!=2    
#endif 
  return 0;
}

void SayTime(void)
{
  start_vibra();
  vibra_count1=vibra_count;
  
  if (!IsMediaActive()) Play(folder_path, "x.wav"); 
  
  if (PLAY_PARAM) NEXT_PLAY_FUNK=1;
    else
      NEXT_PLAY_FUNK=0;
}

//++++++++++++++++++++++++++++++/*�������������� �������*/+++++++++++++++++++++++++++++++++

void Check(void)
{
  GetDateTime(&date,&CurTime);
  
   if ((on_off==1)&&
       ((CurTime.hour*60+CurTime.min)<=(TimeTo.hour*60+TimeTo.min))&&
         ((CurTime.hour*60+CurTime.min)>=(TimeFrom.hour*60+TimeFrom.min))&&
           (CurTime.min==begin_minute)&&(!IsCalling())) 
    {
      if ((keypad_lock==1)&&(!IsUnlocked())) // ������ ��� ��������.
        {
          SUBPROC((void*)SayTime);
        }
      
      if (keypad_lock!=1) // �� ����� ��������
        {
          SUBPROC((void*)SayTime);
        }
      
      GBS_StartTimerProc(&UPDATE_TMR,262*60,Check);
    }
      else
        {
          tmp=CurTime.min+1;
          if (tmp==60) tmp=0;
          if (tmp==0)
            {
             GBS_StartTimerProc(&UPDATE_TMR,216*1,Check);
            }    
              else
                {
                   GBS_StartTimerProc(&UPDATE_TMR,216*30,Check);
                }   
        }  
  

} //end Check

#pragma inline=forced
int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}
#pragma inline
int strcmp_nocase(const char *s1,const char *s2)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))) if (!c) break;
  return(i);
}

#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])
int MyIDLECSM_onMessage(CSM_RAM* data, GBS_MSG* msg)
{
  int csm_result;

  csm_result=old_icsm_onMessage(data, msg);     

  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"TalkPhone config updated!");
      RereadSettings();
    }
  }
  
  if (msg->msg==MSG_PLAYFILE_REPORT)
    {
      if ((msg->submess>>16)==PLAY_ID) 
      { 
        if (((msg->submess&0xFFFF)==7)||((msg->submess&0xFFFF)==5))
          {
            switch (NEXT_PLAY_FUNK)
              {
                case 0:
                  PLAY_ID=0;
                break;
  
                case 1:
                  SayHourDig();
                break;

                case 2:
                  SayHourVoice();
                break;

                case 3:
                  SayMinAllDig();
                break;

                case 4:
                  SayMinDig();
                break;
                
                case 5:
                  SayMinVoice();
                break;
              }
          }
      }
        else
          PLAY_ID=0;
    }  
  
  if ((IsGuiOnTop(idlegui_id))&&(show_icon)) //���� IdleGui �� ����� �����
    {
      GUI *igui=GetTopGUI();
      if (igui) //� �� ����������
      {
#ifdef ELKA

	void *canvasdata = BuildCanvas();
#else
	void *idata = GetDataOfItemByID(igui, 2);
	if (idata)
	{
	  void *canvasdata = ((void **)idata)[DISPLACE_OF_IDLECANVAS / 4];
#endif
            FSTATS fstats;
            unsigned int err;
            if (GetFileStats(pic_path,&fstats,&err)!=-1)
              {
                DrawCanvas(canvasdata, cfgX, cfgY, cfgX + GetImgWidth((int)pic_path), cfgY + GetImgHeight((int)pic_path), 1);
	        DrawImg(cfgX, cfgY, (int)pic_path);
              }
#ifdef ELKA
#else
	}
#endif
      }
    }
  
  
  return(csm_result);
}

int my_keyhook(int submsg, int msg)
{
  if (msg==LONG_PRESS)
    {
      /*if ((submsg>='0')&&(submsg<='9')&&(!IsUnlocked()))
        {  
          GetDateTime(&date,&CurTime);
          SayTime();
        }*/
      
      if ((submsg==CALL_BUTTON)&&(!IsUnlocked()))
        {  
          GetDateTime(&date,&CurTime);
          SayTime();
        }
    }
  
  return 0;
}

void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  GBS_DelTimer(&UPDATE_TMR);
  GBS_DelTimer(&tmr_vibra);
  RemoveKeybMsgHook((void *)my_keyhook);
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

int main(void)
{
  RereadSettings();
  
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  icsmd.onMessage=MyIDLECSM_onMessage;
  old_icsm_onClose=icsmd.onClose;
  icsmd.onClose=MyIDLECSM_onClose;  
  icsm->constr=&icsmd;  
  
  #ifdef NEWSGOLD
    LockSched();
      AddKeybMsgHook((void *)my_keyhook);
    UnlockSched();  
  #else
    LockSched();
      if (!AddKeybMsgHook_end((void *)my_keyhook)) ShowMSG(1, (int) "TalkPhone. ���������� ���������������� ����������!"); 
    UnlockSched();  
  #endif
  
  Check();
  
  return 0;
}
