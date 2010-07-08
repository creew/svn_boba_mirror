#include "..\deleaker\mem2.h"
#include "..\include\Lib_Clara_DLL.h" 
#include "..\include\Dir.h" 
#include "header\structs.h"
#include "header\pages.h"
#include "header\externals.h"
#include "header\lng.h"

int LastIndex=0;

int isEvtEditBook(BOOK *);

void Days_OnSelect2(BOOK *bk, GUI* )
{
  MyBOOK *mbk=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem(mbk->days);
  LastIndex=item;
  if (item==0)
  {
    if (mbk->selev->isMinusInBefore==true)
    {
      mbk->selev->isMinusInBefore=false;
    }
    else
    {
      mbk->selev->isMinusInBefore=true;
    }
    BookObj_ReturnPage(bk, PREVIOUS_EVENT);
    BookObj_CallPage(bk,&bk_days);
  }
  else if (item==1)
  {
    mbk->TextInputType=2;
    mbk->inputType=IT_UNSIGNED_DIGIT;
    mbk->minint=0;
    mbk->maxint=1000;
    BookObj_CallPage(bk,&bk_StringInput);
  }
  else if (item==2)
  {
    if (mbk->selev->isMinusInAfter==true)
    {
      mbk->selev->isMinusInAfter=false;
    }
    else
    {
      mbk->selev->isMinusInAfter=true;
    }
    BookObj_ReturnPage(bk, PREVIOUS_EVENT);
    BookObj_CallPage(bk,&bk_days);
  }
  else if (item==3)
  {
    mbk->TextInputType=3;
    mbk->inputType=IT_UNSIGNED_DIGIT;
    mbk->minint=0;
    mbk->maxint=1000;
    BookObj_CallPage(bk,&bk_StringInput);
  }
};

void Days_OnSelect(BOOK *bk, GUI* )
{
  MyBOOK *mbk=(MyBOOK *)bk;
  bool b=false;
  int item=ListMenu_GetSelectedItem(mbk->days);
  if (mbk->curdays[item]==false)
  {
    mbk->curdays[item]=true;
    b=true;
  }
  else
  {
    mbk->curdays[item]=false;
    b=false;
  }

  int strid;
  if (b==true)
  {
    strid=mbk->check_box_checked+0x78000000;
  }
  else
  {
    strid=mbk->check_box_unchecked+0x78000000;
  }
  ListMenu_SetSecondLineText((GUI_LIST *)mbk->days,item,strid);
};

int Days_onLBMessage(GUI_MESSAGE * msg)
{
  switch( GUIonMessage_GetMsg(msg) )
  {
  case 1:
    MyBOOK *mbk=(MyBOOK*)FindBook(isEvtEditBook);
    int item=GUIonMessage_GetCreatedItemIndex(msg);
    int strid;
    SetMenuItemText0(msg,Str2ID(lng[LNG_WEEK+item],0,SID_ANY_LEN));
    if (mbk->curdays[item])
    {
       strid=mbk->check_box_checked+0x78000000;
    }
    else
    {
       strid=mbk->check_box_unchecked+0x78000000;
    }
    SetMenuItemText1(msg,strid);
  }
  return(1);
};

int Days_onLBMessage2(GUI_MESSAGE * msg)
{
  switch( GUIonMessage_GetMsg(msg) )
  {
  case 1:
    MyBOOK *mbk=(MyBOOK*)FindBook(isEvtEditBook);
    int item=GUIonMessage_GetCreatedItemIndex(msg);
    if (item==0)
    {
      if (mbk->selev->isMinusInBefore==true)
      {
        SetMenuItemText0(msg,Str2ID(lng[LNG_REMINDS + REM_BEFORE],0,SID_ANY_LEN));
      }
      else
      {
        SetMenuItemText0(msg,Str2ID(lng[LNG_REMINDS + REM_AFTER],0,SID_ANY_LEN));
      }
    }
    else if (item==1)
    {
      SetMenuItemText0(msg,Str2ID(lng[LNG_REMINDS + REM_MINUTES],0,SID_ANY_LEN));
    }
    else if (item==2)
    {
      if (mbk->selev->isMinusInAfter==true)
      {
        SetMenuItemText0(msg,Str2ID(lng[LNG_REMINDS + REM_BEFOREEND],0,SID_ANY_LEN));
      }
      else
      {
        SetMenuItemText0(msg,Str2ID(lng[LNG_REMINDS + REM_AFTEREND],0,SID_ANY_LEN));
      }
    }
    else if (item==3)
    {
      SetMenuItemText0(msg,Str2ID(lng[LNG_REMINDS + REM_MINUTES],0,SID_ANY_LEN));
    }
  }
  return(1);
};

void Days_OnBack(BOOK *bk, GUI* )
{
  LastIndex=0;
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
};

