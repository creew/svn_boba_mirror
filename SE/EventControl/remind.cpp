/*
*===========================================================================
*                   ������ GUI_REMIND, ��������������. ������ �������.
*===========================================================================
*/
#include "mem.h"
#include "..\\include\Lib_Clara.h" 
#include "..\\include\Dir.h"
#include "header\structs.h"
#include "header\lng.h"

#define MBK(a) MyBOOK *mbk=(MyBOOK*)a;
#define GET_BLUE(x) ((unsigned int)x&0xFF)
#define GET_GREEN(x) (((unsigned int)x>>8)&0xFF)
#define GET_RED(x) (((unsigned int)x>>16)&0xFF)
#define GET_ALPHA(x) (((unsigned int)x>>24)&0xFF)
#define BG_COLOR 0xCC000000

extern char h,m,s;
extern MyBOOK *ECBook;

static const char RemindGuiName[]="gui_reminder";

int GuiRemind_NextRemind(GUI_REMIND *g);
int GuiRemind_NextRemind(GUI_REMIND *g);
void GuiRemind_CheckSelected(GUI_REMIND *g);
wchar_t *checkfile(wchar_t*);
/*
*===========================================================================
*                    �������� ������ �����������
*===========================================================================
*/
int GUI_REMIND_OnCreate(DISP_OBJ_REMIND *db)
{
  db->num=0;
  if (!ECBook->remlst)
    db->rems=List_New();
  else
    db->rems=ECBook->remlst;
  return 1;
};

int GetIconID(wchar_t *txt)
{
  int id=0xFFFF;
  iconidname2id(txt,0xFFFFFFFF,&id);
  return id;
};
/*
*===========================================================================
*                         ����������� ������ �����������
*===========================================================================
*/
void kill_rems(LIST *lst, MyBOOK *mbk, bool check)
{
  if (lst)
  {
    int x=0;
    while (1)
    {
      if (x>=lst->FirstFree || x<0)break;
      REMIND* rem=(REMIND*)ListElement_GetByIndex(lst,x);
      if (rem && (rem->checked || check==false))
      {
        if (rem->text)delete(rem->text);
        if (rem->utext)delete(rem->utext);
        if (rem->time)delete(rem->time);
        delete(rem);
        ListElement_Remove(lst, x);
      }
      else
      {
        x++;
      }
    }
    wchar_t icon=GetIconID(L"CALE_RN_TASK_STATUS_ICN");
    if (lst->FirstFree==0)
    {
      List_Free(lst);
      mbk->remlst=0;
      SetTrayIcon(icon, 0);
    }
    else
    {
      mbk->remlst=lst;
      SetTrayIcon(icon, 1);
    }
  }
};
/*
*===========================================================================
*                     ������ �������������� ��������
*===========================================================================
*/
void DrawGradient(int x, int y, int x2, int y2, int upc, int loc)
{
  int r1=GET_RED(upc),g1=GET_GREEN(upc),b1=GET_BLUE(upc);
  int r2=GET_RED(loc),g2=GET_GREEN(loc),b2=GET_BLUE(loc);
  int a1=GET_ALPHA(upc),a2=GET_ALPHA(loc);
  int i;
  for(i=0;i<x2-x;i++)
  { 
    int r,g,b,a;
    r = r1 + (i * (r2-r1) / (x2-x));
    g = g1 + (i * (g2-g1) / (x2-x));
    b = b1 + (i * (b2-b1) / (x2-x));
    a = a1 + (i * (a2-a1) / (x2-x));
    int cl=a*0x1000000 + r * 0x10000 + g *0x100 + b;
    DrawRect(x+i,y,x+i+1,y2,cl,cl);
  }
};

/*
*===========================================================================
*                      ������ ������������ ��������
*===========================================================================
*/
void DrawGradientVert(int x, int y, int x2, int y2, int upc, int loc)
{
  int r1=GET_RED(upc),g1=GET_GREEN(upc),b1=GET_BLUE(upc);
  int r2=GET_RED(loc),g2=GET_GREEN(loc),b2=GET_BLUE(loc);
  int a1=GET_ALPHA(upc),a2=GET_ALPHA(loc);
  int i;
  for(i=0;i<y2-y;i++)
  { 
    int r,g,b,a;
    r = r1 + (i * (r2-r1) / (y2-y));
    g = g1 + (i * (g2-g1) / (y2-y));
    b = b1 + (i * (b2-b1) / (y2-y));
    a = a1 + (i * (a2-a1) / (y2-y));
    int cl=a*0x1000000 + r * 0x10000 + g *0x100 + b;
    DrawRect(x, y+i, x2, y+i+1, cl, cl);
  }
};

void GUI_REMIND_OnClose(DISP_OBJ_REMIND *db)
{
  if (db->rems)kill_rems(db->rems, ECBook, true);
  db->rems=0;
};

