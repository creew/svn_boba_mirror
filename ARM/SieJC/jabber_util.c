#include "../inc/swilib.h"
#include "history.h"
#include "main.h"
#include "clist_util.h"
#include "jabber_util.h"
#include "string_util.h"
#include "xml_parser.h"
#include "jabber.h"


extern const char JABBER_SERVER[];
extern const char USERNAME[];  
extern const char PASSWORD[];
extern const char RESOURCE[];
extern const char CMP_DATE[];
extern const char VERSION_NAME[];
extern const char VERSION_VERS[];
extern const char OS[];
extern char My_JID_full[];
extern char My_JID[];
extern char logmsg[];

MUC_ITEM*  muctop=NULL;

extern JABBER_STATE Jabber_state;
//const char* PRESENCES[PRES_COUNT] = {"online", "unavailable", "error", "chat", "away", "xa", "dnd", "invisible",
const char* PRESENCES[PRES_COUNT] = {"online", "chat", "away", "xa", "dnd", "invisible", "unavailable", "error",
                                      "subscribe", "subscribed", "unsubscribe", "unsubscribed"};
//const unsigned short PRES_COLORS[PRES_COUNT]  = {15,        21,             14,     16,     3,      18,   2,      20, 21,21,21,21}; //���� ��������� �������
const unsigned short PRES_COLORS[PRES_COUNT]  = {15,        16,             3,     18,     2,      20,   21,      20, 21,21,21,21}; //���� ��������� �������

#define AFFS_CNT 5
#define ROLS_CNT 4

const char* JABBER_AFFS[] = {"none", "outcast", "member", "admin", "owner"};
const char* JABBER_ROLS[] = {"none", "visitor", "participant", "moderator"};

/*
  ������� ������������ Jabber Iq
*/
//Context: HELPER
void SendIq(char* to, char* type, char* id, char* xmlns, char* payload)
{
  char* xmlq=malloc(1024);
  char *xmlq2=malloc(1024);
  char s_to[128];
  char s_id[40];
  if(id)
  {
    snprintf(s_id,40,"id='%s'", id);
  }
  else
  {
    strcpy(s_id, "");
  }
  sprintf(xmlq, "<iq type='%s' %s from='%s'", type, s_id, My_JID_full);
  if(to)
  {
    to =  ANSI2UTF8(to, 128);
    snprintf(s_to, 128, " to='%s'", to);
    strcat(xmlq, s_to);
  }
  if(payload)
  {
    strcat(xmlq, ">\r\n<query xmlns='%s'>\r\n%s\r\n</query>\r\n</iq>");
    sprintf(xmlq2, xmlq, xmlns, payload);
  }
  else
  {
    strcat(xmlq, ">\r\n<query xmlns='%s'/>\r\n</iq>");
    sprintf(xmlq2, xmlq, xmlns);
  }
  sprintf(xmlq2, xmlq, xmlns, payload);
  SendAnswer(xmlq2);
//  Log("IQ_OUT", xmlq2);
  mfree(xmlq2);
  mfree(xmlq);
}

/*
  ������� �����������, �� ���� ������ �������� �������� stream.
  ����� ����� ����� �������� � ��������
*/
// Context:HELPER
void Send_Welcome_Packet()
{
  char streamheader[]="<?xml version='1.0' encoding='UTF-8'?>\n<stream:stream to='%s' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' xml:lang='en'>";
  char* buf=malloc(256);  
  sprintf(buf,streamheader,JABBER_SERVER);
  SendAnswer(buf);
  mfree(buf);
  LockSched();
  strcpy(logmsg,"Send Welcome");
  UnlockSched();
#ifdef LOG_ALL
  Log("CONN",logmsg);
#endif 
}

/*
  ������� ����������
*/
// Context:HELPER
void Send_Disconnect()
{
  char streamend[]="</stream:stream>";
  SendAnswer(streamend);
  strcpy(logmsg,"Send Disconnect");
#ifdef LOG_ALL
  Log("CONN",logmsg);
#endif 
}

