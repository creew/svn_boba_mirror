#include "../inc/swilib.h"
#include "naticq.h"
#include "history.h"

extern const char HIST_PATH[64];
extern const int LOG_ALL;
/*
  �������� ������ � ������� �������� CLIST
*/
void Add2History(CLIST *CListEx, char *header, char *message)
{
  volatile int hFile;
  unsigned int io_error = 0;
  char error[36];
  const char delim[] = "\r\n----------\r\n";
  
  if (!LOG_ALL) return;
  char fullname[128];
  snprintf(fullname,127,"%s\\%u.txt", HIST_PATH, CListEx->uin);
  
  // ��������� ���� �� �������� � ������ � ������ �������
  hFile = fopen(fullname,A_ReadWrite + A_Append + A_BIN,P_READ+P_WRITE, &io_error);
  if(io_error==2) // ��� �����
  {
    hFile = fopen(fullname,A_ReadWrite+A_Create+ A_BIN,P_READ+P_WRITE, &io_error);
  }
  if(!io_error)
  {
    fwrite(hFile, delim, sizeof(delim)-1, &io_error);
    fwrite(hFile, header, strlen(header), &io_error);
    fwrite(hFile, message, strlen(message), &io_error);
    fclose(hFile, &io_error);
  }
  else
  {
    snprintf(error,35, "������ I/O  #%u", io_error);
    ShowMSG(1,(int)error); 
  }
}

void GetStatusById(char *buffer, int id)
{
  
  // ??? ��� ���� ����������� �� ���������
  // ������, �� ��� ���� ������...
switch(id)
{
case 0: {strcpy(buffer,"Online");break;}
case 1: {strcpy(buffer, "Invisible");break;}
case 2: {strcpy(buffer, "Away");break;}
case 3: {strcpy(buffer, "N/A");break;}
case 4: {strcpy(buffer, "Occupied");break;}
case 5: {strcpy(buffer, "DND");break;}
case 6: {strcpy(buffer, "Online");break;}
case 7: {strcpy(buffer, "FFC");break;}
default: {sprintf(buffer, "UNK: %d", id);}
}
}

void LogStatusChange(CLIST *CListEx)
{
  extern const int LOG_STATCH;
  if(!LOG_STATCH){return;}
  if(CListEx->state>7){return;}
  if(CListEx->log==NULL){return;}
  if(strlen(CListEx->log)==0){return;}
  char hdr[] = "(System message)";
  char msg[] = "%s ������ ������ �� %s\r\n";
  char message[100];
  char nickname[64];
  char status[20];
  if(CListEx->name)
  {
    strcpy(nickname, CListEx->name);
  }
  else
  {
    snprintf(nickname,63, "%u", CListEx->uin);
  }
  GetStatusById(status, CListEx->state);
  sprintf(message, msg, nickname, status);
  Add2History(CListEx, hdr, message);
}
