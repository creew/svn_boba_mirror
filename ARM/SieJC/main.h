#ifndef _MAIN_H_
  #define _MAIN_H_

#include "jabber.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////// ��������� �������������� ��������� ������� ////////////////
//#define LOG_ALL       // �������� ���������������� �������� / ��������� �������

#define VIBRA_POWER 10 // ���� ����� ��� ������� ������� ����

#define USE_PNG_EXT   // ������������ �� ���������� PNG ��� �������� (ELFLoader >2.0)
////////////////////////////////////////////////////////////////////////////////

#define TMR_SECOND 216

// ����� ������� ������ MAIN.C

#define XML_BUFFER_SIZE 64*1024 // ������ ������ � ������
#define REC_BUFFER_SIZE 512 // ������ ������ ����� � ������

typedef struct
{
  int buf_size;
  char* xml_buffer;
} IPC_BUFFER;


typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  GUI gui;
  WSHDR *ws1;
  WSHDR *ws2;
  int i1;
}MAIN_GUI;

typedef enum
{
  JS_NOT_CONNECTED, 
  JS_CONNECTED_STATE, 
  JS_AUTH_OK, 
  JS_AUTH_ERROR, 
  JS_ONLINE,
  JS_ERROR
} JABBER_STATE;

////////////////////////////////
// ���-����� ���� ��������� �� http://www.xmpp.org/rfcs/rfc3921.html � + ����
typedef enum
{
  MSG_NORMAL, // ���� "������", �� �������, See RFC
  MSG_ME,     // �� ���� ��������
  MSG_CHAT,   // �� �������� �� ���
  MSG_GCHAT,   // �����������
  MSG_SYSTEM,  // ������� ���� ��������� ���������
  MSG_STATUS   // ��������� ���������
} MESS_TYPE;


// ���� ����� � ������ ���������
typedef enum
{   
  T_NORMAL,       // ������� (������ ������)
  T_VIRTUAL,      // �������� ��� �������� ��������, ��� ��������
  T_CONF_ROOT,    // ������ ����������� (����� �������� ��������� ���������)
  T_CONF_NODE     // �������� ����������� (���������� ������ "������" ����) 
}RES_TYPE;

typedef struct
{
  MESS_TYPE mtype;
  char* mess;
  void *next; 
}LOG_MESSAGE;

typedef struct
{
  JABBER_GC_AFFILIATION aff;
  JABBER_GC_ROLE role;
}CONF_PRIV;

typedef struct
{
  char* name;                   // ������������ ��� �������
  char* full_name;              // ������ JID (jid/resource)
  RES_TYPE entry_type;          // ��� ������ (��. RES_TYPE)
  char status;                  // ������
  char* status_msg;             // ��������� ���������
  unsigned int has_unread_msg;  // ���� �� ������������� � �������
  unsigned int total_msg_count; // ����� ���������� ���������
  LOG_MESSAGE* log;             // ������ ���� ���������
  CONF_PRIV muc_privs;          // ��� ��� ��������������� ��������� - ��� ����
  void* next;                   // ��������� ���������
} TRESOURCE;

typedef struct
{
  char* JID;
  int ResourceCount;
  TRESOURCE* res_list;
  char* name;
  JABBER_SUBSCRIPTION subscription;
  char wants_subscription;
  char group;     // >128 -> ����������� 
  void *next;
} CLIST;


// ��� ������ �����������
typedef struct
{
  char* conf_jid; //UTF-8
  void* next;
}MUC_ITEM;

// ��� ������� ����� ���������� ���������� � ����
typedef struct
{
  short priority;
  char status;
  char* message;
}PRESENCE_INFO;

void SendAnswer(char *str);

void start_vibra();
#endif
