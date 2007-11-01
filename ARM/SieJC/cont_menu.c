#include "../inc/swilib.h"
#include "main.h"
#include "clist_util.h"
#include "jabber_util.h"
#include "string_util.h"
#include "vCard.h"
#include "lang.h"

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
#pragma inline
void patch_rect(RECT*rc,int x,int y, int x2, int y2)
{
  rc->x=x;
  rc->y=y;
  rc->x2=x2;
  rc->y2=y2;
}
//==============================================================================


#define MAX_ITEMS 9       // ������������ ���������� ������� ����

#define MI_CONF_LEAVE       1
#define MI_QUERY_VERSION    2
#define MI_LOGIN_LOGOUT     3
#define MI_DISCO_QUERY      4
#define MI_HISTORY_OPEN     5
#define MI_TIME_QUERY       6
#define MI_VCARD_QUERY      7
#define MI_MUC_ADMIN        8
#define MI_LASTACTIV_QUERY  9

char Menu_Contents[MAX_ITEMS-1];
int cmS_ICONS[MAX_ITEMS+1];


int Contact_MenuID;
int Req_Close_Cont_Menu=0;
const char contmenu_header[]=LG_MENUCONTACT;
int menusoftkeys[]={0,1,2};
HEADER_DESC contact_menuhdr={0,0,0,0,NULL,(int)contmenu_header,LGP_NULL};
SOFTKEY_DESC clmenu_sk[]=
{
  {0x0018,0x0000,(int)LG_SELECT},
  {0x0001,0x0000,(int)LG_BACK},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB clmenu_skt=
{
  clmenu_sk,0
};


void contact_menu_ghook(void *data, int cmd);
int contact_menu_onkey(void *data, GUI_MSG *msg);
void contact_menu_iconhndl(void *data, int curitem, void *unk);

MENU_DESC contact_menu=
{
  8,contact_menu_onkey,contact_menu_ghook,NULL,
  menusoftkeys,
  &clmenu_skt,
  0x11,
  contact_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};

//////////////////////// Menu "muc#admin" ///////////////////////////
#define MAX_MUC_ADMIN_ITEMS 9

#define MA_CONF_KICK_THIS   1
#define MA_CONF_BAN_THIS    2
#define MA_CONF_VREJ_THIS   3
#define MA_CONF_VGR_THIS    4
#define MA_CONF_PARTICIPANT 5
#define MA_CONF_MEMBER      6
#define MA_CONF_MODERATOR   7
#define MA_CONF_ADMIN       8
#define MA_CONF_OWNER       9

char MA_Menu_Contents[MAX_MUC_ADMIN_ITEMS-1];
const char ma_menu_header[]=LG_MUC_ADMIN;
int mamenusoftkeys[]={0,1,2};
HEADER_DESC ma_menuhdr={0,0,0,0,NULL,(int)ma_menu_header,LGP_NULL};
SOFTKEY_DESC ma_menu_sk[]=
{
  {0x0018,0x0000,(int)LG_SELECT},
  {0x0001,0x0000,(int)LG_BACK},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB ma_menu_skt=
{
  ma_menu_sk,0
};

void ma_menu_ghook(void *data, int cmd);
int ma_menu_onkey(void *data, GUI_MSG *msg);
void ma_menu_iconhndl(void *data, int curitem, void *unk);

MENU_DESC ma_menu=
{
  8,ma_menu_onkey,ma_menu_ghook,NULL,
  mamenusoftkeys,
  &ma_menu_skt,
  0x10, // 0x11 ���� ���� ���� � ��������
  ma_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};

void InitMAMenuArray()
{
  for(int i=0;i<MAX_MUC_ADMIN_ITEMS;i++)MA_Menu_Contents[i]=0;
}

void Disp_MA_Menu()
{
  TRESOURCE *Act_contact = CList_GetActiveContact();
  InitMAMenuArray();
  int n_items=0;
  if(Act_contact->muc_privs.aff<AFFILIATION_ADMIN)
    {
      MA_Menu_Contents[n_items++]=MA_CONF_KICK_THIS;
      MA_Menu_Contents[n_items++]=MA_CONF_BAN_THIS;
      MA_Menu_Contents[n_items++]=MA_CONF_PARTICIPANT;
      MA_Menu_Contents[n_items++]=MA_CONF_MEMBER;
      MA_Menu_Contents[n_items++]=MA_CONF_MODERATOR;
      MA_Menu_Contents[n_items++]=MA_CONF_ADMIN;
      MA_Menu_Contents[n_items++]=MA_CONF_OWNER;
      if(Act_contact->muc_privs.role==ROLE_VISITOR)
      {
        MA_Menu_Contents[n_items++]=MA_CONF_VGR_THIS;
      }
      else MA_Menu_Contents[n_items++]=MA_CONF_VREJ_THIS;
    }
  patch_rect(&ma_menuhdr.rc,0,YDISP,ScreenW()-1,HeaderH()+YDISP);
  CreateMenu(0,0,&ma_menu,&ma_menuhdr,0,n_items,0,0);
}
/////////////////////////////////////////////////////////////////////

void contact_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)  // onFocus
  {
    DisableIDLETMR();
    if(Req_Close_Cont_Menu)
    {
      Req_Close_Cont_Menu=0;
      GeneralFunc_flag1(Contact_MenuID,1);
    }
  }
}

