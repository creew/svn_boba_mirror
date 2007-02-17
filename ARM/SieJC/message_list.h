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

// ���������� �������� �� ������
#define CHAR_ON_LINE 22

// �����, ��� ����� �������������� ��� ������ �����
typedef struct
{
  MESS_TYPE mtype;
  char* mess;
  void* next;
}DISP_MESSAGE;

// ���������� ������ ���������
void Display_Message_List(TRESOURCE* ContEx);

// ������� ���� ��������� ������ ���������
void Init_Message(TRESOURCE* ContEx);
#endif
