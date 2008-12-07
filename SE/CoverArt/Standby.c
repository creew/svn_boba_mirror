#include "..\include\Lib_Clara.h"
#include "..\include\dir.h"
#include "structs.h"
#include "decoder_init.h"
#include "draw.h"
#include "config_data.h"
#include "Externals.h"

wchar_t lastmusic[1024];
wchar_t nowmus[1024];
bool isInStandby=false;
bool isAPBookFound=false;

DISP_DESC *Standby_DD;
DISP_OBJ *Standby_DO;
DISP_OBJ_ONREDRAW_METHOD Standby_DefaultRedraw;

DISP_DESC *Softkey_DD;
DISP_OBJ *Softkey_DO;
DISP_OBJ_ONREDRAW_METHOD Softkey_DefaultRedraw;

DISP_DESC *StatusIndication_DD;
DISP_OBJ *StatusIndication_DO;
DISP_OBJ_ONREDRAW_METHOD StatusIndication_DefaultRedraw;

void *GC=0;

u16 Standby_RedrawTimer;

int locTime=1000;
int locCover_X;
int locCover_Y;
int locCover_MaxX;
int locCover_MaxY;

extern LABEL labels[10];
extern UI_IMG img;
extern MUSIC nowmusic;
extern bool PlayerRunned;
extern TRACK_DESC *currenttrack;
extern int CurrentTime;

void Redraw(DISP_OBJ_ONREDRAW_METHOD DefRedraw, int DrawOld, signed int plusY, DISP_OBJ* DO, int a, int b, int c, int type, int DrawCover, int DrawCustom);
void Softkey_SetOnRedraw();
void StatusIndication_SetOnRedraw();

bool SoftUsed=false;
bool StatusUsed=false;

BOOK *Standby=0;
GUI * MainInput=0;

int CheckStandby()
{
  if (SESSION_GetTopBook(root_list_get_session(root_list_get_session_count() - 1))==Standby)
  {
    if (MainInput_getVisible(MainInput)==0)
    {
      return 1;
    }
  }
  return 0;
}

void Standby_CARedraw(DISP_OBJ* DO,int a,int b,int c)
{
  isInStandby=true;
  Redraw(Standby_DefaultRedraw, DrawStandby, (-1)*StatusSize, DO, a, b, c, isInStandby, Cover_DrawStandby, Custom_DrawStandby);
};

void Softkey_CARedraw(DISP_OBJ *DO,int a,int b,int c)
{
  if (DrawSoft2 && !isInStandby)
  {
    Redraw(Softkey_DefaultRedraw, DrawSoft2, SoftSize-YSize, DO, a, b, c, 1, Cover_DrawSoft, Custom_DrawSoft);
  }
  else
  {
    Redraw(Softkey_DefaultRedraw, DrawSoftkey, SoftSize-YSize, DO, a, b, c, isInStandby, Cover_DrawSoft, Custom_DrawSoft);
  }
};


void StatusIndication_CARedraw(DISP_OBJ *DO,int a, int b,int c)
{
  if (DrawStatus2 && !isInStandby)
  {
    Redraw(StatusIndication_DefaultRedraw, DrawStatus2, 0, DO, a, b, c, 1, Cover_DrawStatus, Custom_DrawStatus);
  }
  else
  {
    Redraw(StatusIndication_DefaultRedraw, DrawStatus, 0, DO, a, b, c, isInStandby, Cover_DrawStatus, Custom_DrawStatus);
  }
};

