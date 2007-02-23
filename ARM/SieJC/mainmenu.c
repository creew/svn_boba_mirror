#include "../inc/swilib.h"
#include "main.h"
#include "roster_icons.h"
#include "mainmenu.h"
#include "status_change.h"
//==============================================================================
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
//==============================================================================

#define N_ITEMS 4

int MainMenu_ID;

extern char My_Presence;

void AboutDlg()
{
  ShowMSG(1,(int)"Siemens natJabber Client\n(c)Kibab, Ad");  
};


void Dummy()
{
  ShowMSG(1,(int)"������ � ���������� :)");  
};


// Ad, ���� ������ ������, ������� ������ ������!
int icon[]={0x3DB,0};
int about_icon[]={0x4DB,0};
int dummy_icon[] = {0x50E,0};

HEADER_DESC menuhdr={0,0,131,21,NULL,(int)"����",0x7FFFFFFF};

int mmenusoftkeys[]={0,1,2};

MENUITEM_DESC menuitems[N_ITEMS]=
{
  {dummy_icon,(int)"�������",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {dummy_icon,(int)"������",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {dummy_icon,(int)"�����������",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {about_icon,(int)"�� �����...",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};

void *menuprocs[N_ITEMS]={(void *)Dummy, (void *)DispStatusChangeMenu, (void *)Dummy, (void *) AboutDlg};

SOFTKEY_DESC mmenu_sk[]=
{
  {0x0018,0x0000,(int)"�����"},
  {0x0001,0x0000,(int)"�����"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB mmenu_skt=
{
  mmenu_sk,0
};

MENU_DESC tmenu=
{
  8,NULL,NULL,NULL,
  mmenusoftkeys,
  &mmenu_skt,
  1,
  NULL,
  menuitems,
  menuprocs,
  N_ITEMS
};

int S_ICONS[1];
char mypic[128];
void MM_Show()
{
#ifdef USE_PNG_EXT
  Roster_getIconByStatus(mypic, My_Presence);
  S_ICONS[0]=(int)mypic;
  menuitems[1].icon = S_ICONS;
#else
  menuitems[1].icon = Roster_getIconByStatus(My_Presence);
#endif  
  patch_header(&menuhdr);
  MainMenu_ID = CreateMenu(0,0,&tmenu,&menuhdr,0,N_ITEMS,0,0);
}