// ��������� Iq-��������
char auth_id[] = "SieJC_auth_req";
char rost_id[] = "SieJC_rost_req";
char vreq_id[] = "SieJC_vers_req";

/*
  ����������� �� Jabber-�������
  ����� �����, ��� ����������.
*/
//Context: HELPER
void Send_Auth()
{
  sprintf(My_JID, "%s@%s",USERNAME, JABBER_SERVER);
  sprintf(My_JID_full,"%s/%s",My_JID, RESOURCE);
  char* payload = malloc(256);
  sprintf(payload,"<username>%s</username>\n<password>%s</password>\n<resource>%s</resource>",USERNAME, PASSWORD, RESOURCE);
  SendIq(NULL, "set", auth_id, IQ_AUTH, payload);  
  LockSched();
  strcpy(logmsg,"Send auth");
  UnlockSched();
}



//Context: HELPER
void _sendversionrequest(char *dest_jid)
{
  char typ[]=IQTYPE_GET;
  char iq_v[]=IQ_VERSION;

  SendIq(dest_jid, typ, vreq_id, iq_v, NULL);
  mfree(dest_jid);
}

// ������� ������ � ������ ������������ � ��������� JID
// JID ��������� � ANSI-���������
void Send_Version_Request(char *dest_jid) 
{
  char *to=malloc(128);
  strcpy(to, dest_jid);
  SUBPROC((void*)_sendversionrequest,to);
}


/*
  ������� ��� ����������� (� ���������, ����� ����� �� ��� ������������ 
  ����������� ���������, � �� ���������� � �������� ������ ��������)

�������� message �� ������ ������������ � ���������� ���������!
*/
// Context: HELPER
void Send_Presence(PRESENCE_INFO *pr_info/*short priority, char status, char* message*/)
{
  extern char My_Presence;
  My_Presence = pr_info->status;
  
  char* presence = malloc(1024);
  if(pr_info->message)
  {
    char presence_template[]="<presence><priority>%d</priority><show>%s</show><status>%s</status></presence>";
    snprintf(presence,1024,presence_template, pr_info->priority, PRESENCES[pr_info->status], pr_info->message);
  }
  else
  {
    char presence_template[]="<presence><priority>%d</priority><show>%s</show></presence>";
    snprintf(presence,1024,presence_template, pr_info->priority, PRESENCES[pr_info->status]);   
  }
  SendAnswer(presence);
  
// MUC support
  MUC_ITEM* m_ex = muctop;
  while(m_ex)
  {
    if(pr_info->message)
    {
      char presence_template[]="<presence from='%s' to='%s'><show>%s</show><status>%s</status></presence>";
      snprintf(presence,1024,presence_template, My_JID_full, m_ex->conf_jid, PRESENCES[pr_info->status], pr_info->message);
    }
    else
    {
      char presence_template[]="<presence from='%s' to='%s'><show>%s</show></presence>";
      snprintf(presence,1024,presence_template, My_JID_full, m_ex->conf_jid, PRESENCES[pr_info->status]);
    }
    SendAnswer(presence);
    m_ex=m_ex->next;
  };  
  
  mfree(presence);
  if(pr_info->message)mfree(pr_info->message);
  mfree(pr_info);
  LockSched();
  strcpy(logmsg,"Send presence");
  UnlockSched();
}

/*
  ������� ������ �������
*/
// Context: HELPER
void Send_Roster_Query()
{
  SendIq(NULL, IQTYPE_GET, rost_id, IQ_ROSTER, NULL); 
  LockSched();
  strcpy(logmsg,"Send roster Q");
  UnlockSched();
}

unsigned int m_num=0;


// Context: HELPER
void SendMessage(char* jid, IPC_MESSAGE_S *mess)
{
/*
  <message to='romeo@montague.net' id='message22'>
      <body>Art thou not Romeo, and a Montague?</body>
      <x xmlns='jabber:x:event'>
        <offline/>
        <delivered/>
        <displayed/>
        <composing/>
      </x>
    </message>
*/
 
  // ���������� � JID 
//  char IsNonLatin=0; // ��� ��������� ��������
//  int jlen=strlen(jid);
//  for(int i=0;i<jlen;i++)
//  {
//    if(*(jid+i)>127)IsNonLatin=1;
//  }

  char* utf8_jid = ANSI2UTF8(jid, 128);
  //mess->body = Correct_UTF8_String(mess->body);
  char mes_template[]="<message to='%s' id='SieJC_%d' type='%s'><body>%s</body></message>";
  char* msg_buf = malloc(MAX_MSG_LEN*2+200);
  if(mess->IsGroupChat)
  {
     sprintf(msg_buf, mes_template, utf8_jid, m_num, MSGSTR_GCHAT, mess->body);
  }else sprintf(msg_buf, mes_template, utf8_jid, m_num, MSGSTR_CHAT, mess->body);
  mfree(mess->body);
  mfree(mess);
  mfree(utf8_jid);
#ifdef LOG_ALL
  LockSched();
  Log("MESS_OUT", msg_buf);
  UnlockSched();
#endif
  SendAnswer(msg_buf);
  mfree(msg_buf);
  m_num++;
}

// Context: HELPER
void Report_VersionInfo(char* id, char *to)
{
  char answer[200];  
  char *ph_model = Get_Phone_Info(PI_MODEL);
  char *ph_sw = Get_Phone_Info(PI_SW_NUMBER);
  sprintf(answer, "<name>%s</name><version>%s (%s)</version><os>SIE-%s/%s %s</os>", VERSION_NAME, VERSION_VERS, CMP_DATE, ph_model, ph_sw, OS);
  SendIq(to, IQTYPE_RES, id, IQ_VERSION, answer);

  mfree(id);
  mfree(to);
}

JABBER_SUBSCRIPTION GetSubscrType(char* subs)
{
  if(!subs)return SUB_NONE;
  if(!strcmp(subs,"none"))return SUB_NONE;
  if(!strcmp(subs,"both"))return SUB_BOTH;
  if(!strcmp(subs,"to"))return SUB_TO;
  if(!strcmp(subs,"from"))return SUB_FROM;
  
  return SUB_NONE;  
}

char* Get_Resource_Name_By_FullJID(char* full_jid)
{
  char* pre_res = strstr(full_jid, "/");
  char* res_name = pre_res==NULL ? NULL : pre_res +1;
  return res_name;
}

//Context: HELPER
void Send_Initial_Presence_Helper()
{
  char ansi_msg[]="Online";
  char *message = ANSI2UTF8(ansi_msg, strlen(ansi_msg));
  PRESENCE_INFO *pr_info = malloc(sizeof(PRESENCE_INFO));
  pr_info->priority = 16;
  pr_info->status=PRESENCE_ONLINE;
  pr_info->message=message;
  Send_Presence(pr_info);
  Jabber_state = JS_ONLINE;
}

//Context: HELPER
void _enterconference(MUC_ENTER_PARAM *param)
{
  char magic_ex[]="<presence from='%s' to='%s/%s'><x xmlns='http://jabber.org/protocol/muc'/><history maxstanzas='%d'/></presence>";
  char* magic = malloc(1024);
  sprintf(magic,magic_ex, My_JID_full, param->room_name,param->room_nick, param->mess_num);
  SendAnswer(magic);
  mfree(magic);
  mfree(param->room_nick);
  mfree(param->room_name);
  if(param->pass)mfree(param->pass);
  mfree(param);
}