/*
���������� ������� ������ ����. ������ ������� �������� ����������� ��������

*/
int contact_menu_onkey(void *data, GUI_MSG *msg)
{
  int i=GetCurMenuItem(data);
  if(msg->keys==0x18 || msg->keys==0x3D)
  {
    switch(Menu_Contents[i])
    {
    case MI_CONF_LEAVE:
      {
        CLIST* room=CList_FindContactByJID(CList_GetActiveContact()->full_name);
        Leave_Conference(room->JID);
        CList_MakeAllResourcesOFFLINE(room);
        break;
      }
    case MI_TIME_QUERY:
      {
        Send_Time_Request((CList_GetActiveContact()->full_name));
        break;
      }

    case MI_VCARD_QUERY:
      {
        if (!Show_vCard(CList_GetActiveContact()->full_name))
          Send_Vcard_Request((CList_GetActiveContact()->full_name));
        break;
      }

    case MI_LASTACTIV_QUERY:
    {
      Send_LastActivity_Request((CList_GetActiveContact()->full_name));
      break;
    }

    case MI_QUERY_VERSION:
      {

        Send_Version_Request(CList_GetActiveContact()->full_name);
        break;
      }

    case MI_LOGIN_LOGOUT:
      {

        char *pres_str = malloc(256);
        TRESOURCE *Transport = CList_GetActiveContact();
        if(Transport->status==PRESENCE_OFFLINE)
        {
          snprintf(pres_str,255,"<presence to='%s'/>", Transport->full_name);
        }
        else
        {
          snprintf(pres_str,255,"<presence to='%s' type='unavailable'/>", Transport->full_name);
        }
        SUBPROC((void*)_sendandfree,pres_str);
        break;
      }

    case MI_DISCO_QUERY:
      {
        Send_DiscoInfo_Request(CList_GetActiveContact()->full_name);
        break;
      }
      ////////////////////////HISTORY
    case MI_HISTORY_OPEN:
      {
        //� ���������� JID ��� ��������, � � ���� � ��� �������� ����������, ��� ����� � �������.
        extern const char HIST_PATH[128];
        WSHDR *wsfn=AllocWS(255);
        wsprintf(wsfn,"%s%s.txt",HIST_PATH,CList_FindContactByJID(CList_GetActiveContact()->full_name)->JID);
        ExecuteFile(wsfn,NULL,NULL);
        FreeWS(wsfn);
        break;
      }
      
    case MI_MUC_ADMIN:
      {
        Disp_MA_Menu();
        break;
      }
      
    default:
      {
        MsgBoxError(1,(int)LG_UNKACTION);
      }
    }

    return 1;
  }
  //  Req_Close_Cont_Menu = 1;
  return 0;
}

void InitMenuArray()
{
  for(int i=0;i<MAX_ITEMS;i++)Menu_Contents[i]=0;
}

