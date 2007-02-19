#include "../inc/swilib.h"
#include "main.h"
#include "jabber.h"
#include "clist_util.h"
#include "jabber_util.h"
#include "string_util.h"
#include "history.h"

CLIST* cltop = NULL;


char Display_Offline = 1;         // ���������� �� �������-�������������

char lineColor = 0;               // ad: ���� ������� �������

unsigned int NContacts = 0;       // ����� ��������� (� ��������) � ������
unsigned int N_Disp_Contacts = 0; // ������� �� ��� ������ ������������ 

unsigned int Active_page = 1;     // ������� �������� �������� ������
unsigned int N_cont_disp=0;       // ������� ��������� �� ��������
unsigned int CursorPos = 1;       // ������� ������� �������

TRESOURCE* ActiveContact = NULL;

extern char logmsg[512];
extern const unsigned short PRES_COLORS[PRES_COUNT];
extern char My_Presence;
extern const char* PRESENCES[PRES_COUNT];
extern JABBER_STATE Jabber_state;

/*
    ������������ ���������, ������� ���������� ���������� �������-�����
*/
void CList_RedrawCList()
{
  // ����������, ����� ��������� ���������� �� �������� ������
  int font_y = GetFontYSIZE(CLIST_FONT)+2; // ad: ����� ��� ����� ������� ����� ���� 1 ������... ����� ������� �������� �� ������� ������
  int scr_w=ScreenW();
  N_cont_disp = sdiv(font_y,ScreenH()-CLIST_Y1)-1;
  if(!cltop)return; 
   
  LockSched(); 
  N_Disp_Contacts = Display_Offline? NContacts : CList_GetNumberOfOnlineUsers();
  // ���������� ���������� ������� ������ ���������
  int pages_number = sdiv(N_cont_disp, N_Disp_Contacts);
  if(N_cont_disp*pages_number<N_Disp_Contacts){pages_number++;};
  
  CLIST* ClEx = cltop;
  WSHDR* out_ws = AllocWS(256);
  int i=1;
  int start_y;
  int fcolor;
  char cur[2]=">\0";
  TRESOURCE* resEx;

  char Alternation = 1;             // ad: ��������� �����������

  WSHDR* ClEx_name = AllocWS(128);
  WSHDR* ResEx_name = AllocWS(128);
  
  while(ClEx)
  {
    if(ClEx->ResourceCount)
    {
      resEx = ClEx->res_list;
      while(resEx)
      {
        
        if((i>(Active_page-1)*N_cont_disp) && (Display_Offline  |  resEx->status!=PRESENCE_OFFLINE | resEx->has_unread_msg ))
        {
          if(i==CursorPos)
          {
            cur[0]='�';
            lineColor=CURSOR;
            ActiveContact = resEx;
          } else{ 
            cur[0]=' ';
            lineColor=(Alternation==1)? 0 : 22;
          }
          //wsprintf(out_ws,"%s TE rfST %d", cur, i);
          ascii2ws(ClEx_name, ClEx->name);
          if(resEx->name)
          {
            ascii2ws(ResEx_name,resEx->name);
            wsprintf(out_ws,"%s %d %w/%w", cur, resEx->has_unread_msg, ClEx_name, ResEx_name);
          }
          else
          {
            wsprintf(out_ws,"%s %d %w", cur, resEx->has_unread_msg, ClEx_name);            
          }
/*          
          // ��� ��� ��������� ����������� ������ � ���������� �����. ���.
          if(resEx->name) {
            wsprintf(out_ws,"%s %d %s/%s", cur, resEx->has_unread_msg, ClEx->name, resEx->name);
          } else {
            wsprintf(out_ws,"%s %d %s", cur, resEx->has_unread_msg, ClEx->name);
          }
*/
          
                    
          start_y = CLIST_Y1 + (i - (Active_page-1)*N_cont_disp)*font_y;
          
          if(resEx->has_unread_msg){fcolor=CLIST_F_COLOR_0;}else{fcolor=PRES_COLORS[resEx->status];}

          DrawRoundedFrame(0,start_y,scr_w-1,start_y+font_y,0,0,0,
		   GetPaletteAdrByColorIndex(15),  //ad: ������ � ������������... ��� �����������
		   GetPaletteAdrByColorIndex(lineColor)); //ad: �� ����� ����� �������� ������� ��������, ������ �������
          
          DrawString(out_ws,1,start_y+1,scr_w-1,start_y+font_y,SMALL_FONT,0,GetPaletteAdrByColorIndex(fcolor),GetPaletteAdrByColorIndex(23));

          Alternation=(Alternation==1)?0:1; //ad: ������������� �����������          
        }
        if(Display_Offline  |  resEx->status!=PRESENCE_OFFLINE | resEx->has_unread_msg)i++;
        resEx = resEx->next;
        if(i>Active_page*N_cont_disp)break;
      }
    }
    ClEx = ClEx->next;
    if(i>Active_page*N_cont_disp)break;
  }
  UnlockSched();

  LockSched();
//  sprintf(logmsg, "P=%d;C=%d;N=%d;ND=%d",Active_page, CursorPos,N_Disp_Contacts,N_cont_disp);
  if(Jabber_state==JS_ONLINE)sprintf(logmsg, "Self=%s",PRESENCES[My_Presence]);
  UnlockSched();
  FreeWS(ClEx_name);
  FreeWS(ResEx_name);
  FreeWS(out_ws);
}