// ������ � �����������
// ��� ����� � UTF-8 :)
void Enter_Conference(char *room, char *roomnick, char N_messages)
{
  // ��������� ������� ����������� � ������
  CLIST* Conference = CList_FindContactByJID(room);
  if(!Conference)
  {
    CList_AddContact(room,room, SUB_BOTH, 0, 129);
  }
  else
  {
    Conference->res_list->status=PRESENCE_ONLINE;
  }
  
  // ������� ��������� ��� �������� � HELPER
  MUC_ENTER_PARAM* par = malloc(sizeof(MUC_ENTER_PARAM));
//  par->room_nick =ANSI2UTF8(roomnick, strlen(roomnick)*2);
//  par->room_name = ANSI2UTF8(room, strlen(room)*2);

  par->room_nick =malloc(strlen(roomnick)*2);
  par->room_name = malloc(strlen(room)*2);
  strcpy(par->room_nick, roomnick);
  strcpy(par->room_name, room);  
  
  par->pass=NULL;
  par->mess_num=N_messages;
  SUBPROC((void*)_enterconference, par);
  
  // ������������ ����������� � ������ �����������
  MUC_ITEM* mi = malloc(sizeof(MUC_ITEM));
  mi->conf_jid = malloc(strlen(par->room_name)*2+strlen(par->room_nick)*2+2);
  sprintf(mi->conf_jid, "%s/%s", par->room_name, par->room_nick);
  mi->next=NULL;
  MUC_ITEM* m_ex = muctop;
  if(muctop)
  {
    while(m_ex->next){m_ex=m_ex->next;};
    m_ex->next = mi;
  }
  else
  {
    muctop = mi;
  }
}


//Context: HELPER
void _leaveconference(char *conf_jid)
{
  char pr_templ[] = "<presence from='%s' to='%s' type='unavailable'/>";
  char *my_jid =ANSI2UTF8(My_JID_full, strlen(My_JID_full)*2);
  char* pr=malloc(1024);
  sprintf(pr, pr_templ,my_jid,conf_jid);
  mfree(my_jid);  
  mfree(conf_jid);
  SendAnswer(pr);
  mfree(pr);
}

// ������� �� �����������
// ��� ������� � ANSI
void Leave_Conference(char* room)
{
  char* utf8_room=ANSI2UTF8(room, strlen(room)*2);
  // ���� ��������� �������� � ������ ��� �����������
  MUC_ITEM* m_ex = muctop;
  while(m_ex)
  {
    if(strstr(m_ex->conf_jid, utf8_room))
    {
      char* cj = malloc(strlen(m_ex->conf_jid)*2+1);
      strcpy(cj, m_ex->conf_jid);
      SUBPROC((void*)_leaveconference, cj);
      break;
    }
    m_ex = m_ex->next;
  }

  MUC_ITEM* m_ex2 = muctop;
  if(muctop==m_ex)
  {
    mfree(m_ex->conf_jid);
    muctop=m_ex->next;
    mfree(m_ex);
  }
  while(m_ex2)
  {
    if(m_ex2->next==m_ex)
    {
      m_ex2->next = m_ex->next;   // �������� �� �������
      mfree(m_ex->conf_jid);
      mfree(m_ex);
      break;
    }
    m_ex2 = m_ex2->next;
  }  
  mfree(utf8_room); 
  ShowMSG(1,(int)"����� �� MUC ��������"); 
}


// ���������� ������ ������  
void MUCList_Destroy()
{
  LockSched();
  MUC_ITEM* cl=muctop;
  muctop=NULL;
  while(cl)
  {
    MUC_ITEM *p;
    mfree(cl->conf_jid);
    p=cl;
    cl=(MUC_ITEM*)(cl->next);
    mfree(p);
    p=NULL;
  }  
  UnlockSched();
}



// ��� ������ ��������
void Send_Presence_MMIStub()
{
  SUBPROC((void*)Send_Initial_Presence_Helper);
}

// �������� ��� �������� � �������
void ChangeRoster(XMLNode* items)
{
  XMLNode* rostEx = items;
  CLIST* Cont_Ex;
  char *name;
  char* w_subscr;
  char aname[]="jid";  // ��������� Tool Internal Error
  char asub[]="subscribe";
  char aask[]="ask";
  char* jid;
  char w_subscr_flag;
  while(rostEx)
  {
    jid = XML_Get_Attr_Value(aname,rostEx->attr);
    JABBER_SUBSCRIPTION r_subscr=GetSubscrType(XML_Get_Attr_Value("subscription",rostEx->attr));
    name = XML_Get_Attr_Value("name",rostEx->attr);
    w_subscr = XML_Get_Attr_Value(aask,rostEx->attr);
    if(w_subscr)
    {
      w_subscr_flag = !strcmp(w_subscr, asub) ? 1: 0;
    }
    else
    {
      w_subscr_flag = 0;
    }
    Cont_Ex = CList_FindContactByJID(jid);
    if(Cont_Ex)
    {
      CList_ChangeContactParams(Cont_Ex,name,r_subscr, w_subscr_flag,0);
    }
    rostEx=rostEx->next;
  }
}

