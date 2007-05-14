/*
    SieNatJabber Project
    ������� ������ � �������-������

    ������ API ������ �������������� �� ���� ��������� �������-�����
*/

#ifndef _CLIST_UTIL_H_
  #define _CLIST_UTIL_H_

#include "main.h"

#define CHAR_ON_LINE 20

#define CLIST_FONT FONT_SMALL     // ����� �������-�����

#define CLIST_Y1 5 + SCR_START               // ad: ������ ������ (26->5)
/* ACHTUNG
#ifdef STD_PALETTE
#define CURSOR 6                 // ad: ���� �������

#define CURSOR_BORDER 20         // ad: ���� ������ �������

#define CLIST_F_COLOR_0 1         // ���� ������
#define CLIST_F_COLOR_1 15        // ���� ������ (���� ���������)
#define CONTACT_BG_0 0
#define CONTACT_BG_1 22
#else
const RGBA CURSOR = {80, 80, 255, 100};
const RGBA CURSOR_BORDER = {200, 200, 200, 100};
const RGBA CLIST_F_COLOR_0 = {0, 0, 0, 100};
const RGBA CLIST_F_COLOR_1 = {0, 0, 170, 100};
const RGBA CONTACT_BG_0 = {255, 255, 255, 100};
const RGBA CONTACT_BG_1 = {220, 220, 220, 100};
#endif*/

// ���������� ������ ���������
  void CList_RedrawCList();

// �������� ���������� �������� �� FullJID (JID ������ � ��������)
  CLIST* CList_FindContactByJID(char* jid);

// ������, ���� �� ��� ����� ������ � ��������, �� FullJID
  TRESOURCE* CList_IsResourceInList(char* jid);

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

// ����� ���� �������� � ����� � ���������
  void CList_MUC_SetRole(char* jid, CONF_PRIV priv);

// �������� ��������� � ������ ��������� ��������
  void CList_AddMessage(char* jid, MESS_TYPE mtype, char* mtext);

// �������� ��������� ���������
  void CList_AddSystemMessage(char* jid, char status, char* status_msg);
// ��� ��, �� ��������� ANSI-������. ������� � ��������� ������� :)
  void CList_AddSystemMessageA(char* jid, char status, char* ansi_status_msg);

  TRESOURCE* CList_AddResourceWithPresence(char* jid, char status, char* status_msg);

// �������� �������, �� ������� ����� ������
  TRESOURCE* CList_GetActiveContact();

// ������ ���������� ��� ������� ��������� ��������. Useful ��� �����������.
  void CList_MakeAllResourcesOFFLINE(CLIST* ClEx);

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

// ����� ������ ���������
void KillMsgList(LOG_MESSAGE* messtop);

#endif
