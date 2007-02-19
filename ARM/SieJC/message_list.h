/*
    SieNatJabber Project
    ������� ������ ��������� ���������    
*/
#ifndef _MESSAGE_LIST_H_
  #define _MESSAGE_LIST_H_

// ��������, ������� � �������� �������� �������
#define HIST_DISP_OFS 20

// �����. ����� ������ :)
#define MESSAGEWIN_BGCOLOR 17
#define MESSAGEWIN_TITLE_BGCOLOR 7
#define MESSAGEWIN_TITLE_FONT 1

#define MESSAGEWIN_MY_BGCOLOR 10
#define MESSAGEWIN_CH_BGCOLOR 13
#define MESSAGEWIN_SYS_BGCOLOR 0
#define MESSAGEWIN_STATUS_BGCOLOR 0


// �����, ��� ����� �������������� ��� ������ �����
typedef struct
{
  MESS_TYPE mtype;
  WSHDR* mess;
  void* next;
}DISP_MESSAGE;

// ���������� ������ ���������
void Display_Message_List(TRESOURCE* ContEx);

// ������� ���� ��������� ������ ���������
void Init_Message(TRESOURCE* ContEx);

void ParseMessagesIntoList(TRESOURCE* ContEx);
#endif
