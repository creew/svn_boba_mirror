/*
    SieNatJabber Project
    ������� ������ � �������-������

    ������������� ��� �������� ������ XML
*/

#ifndef _JABBER_UTIL_H_
  #define _JABBER_UTIL_H_

#include "xml_parser.h"

//#define MAX_MSG_LEN 4096
#define MAX_MSG_LEN 128 // ���! ����? ��� ���� ����! ����� ��������...

#define PRES_COUNT 8
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
void Send_Presence(short priority, char status, char* message);

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

void SendMessage(char* jid, char* body);

// UTF8 ��� ��������
char* Correct_UTF8_String(char* utf8_jid);

// � ��� ������ ����� ���� ���� ������ (�) �������� �.�.
char* ANSI2UTF8(char* ansi_str, unsigned int maxlen);
#endif