void Redraw(DISP_OBJ_ONREDRAW_METHOD DefRedraw, int DrawOld, signed int PlusY, DISP_OBJ* DO, int a, int b, int c, int IsInStandby, int DrawCover, int DrawCustom)
{
  if (DrawOld!=0)
  {
    if (IsInStandby && PlayerRunned)
    {
      if (DrawOld==2)
      {
        DefRedraw(DO,a,b,c);
      }
      GC=get_DisplayGC();
      if (CustomImage_DrawAfter==false)
      {
         if (img.isImage==true && CustomImage_Show==true && DrawCustom==true)
         {
            if (CustomImage_Draw==0)
            {
              putchar(GC,img.x,img.y+PlusY,img.mx,img.my,img.ImageID);
            }
            else if (CustomImage_Draw==1 && isInStandby==true)
            {
              putchar(GC,img.x,img.y+PlusY,img.mx,img.my,img.ImageID);
            }
            else if (CustomImage_Draw==2 && isInStandby==false)
            {
              putchar(GC,img.x,img.y+PlusY,img.mx,img.my,img.ImageID);
            }
          }
        }
        if (nowmusic.meta.HasImage)
        {
          if (COVER_SHOW==true && DrawCover==true)
          {
            if (COVER_DRAW==0)
            {
              putchar(GC,locCover_X,locCover_Y+PlusY,locCover_MaxX,locCover_MaxY,nowmusic.meta.ImageID);
            }
            else if (COVER_DRAW==1 && isInStandby==true)
            {
              putchar(GC,locCover_X,locCover_Y+PlusY,locCover_MaxX,locCover_MaxY,nowmusic.meta.ImageID);
            }
            else if (COVER_DRAW==2 && isInStandby==false)
            {
              putchar(GC,locCover_X,locCover_Y+PlusY,locCover_MaxX,locCover_MaxY,nowmusic.meta.ImageID);
            }
          }
        }
        if (CustomImage_DrawAfter==true)
        {
          if (img.isImage==true && CustomImage_Show && DrawCustom==true)
          {
            if (CustomImage_Draw==0)
            {
              putchar(GC,img.x,img.y+PlusY,img.mx,img.my,img.ImageID);
            }
            else if (CustomImage_Draw==1 && isInStandby==true)
            {
              putchar(GC,img.x,img.y+PlusY,img.mx,img.my,img.ImageID);
            }
            else if (CustomImage_Draw==2 && isInStandby==false)
            {
              putchar(GC,img.x,img.y+PlusY,img.mx,img.my,img.ImageID);
            }
          }
        }
        if (ProgressBar_Show)
        {
          DrawProgress(ProgressBar_bColor,ProgressBar_cColor, ProgressBar_oColor, pbrect, CurrentTime, currenttrack->fulltime, PlusY, ProgressBar_Style); 
        }
        int x;
        for (x=0;x<10;x++)
        {
          if (labels[x].Hide==false)
          {
            DrawLine(labels[x].SHOW,labels[x].Font,labels[x].str,labels[x].CT,labels[x].X,labels[x].Y+PlusY+AnimYPlus,labels[x].MaxX,labels[x].MaxY+PlusY+AnimYPlus,labels[x].borderColor,labels[x].Color,SID_ANY_LEN, labels[x].UsedInAnim, &labels[x]);
          }
          else if (labels[x].Hide==true)
          {
            if (isInStandby==true)
            {
              DrawLine(labels[x].SHOW,labels[x].Font,labels[x].str,labels[x].CT,labels[x].X,labels[x].Y+PlusY+AnimYPlus,labels[x].MaxX,labels[x].MaxY+PlusY+AnimYPlus,labels[x].borderColor,labels[x].Color,SID_ANY_LEN, labels[x].UsedInAnim, &labels[x]);
            }
          }
        }
        if (DrawOld==1)
        {
          DefRedraw(DO,a,b,c);
        }
    }
    else
    {
      DefRedraw(DO,a,b,c);
    }
  }
  else
  {
    DefRedraw(DO,a,b,c);
  }
};

int GetUpdateTime()
{
  if (AllowAnim==true)
  {
    return AnimUpdateTime;
  }
  else
  {
    return DefUpdateTime;
  }
};

void Standby_OnRedrawTimer(u16 timer,LPARAM lparam)
{
  if (isInStandby==true)
  {
    isInStandby=CheckStandby();
  }
  if (PlayerRunned)
  {
    isAPBookFound=true;
    if (isInStandby==true)
    {
      NextScrollFrame(isInStandby);
      if (AllowAnim==true)
      {
        int t=0;
        if (t=Anim_NextFrame())
        {
          int x;
          for (x=0;x<(t-1);x++)
          {
            Anim_NextFrame();
          }
        }
      }
      InvalidateRect(Standby_DO,0);
      if (AffectSoft==true)
      {
        InvalidateRect(Softkey_DO,0);
      }
      if (AffectStatus==true)
      {
        InvalidateRect(StatusIndication_DO,0);
      }
    }
    else if (isInStandby==false)
    {
      NextScrollFrame(isInStandby);
      if (DrawSoft2!=false && AffectSoft==true)
      {
        InvalidateRect(Softkey_DO,0);
      }
      if (DrawStatus2!=false && AffectStatus==true)
      {
        InvalidateRect(StatusIndication_DO,0);
      }
    }
  }
  else
  {
    //���� ��� ��� ������ ��� ������, �� ���� ������ ��� ������� � ������
    if (isAPBookFound)
    {
      if (isInStandby==true)
      {
        InvalidateRect(Standby_DO,0);
        if (AffectSoft==true)
        {
          InvalidateRect(Softkey_DO,0);
        }
        if (AffectStatus==true)
        {
          InvalidateRect(StatusIndication_DO,0);
        }
      }
      else if (isInStandby==false)
      {
        if (DrawSoft2!=false && AffectSoft==true)
        {
          InvalidateRect(Softkey_DO,0);
        }
        if (DrawStatus2!=false && AffectStatus==true)
        {
          InvalidateRect(StatusIndication_DO,0);
        }
      }
    }
    isAPBookFound=false;
  }
  Timer_ReSet(&Standby_RedrawTimer,locTime,Standby_OnRedrawTimer,0);
};