/*
*===========================================================================
*                      ��������� ������������ � ������������ ����������
*===========================================================================
*/
void DrawProgressVert(RECT rect, int progress, int time, int upc, int loc)
{
  float Length;
  float CurLength=(float)progress;
  float inLength=(float)time;
  float Procent=(float)CurLength/inLength;
  float size=(float)((float)rect.x2-(float)rect.x1);
  float start = (float)rect.x1;
  Length = start+((Procent)*size);
  DrawGradientVert(rect.x1, rect.y1, (int)Length, rect.y2, upc, loc);
};

void DrawLine(int str, int ct, int x, int y, int x2, int y2, int between, int c)
{
  DrawString(str,ct,x,y,x2,y2,between,3,c,c);
  TextFree(str);
};
/*
*===========================================================================
*                      ���������� ��������� ���
*===========================================================================
*/
void GUI_REMIND_OnRedraw(DISP_OBJ_REMIND *db,int ,int,int)
{
  DispObject_SetLayerColor((DISP_OBJ*)db,BG_COLOR);
  if (db->rems && db->rems->FirstFree)
  {
    int width=Display_GetWidth(0);
    int height=Display_GetHeight(0);
    REMIND *rem=(REMIND*)ListElement_GetByIndex(db->rems,db->num);
    int cl=0xFFAA0000;
    if (rem->checked)cl=0xFF00AA00;
    SetFont(FONT_E_16R);
    DrawGradientVert(0, 0, width, 25, cl, 0);//BG_COLOR);
    wchar_t str[50];
    snwprintf(str,49,L"%02d/%02d",db->num+1,db->rems->FirstFree);
    DrawLine(Str2ID(L"EventControl",0,SID_ANY_LEN),0,25,0,width,25,20,clWhite);
    DrawLine(Str2ID(str,0,SID_ANY_LEN),1,0,0,width,25,20,clWhite);

    int icon_id=0xFFFF;
    iconidname2id(L"CALE_RN_APPOINTMENT_STATUS_ICN",-1,&icon_id);
    GC *gc=get_DisplayGC();
    putchar(gc,0,0,0,0,icon_id);
    SetFont(FONT_E_20I);
    DrawLine(Str2ID(rem->text,0,SID_ANY_LEN),2,0,height/2-10,width,height,20,clWhite);
    SetFont(FONT_E_16R);
    DrawLine(Str2ID(rem->utext,0,SID_ANY_LEN),2,0,height-44,width,height,0,clWhite);
    RECT rec={0,width,height-25,height};
    DrawProgressVert(rec, db->num+1, db->rems->FirstFree, clBlack, cl);
    DrawGradient(width-80, 60, width, 85, 0, cl);
    wchar_t time[50];
    snwprintf(time, 49, L"/%02d:%02d:%02d", h, m, s);
    SetFont(FONT_E_20R);
    DrawLine(Str2ID(time,0,SID_ANY_LEN),1,0,60,width,80,0,clWhite);
    SetFont(FONT_E_16R);
    DrawLine(Str2ID(rem->time,0,SID_ANY_LEN),1,0,63,width-80,80,0,clWhite);
    int font_y=GetImageHeight(L' ')+4;
    DrawLine(Str2ID(lng[LNG_INFO],0,SID_ANY_LEN),0,0,height-font_y,width,height,0,clWhite);
    DrawLine(Str2ID(lng[LNG_OK],0,SID_ANY_LEN),2,0,height-font_y,width,height,0,clWhite);
    DrawLine(Str2ID(lng[LNG_MARK],0,SID_ANY_LEN),1,0,height-font_y,width,height,0,clWhite);
  }
};

/*
*===========================================================================
*                       ���������� ������� ������ ��/�����
*===========================================================================
*/
void Reminder_onInfo(BOOK *bk, void *)
{
  MessageBox(0x6FFFFFFF,Str2ID(lng[LNG_FINFO],0,SID_ANY_LEN),0,1,0,0);
};

/*
*===========================================================================
*                       ���������� ������� ������ ��/�����
*===========================================================================
*/
void Reminder_onOK(BOOK *bk, void *)
{
  PlayFile(checkfile(L"stop.mp3"),L"stop.mp3");
  MBK(bk);
  if (!mbk->remind) 
  {
    BookObj_Hide(bk, 0);
    return;
  }
  int res=GuiRemind_NextRemind(mbk->remind);
  if (res==0)
  {
    GUI_Free(mbk->remind);
    mbk->remind=0;
    BookObj_Hide(bk, 0);
  }
};

/*
*===========================================================================
*                       ���������� ������� ������ "C"
*===========================================================================
*/
void Reminder_onDel(BOOK *bk, void *)
{
  MBK(bk);
  GuiRemind_CheckSelected(mbk->remind);
};

