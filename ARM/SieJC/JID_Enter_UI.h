/*
    SieNatJabber Project
    ���� � �����������
*/

#ifndef _JID_UTIL_H_
  #define _JID_UTIL_H_
typedef struct {
  char jid_sub;
  char jid_ask;
  char jid_del;
  char jid_add;
} JIDENTER_SETTINGS;
// �������� ������ Add/Change. ���� ClEx=NULL �� ����� ���������� ��������
void Disp_JID_Enter_Dialog(CLIST* ClEx);

#endif
