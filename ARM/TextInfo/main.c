#include "..\inc\swilib.h"
#include "conf_loader.h"
#include "TextInfo.h"
#include "local_ipc.h"

#define TMR_SECOND 216
extern void kill_data(void *p, void (*func_p)(void *));

const char ipc_my_name[]=IPC_TEXTINFO_NAME;

CSM_DESC icsmd;

WSHDR *ws;

typedef struct{
  int enabled;
  RECT rc;
  WSHDR *ws;
  char pen[4];
  unsigned short font;
  unsigned short type;
} TInfo;

TInfo InfoData[11];
 
GBSTMR mytmr;
const IPC_REQ my_ipc={
  ipc_my_name,
  ipc_my_name,
  NULL
};


// ----------------------------------------------------------------------------
#pragma segment="ELFBEGIN"
void ElfKiller(void)
{
  kill_data(__segment_begin("ELFBEGIN"),(void (*)(void *))mfree_adr());
}

void InitInfoData(void);
void TimerProc(void)
{
  InitInfoData();
  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_UPDATE_STAT,&my_ipc);
}

void FillInfoData(TInfo *Info,int x_start,int y_start, int font,const char *color)
{  
  Info->rc.x=x_start;
  Info->rc.y=y_start;
  Info->rc.x2=x_start+Get_WS_width(Info->ws,font);
  Info->rc.y2=y_start+GetFontYSIZE(font);
  Info->font=font;
  memcpy(Info->pen,color,4);
}
  
int wsprintf_bytes(WSHDR *ws, unsigned int bytes)
{
  	//char *str;
	if (bytes<=1024)
		return (wsprintf(ws,BYTES_FMT,bytes,BYTES_SG));
	 //str=BYTES_SG;
	else 
	{
		bytes>>=10;
		return (wsprintf(ws,BYTES_FMT,bytes,KBYTES_SG));
		//str=KBYTES_SG;
	}  
}

void InitInfoData(void)
{
  RAMNET *net_data;
  int c;
  int Free;
  int Total;
  unsigned int err;
  
  if(NET_ENA)
  {
    InfoData[0].enabled=1;
    net_data=RamNet();
    c=(net_data->ch_number>=255)?'=':'-';
    wsprintf(InfoData[0].ws,NET_FMT,c,net_data->power);
    FillInfoData(&InfoData[0],NET_X,NET_Y,NET_FONT,NET_COLORS);
  }
  else
  {
    InfoData[0].enabled=0;
  }
  
  if (TEMP_ENA)
  {
    InfoData[1].enabled=1;
    c=GetAkku(1,3)-0xAAA+15;
    wsprintf(InfoData[1].ws,TEMP_FMT,c/10,c%10);
    FillInfoData(&InfoData[1],TEMP_X,TEMP_Y,TEMP_FONT,TEMP_COLORS);
  }
  else
  {
    InfoData[1].enabled=0;
  }
  
  if (VOLT_ENA)
  {
    InfoData[2].enabled=1;
    c=GetAkku(0,9);
    wsprintf(InfoData[2].ws,VOLT_FMT,c/1000,(c%1000)/10);
    FillInfoData(&InfoData[2],VOLT_X,VOLT_Y,VOLT_FONT,VOLT_COLORS);
  }
  else
  {
    InfoData[2].enabled=0;
  }
  
  if (CAP_ENA)
  {
    InfoData[3].enabled=1;
    c=*RamCap();
    wsprintf(InfoData[3].ws,CAP_FMT,c);
    FillInfoData(&InfoData[3],ACCU_X,ACCU_Y,ACCU_FONT,ACCU_COLORS);
  }
  else
  {
    InfoData[3].enabled=0;
  }
  
  if (CPU_ENA)
  {
    InfoData[4].enabled=1;
    c=GetCPULoad();
    wsprintf(InfoData[4].ws,CPU_FMT,c);
    FillInfoData(&InfoData[4],CPU_X,CPU_Y,CPU_FONT,CPU_COLORS);
  }
  else
  {
    InfoData[4].enabled=0;
  }

  if (GPRS_ENA)
  {
    InfoData[5].enabled=1;
    RefreshGPRSTraffic();
    c=*GetGPRSTrafficPointer();
    wsprintf_bytes(InfoData[5].ws,c);
    FillInfoData(&InfoData[5],GPRS_X,GPRS_Y,GPRS_FONT,GPRS_COLORS);
  }
  else
  {
    InfoData[5].enabled=0;
  }
  
  if (RAM_ENA)
  {
    InfoData[6].enabled=1;
    c=GetFreeRamAvail();
    wsprintf_bytes(InfoData[6].ws,c);
    FillInfoData(&InfoData[6],RAM_X,RAM_Y,RAM_FONT,RAM_COLORS);  
  }
  else
  {
    InfoData[6].enabled=0;
  }
  if (FLEX0_ENA)
  {
    InfoData[7].enabled=1;
    c=GetFreeFlexSpace(0,&err) / 1024;
    if (!cfgMB0) c=c;
    else
      c=c/1024;
    wsprintf(InfoData[7].ws,FLEX0_FMT,c);
    FillInfoData(&InfoData[7],FLEX0_X,FLEX0_Y,FLEX0_FONT,FLEX0_COLORS);
  }
  else
  {
    InfoData[7].enabled=0;
  }
  if (FLEX4_ENA)
  {
    InfoData[8].enabled=1;
    c=GetFreeFlexSpace(4,&err) / 1024;
    if (!cfgMB4) c=c;
      else
        c=c/1024;
    wsprintf(InfoData[8].ws,FLEX4_FMT,c);
    FillInfoData(&InfoData[8],FLEX4_X,FLEX4_Y,FLEX4_FONT,FLEX4_COLORS);  
  }
  else
  {
    InfoData[8].enabled=0;
  }
  if (PER0_ENA)
  {
    InfoData[9].enabled=1;
    Free=GetFreeFlexSpace(0,&err);
    Total=GetTotalFlexSpace(0,&err);
    c=(long long)Free*100/Total;
    wsprintf(InfoData[9].ws,PER0_FMT,c);
    FillInfoData(&InfoData[9],PER0_X,PER0_Y,PER0_FONT,PER0_COLORS);
  }
  else
  {
    InfoData[9].enabled=0;
  }
  if (PER4_ENA)
  {
    InfoData[10].enabled=1;
    Free=GetFreeFlexSpace(4,&err);
    Total=GetTotalFlexSpace(4,&err);
    c=(long long)Free*100/Total;
    wsprintf(InfoData[10].ws,PER4_FMT,c);
    FillInfoData(&InfoData[10],PER4_X,PER4_Y,PER4_FONT,PER4_COLORS);
  }
  else
  {
    InfoData[10].enabled=0;
  }
}

// ----------------------------------------------------------------------------
#define idlegui_id (((int *)FindCSMbyID(CSM_root()->idle_id))[DISPLACE_OF_IDLEGUI_ID/4])

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

int maincsm_onmessage(CSM_RAM* data,GBS_MSG* msg)
{
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"TextInfo config updated!");
      InitConfig();
    }
  }
  if (msg->msg==MSG_IPC)
  {
    IPC_REQ *ipc;
    if ((ipc=(IPC_REQ*)msg->data0))
    {
      if (strcmp_nocase(ipc->name_to,ipc_my_name)==0)
      {
        switch (msg->submess)
        {
        case IPC_UPDATE_STAT:
#ifdef NEWSGOLD
          if (!getCpuUsedTime_if_ena())
          {
            StartCpuUsageCount();
          }
#endif
          GBS_StartTimerProc(&mytmr, REFRESH*TMR_SECOND/10, TimerProc);
        }
      }
    }
  }
  if (IsGuiOnTop(idlegui_id)) //���� IdleGui �� ����� �����
  {
    GUI *igui=GetTopGUI();
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
        for (int i=0; i!=11; i++)
        {
          if (!InfoData[i].enabled)
			  continue;
          DrawCanvas(canvasdata, InfoData[i].rc.x, InfoData[i].rc.y, InfoData[i].rc.x2, InfoData[i].rc.y2, 1);
          DrawString(InfoData[i].ws, InfoData[i].rc.x, InfoData[i].rc.y, InfoData[i].rc.x2, InfoData[i].rc.y2, InfoData[i].font,
                     0,InfoData[i].pen, GetPaletteAdrByColorIndex(23));         
        }            
      }
    }
  }
  return(1);
}


static void maincsm_oncreate(CSM_RAM *data)
{
  for (int i=0;i<11; i++)
  {
    InfoData[i].ws=AllocWS(20);
  }  
  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_UPDATE_STAT,&my_ipc);
}

static void maincsm_onclose(CSM_RAM *csm)
{
  GBS_DelTimer(&mytmr);
  for (int i=0;i!=11; i++)
  {
    FreeWS(InfoData[i].ws);
  }  
  SUBPROC((void *)ElfKiller);
}

static unsigned short maincsm_name_body[140];
const int minus11=-11;

static const struct
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
  sizeof(CSM_RAM),
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


static void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"TextInfo");
}
// ----------------------------------------------------------------------------

int main(void)
{
  CSM_RAM *save_cmpc;
  CSM_RAM main_csm;
  InitConfig();
  UpdateCSMname();
  LockSched();
  save_cmpc=CSM_root()->csm_q->current_msg_processing_csm;
  CSM_root()->csm_q->current_msg_processing_csm=CSM_root()->csm_q->csm.first;
  CreateCSM(&MAINCSM.maincsm,&main_csm,0);
  CSM_root()->csm_q->current_msg_processing_csm=save_cmpc;
  UnlockSched();
  return (0);
}
