/*
    SieNatJabber Project
    ������� ������ � �������-������

    ������ API ������ �������������� �� ���� ��������� �������-�����
*/

#ifndef _CLIST_UTIL_H_
  #define _CLIST_UTIL_H_

#include "main.h"

#define CHAR_ON_LINE 22

#define CURSOR 6                 // ad: ���� �������

#define CURSOR_BORDER 20         // ad: ���� ������ �������

#define CLIST_FONT SMALL_FONT     // ����� �������-�����

#define CLIST_Y1 5               // ad: ������ ������ (26->5) 

#define CLIST_F_COLOR_0 1         // ���� ������ 
#define CLIST_F_COLOR_1 15        // ���� ������ (���� ���������)

// ���������� ������ ���������
  void CList_RedrawCList();

// �������� ���������� �������� �� FullJID (JID ������ � ��������)
  CLIST* CList_FindContactByJID(char* jid);

// �������� � ����� �������. ���������� ��������� ���������� ��������.
  CLIST* CList_AddContact(char* jid,
                          char* name,
                          JABBER_SUBSCRIPTION subscription,
                          char wants_subscription,
                          char group
                          );

// �������� ��������� �������� (�������� �������� � ��)

  void CList_ChangeContactParams(CLIST* Cont_Ex,
                          char* name,
                          JABBER_SUBSCRIPTION subscription,
                          char wants_subscription,
                          char group);
  


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
  
// �������� ����� �������
  unsigned int CList_GetNumberOfUsers();
  
// �������� ����� �������������
  unsigned int CList_GetUnreadMessages();
  
// ������� �� ��������� ������� � ������������� ����������
  void nextUnread();
  
// ������ ������ �� ������ ������(��� ���������� ���� ����������)
  void MoveCursorTo(TRESOURCE* NewResEx);

void CList_Display_Popup_Info(TRESOURCE* ResEx);
  
// ���������� ��������
  void CList_MoveCursorUp();
  void CList_MoveCursorDown();

  void CList_MoveCursorHome();
  void CList_MoveCursorEnd();
  
#endif
