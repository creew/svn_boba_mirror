#include "..\inc\cfg_items.h"
#include "..\inc\swilib.h"

// ������:

// 1. ���������� ��������� ��� ��������� ����������� ������
__root const CFG_HDR cfghdr0={CFG_CHECKBOX,"Show message on activation?",0,2};
__root const int b_welcome_message=1;

// 2. ��������� ��� ���������
__root const CFG_HDR cfghdr1={CFG_STR_WIN1251,"Message on activation:",0,127};
__root const char s_welcome_message[128]="Phone loaded in safe mode!";

// 3. ������� �� ������
__root const CFG_HDR cfghdr2={CFG_CBOX,"Questions",0,3};
__root const int i_questions=1;
__root const CFG_CBOX_ITEM cfgcbox0[]={"none", "for all deamons", "for each deamon"}; // 0 - ������� ��������, ���� ������ �� ���������, 1 - ���� ������ ��� ���� �������, 2 - ��������� ������ ��� ������� ������
