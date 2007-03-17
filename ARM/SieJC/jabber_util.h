/*
    SieNatJabber Project
    ������� ������ � �������-������

    ������������� ��� �������� ������ XML
*/

#ifndef _JABBER_UTIL_H_
  #define _JABBER_UTIL_H_

#include "xml_parser.h"

#define MAX_MSG_LEN 4096

#define PRES_COUNT 12

typedef enum
{
  ADM_KICK,             // ��� 
  ADM_BAN,              // ���
  ADM_VOICE_REMOVE,     // ������ ������
  ADM_VOICE_GRANT       // ������������ �����
}MUC_ADMIN;

/*
  ������� ������������ Jabber Iq
*/
void SendIq(char* to, char* type, char* id, char* xmlns, char* payload);

/*
  ������� �����������, �� ���� ������ �������� �������� stream.
  ����� ����� ����� �������� � ��������
*/
// Context:HELPER
void Send_Welcome_Packet();

void Send_Welcome_Packet_SASL();

/*
  ������� ����������
*/
void Send_Disconnect();

/*
  ����������� �� Jabber-�������
  ����� �����, ��� ����������.
*/
void Send_Auth();

/*
  ������� ��� ����������� (� ���������, ����� ����� �� ��� ������������ 
  ����������� ���������, � �� ���������� � �������� ������ ��������)
*/
// Context: HELPER
void Send_Presence(PRESENCE_INFO *pr_info);

// ������� ������ � ������ ������������ � ��������� JID
// JID ��������� � ANSI-���������
void Send_Version_Request(char *dest_jid);

/*
  ������� ������ �������
*/
// Context: HELPER
void Send_Roster_Query();

/*
 ��������� �������� Iq-��������
*/
void Process_Iq_Request(XMLNode* nodeEx);


/*
�������� :)
*/
void Process_Presence_Change(XMLNode* node);

/*
�������� ���������
*/
void Process_Incoming_Message(XMLNode* nodeEx);

// kibab612@jabber.ru/SieJC  => SieJC
char* Get_Resource_Name_By_FullJID(char* full_jid);

/*
�������� ���������� ����� ������� ���� ����������� �� ������ � �����������
*/
unsigned short GetPresenceIndex(char* presence_str);

/*
�������� ���������� ����� ������� ���� ����/���������� �� ������ � �����������
*/
unsigned short GetAffRoleIndex(char* str);

// ���������� ���������������� ������
void MUC_Admin_Command(char* room_name, char* room_jid, MUC_ADMIN cmd, char* reason);

typedef struct
{
  char IsGroupChat;
  char* body;  
}IPC_MESSAGE_S;

typedef struct
{
  char* room_nick;
  char* room_name;
  char* pass;
  int mess_num;
}MUC_ENTER_PARAM;

// Context: HELPER
void SendMessage(char* jid, IPC_MESSAGE_S *mess);

// ���������� ������ ������  
void MUCList_Destroy();

// ����� � �����������
void Enter_Conference(char *room, char *roomnick, char N_messages);

// ����� �� �����������
void Leave_Conference(char* room);

///////////////////
void Use_Md5_Auth_Report();
void Decode_Challenge(char *challenge);
void Destroy_SASL_Ctx();
void Send_Login_Packet();
void Process_Auth_Answer(char *challenge);
void SASL_Open_New_Stream();
void SASL_Bind_Resource();
void SASL_Init_Session();
void SASL_Process_Error(XMLNode *nodeEx);
////
void Compression_Ask();
void Compression_Init_Stream();
#endif