void FillRoster(XMLNode* items)
{
  XMLNode* rostEx = items;
  int i=0;
  char* name;
  char* w_subscr;
  char w_subscr_flag;
  while(rostEx)
  {
    JABBER_SUBSCRIPTION r_subscr=GetSubscrType(XML_Get_Attr_Value("subscription",rostEx->attr));
    name = XML_Get_Attr_Value("name",rostEx->attr); 
    w_subscr = XML_Get_Attr_Value("ask",rostEx->attr);
    if(w_subscr)
    {
      w_subscr_flag = !strcmp(w_subscr, "subscribe") ? 1: 0;
    }
    else
    {
      w_subscr_flag = 0;
    }
    CList_AddContact(XML_Get_Attr_Value("jid",rostEx->attr),
                          name,
                          r_subscr,
                          w_subscr_flag,
                          0
                          );
   //if(name)mfree(name);
    
   rostEx=rostEx->next;
   i++;
  }
  // ����� ������� �������� ��������
  extern GBSTMR TMR_Send_Presence;
  GBS_StartTimerProc(&TMR_Send_Presence, TMR_SECOND*1, Send_Presence_MMIStub);
}

/*
 ��������� �������� Iq-��������
*/
void Process_Iq_Request(XMLNode* nodeEx)
{
  char* iqtype = NULL;
  char* id = NULL;
  char *from = NULL;
  char gget[]=IQTYPE_GET;
  char gres[]=IQTYPE_RES;
  char gerr[]=IQTYPE_ERR;
  char gset[]=IQTYPE_SET;
  const char iq_version[]=IQ_VERSION;

  iqtype = XML_Get_Attr_Value("type",nodeEx->attr);
  from = XML_Get_Attr_Value("from",nodeEx->attr);
  id = XML_Get_Attr_Value("id",nodeEx->attr);
  
// ��������� ������� ������������ ���������
if(!iqtype) return;

if(!strcmp(gget,iqtype)) // Iq type = get
{
  XMLNode* query;
  if(!(query = XML_Get_Child_Node_By_Name(nodeEx, "query")))return;
  char* q_type = XML_Get_Attr_Value("xmlns", query->attr);
  if(!q_type)return;
  // ��� �� ����� XMLNS ������������ �������
  if(!strcmp(q_type,iq_version))
  {
    // jabber:iq:version 
    if(from)
    {
        // ������� ����������, ����� � ��� �������� ������ 
        // � ��������� ���������� � HELPER
      char* loc_id = NULL;
      if(id)
        {
          loc_id=malloc(strlen(id)+1);
          strcpy(loc_id,id);
        }
        char* loc_from=malloc(strlen(from)+1);
        strcpy(loc_from,from);
        SUBPROC((void*)Report_VersionInfo,loc_id, loc_from);
    }
  }
}


// ���������  Iq type = result
if(!strcmp(gres,iqtype)) 
{
  if(!strcmp(id,auth_id))   // �����������
  {
    Jabber_state = JS_AUTH_OK;
    CList_AddContact(My_JID, "(Me)", SUB_BOTH,0,0);
    SUBPROC((void*)Send_Roster_Query);
  }
  
  if(!strcmp(id,rost_id))   // ������ �������
  {
    XMLNode* query;
    if(!(query = XML_Get_Child_Node_By_Name(nodeEx, "query")))return;    
    char* q_type = XML_Get_Attr_Value("xmlns", query->attr);
    if(!q_type)return;
    if(!strcmp(q_type,IQ_ROSTER))
    {
      // jabber:iq:roster
      FillRoster(query->subnode);
    }    
  }
  
  if(!strcmp(id,vreq_id))   // ������ ������ (�����)
  {
    XMLNode* query;
    if(!(query = XML_Get_Child_Node_By_Name(nodeEx, "query")))return;    
    char* q_type = XML_Get_Attr_Value("xmlns", query->attr);
    if(!q_type)return;
    if(!strcmp(q_type,IQ_VERSION))
    {
      char no_os[]="(��� ������)";
      char* vers_os_str;
      XMLNode *cl_name=XML_Get_Child_Node_By_Name(query, "name");
      XMLNode *cl_version=XML_Get_Child_Node_By_Name(query, "version");
      XMLNode *cl_os=XML_Get_Child_Node_By_Name(query, "os");
      if(cl_os)
      {
        vers_os_str = cl_os->value;
      }
      else
      {
        vers_os_str=no_os;
      }
      //��������� ���������
      char *reply=malloc(512);
      snprintf(reply, 512,"������ �������:\n���:%s\n������:%s\n��:%s",cl_name->value, cl_version->value, vers_os_str);
      CList_AddMessage(from, MSG_SYSTEM, reply);      
      ShowMSG(1,(int)reply);
      mfree(reply);
    }    
    
  }  

}

// ���������  Iq type = set
if(!strcmp(gset,iqtype)) 
{
    XMLNode* query;
    if(!(query = XML_Get_Child_Node_By_Name(nodeEx, "query")))return;    
    char* q_type = XML_Get_Attr_Value("xmlns", query->attr);
    if(!q_type)return;
    
    if(!strcmp(q_type,IQ_ROSTER))
    {
      // jabber:iq:roster
      ChangeRoster(query->subnode);
    }   
}


if(!strcmp(gerr,iqtype)) // Iq type = error
{

  // ����������� ������
  XMLNode* error = XML_Get_Child_Node_By_Name(nodeEx, "error");
  if(!error)return;
  char* errcode = XML_Get_Attr_Value("code", error->attr);
//  Jabber_state = JS_ERROR;
  if(errcode)sprintf(logmsg,"ERR:%s",errcode);
  ShowDialog_Error(1,(int)logmsg);
  if(!strcmp(id,auth_id))
  {
    Jabber_state = JS_AUTH_ERROR;
    strcat(logmsg,"\nAuth error!");
  }
  
}

}

