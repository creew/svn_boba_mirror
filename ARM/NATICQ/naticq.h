#ifndef _NATICQ_H_
  #define _NATICQ_H_
/*
  ���� ������, ����� ��� ���� ������� �������
*/

typedef struct
{
  void *next;
  unsigned int uin;
  char name[64];
  unsigned short state;
  int isunread;
  char *log;
  char *answer;
}CLIST;

#endif
