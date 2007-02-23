/*
    SieNatJabber Project
    ������� ������ ��������� ���������    
*/
#ifndef _MESSAGE_LIST_H_
  #define _MESSAGE_LIST_H_

// ��������, ������� � �������� �������� �������
#define HIST_DISP_OFS 14

// �����. ����� ������ :)
//����� ���
#define MESSAGEWIN_BGCOLOR 0
//���������
#define MESSAGEWIN_TITLE_BGCOLOR 21
//���� ������
#define MESSAGEWIN_TITLE_FONT 1

//���������
#define MESSAGEWIN_MY_BGCOLOR 0
//��������
#define MESSAGEWIN_CH_BGCOLOR 22
#define MESSAGEWIN_GHAT_BGCOLOR 13
#define MESSAGEWIN_SYS_BGCOLOR 0
#define MESSAGEWIN_STATUS_BGCOLOR 0


/*
0 - �����
1 - ������
2 - �������
3 - �����
4 - �������
5 - ���������
6 - ����-����� )
7 - ������
8 - �����-�������
9 - �������
10 - ����-�������
11 - ���������
12 - ����-�������
13 - ����-������
14 - ��������
15 - �����-�����
16 - �������-�������
17 - �����-���������
18 - �������-�����-������� )
19 - ���������-�������� )
20 - ���������-�����
21 - �����
22 - ������-�����
23 - ����������
*/

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