/*
�������� :)
*/
void Process_Presence_Change(XMLNode* node)
{
  CONF_PRIV priv;
  char* from = XML_Get_Attr_Value("from",node->attr);
  if(!from)return;
  
  char status;
  char* msg=NULL;
  char* pr_type = XML_Get_Attr_Value("type",node->attr);
  if(pr_type)
  {
    status = GetPresenceIndex(pr_type);    
  }
  else
  {
    XMLNode* status_node = XML_Get_Child_Node_By_Name(node,"show");
    if(!status_node)
    {
      status = PRESENCE_ONLINE;
    }
    else
    {
      status = GetPresenceIndex(status_node->value);    
    }
    
    XMLNode* statusmsg_node = XML_Get_Child_Node_By_Name(node,"status");
    if(statusmsg_node)msg = statusmsg_node->value;
  }  
  
   // ��������������� ������, ��� ������� ��� ���-�� �� �����������. ��� �����.
    XMLNode* x_node = XML_Get_Child_Node_By_Name(node,"x");
    if(x_node)
    {
    if(!strcmp(XML_Get_Attr_Value("xmlns", x_node->attr), XMLNS_MUC)) // ������� �� ����������� 
    {
      CLIST* Conference = CList_FindContactByJID(from);
      // �������� �������� ���� error (��� �������� �� ��������� ������ ������)
      XMLNode* err_node = XML_Get_Child_Node_By_Name(node,"error");
      if(err_node)  // ���� ������!
      {
        // ���� ����� ������
        XMLNode* err_desc = XML_Get_Child_Node_By_Name(err_node,"text");
        if(err_desc->value)msg = err_desc->value;
        ShowDialog_Error(1,(int)err_desc->value);
        CList_AddSystemMessage(Conference->JID,PRESENCE_OFFLINE, err_desc->value);
      }
      
    }
// ��� ������
char loc_actor[]="actor";
char loc_jid[]="jid";
char loc_reason[]="reason";
static char r[128];       // ������, ����� �� ����� � ��� ���������� ���������

    if(!strcmp(XML_Get_Attr_Value("xmlns", x_node->attr), XMLNS_MUC_USER)) // ������� �� ����������� � ������������
    {
      
      // ������� ��������� �����, � ������� �� ����������
      CLIST* Conference = CList_FindContactByJID(from);
      if(!Conference)
      {
        return;
      }
      char* nick = Get_Resource_Name_By_FullJID(from);
      
      // ��� ����� ������������ ������� �����/������ � �����
      // ��� ����������, ��� ����� (����� ��, �����...)
      XMLNode* item = XML_Get_Child_Node_By_Name(x_node,"item");
      if(status==PRESENCE_ONLINE) // ����
      {
        TRESOURCE* ResEx = CList_IsResourceInList(from);
        char* affiliation = XML_Get_Attr_Value("affiliation", item->attr);
        char* role =  XML_Get_Attr_Value("role", item->attr);
        if(ResEx)
        {
          sprintf(r, "%s ������ %s � %s", nick, affiliation, role);
        }
        else sprintf(r, "%s ������������� ��� %s � %s", nick, affiliation, role);
        priv.aff = (JABBER_GC_AFFILIATION)GetAffRoleIndex(affiliation);
        priv.role = (JABBER_GC_ROLE)GetAffRoleIndex(role);        
        CList_AddSystemMessage(Conference->JID,PRESENCE_ONLINE, r);
      }
      

      if(status==PRESENCE_OFFLINE) // �����
      {
        sprintf(r, "%s �����", nick);
        CList_AddSystemMessage(Conference->JID,PRESENCE_OFFLINE, r);
      }
      
      
      // �������� �������� ���� �������
      XMLNode* sstatus = XML_Get_Child_Node_By_Name(x_node,"status");
      if(sstatus)  // ���� ������!
      {
        // �������� ��� �������
        char* st_code=XML_Get_Attr_Value("code", sstatus->attr);
        sprintf(r,"%s - [Unknown action]",nick);
        // ������ ���� �������� - ������ �������:)
        if(!strcmp(st_code, MUCST_KICKED)) sprintf(r, MUCST_R_KICK,nick); // ��������� � ����
        if(!strcmp(st_code, MUCST_BANNED)) sprintf(r, MUCST_R_BAN, nick); // ��������� � ����
        if(!strcmp(st_code, MUCST_CHNICK)) sprintf(r, MUCST_R_CHNICK, nick,  XML_Get_Attr_Value("nick", item->attr)); // ��������� � ����� ����
        //sprintf(r,r,nick);
        XMLNode* item = XML_Get_Child_Node_By_Name(x_node,"item");
        if(item)
        {
          XMLNode* actor = XML_Get_Child_Node_By_Name(item,loc_actor); // ����� ������ ��� ��������� �������, ��� �������� �����������
          if(actor)strcat(r, XML_Get_Attr_Value(loc_jid, actor->attr));

          XMLNode* reason = XML_Get_Child_Node_By_Name(item,loc_reason); // � ����� ��� ���� �������, �� ���?
          if(reason)
          {
            strcat(r, "; Reason: ");
            strcat(r, reason->value);
          }
                    
          
        }
        ShowDialog_Error(1,(int)r);
        CList_AddSystemMessage(Conference->JID,status, r);
        msg = r;
      }
      
    }    
    
    }  
  CList_AddResourceWithPresence(from, status, msg);
  CList_MUC_SetRole(from, priv);
}