TRESOURCE* CList_GetActiveContact()
{
  return ActiveContact;
}

unsigned int CList_GetNumberOfOnlineUsers()
{
  unsigned int Online=0;
  CLIST* ClEx;
  TRESOURCE* ResEx;
  if(!(ClEx = cltop))return 0;
  while(ClEx)
  {
    ResEx = ClEx->res_list;
    while(ResEx)
    {
      if(ResEx->status!=PRESENCE_OFFLINE)Online++;
      ResEx=ResEx->next;
    }
    ClEx = ClEx->next;
  }
  return Online;
}

void CList_ToggleOfflineDisplay()
{
  Display_Offline = !Display_Offline;
  REDRAW();
}

// ����� ������ ���������
void KillMsgList(LOG_MESSAGE* messtop)
{
  LOG_MESSAGE* cl=messtop;
  while(cl)
  {
    LOG_MESSAGE *p;
    mfree(cl->mess);
    p=cl;
    cl=cl->next;
    mfree(p);
  }     
}


// ����� ������ ��������
void KillResourceList(TRESOURCE* res_list)
{
  TRESOURCE* cl=res_list;
  while(cl)
  {
    TRESOURCE *p;
    if(cl->name)mfree(cl->name);
    mfree(cl->full_name);
    if(cl->status_msg)mfree(cl->status_msg);
    if(cl->log)KillMsgList(cl->log);
    p=cl;
    cl=cl->next;
    mfree(p);
    p=NULL;
  }     
}

// �������� ���������� �������� �� FullJID (JID ������ � ��������)
CLIST* CList_FindContactByJID(char* jid)
{
  LockSched();
  CLIST* ClEx = cltop;
  while(ClEx)
  {
    if(strstr(jid,ClEx->JID)==jid)
    {
      UnlockSched();
      return ClEx;
    }
    ClEx = ClEx->next;
  }
  UnlockSched();
  return NULL;
}

// ������, ���� �� ��� ����� ������ � ��������, �� FullJID
TRESOURCE* CList_IsResourceInList(char* jid)
{
  CLIST* ClEx = CList_FindContactByJID(jid);
  if(!ClEx)return NULL;
  LockSched();
  TRESOURCE* ResEx = ClEx->res_list;
  while(ResEx)
  {
    if(!strcmp(ResEx->full_name, jid))
    {
      UnlockSched();
      return ResEx;
    }
    ResEx = ResEx->next;
  }
  UnlockSched();
  return NULL;
}

// �������� ������ � �������� 
void CList_Ch_Status(TRESOURCE* resource,
                       char status,
                       char* status_msg
                       )
{
  LockSched();
  if(resource->status_msg)
  {
    mfree(resource->status_msg);
    resource->status_msg = NULL;
  }
  if(status_msg)
  {
    resource->status_msg = malloc(strlen(status_msg)+1);
    strcpy(resource->status_msg, status_msg);
  }
  resource->status = status;
  UnlockSched();
}


