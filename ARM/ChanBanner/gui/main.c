#include "../../inc/swilib.h"

#ifdef NEWSGOLD
#define CBOX_CHECKED 0xE116
#define CBOX_UNCHECKED 0xE117
#else
#define CBOX_CHECKED 0xE10B
#define CBOX_UNCHECKED 0xE10C
#endif

void Killer(void)
{
  extern void *ELF_BEGIN;
  extern void kill_data(void *p, void (*func_p)(void *));
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

const char chanbantab_file[]="4:\\ZBin\\var\\chan_ban.tab";
char ban_tab[972];

char hdrtxt[]="ChanBanner Config";

HEADER_DESC menuhdr={0,0,131,21,NULL,(int)hdrtxt,LGP_NULL};
int menusoftkeys[]={0,1,2};
SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Save"},
  {0x0001,0x0000,(int)"Close"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

void menu_ghook(void *data, int cmd);
int menu_onkey(void *data, GUI_MSG *msg);
void menu_iconhndl(void *data, int curitem, void *unk);

MENU_DESC menu=
{
  8,menu_onkey,menu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  0x10,
  menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};

int create_menu(void)
{
  return(CreateMenu(0,0,&menu,&menuhdr,0,972,0,0));
}

void menu_iconhndl(void *data, int curitem, void *unk)
{
  int i;
  void *item=AllocMenuItem(data);
  WSHDR *ws=AllocMenuWS(data,30);
  i=curitem;
  if (i>124)
  {
    if (i>174)
    {
      i-=175;
      i+=512;
    }
    else
    {
      i-=125;
      i+=974;
    }
  }
  wsprintf(ws,"%d: ",i);
  switch(ban_tab[curitem])
  {
  case 0:
    wstrcatprintf(ws,"standart");
    break;
  case 1:
    wstrcatprintf(ws,"banned");
    break;
  default:
    wstrcatprintf(ws,"??? %d",ban_tab[curitem]);
    break;
  }
  SetMenuItemText(data,item,ws,curitem);
}

void menu_ghook(void *data, int cmd)
{
}

int menu_onkey(void *data, GUI_MSG *msg)
{
  int c;
  int l;
  int pos=GetCurMenuItem(data);
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    c=msg->gbsmsg->submess;
    if ((c>='0')&&(c<='9'))
    {
      int i=pos;
      c-='0';
      if (i>124)
      {
	if (i>174)
	{
	  i-=175;
	  i+=512;
	}
	else
	{
	  i-=125;
	  i+=974;
	}
      }
      i=i*10+c;
      if (i>124)
      {
	if ((i>973)&&(i<1024))
	{
	  i-=973;
	  i+=125;
	}
	else
	  if ((i>511)&&(i<885))
	  {
	    i-=512;
	    i+=175;
	  }
	  else
	    i=0;
      }
      SetCursorToMenuItem(data,i);
      RefreshGUI();
      return(-1);
    }
  }
  if (msg->keys==0x18)
  {
    GeneralFuncF1(1);
    return(-1);
  }
  if (msg->keys==0x3D)
  {
    l=pos;
    c=ban_tab[l];
    if (c<1) c++; else c=0;
    ban_tab[l]=c;
    RefreshGUI();
    return(-1);
  }
  return(0);
}

void LoadConfig(void)
{
  unsigned int ul;
  int f;
  memset(ban_tab,0,sizeof(ban_tab));
  if ((f=fopen(chanbantab_file,A_ReadOnly+A_BIN,P_READ,&ul))==-1) return;
  fread(f,ban_tab,sizeof(ban_tab),&ul);
  fclose(f,&ul);
}

void SaveConfig(void)
{
  unsigned int ul;
  int f;
  if ((f=fopen(chanbantab_file,A_ReadWrite+A_Create+A_Truncate+A_BIN,P_READ+P_WRITE,&ul))!=-1)
  {
    fwrite(f,ban_tab,sizeof(ban_tab),&ul);
    fclose(f,&ul);
  }
  GBS_SendMessage(MMI_CEPID, MSG_RECONFIGURE_REQ,0,chanbantab_file);
}

void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=create_menu();
}

void maincsm_onclose(CSM_RAM *csm)
{
  SUBPROC((void *)Killer);
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      if ((int)msg->data1==1)
	SaveConfig();
      else 
	csm->csm.state=-3;
      csm->gui_id=0;
    }
  }
  if ((msg->msg==MSG_RECONFIGURE_REQ)&&(chanbantab_file==(char *)msg->data0))
  {
    csm->csm.state=-3;
  }
  return(1);
}

unsigned short maincsm_name_body[140];
const int minus11=-11;

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


static void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"ChanBannerGUI");
}

int main(const char *elf_name, const char *fname)
{
  MAIN_CSM main_csm;
  LoadConfig();
  UpdateCSMname();
  LockSched();
  /*maincsm_id=*/CreateCSM(&MAINCSM.maincsm,&main_csm,0);
  UnlockSched();
  return 0;
}
