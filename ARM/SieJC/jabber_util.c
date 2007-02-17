#include "../inc/swilib.h"
#include "SieJC.h"
#include "history.h"
#include "main.h"
#include "clist_util.h"
#include "jabber_util.h"
#include "xml_parser.h"
#include "jabber.h"

extern const char JABBER_HOST[];
extern const unsigned int JABBER_PORT;
extern const char USERNAME[];  
extern const char PASSWORD[];
extern const char RESOURCE[];
extern const char VERSION_NAME[];
extern const char VERSION_VERS[];
extern const char OS[];
extern char My_JID_full[];
extern char My_JID[];
extern char logmsg[];

extern JABBER_STATE Jabber_state;

const char* PRESENCES[PRES_COUNT] = {"online", "unavailable", "error", "chat", "away", "xa", "dnd", "invisible"};
const unsigned short PRES_COLORS[PRES_COUNT]  = {15,        22,             14,     16,     3,      18,   2,      20};


/*
  ������� ������������ Jabber Iq
*/
//Context: HELPER
void SendIq(char* to, char* type, char* id, char* xmlns, char* payload)
{
  char* xmlq=malloc(1024);
  char *xmlq2=malloc(1024);
  char s_to[40];
  sprintf(xmlq, "<iq type='%s' id='%s' from='%s'", type, id, My_JID_full);
  if(to)
  {
    sprintf(s_to, " to='%s'", to);
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
  sprintf(buf,streamheader,JABBER_HOST);
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
//  connect_state = 0;
  char streamend[]="</stream:stream>";
  SendAnswer(streamend);
  strcpy(logmsg,"Send Disconnect");
#ifdef LOG_ALL
  Log("CONN",logmsg);
#endif 
}

// ��������� Iq-�������� ����������� � �������
char auth_id[] = "SieJC_auth_req";
char rost_id[] = "SieJC_rost_req";

/*
  ����������� �� Jabber-�������
  ����� �����, ��� ����������.
*/
//Context: HELPER
void Send_Auth()
{
  sprintf(My_JID, "%s@%s",USERNAME, JABBER_HOST);
  sprintf(My_JID_full,"%s/%s",My_JID, RESOURCE);
  char* payload = malloc(256);
  sprintf(payload,"<username>%s</username>\n<password>%s</password>\n<resource>%s</resource>",USERNAME, PASSWORD, RESOURCE);
  SendIq(NULL, "set", auth_id, IQ_AUTH, payload);  
  LockSched();
  strcpy(logmsg,"Send auth");
  UnlockSched();
#ifdef LOG_ALL
  Log("USER->", logmsg);
#endif 
}

void Send_VReq()
{
  char to[]="kibab612@jabber.ru/Miranda_Home";
  char typ[]="get";
  char iqid[]="SieJC_VR";
  char iq_v[]=IQ_VERSION;
  SendIq(to, typ, iqid, iq_v, NULL);
  return;
}


/*
  ������� ��� ����������� (� ���������, ����� ����� �� ��� ������������ 
  ����������� ���������, � �� ���������� � �������� ������ ��������)
*/
// Context: HELPER
void Send_Presence()
{
  char presence[]="<presence><priority>-100</priority><status></status></presence>";
  SendAnswer(presence);
  LockSched();
  strcpy(logmsg,"Send presence");
  UnlockSched();
#ifdef LOG_ALL
  Log("USER->", logmsg);
#endif 
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
#ifdef LOG_ALL
  Log("USER->", logmsg);
#endif  
}

// Context: HELPER
void Report_VersionInfo(char* id, char *to)
{
  char answer[100];
 
  sprintf(answer, "<name>%s</name><version>%s</version><os>%s</os>", VERSION_NAME, VERSION_VERS, OS);
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
  char* res_name = (char*)(strstr(full_jid, "/") +1);
  return res_name;
}

void FillRoster(XMLNode* items)
{
  XMLNode* rostEx = items;
  int i=0;
  char* name;
  while(rostEx)
  {
    
    JABBER_SUBSCRIPTION r_subscr=GetSubscrType(XML_Get_Attr_Value("subscription",rostEx->attr));
    name = XML_Get_Attr_Value("name",rostEx->attr);//convUTF8_to_ANSI(XML_Get_Attr_Value("name",rostEx->attr));
    CList_AddContact(XML_Get_Attr_Value("jid",rostEx->attr),
                          name,
                          r_subscr,
                          0
                          );
   //if(name)mfree(name);
    
   rostEx=rostEx->next;
   i++;
  }
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
  const char iq_version[]=IQ_VERSION;

  iqtype = XML_Get_Attr_Value("type",nodeEx->attr);
  from = XML_Get_Attr_Value("from",nodeEx->attr);
  id = XML_Get_Attr_Value("id",nodeEx->attr);
  
// ��������� ������� ������������ ���������
if(!iqtype || !id) return;

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
        char* loc_id=malloc(strlen(id)+1);
        strcpy(loc_id,id);        
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
    CList_AddContact(My_JID, "(Me)", SUB_BOTH,0);
    SUBPROC((void*)Send_Roster_Query);
    //Send_Presence();
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
  
  
  
}

if(!strcmp(gerr,iqtype)) // Iq type = error
{

  // ����������� ������
  XMLNode* error = XML_Get_Child_Node_By_Name(nodeEx, "error");
  if(!error)return;
  char* errcode = XML_Get_Attr_Value("code", error->attr);
  Jabber_state = JS_ERROR;
  if(errcode)sprintf(logmsg,"ERR:%s",errcode);
  ShowMSG(1,(int)logmsg);
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
    
    XMLNode* statusmsg_node = XML_Get_Child_Node_By_Name(node,"show");
    if(statusmsg_node)msg = statusmsg_node->value;
  }  
  CList_AddResourceWithPresence(from, status, msg);
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
    char* m = malloc(100+strlen(msgnode->value));
    sprintf(m,"%s: %s", XML_Get_Attr_Value("from",nodeEx->attr), msgnode->value);
    ShowMSG(1,(int)m);
    mfree(m);
    CList_AddMessage(XML_Get_Attr_Value("from",nodeEx->attr), MSG_CHAT, msgnode->value);
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
  �������������� ������ ������ �� ��������� UTF-8 � ANSI
IN:
  - tmp_out: ���� �������� ���������. ����� ��� ������ ������������
             � � ��� ������ ���� ���������� �����
  - UTF8_str: ������ ����� ������ ��� ��������������
  - size: ������� ����� ������ ��� �������������� (UTF8_str)
  - fact - ���� �������� �������� ������ ������ � ������

OUT:  �������������� �����. 
*/
void* convUTF8_to_ANSI(char* tmp_out, char *UTF8_str, unsigned int size, int* fact)
{
  // ������������ ������ UTF8 ��� �������, ���������� � �����
  if(!UTF8_str)return NULL;
  int st_len = size;

  // �������� ������ - �� ������ ������ ������� ��. ��� ���������� ������,
  // ����� ���� ����������� ����� - �� ������� �����. ������� �����, ������ �����,
  // ������ ������, ������ � ����� ������� realloc
  int lastchar = 0;
  int dummy;
  //char* tmp_out =dest_buffer;// malloc(st_len); // ��� 0x00 ����� �� �����������, ��� ���
  char chr, chr2, chr3;
  for(int i=0;i<st_len;i++)
  {
  chr = (*(UTF8_str+i));

	if (chr<0x80)
        {
          *(tmp_out+lastchar)=chr;
          lastchar++;
          goto L_END_CYCLE;
        }
	if (chr<0xc0)
        {
          ShowMSG(1,(int)"Bad UTF-8 Encoding encountered (chr<0xC0)");
          mfree(tmp_out);
          return NULL;
        }
	
        chr2 = *(UTF8_str+i+1);

        if (chr2<0x80)
        {
          ShowMSG(1,(int)"Bad UTF-8 Encoding encountered (chr2<0x80)");          
          mfree(tmp_out);
          return NULL;
        }
	
	if (chr<0xe0) {
	    // cx, dx 
	    char test1 = (chr & 0x1f)<<6;
            char test2 = chr2 & 0x3f;
            *(tmp_out+lastchar)= test1 | test2 + 127 + 0x31;
            i++;
            lastchar++;
            goto L_END_CYCLE;
	}
	if (chr<0xf0) {
	    // cx, dx 
	    chr3= *(UTF8_str+i+2);

	    if (chr3<0x80)
            {
              ShowMSG(1,(int)"Bad UTF-8 Encoding encountered");          
              mfree(tmp_out);
              return NULL;
            }              
	    else
            {
              *(tmp_out+lastchar) =  ((chr & 0x0f)<<12) | ((chr2 &0x3f) <<6) | (chr3 &0x3f);
              i=i+2;
            }
	}

  L_END_CYCLE:
    dummy++;
  }
  tmp_out = realloc(tmp_out,lastchar);
  *fact = lastchar;
  return tmp_out;
}


/*
// ��������� �������
char* convUTF8_to_ANSI(char *UTF8_str)
{
  // ������������ ������ UTF8 ��� �������, ���������� � �����
  if(!UTF8_str)return NULL;
  int st_len = strlen(UTF8_str);

  // �������� ������ - �� ������ ������ ������
  int lastchar = 0;
  int dummy;
  char* tmp_out = malloc(st_len+1);
  zeromem(tmp_out,st_len+1);
  char chr, chr2, chr3;
  for(int i=0;i<st_len;i++)
  {
  chr = (*(UTF8_str+i));

	if (chr<0x80)
        {
          *(tmp_out+lastchar)=chr;
          lastchar++;
          goto L_END_CYCLE;
        }
	if (chr<0xc0)
        {
          ShowMSG(1,(int)"Bad UTF-8 Encoding encountered (chr<0xC0)");
          mfree(tmp_out);
          return NULL;
        }
	
        chr2 = *(UTF8_str+i+1);

        if (chr2<0x80)
        {
          ShowMSG(1,(int)"Bad UTF-8 Encoding encountered (chr2<0x80)");          
          mfree(tmp_out);
          return NULL;
        }
	
	if (chr<0xe0) {
	    // cx, dx 
	    char test1 = (chr & 0x1f)<<6;
            char test2 = chr2 & 0x3f;
            *(tmp_out+lastchar)= test1 | test2 + 127 + 0x31;
            i++;
            lastchar++;
            goto L_END_CYCLE;
	}
	if (chr<0xf0) {
	    // cx, dx 
	    chr3= *(UTF8_str+i+2);

	    if (chr3<0x80)
            {
              ShowMSG(1,(int)"Bad UTF-8 Encoding encountered");          
              mfree(tmp_out);
              return NULL;
            }              
	    else
            {
              *(tmp_out+lastchar) =  ((chr & 0x0f)<<12) | ((chr2 &0x3f) <<6) | (chr3 &0x3f);
              i=i+2;
            }
	}

  L_END_CYCLE:
    dummy++;
  }
  st_len = strlen(tmp_out);
  tmp_out = realloc(tmp_out,st_len+1);
  return tmp_out;
}
*/
