/*
    SieNatJabber Project
    ������� ������ � �������-������

    ������ API ������ �������������� �� ���� ��������� �������-�����
*/

#ifndef _CLIST_UTIL_H_
  #define _CLIST_UTIL_H_

#include "main.h"

// ���������� �������� �� ������
#define CHAR_ON_LINE 22

#define CLIST_FONT SMALL_FONT   // ����� �������-�����
#define CLIST_Y1 26
// ������ ������ �������-�����
#define CLIST_F_COLOR_0 1         // ���� ������ 
#define CLIST_F_COLOR_1 15         // ���� ������ (���� ���������)

// ���������� ������ ���������
  void CList_RedrawCList();

// �������� � ����� �������. ���������� ��������� ���������� ��������.
  CLIST* CList_AddContact(char* jid,
                          char* name,
                          JABBER_SUBSCRIPTION subscription,
                          char group
                          );


// �������� ���������� �������� �� JID
  //TRESOURCE* CList_FindContactByJID(char* jid);

// �������� ��������� � ������ ��������� ��������
  void CList_AddMessage(char* jid, MESS_TYPE mtype, char* mtext);

  TRESOURCE* CList_AddResourceWithPresence(char* jid, char status, char* status_msg);

  TRESOURCE* CList_GetActiveContact();
  
// ���������� ������ ���������  
  void CList_Destroy();

// �������� ����� ����������� ��������� �������
  void CList_ToggleOfflineDisplay();

// �������� ����� ������-�������
  unsigned int CList_GetNumberOfOnlineUsers();
  
// ���������� ��������
  void CList_MoveCursorUp();
  void CList_MoveCursorDown();

  void CList_MoveCursorHome();
  void CList_MoveCursorEnd();
  
#endif
