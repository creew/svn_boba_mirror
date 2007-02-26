/*
    SieNatJabber Project
    ������� ������ � �������-������

    ������������� ��� �������� ������ XML
*/

#ifndef _JABBER_UTIL_H_
  #define _JABBER_UTIL_H_

#include "xml_parser.h"

#define MAX_MSG_LEN 4096
//#define MAX_MSG_LEN 128 // ���! ����? ��� ���� ����! ����� ��������...

#define PRES_COUNT 12
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

// ����� �� �����������, ��� � ANSI
void Leave_Conference(char* room);
#endif
