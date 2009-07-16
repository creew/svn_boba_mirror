#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\var_arg.h"
#include "..\\include\cfg_items.h"
#include "main.h"
#include "menu.h"
#include "language.h"

#define MENU_ITEM_NUM   5

void AutoOKPressed(BOOK * bk, wchar_t *string, int len)
{
  wstrcpy(SIwstr, string);
  AutoLocation=true;
  CheckCurrentCell();
  BookObj_ReturnPage(bk,NIL_EVENT);
}

void AutoBackPressed(BOOK * bk, u16 *string, int len)
{
  BookObj_ReturnPage(bk,NIL_EVENT);
}

void CreateAutoLacationInput()
{
  MyBOOK * bk = (MyBOOK *) FindBook(isMiniGPSBook);
  FREE_GUI(bk->text_input);
  STRID text = Str2ID(SIwstr,0,SID_ANY_LEN);
  bk->text_input = (GUI *)CreateStringInput(0,
                            VAR_BOOK(bk),
                            VAR_STRINP_FIXED_TEXT(Str2ID(LG_CURRENTLOCATION,0,SID_ANY_LEN)),
                            VAR_STRINP_TEXT(text),
                            VAR_STRINP_NEW_LINE(0),
                            VAR_STRINP_ENABLE_EMPTY_STR(0),
                            VAR_STRINP_MAX_LEN(MAX_AUTOLOCATION_LEN - 1),
                            VAR_STRINP_MODE(IT_STRING),
                            VAR_OK_PROC(AutoOKPressed),
                            VAR_PREV_ACTION_PROC(AutoBackPressed),
                            0);
  BookObj_SetFocus(bk,0);
  ShowWindow(bk->text_input);
}

int menu_callback(GUI_MESSAGE * msg)
{
  int str_id;
  switch(msg->msg)
  {
  case 1:
    int curitem = GUIonMessage_GetCreatedItemIndex(msg);
    {
      switch(curitem)
      {
      case 0:
        if(visible)
          str_id  = Str2ID(LG_HIDE,0,SID_ANY_LEN);
        else
          str_id  = Str2ID(LG_SHOW,0,SID_ANY_LEN);
        break;
      case 1:
        if(showCellID)
          str_id  = Str2ID(LG_NAME,0,SID_ANY_LEN);
        else
          str_id  = Str2ID(LG_CELLID,0,SID_ANY_LEN);
        break;
      case 2:
        if(AutoLocation)
          str_id  = Str2ID(LG_AUTOLOCATION_OFF,0,SID_ANY_LEN);
        else
          str_id  = Str2ID(LG_AUTOLOCATION_ON,0,SID_ANY_LEN);
        break;
      case 3:
        str_id  = Str2ID(LG_SETTINGS,0,SID_ANY_LEN);
        break;
      case 4:
        str_id  = Str2ID(LG_ABOUT,0,SID_ANY_LEN);
        break;
      }
    }
    SetMenuItemText0(msg,str_id);
  }
  return(1);
}

void MenuOnBack(BOOK * book, void *)
{
  BookObj_ReturnPage(book,NIL_EVENT);
}

void MenuOnEnter(BOOK * book, void *)
{
  MyBOOK *bk = (MyBOOK *)book;
  int item_num = ListMenu_GetSelectedItem(bk->menu);
  switch(item_num){
  case 0:
    ShowHideProc();
    InvalidateAll();
    BookObj_ReturnPage(book,NIL_EVENT);
    break;
  case 1:
    CellIdProc();
    InvalidateAll();
    BookObj_ReturnPage(book,NIL_EVENT);
    break;
  case 2:
    if (AutoLocation == false)
    {
      CreateAutoLacationInput();
    }
    else
    {
      AutoLocation = false;
      MessageBox(0x6fFFFFFF, Str2ID(LG_AUTOLOCATIONOFF,0,SID_ANY_LEN), 0xFFFF, 1, 5000, 0);
      InvalidateAll();
      BookObj_ReturnPage(book,NIL_EVENT);
    }
    break;
  case 3:
    onBcfgConfig(0, book);
    BookObj_ReturnPage(book,NIL_EVENT);
    break;
  case 4:
    ShowAuthorInfo(0 , book);
    BookObj_ReturnPage(book,NIL_EVENT);
    break;
  }
}

void MenuOnKey(DISP_OBJ *db, int key, int unk, int repeat, int type)
{
  void (*OldOnKey)(void *, int, int, int, int);
  MyBOOK * bk = (MyBOOK *) FindBook(isMiniGPSBook);
  OldOnKey = (void(*)(void *,int,int,int,int))bk->oldOnKey;
  OldOnKey(db, key, unk, repeat, type);
  if(type == KBD_SHORT_RELEASE)
  {
    int num = key - KEY_DIGITAL_0;
    if(num == 2)
    {
      int item = ListMenu_GetSelectedItem(bk->menu) - 1;
      if(item < 0) item = MENU_ITEM_NUM - 1;
      SetCursorToItem(bk->menu, item);
    }
    else if((num == 8) || (num == 0))
    {
      int item = ListMenu_GetSelectedItem(bk->menu) + 1;
      if(item >= MENU_ITEM_NUM) item = 0;
      SetCursorToItem(bk->menu, item);
    }
    else if(num == 5)
    {
      MenuOnEnter((BOOK*)bk, 0);
    }
  }
}

void CreateMenu(MyBOOK *mbk)
{
  mbk->menu = CreateListObject(&mbk->book,0);

  GuiObject_SetTitleText(mbk->menu, Str2ID(LELFNAME,0,SID_ANY_LEN));
  SetNumOfMenuItem(mbk->menu, MENU_ITEM_NUM);
  OneOfMany_SetonMessage((GUI_ONEOFMANY*)mbk->menu, menu_callback);
  SetCursorToItem(mbk->menu,0);

//  SetMenuItemStyle(mbk->menu, 4);

  GUIObject_Softkey_SetAction(mbk->menu,ACTION_BACK, MenuOnBack);
  GUIObject_Softkey_SetAction(mbk->menu,ACTION_LONG_BACK, MenuOnBack);
  GUIObject_Softkey_SetAction(mbk->menu,ACTION_SELECT1, MenuOnEnter);

  mbk->oldOnKey = (void *)DISP_OBJ_GetOnKey(mbk->menu->DISP_OBJ);
  DISP_DESC_SetOnKey( DISP_OBJ_GetDESC (mbk->menu->DISP_OBJ), (DISP_OBJ_ONKEY_METHOD)MenuOnKey );

  BookObj_SetFocus(mbk,0);
  ShowWindow(mbk->menu);
}

static int MenuPageOnEnter(void *, BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  CreateMenu(mbk);
  return (1);
}

static int MenuPageOnExit(void *, BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  FREE_GUI(mbk->menu);
  FREE_GUI(mbk->text_input);
  BookObj_Hide(bk,0);
//  BookObj_SetFocus(Find_StandbyBook(),0);
  return (1);
}

const PAGE_MSG bk_msglst_menu[] @ "DYN_PAGE"  =
{
  PAGE_ENTER_EVENT_TAG,    MenuPageOnEnter,
  PAGE_EXIT_EVENT_TAG,     MenuPageOnExit,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_menu = {"MiniGPS_Menu_Page",0,bk_msglst_menu};

void ShowMenu(BOOK * book)
{
  BookObj_CallPage(book, &bk_menu);
}