/*
���������� ��������� ������� ������������� ����
��� �� ���� ������ ������ �����, �������� ��� ���� ������ � �����.
��� �������� �� ��������� �� ������ ������� Menu_Contents, � ������� �������,
����� ������ � � ����� ������� ���������� �������.
*/
void contact_menu_iconhndl(void *data, int curitem, void *unk)
{
  //  cmS_ICONS[0]=(int)cmdummy_icon;
  WSHDR *ws;
  extern const char percent_t[];
  char test_str[48];
  void *item=AllocMenuItem(data);
  strcpy(test_str,"(������)");

  TRESOURCE *Act_contact = CList_GetActiveContact();

  switch(Menu_Contents[curitem])
  {
  case MI_CONF_LEAVE:
    {
      strcpy(test_str,LG_ABANDON);
      break;
    }

  case MI_QUERY_VERSION:
    {
      strcpy(test_str,LG_VERCLIENT);
      break;
    }

  case MI_DISCO_QUERY:
    {
      strcpy(test_str,LG_INFOFDISC);
      break;
    }
  case MI_TIME_QUERY:
    {
      strcpy(test_str,LG_QUERYTIME);
      break;
    }

  case MI_VCARD_QUERY:
    {
      strcpy(test_str,LG_QUERYVCARD);
      break;
    }

  case MI_LASTACTIV_QUERY:
    {
      strcpy(test_str,LG_QUERYLAST);
      break;
    }

  case MI_HISTORY_OPEN:
    {
      strcpy(test_str,LG_OHISTORY);
      break;
    }
  case MI_LOGIN_LOGOUT:
    {
      if(Act_contact->status==PRESENCE_OFFLINE)
      {
        strcpy(test_str,LG_ON);
      }
      else
      {
        strcpy(test_str,LG_OFF);
      }
      break;
    }
    
  case MI_MUC_ADMIN:
    {
      strcpy(test_str,LG_MUC_ADMIN);
      break;
    }
    
  }
  //ShowMSG(1,(int)test_str);
  ws=AllocMenuWS(data,strlen(test_str));
  wsprintf(ws,percent_t,test_str);

  SetMenuItemIconArray(data,item,cmS_ICONS+Menu_Contents[curitem]);
  SetMenuItemText(data,item,ws,curitem);
  //SetMenuItemIcon(data,curitem,Menu_Contents[curitem]);  // 0 = ������ ������
  /*
  CLIST *t;
  WSHDR *ws;
  void *item=AllocMenuItem(data);

  t=FindContactByN(curitem);
  if (t)
  {
  ws=AllocMenuWS(data,strlen(t->name));
  wsprintf(ws,percent_t,t->name);
}
  else
  {
  ws=AllocMenuWS(data,10);
  wsprintf(ws,"error");
}
  SetMenuItemIconArray(data,item,S_ICONS);
  SetMenuItemText(data,item,ws,curitem);
  SetMenuItemIcon(data,curitem,GetIconIndex(t));
  */
};

/*
#define MI_CONF_LEAVE       1
#define MI_CONF_KICK_THIS   2
#define MI_CONF_BAN_THIS    3
#define MI_CONF_VREJ_THIS   4
#define MI_CONF_VGR_THIS    5
#define MI_QUERY_VERSION    6
*/
extern const char PATH_TO_PIC[128];

char ICON_CONF_LEAVE[128];// = {(int)PATH_TO_PIC"menu_muc_leave.png",0};
char ICON_CONF_KICK_THIS[128];
char ICON_CONF_BAN_THIS[128];
char ICON_CONF_VREJ_THIS[128];
char ICON_CONF_VGR_THIS[128];
char ICON_QUERY_VERSION[128];
char ICON_LOGIN_LOGOUT[128];
char ICON_QUERY_DISCO[128];
char ICON_HISTORY_OPEN[128];
char ICON_QUERY_TIME[128];
char ICON_QUERY_VCARD[128];
char ICON_MUC_ADMIN[128];
char ICON_QUERY_LASTACTIV[128];

