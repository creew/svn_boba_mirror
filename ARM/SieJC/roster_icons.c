#include "../inc/swilib.h"
#include "main.h"
#include "jabber.h"
#include "clist_util.h"
#include "jabber_util.h"
#include "string_util.h"
#include "history.h"

unsigned int conference=0x18C;

unsigned int noauth=0x185;
unsigned int ask=0x185;

unsigned int message=0x24C;
unsigned int system=0x24C;

unsigned int online=0x22C;
unsigned int unavailable=0x306;
unsigned int error=0x185;
unsigned int chat=0x18C;
unsigned int away=0x22F;
unsigned int xa=0x17A;
unsigned int dnd=0x231;
unsigned int invisible=0x306;

extern const char* PRESENCES[PRES_COUNT];
extern const char PATH_TO_PIC[256];


#ifdef USE_PNG_EXT
char* Roster_getIconByStatus(char* path_to_pic, char status) //������ ���� � �������� �������� �� �������
{
  strcpy(path_to_pic, PATH_TO_PIC);

  // ���� ������������� ��������� ���
  if(status<PRESENCE_INVISIBLE) // ���� ���������� ������
  {
    strcat(path_to_pic, PRESENCES[status]);
  }
  else
    if(status==50)
    {
      strcat(path_to_pic, "message");
    }
    else
    {
      strcat(path_to_pic, PRESENCES[PRESENCE_OFFLINE]); // ����� ���� �������
    }
  strcat(path_to_pic, ".png");
  return path_to_pic;
}
#else
int Roster_getIconByStatus(char status) //������ ����� �������� �� �������
{
  unsigned int img_num=0;
  
  switch (status) {
    case 0:
      img_num=online;
      break;
    case 1:
      img_num=unavailable;
      break;
    case 2:
      img_num=error;
      break;
    case 3:
      img_num=chat;
      break;
    case 4:
      img_num=away;
      break;
    case 5:
      img_num=xa;
      break;
    case 6:
      img_num=dnd;
      break;
    case 7:
      img_num=invisible;
      break;
    case 8:
      img_num=noauth;
      break;
    case 9:
      img_num=ask;
      break;
    case 10:
      img_num=noauth;
      break;
    case 11:
      img_num=ask;
      break;
    case 50: // ������ ���������, �� ������ ��� ������ � ���� ������, �� ����� ���� �����
      img_num=message;
      break;
  }
  return img_num;
}
#endif

#ifdef USE_PNG_EXT
void Roster_getIcon(char* path_to_pic, CLIST* ClEx, TRESOURCE* resEx) 
{
          strcpy(path_to_pic, PATH_TO_PIC);
          
          // ���� ��� �����������
          if(resEx->entry_type == T_CONF_ROOT && !resEx->has_unread_msg){strcat(path_to_pic, "conference");goto L_DONE;}
          
          // ���� � ��� ��� �������� � � �������� ��� ������������� ���������
          if(((ClEx->subscription== SUB_FROM) || (ClEx->subscription== SUB_NONE))&& !resEx->has_unread_msg)
          {
            strcat(path_to_pic, "noauth"); // ������ "��� �����������"
            goto L_DONE;
          }
          
          // ���� ����� ���� ������� ��������
          if(ClEx->wants_subscription)
          {
            strcat(path_to_pic, "ask");
            goto L_DONE;
          }          
          
          // ���� � ����������� �������� ���� ������������� ���������
          if(resEx->has_unread_msg)
          {
            // ���� � ���� � ���� �� � ������ ����������
            if(resEx->status<PRESENCE_INVISIBLE)
            {
              strcat(path_to_pic, "message");
            }
            else
            {
              strcat(path_to_pic, "system");      // � ����� �� ���-�� ������� � ���������  
            }
          }
          else
          {
            // ���� �� ������������� ��������� ���
            if(resEx->status<PRESENCE_INVISIBLE) // ���� ���������� ������
            {
              strcat(path_to_pic, PRESENCES[resEx->status]);
            }
            else
            {
              strcat(path_to_pic, PRESENCES[PRESENCE_OFFLINE]); // ����� ���� �������
            }
          }
        L_DONE:
          strcat(path_to_pic, ".png");

}
#else
int Roster_getIcon(CLIST* ClEx, TRESOURCE* resEx) {
unsigned int img_num=0;

          // ���� ��� �����������
          if(resEx->entry_type == T_CONF_ROOT && !resEx->has_unread_msg)
            img_num=conference;
          
          // ���� � ��� ��� �������� � � �������� ��� ������������� ���������
          if(((ClEx->subscription== SUB_FROM) || (ClEx->subscription== SUB_NONE))&& !resEx->has_unread_msg)
            img_num=noauth;
          
          // ���� ����� ���� ������� ��������
          if(ClEx->wants_subscription)
            img_num=ask;  
          
          // ���� � ����������� �������� ���� ������������� ���������
          if(resEx->has_unread_msg)
          {
            // ���� � ���� � ���� �� � ������ ����������
            if(resEx->status<PRESENCE_INVISIBLE)
              img_num=message;
            else
              img_num=system;      // � ����� �� ���-�� ������� � ���������  
          }
          else
          {
            // ���� �� ������������� ��������� ���   
            img_num=Roster_getIconByStatus(resEx->status);
          }
    return img_num;
}
#endif

void Roster_DrawIcon(char x, char y,int img_num)
{
  DrawImg(x, y, img_num);
}
  