TRESOURCE* CList_AddResourceWithPresence(char* jid, char status, char* status_msg)
{
  TRESOURCE* qq = CList_IsResourceInList(jid);
  
  // ���� ����� ������ ��� ����, ��� �� ���������. 
  // ����� ��� ������ ��������.
  if(qq)
  {
    CList_Ch_Status(qq, status, status_msg);
    return qq;
  }
  CLIST* ClEx = cltop;
  LockSched();
  while(ClEx)
  {
    
    if(strstr(jid,ClEx->JID)==jid) // ���, ������ ���, ��� ��� ������������� ������ ������!
    {
      TRESOURCE* ResEx=malloc(sizeof(TRESOURCE));//ClEx->res_list;
      char *resname_ptr=Get_Resource_Name_By_FullJID(jid);
      ResEx->name = malloc(strlen(resname_ptr)+1);
      ResEx->full_name = malloc(strlen(jid)+1);
      strcpy(ResEx->full_name, jid);
      strcpy(ResEx->name, resname_ptr);
      if(status_msg)
      {
        ResEx->status_msg = malloc(strlen(status_msg)+1);
        strcpy(ResEx->status_msg, status_msg);
      }
      else
      {
        ResEx->status_msg = NULL;
      }
            
      ResEx->status = status;
      ResEx->virtual = 0;
      ResEx->has_unread_msg=0;
      ResEx->log = NULL;
      
      // ������� ������������, ���� �� ����
      if(ClEx->res_list->virtual==1)
      {
        KillResourceList(ClEx->res_list);
        ClEx->ResourceCount=0;
        NContacts--;
        ClEx->res_list=NULL;
      }    
      
      TRESOURCE* existing_res=ClEx->res_list;
      
      if(!existing_res)
      {
        ClEx->res_list = ResEx;
        ClEx->ResourceCount=1;
      }
      else
      {
        while(existing_res->next){existing_res=existing_res->next;}
        existing_res->next = ResEx;
        ClEx->ResourceCount++;
      }
      NContacts++;
      ResEx->next=NULL;
      CursorPos = 1;
      Active_page=1;
      UnlockSched();
      return ResEx;
    }
    ClEx = ClEx->next;
  }
CursorPos = 1;
Active_page=1;
UnlockSched();  
return NULL;
}


// �������� � ����� �������. ���������� ��������� ���������� ��������.
CLIST* CList_AddContact(char* jid,
                          char* name,
                          JABBER_SUBSCRIPTION subscription,
                          char group)
{
  CLIST* Cont_Ex = malloc(sizeof(CLIST));

  // ��� ����� ���� �� ���������
  if(name)
  {
    Cont_Ex->name = malloc(strlen(name)+1);
    strcpy(Cont_Ex->name, name);
  }
  else
  {
    Cont_Ex->name = malloc(strlen(jid)+1);
    strcpy(Cont_Ex->name, jid);
  }

  Cont_Ex->JID = malloc(strlen(jid)+1);
  strcpy(Cont_Ex->JID, jid);
  Cont_Ex->subscription = subscription;
  Cont_Ex->group = group;
  Cont_Ex->next=NULL;
   
  TRESOURCE* ResEx = malloc(sizeof(TRESOURCE));
  ResEx->log=NULL;
  ResEx->next=NULL;
  ResEx->status=PRESENCE_OFFLINE;
  ResEx->status_msg=NULL;
  ResEx->has_unread_msg=0;
  ResEx->virtual=1; // �� ����� �������� ����� ��� �����
  ResEx->name = NULL;
  ResEx->full_name = malloc(strlen(jid)+1);
  strcpy(ResEx->full_name, jid);
//  strcat(ResEx->full_name, "/");
//  strcat(ResEx->full_name, ResEx->name);
  Cont_Ex->res_list=ResEx;
  Cont_Ex->ResourceCount=1;

  LockSched();
  if(!cltop)
  {
    cltop = Cont_Ex;
  }
  else
  {
    CLIST* tmp=cltop;
    while(tmp->next){tmp=tmp->next;}
    tmp->next=Cont_Ex;
  }

  NContacts++;
  UnlockSched();
  CursorPos = 1;
  Active_page=1;
  return Cont_Ex;
}