void Init_Icon_array()
{
  TRESOURCE *Act_contact = CList_GetActiveContact();

  strcpy(ICON_CONF_LEAVE, PATH_TO_PIC);strcat(ICON_CONF_LEAVE, "menu_muc_leave.png");
  strcpy(ICON_QUERY_VERSION, PATH_TO_PIC);strcat(ICON_QUERY_VERSION, "menu_version.png");
  strcpy(ICON_QUERY_DISCO, PATH_TO_PIC);strcat(ICON_QUERY_DISCO, "menu_version.png");
  strcpy(ICON_HISTORY_OPEN, PATH_TO_PIC);strcat(ICON_HISTORY_OPEN, "menu_version.png");
  strcpy(ICON_QUERY_TIME, PATH_TO_PIC);strcat(ICON_QUERY_TIME, "menu_version.png");
  strcpy(ICON_QUERY_VCARD, PATH_TO_PIC);strcat(ICON_QUERY_VCARD, "menu_version.png");
  strcpy(ICON_QUERY_LASTACTIV, PATH_TO_PIC);strcat(ICON_QUERY_LASTACTIV, "menu_version.png");
  strcpy(ICON_LOGIN_LOGOUT, PATH_TO_PIC);
  strcpy(ICON_MUC_ADMIN, PATH_TO_PIC);strcat(ICON_MUC_ADMIN, "menu_muc_admin.png");
  if(Act_contact->entry_type==T_TRANSPORT)
    if(Act_contact->status==PRESENCE_OFFLINE)
    {
      strcat(ICON_LOGIN_LOGOUT, "menu_version.png");
    }
    else strcat(ICON_LOGIN_LOGOUT, "menu_no_icon.png");

  for(int i=0;i<=MAX_ITEMS;i++)cmS_ICONS[i]=0;
  cmS_ICONS[MI_CONF_LEAVE]=(int)ICON_CONF_LEAVE;
  cmS_ICONS[MI_QUERY_VERSION]=(int)ICON_QUERY_VERSION;
  cmS_ICONS[MI_DISCO_QUERY]=(int)ICON_QUERY_DISCO;
  cmS_ICONS[MI_HISTORY_OPEN]=(int)ICON_HISTORY_OPEN;
  cmS_ICONS[MI_TIME_QUERY]=(int)ICON_QUERY_TIME;
  cmS_ICONS[MI_VCARD_QUERY]=(int)ICON_QUERY_VCARD;
  cmS_ICONS[MI_LASTACTIV_QUERY]=(int)ICON_QUERY_LASTACTIV;  
  cmS_ICONS[MI_LOGIN_LOGOUT]=(int)ICON_LOGIN_LOGOUT;
  cmS_ICONS[MI_MUC_ADMIN]=(int)ICON_MUC_ADMIN;
}

void Disp_Contact_Menu()
{
  int n_items=0;
  InitMenuArray();
  Init_Icon_array();
  TRESOURCE *Act_contact = CList_GetActiveContact();
  // ������ ����������, ����� ������ � ��� �����, � �������
  if(!Act_contact)return;

  if((Act_contact->entry_type!=T_CONF_ROOT)&&(Act_contact->entry_type!=T_GROUP)) //� ������ ������ ������� �������
  {
    Menu_Contents[n_items++]=MI_QUERY_VERSION;
  }

  if((Act_contact->entry_type!=T_GROUP)&&(Act_contact->entry_type!=T_TRANSPORT)&&(Act_contact->entry_type!=T_CONF_NODE))
  {
    Menu_Contents[n_items++]=MI_HISTORY_OPEN;
  }

  if(Act_contact->entry_type==T_TRANSPORT)
  {
    Menu_Contents[n_items++]=MI_LOGIN_LOGOUT;
  }

  if(Act_contact->entry_type==T_CONF_ROOT)
  {
    Menu_Contents[n_items++]=MI_CONF_LEAVE;
  }

  if(Act_contact->entry_type!=T_GROUP)
  {
    Menu_Contents[n_items++]=MI_DISCO_QUERY;
  }

 if((Act_contact->entry_type!=T_GROUP)&&(Act_contact->entry_type!=T_CONF_ROOT))
  {
    Menu_Contents[n_items++]=MI_VCARD_QUERY;
    Menu_Contents[n_items++]=MI_TIME_QUERY;
    Menu_Contents[n_items++]=MI_LASTACTIV_QUERY;    
  }

  if(Act_contact->entry_type==T_CONF_NODE)
  {
    if(Act_contact->muc_privs.aff<AFFILIATION_ADMIN)
    {
      Menu_Contents[n_items++]=MI_MUC_ADMIN;
    }
  }

  if(n_items+1)
  {
    patch_rect(&contact_menuhdr.rc,0,YDISP,ScreenW()-1,HeaderH()+YDISP);
    Contact_MenuID=CreateMenu(0,0,&contact_menu,&contact_menuhdr,0,n_items,0,0);
  }
  else
  {
    MsgBoxError(1,(int)LG_NOSUPACTION);
  }
}