/*
*===========================================================================
*                      ������� ��������� ������� ������
*===========================================================================
*/
void GUI_REMIND_OnKey(DISP_OBJ_REMIND *db,int key,int,int repeat,int type)
{
  if (type==KBD_SHORT_PRESS)
  {
    if (key==KEY_DEL || key==KEY_RIGHT_SOFT)
    {
      Reminder_onDel(&ECBook->bk,0);
    }
    if (key==KEY_ENTER || key==KEY_ESC)
    {
      Reminder_onOK(&ECBook->bk,0);
    }
    if (key==KEY_LEFT_SOFT)
    {
      Reminder_onInfo(&ECBook->bk,0);
    }
    if (key==KEY_LEFT)
    {
      if (db->num)db->num--;
      InvalidateRect((DISP_OBJ*)db,0);
    }
    if (key==KEY_RIGHT)
    {
      if (db->num!=db->rems->FirstFree-1)db->num++;
      InvalidateRect((DISP_OBJ*)db,0);
    }
  }
};


/*
*===========================================================================
*                     ������� ������ ����������
*===========================================================================
*/
void GUI_REMIND_OnRefresh(DISP_OBJ_REMIND *DO)
{
  DispObject_SetRefreshTimer((DISP_OBJ*)DO, 1000);
  InvalidateRect((DISP_OBJ*)DO, 0);
};

/*
*===========================================================================
*                     ����������� ���
*===========================================================================
*/
void GUI_REMIND_constr(DISP_DESC *desc)
{
  DISP_DESC_SetName(desc,RemindGuiName);
  DISP_DESC_SetSize(desc,sizeof(DISP_OBJ_REMIND));
  DISP_DESC_SetOnCreate(desc,(DISP_OBJ_ONCREATE_METHOD)GUI_REMIND_OnCreate);
  DISP_DESC_SetOnClose(desc,(DISP_OBJ_ONCLOSE_METHOD)GUI_REMIND_OnClose);
  DISP_DESC_SetOnRedraw(desc,(DISP_OBJ_ONREDRAW_METHOD)GUI_REMIND_OnRedraw);
  DISP_DESC_SetOnKey(desc,(DISP_OBJ_ONKEY_METHOD)GUI_REMIND_OnKey);
  DISP_DESC_SetonRefresh(desc,(DISP_OBJ_METHOD)GUI_REMIND_OnRefresh);
};

/*
*===========================================================================
*                       ���������� ���
*===========================================================================
*/
void GUI_REMIND_destr(DISP_DESC *desc)
{
  return;
};

/*
*===========================================================================
*                             �������� ���
*===========================================================================
*/
GUI_REMIND *GUI_REMIND_Create(BOOK *bk)
{
  GUI_REMIND *gui_read=new GUI_REMIND;
  if (!CreateObject( gui_read,GUI_REMIND_destr, GUI_REMIND_constr,bk,0,0,0))
  {
    delete gui_read;
    return 0;    
  }
  DispObject_SetLayerColor( GUIObj_GetDISPObj( gui_read), BG_COLOR);
  if (bk) addGui2book(bk, gui_read);
  DispObject_SetLayerColor( GUIObj_GetDISPObj( gui_read), BG_COLOR);
  DispObject_SetRefreshTimer( GUIObj_GetDISPObj( gui_read), 1000);
  return gui_read;
};

/*
*===========================================================================
*                 ���������� � ��� ������ �����������
*===========================================================================
*/
void GuiRemind_AddNote(GUI_REMIND *g, REMIND *rem)
{
  if (!g)return;
  if (!rem)return;
  DISP_OBJ_REMIND *DO=(DISP_OBJ_REMIND*) GUIObj_GetDISPObj(g);
  if (!DO)return;
  ListElement_Add(DO->rems,rem);
};

void GuiRemind_CheckSelected(GUI_REMIND *g)
{
  GUI *gb = g;
  DISP_OBJ_REMIND *db= (DISP_OBJ_REMIND*)GUIObj_GetDISPObj(gb);
  if (db->rems && db->num<db->rems->FirstFree)
  {
    REMIND *rem=(REMIND*)ListElement_GetByIndex(db->rems,db->num);
    if (rem->checked)
      rem->checked=false;
    else
    {
      rem->checked=true;
      GuiRemind_NextRemind(g);
    }
    InvalidateRect((DISP_OBJ*)db,0);
  }
};

int GuiRemind_NextRemind(GUI_REMIND *g)
{
  GUI *gb = g;
  DISP_OBJ_REMIND *db=(DISP_OBJ_REMIND*)GUIObj_GetDISPObj(gb);
  if (db->rems)
  {
    if (db->num!=db->rems->FirstFree-1)
      db->num++;
    else
      return 0;
    InvalidateRect((DISP_OBJ*)db,0);
  }
  else
    return 0;
  return 1;
};
