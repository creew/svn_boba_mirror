#ifndef _BOOKMAN_DAEMON_H_
#define _BOOKMAN_DAEMON_H_

//������� ������
void CreateDaemon( void );

//������� ������
void DestroyDaemon( void );

//��� ������
int isBookmanDaemonBook( BOOK* book );

typedef struct _DaemonBook : BOOK
{
  wchar_t ActiveTAB;
}DaemonBook;

#endif