//////////////////////// Menu "muc#admin" ///////////////////////////
void ma_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)  // onFocus
  {
    DisableIDLETMR();
  }
}

int ma_menu_onkey(void *data, GUI_MSG *msg)
{
  int i=GetCurMenuItem(data);
  MUC_ADMIN admin_cmd;
  if(msg->keys==0x18 || msg->keys==0x3D)
  {
    CLIST* room=CList_FindContactByJID(CList_GetActiveContact()->full_name);
    char* nick = Get_Resource_Name_By_FullJID(CList_GetActiveContact()->full_name);
    if(MA_Menu_Contents[i]==MA_CONF_KICK_THIS)admin_cmd=ADM_KICK;
    if(MA_Menu_Contents[i]==MA_CONF_BAN_THIS)admin_cmd=ADM_BAN;
    if(MA_Menu_Contents[i]==MA_CONF_VREJ_THIS)admin_cmd=ADM_VOICE_REMOVE;
    if(MA_Menu_Contents[i]==MA_CONF_VGR_THIS)admin_cmd=ADM_VOICE_GRANT;
    if(MA_Menu_Contents[i]==MA_CONF_PARTICIPANT)admin_cmd=ADM_PARTICIPANT;
    if(MA_Menu_Contents[i]==MA_CONF_MEMBER)admin_cmd=ADM_MEMBER;
    if(MA_Menu_Contents[i]==MA_CONF_MODERATOR)admin_cmd=ADM_MODERATOR;
    if(MA_Menu_Contents[i]==MA_CONF_ADMIN)admin_cmd=ADM_ADMIN;
    if(MA_Menu_Contents[i]==MA_CONF_OWNER)admin_cmd=ADM_OWNER;
    MUC_Admin_Command(room->JID, nick, admin_cmd, "SieJC_muc#admin");
    return 1;
  }
  return 0;
}

void ma_menu_iconhndl(void *data, int curitem, void *unk)
{
  WSHDR *ws;
  extern const char percent_t[];
  char test_str[48];
  void *item=AllocMenuItem(data);
  strcpy(test_str,"(������)");

  TRESOURCE *Act_contact = CList_GetActiveContact();

  switch(MA_Menu_Contents[curitem])
  {
  case MA_CONF_KICK_THIS:
    {
      strcpy(test_str,LG_KIK);
      break;
    }
  case MA_CONF_BAN_THIS:
    {
      strcpy(test_str,LG_BAN);
      break;
    }
  case MA_CONF_VREJ_THIS:
    {
      strcpy(test_str,LG_LVOISE);
      break;
    }
  case MA_CONF_VGR_THIS:
    {
      strcpy(test_str,LG_GVOISE);
      break;
    }

  case MA_CONF_PARTICIPANT:
    {
      strcpy(test_str,LG_PARTICIPANT);
      break;
    }

  case MA_CONF_MEMBER:
    {
      strcpy(test_str,LG_MEMBER);
      break;
    }

  case MA_CONF_MODERATOR:
    {
      strcpy(test_str,LG_MODERATOR);
      break;
    }

  case MA_CONF_ADMIN:
    {
      strcpy(test_str,LG_ADMIN);
      break;
    }

  case MA_CONF_OWNER:
    {
      strcpy(test_str,LG_OWNER);
      break;
    }
  }

  ws=AllocMenuWS(data,strlen(test_str));
  wsprintf(ws,percent_t,test_str);

  SetMenuItemText(data,item,ws,curitem);
}