MESS_TYPE Get_Message_Type(char* mess_type_str)
{
  char m_chat[]=MSGSTR_CHAT;
  char m_gchat[]=MSGSTR_GCHAT;
  if(!mess_type_str)return MSG_NORMAL;
  if(!strcmp(mess_type_str,m_chat ))return MSG_CHAT;
  if(!strcmp(mess_type_str,m_gchat ))return MSG_GCHAT;
  return MSG_NORMAL;
}

//Context: HELPER
void _mucadmincmd(char* room, char* iq_payload)
{
  //char* to, char* type, char* id, char* xmlns, char* payload
  char admin_iqid[]="SieJC_adm";
  char iqtypeset[]=IQTYPE_SET;
  SendIq(room, iqtypeset, admin_iqid, XMLNS_MUC_ADMIN, iq_payload);
  mfree(iq_payload);
  mfree(room);
}

// ���������� ���������������� ������
void MUC_Admin_Command(char* room_name, char* room_jid, MUC_ADMIN cmd, char* reason)
{
  char* payload = malloc(1024);
  char payload_tpl[]="<item nick='%s' %s='%s'><reason>%s</reason></item>";
  char* utf8_roomname= ANSI2UTF8(room_name, 128);
  char* utf8_roomjid= ANSI2UTF8(room_jid, 128);
  char it[20];
  char val[20];
  char aff[]="affiliation";
  char role[]="role";
/*
const char* JABBER_AFFS[] = {"none", "outcast", "member", "admin", "owner"};
const char* JABBER_ROLS[] = {"none", "visitor", "participant", "moderator"};
*/
  switch(cmd)
  {
  case ADM_KICK:
    {
      strcpy(it,role);
      strcpy(val,JABBER_ROLS[ROLE_NONE]);
      break;
    }
  case ADM_BAN:
    {
      strcpy(it,aff);
      strcpy(val,JABBER_AFFS[AFFILIATION_NONE]);
      break;
    }
    
  }
  
  snprintf(payload, 1023, payload_tpl, utf8_roomjid, it, val, reason);
//  ShowMSG(1,(int)payload);
  SUBPROC((void*)_mucadmincmd, utf8_roomname, payload);
//  mfree(payload);
//  mfree(utf8_roomname);
  mfree(utf8_roomjid);
}