void Days_OnWorkdays(BOOK *bk, GUI* )
{
  MyBOOK *mbk=(MyBOOK*)bk;
  bool days[7];
  memcpy(days, mbk->selev->days, 7*sizeof(bool));
  if (days[0] && days[1] && days[2] && days[3] && days[4])
  {
    mbk->selev->days[0]=0;
    mbk->selev->days[1]=0;
    mbk->selev->days[2]=0;
    mbk->selev->days[3]=0;
    mbk->selev->days[4]=0;
  }
  else
  {
    mbk->selev->days[0]=1;
    mbk->selev->days[1]=1;
    mbk->selev->days[2]=1;
    mbk->selev->days[3]=1;
    mbk->selev->days[4]=1;
  }
};

void Days_OnWeekend(BOOK *bk, GUI* )
{
  MyBOOK *mbk=(MyBOOK*)bk;
  bool days[7];
  memcpy(days, mbk->selev->days, 7*sizeof(bool));
  if (days[5] && days[6])
  {
    mbk->selev->days[5]=0;
    mbk->selev->days[6]=0;
  }
  else
  {
    mbk->selev->days[5]=1;
    mbk->selev->days[6]=1;
  }
};

void Days_OnEveryday(BOOK *bk, GUI* )
{
  MyBOOK *mbk=(MyBOOK*)bk;
  bool days[7];
  memcpy(days, mbk->selev->days, 7*sizeof(bool));
  if (days[0] && days[1] && days[2] && days[3] && days[4] && days[5] && days[6])
  {
    mbk->selev->days[0]=0;
    mbk->selev->days[1]=0;
    mbk->selev->days[2]=0;
    mbk->selev->days[3]=0;
    mbk->selev->days[4]=0;
    mbk->selev->days[5]=0;
    mbk->selev->days[6]=0;
  }
  else
  {
    mbk->selev->days[0]=1;
    mbk->selev->days[1]=1;
    mbk->selev->days[2]=1;
    mbk->selev->days[3]=1;
    mbk->selev->days[4]=1;
    mbk->selev->days[5]=1;
    mbk->selev->days[6]=1;
  }
};
GUI_LIST * Days_CreateGuiList(BOOK * book)
{
  GUI_LIST * lo=0;
  if (lo=CreateListObject(book,0))
  {
    MyBOOK *mbk=(MyBOOK*)book;
    if (mbk->daystype==0)
    {
      GuiObject_SetTitleText(lo,Str2ID(lng[DAYS],0,SID_ANY_LEN));
      SetNumOfMenuItem(lo,7);
      SetCursorToItem(lo,0);
      ListMenu_SetOnMessages(lo,Days_onLBMessage);
      SetMenuItemStyle(lo,2);
      GUIObject_Softkey_SetAction(lo,ACTION_BACK, Days_OnBack);
      GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,Days_OnSelect);
      GUIObject_Softkey_SetAction(lo,0, Days_OnWorkdays);
      GUIObject_Softkey_SetText(lo,0,Str2ID(lng[LNG_WORKDAYS],0,SID_ANY_LEN));
      GUIObject_Softkey_SetAction(lo,1, Days_OnWeekend);
      GUIObject_Softkey_SetText(lo,1,Str2ID(lng[LNG_WEEKEND],0,SID_ANY_LEN));
      GUIObject_Softkey_SetAction(lo,2, Days_OnEveryday);
      GUIObject_Softkey_SetText(lo,2,Str2ID(lng[LNG_ALLDAYS],0,SID_ANY_LEN));
    }
    else
    {
      GuiObject_SetTitleText(lo,Str2ID(lng[LNG_REMINDS],0,SID_ANY_LEN));
      SetNumOfMenuItem(lo,4);
      SetCursorToItem(lo,LastIndex);
      ListMenu_SetOnMessages(lo,Days_onLBMessage2);
      SetMenuItemStyle(lo,0);
      GUIObject_Softkey_SetAction(lo,ACTION_BACK, Days_OnBack);
      GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,Days_OnSelect2);
    }
  }
  return(lo);
};

GUI_LIST *create_days(BOOK *book)
{
  MyBOOK *mbk=(MyBOOK *)book;
  mbk->days=Days_CreateGuiList(book);
  ShowWindow(mbk->days);
  return(mbk->days);
};



//------------------------------------------------------------------------------
static int Days_OnEnter(void *, BOOK * bk)
{
  create_days(bk);
  return 1;
};

static int Days_OnExit(void *, BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  FREE_GUI(mbk->days);
  return (1);  
};


const PAGE_MSG bk_msglst_days[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,    Days_OnEnter,
  PAGE_EXIT_EVENT_TAG,     Days_OnExit,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_days = {"EvtEdit_Days_Page",0,bk_msglst_days};
//------------------------------------------------------------------------------
