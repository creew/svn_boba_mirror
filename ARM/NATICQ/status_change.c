#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "NatICQ.h"
#include "main.h"
#include "status_change.h"

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


extern int CurrentStatus;
extern  int S_ICONS[11];

int StatChange_Menu_ID;

void Change_Status(char status)
{
    CurrentStatus = status;
    TPKT *p;
    p=malloc(sizeof(PKT)+1);
    p->pkt.uin=0;               // ������; ���� ����� ��������������� �� �������
    p->pkt.type=T_MY_STATUS_CH; // ��� ������: ��������� �������
    p->pkt.data_len=1;          // ����� ������: 1 ����
    memcpy(p->data, &status, 1);
/* 
    // ���� ������ �� �������� �� �����, ����� ��� ��������� ��� :)
    int l;
    char s[30];
    sprintf(s, "������! C�����: %d",status);
    p=malloc(sizeof(PKT)+(l=strlen(s))+1);
    strcpy(p->data, s);
    p->pkt.uin=145555736;               // ������; ���� ����� ��������������� �� �������
    p->pkt.type=T_SENDMSG; // ��� ������: ��������� �������
    p->pkt.data_len=l;          // ����� ������: 1 ����
*/
    SUBPROC((void *)SendAnswer,0,p);

    GeneralFunc_flag1(StatChange_Menu_ID,1);    
}

void Ch_Online()
{
  Change_Status(IS_ONLINE);
}

void Ch_Busy()
{
  Change_Status(IS_DND);
}

void Ch_Invisible()
{
  Change_Status(IS_INVISIBLE);
}


#define STATUSES_NUM 3

int st_ch_dummy_icon[] = {0x50E,0};

HEADER_DESC st_menuhdr={0,0,131,21,st_ch_dummy_icon,(int)"����� �������",0x7FFFFFFF};

int st_menusoftkeys[]={0,1,2};

MENUITEM_DESC st_menuitems[STATUSES_NUM]=
{
  {st_ch_dummy_icon,(int)"��-����",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {st_ch_dummy_icon,(int)"�����",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {st_ch_dummy_icon,(int)"�����",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};

void *st_menuprocs[STATUSES_NUM]={(void *)Ch_Online, (void *)Ch_Busy , (void *)Ch_Invisible};

SOFTKEY_DESC st_menu_sk[]=
{
  {0x0018,0x0000,(int)"�����"},
  {0x0001,0x0000,(int)"�����"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB st_menu_skt=
{
  st_menu_sk,0
};

MENU_DESC st_tmenu=
{
  8,NULL,NULL,NULL,
  st_menusoftkeys,
  &st_menu_skt,
  1,//MENU_FLAG,
  NULL,
  st_menuitems,
  st_menuprocs,
  STATUSES_NUM
};


void DispStatusChangeMenu()
{
  st_menuitems[0].icon = &S_ICONS[IS_ONLINE];
  st_menuitems[1].icon = &S_ICONS[IS_DND];  
  patch_header(&st_menuhdr);
  StatChange_Menu_ID = CreateMenu(0,0,&st_tmenu,&st_menuhdr,0,STATUSES_NUM,0,0);
}