void Standby_SetOnRedraw()
{
  //������� ������� ���������� ��� ����������� ��.
  wstrcpy(lastmusic,L"no");
  
  Standby_DO=((GUI*)SBY_GetStatusIndication(Find_StandbyBook()))->DISP_OBJ;
  Standby_DefaultRedraw=DISP_OBJ_GetOnRedraw(Standby_DO);
  Standby_DD=DISP_OBJ_GetDESC (Standby_DO);
  DISP_DESC_SetOnRedraw(Standby_DD,Standby_CARedraw); 
  Standby_RedrawTimer=Timer_Set(GetUpdateTime(),Standby_OnRedrawTimer,0);
  //����� �� ��� �����/������?
  if (AffectSoft==true)
  {
    Softkey_SetOnRedraw();
  }
  if (AffectStatus==true)
  {
    StatusIndication_SetOnRedraw();
  }
};
int isSoftkeysBook(BOOK *bk)
{
  if (strcmp(bk->xbook->name,"Softkeys")==0) { return 1; }
  else { return 0; }
};
typedef struct
{
  void (*proc)(DISP_OBJ *, int a, int b, int c);
  int top; //0-behind, 1 - front
}SOFT_REDRAW;
typedef struct
{
  BOOK A2Book;
  LIST *proc;
  //int add;
  //int remove;
  void (*Softkeys_Add)(SOFT_REDRAW *func);
  void (*Softkeys_Remove)(SOFT_REDRAW *func);
}SoftkeysBOOK;
SOFT_REDRAW *our=0;

//void (*Softkeys_Add)(BOOK *bk, void *sr)=(void (*)(BOOK *bk, void *sr));

void Softkey_SetOnRedraw()
{
  Standby=Find_StandbyBook();
  MainInput=SBY_GetMainInput(Standby);
  //������� �����
  SoftUsed=true;
  Softkey_DO=DispObject_SoftKeys_Get();
  Softkey_DefaultRedraw=DISP_OBJ_GetOnRedraw(Softkey_DO);
  Softkey_DD=DISP_OBJ_GetDESC(Softkey_DO);
  DISP_DESC_SetOnRedraw(Softkey_DD,Softkey_CARedraw);
};

void StatusIndication_SetOnRedraw()
{
  //������� ������ 
  StatusUsed=true;
  StatusIndication_DO=*StatusRow_p();
  StatusIndication_DefaultRedraw=DISP_OBJ_GetOnRedraw(StatusIndication_DO);
  StatusIndication_DD=DISP_OBJ_GetDESC(StatusIndication_DO);
  DISP_DESC_SetOnRedraw(StatusIndication_DD,StatusIndication_CARedraw);
};
int compare(void *p1, void *p2)
{
  if (p1==p2)
  {
    return 0;
  }
  return 1;
}
void Standby_ReturnRedraw()
{ 
  DISP_DESC_SetOnRedraw(Standby_DD, Standby_DefaultRedraw); 
  //���� ��� �������� "�������������� ����" � ����, ������ ���� ����������� ��� �� ������:
  if (SoftUsed==true)
  {
    DISP_DESC_SetOnRedraw(Softkey_DD, Softkey_DefaultRedraw);
  }
  if (StatusUsed==true)
  {
    DISP_DESC_SetOnRedraw(StatusIndication_DD, StatusIndication_DefaultRedraw); 
  }
  Timer_Kill(&Standby_RedrawTimer);
};