// �������� ��������� � ������ ��������� ��������
void CList_AddMessage(char* jid, MESS_TYPE mtype, char* mtext)
{
  TTime now_time;
  TDate now_date;
  GetDateTime(&now_date,&now_time);
  char datestr[127];
  if(mtype==MSG_ME)
  {
    extern char My_JID[128];
    sprintf(datestr, "%s: %02d:%02d %02d-%02d\r\n",My_JID,now_time.hour,now_time.min,now_date.day,now_date.month);
  }
  else
  {
    sprintf(datestr, "%s: %02d:%02d %02d-%02d\r\n",jid,now_time.hour,now_time.min,now_date.day,now_date.month);
  }

  
  CLIST* contEx = CList_FindContactByJID(jid);
  if(!contEx)
  {
    Log("MESS_LOST",mtext);
    return;
  }
  TRESOURCE* cont = CList_IsResourceInList(jid);
  if(!cont)
  {
    // � �������� ��� �������� ��� ������ ������� ���. ��������� �� ������ ��.
    // ����� ��������, ������ �����, ���������, ���� � �������� �������� �� 
    // ������ � �������� (MRIM)
    cont=contEx->res_list;
  }
  if(mtype!=MSG_ME)cont->has_unread_msg++;
  LOG_MESSAGE* mess = malloc(sizeof(LOG_MESSAGE));
  mess->mess = malloc(strlen(mtext)+1);
  strcpy(mess->mess, mtext);
  mess->mtype=mtype;
  LockSched();
  if(!cont->log)
  {
    cont->log = mess;
  }
  else
  {
    LOG_MESSAGE* tmp=cont->log;
    while(tmp->next){tmp=tmp->next;}
    tmp->next=mess;
  }
  mess->next=NULL;
  UnlockSched();
  Add2History(CList_FindContactByJID(jid), datestr,mtext);
}

/*
//���������� ������
void FreeCLIST(void)
{
  CLIST *cl=(CLIST*)cltop;
  cltop=0;
  while(cl)
  {
    CLIST *p;
//    if (cl->log) mfree(cl->log);
//    if (cl->answer) mfree(cl->answer);
    p=cl;
    cl=(CLIST*)(cl->next);
    mfree(p);
  }
}

*/

// ���������� ������ ���������  
void CList_Destroy()
{
  LockSched();
  CLIST* cl=cltop;
  cltop=NULL;
  while(cl)
  {
    CLIST *p;
    if(cl->res_list) KillResourceList(cl->res_list);
    cl->ResourceCount=0;
    mfree(cl->JID);
    mfree(cl->name);
    p=cl;
    cl=(CLIST*)(cl->next);
    mfree(p);
    p=NULL;
  }  
  UnlockSched();
}


// ����������� ������� ����� ��� �������
void CList_Display_Popup_Info(TRESOURCE* ResEx)
{
  if(!ResEx)return;
  char msg_ex[]="JID: %s\n������:%s";
  char* msg = malloc(1024);
  if(ResEx->status_msg)
  {
    ShowMSG(0,(int)ResEx->status_msg);
  }
  snprintf(msg,1024,msg_ex,ResEx->full_name, PRESENCES[ResEx->status]);
  ShowMSG(0, (int)msg);
  mfree(msg);
}

// ���������� ��������
void CList_MoveCursorUp()
{
  if(!N_Disp_Contacts)return;
  if(CursorPos==1)
  {
    CursorPos=N_Disp_Contacts;
    Active_page = sdiv(N_cont_disp, N_Disp_Contacts)+1;
  }
  else
  {
    CursorPos--;
    if(CursorPos<=(Active_page-1)*N_cont_disp){Active_page--;}
  }
  REDRAW();
};

void CList_MoveCursorDown()
{
  if(!N_Disp_Contacts)return;
  CursorPos++;
  if(CursorPos>N_Disp_Contacts){CursorPos=1;Active_page=1;}
  if(Active_page*N_cont_disp<CursorPos){Active_page++;}
  REDRAW();
};

void CList_MoveCursorHome()
{
  if(!N_Disp_Contacts)return;
  CursorPos =1;
  Active_page = 1;
  REDRAW();  
};

void CList_MoveCursorEnd()
{
  if(!N_Disp_Contacts)return;
  CursorPos = N_Disp_Contacts;
  Active_page = sdiv(N_cont_disp, N_Disp_Contacts)+1;
  REDRAW();  
};