/*
�������� ���������
*/
void Process_Incoming_Message(XMLNode* nodeEx)
{
  XMLNode* msgnode = XML_Get_Child_Node_By_Name(nodeEx,"body");
  if(msgnode)
  if(msgnode->value)
  {
    MESS_TYPE msgtype = Get_Message_Type(XML_Get_Attr_Value("type",nodeEx->attr));
    
    // �� ���������� ������ ��� ��������, ��� ������ ������� ���
    if(msgtype!=MSG_GCHAT)
    {
      char* m = malloc(100+strlen(msgnode->value));
      sprintf(m,"%s: %s", XML_Get_Attr_Value("from",nodeEx->attr), msgnode->value);
      ShowMSG(1,(int)m);
      mfree(m);
    }
    CList_AddMessage(XML_Get_Attr_Value("from",nodeEx->attr), msgtype, msgnode->value);
    start_vibra(1);
  }  
}

/*
�������� ���������� ����� ������� ���� ����������� �� ������ � �����������
*/
unsigned short GetPresenceIndex(char* presence_str)
{
  if(!presence_str)return 0;
  for(int i=0;i<PRES_COUNT;i++)
  {
    if(!strcmp(presence_str, PRESENCES[i]))return i;
  }
  return 0;
}

/*
�������� ���������� ����� ������� ���� ����/���������� �� ������ � �����������
*/
unsigned short GetAffRoleIndex(char* str)
{
  if(!str)return 0;
  int i;
  for(i=0;i<AFFS_CNT;i++)
  {
    if(!strcmp(str, JABBER_AFFS[i]))return i;
  }
  for(i=0;i<ROLS_CNT;i++)
  {
    if(!strcmp(str, JABBER_ROLS[i]))return i;
  }

  return 0;
}
