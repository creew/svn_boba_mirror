#include "mem.h"
#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "header\structs.h"
#include "header\pages.h"
#include "header\lng.h"
#include "header\externals.h"
#include "..\\include\var_arg.h"

typedef struct
{
  BOOK * book;
}MSG;

#define IDN_SAVE_BEFORE_EXIT L"HPB_SAVE_BEFORE_EXITQ_TXT"
#define IDN_CHANGES_HAVE_BEEN_MADE L"HPB_SAVE_BEFORE_EXITD_TXT"
#define IDN_CHECKBOX_UNCHECKED_ICON L"CHECKBOX_ICN"
#define IDN_CHECKBOX_CHECKED_ICON L"CHECKMARK_IN_BOX_ICN"
#define SID_NULL 0x6FFFFFFF
#define BUILD "public version"

extern LIST *events;
extern wchar_t *items[];
extern int fsize;
extern wchar_t *extFolder;
extern wchar_t *intFolder;

int lng_load(wchar_t*,wchar_t*);
void InitializeFolders();
void destroy_innative_lng();
int InitializeEvents(BOOK *myBook, wchar_t *fpath, wchar_t *fname);
int DestroyEvents(MyBOOK*,bool save);
void Escape(MyBOOK *mbk, bool dontsave);
void OnBackGui(BOOK * bk, void *);

MyBOOK * myBook;
wchar_t *defPath=0;
wchar_t *defName=0;

int GetStrID(wchar_t *txt)
{
  int id=0x6FFFFFFF;
  textidname2id(txt,0xFFFFFFFF,&id);
  return id;
};

int GetIconID(wchar_t *txt)
{
  int id=0xFFFF;
  iconidname2id(txt,0xFFFFFFFF,&id);
  return id;
};

void elf_exit(void)
{
  trace_done();
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
};

static void onMyBookClose(BOOK * book)
{
  
  destroy_innative_lng();
  
  if (intFolder)
    delete(intFolder);
  if (defPath)
    delete(defPath);
  if (defName)
    delete(defName);
  if (extFolder)
    delete(extFolder);
  MyBOOK *mbk=(MyBOOK*)book;
  if (mbk->indpath)
    delete(mbk->indpath);
  if (mbk->indname)
    delete(mbk->indname);
  
  SUBPROC(elf_exit);
};

int isEvtEditBook(BOOK * struc)
{
  return(struc->onClose == onMyBookClose);
};

static int TerminateElf(void *mess ,BOOK *book)
{
  Escape((MyBOOK*)book, true);
  return(1);
};

static int ShowAuthorInfo(void *mess ,BOOK *book)
{
  MSG * msg = (MSG *)mess;
  wchar_t ustr[64];
  snwprintf(ustr,MAXELEMS(ustr)-1,L"\nEvtEdit v1.0\n(c) UltraShot\n%s",BUILD);
  MessageBox(SID_NULL,Str2ID(ustr,0,MAXELEMS(ustr)-1),0, 1 ,5000,msg->book);
  return(1);
};

const PAGE_MSG bk_msglst_base[] @ "DYN_PAGE"  =
{
  ELF_TERMINATE_EVENT,     TerminateElf,
  ELF_SHOW_INFO_EVENT,     ShowAuthorInfo,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_base = {"EvtEdit_Base_Page",0,bk_msglst_base};  wchar_t ustr[64];

extern wchar_t *items[50];

int onLBMessage(GUI_MESSAGE * msg)
{
  MyBOOK * myBook = (MyBOOK *) FindBook(isEvtEditBook);
  switch( GUIonMessage_GetMsg(msg) )
  {
  case 1:
    int item=GUIonMessage_GetCreatedItemIndex(msg);
    if (item<events->FirstFree)
    {
      EVENT* evt=(EVENT *)ListElement_GetByIndex(events,item);
      if (evt->txt)
      {
        SetMenuItemText0(msg,Str2ID(evt->txt,0,SID_ANY_LEN));
      }
      else
      {
        SetMenuItemText0(msg,Str2ID(items[evt->type],0,SID_ANY_LEN));
      }
      if (evt->param)
      {
        SetMenuItemText1(msg,Str2ID(evt->param,0,SID_ANY_LEN));
      }
      SetListObjectItemIcon(msg,0,GetIconID(L"CALE_CALENDAR_MENU_ICN"));
    }
    /*
    else if (item==events->FirstFree)
    {
      GUIonMessage_SetLineSeparator(msg,1);
      //SetMenuItemText0(msg,Str2ID(lng[LNG_CREATE],0,SID_ANY_LEN));
      //SetListObjectItemIcon(msg,0,GetIconID(L"RN_SHORTCUT_LIST_SETTINGS_ICN"));
    }
    */
    else if (item==events->FirstFree)
    {
      SetMenuItemText0(msg,Str2ID(lng[LNG_CREATE],0,SID_ANY_LEN));
      SetListObjectItemIcon(msg,0,GetIconID(L"RN_SHORTCUT_LIST_SETTINGS_ICN"));
    }
    break;
  }
  return(1);
};

wchar_t *iticons[] = {L"TR_TIME_UPDATE_ICN", L"TR_TIME_UPDATE_ICN", L"CALE_TASK_MISC_ICN", L"TIMER_ACTIVE_ICN", L"TIMER_ACTIVE_ICN", L"TIMER_ACTIVE_ICN", L"CALE_TASK_MISC_ICN", L"TR_ALARMS_MENU_ICN", L"RN_URL_OPERATOR_ICN"};

int onLBMessage2(GUI_MESSAGE * msg)
{
  MyBOOK * myBook = (MyBOOK *) FindBook(isEvtEditBook);
  switch( GUIonMessage_GetMsg(msg) )
  {
  case 1:
    MyBOOK *mbk=(MyBOOK*)FindBook(isEvtEditBook);
    int item=GUIonMessage_GetCreatedItemIndex(msg);
    EVENT* evt=mbk->selev;
    SetMenuItemText0(msg,Str2ID(lng[item],0,SID_ANY_LEN));
    SetMenuItemText2(msg,Str2ID(lng[MENU+item],0,SID_ANY_LEN));
    SetListObjectItemIcon(msg,0,GetIconID(iticons[item]));
    if (item==0)
    {
      wchar_t time[50];
      snwprintf(time, 50,L"%02d:%02d:%02d", evt->time.hour, evt->time.min, evt->time.sec);
      SetMenuItemText1(msg,Str2ID(time,0,SID_ANY_LEN));
    }
    else if (item==1)
    {
      wchar_t time[50];
      snwprintf(time, 50,L"%02d:%02d:%02d", evt->time2.hour, evt->time2.min, evt->time2.sec);
      SetMenuItemText1(msg,Str2ID(time,0,SID_ANY_LEN));
    }
    else if (item==2)
    {
      if (evt->param)
      {
        SetMenuItemText1(msg,Str2ID(evt->param,0,SID_ANY_LEN));
      }
    }
    else if (item==3)
    {
      wchar_t time[50];
      wstrcpy(time, L"");
      bool days[7];
      memcpy(days, evt->days, 7*sizeof(bool));
      int x;
      for (x=0;x<7;x++)
      {
        if (days[x])
          wstrcat(time,L"*");
        else
          wstrcat(time,L"�");
      }
      SetMenuItemText1(msg,Str2ID(time,0,SID_ANY_LEN));
    }
    else if (item==4)
    {
      wchar_t time[50];
      snwprintf(time, 50,L"%02d.%02d.%02d", evt->date.day, evt->date.mon, evt->date.year);
      SetMenuItemText1(msg,Str2ID(time,0,SID_ANY_LEN));
    }
    else if (item==5)
    {
      wchar_t time[50];
      snwprintf(time, 50,L"%02d.%02d.%02d", evt->date2.day, evt->date2.mon, evt->date2.year);
      SetMenuItemText1(msg,Str2ID(time,0,SID_ANY_LEN));
    }
    else if (item==6)
    {
      if (evt->txt)
      {
        SetMenuItemText1(msg,Str2ID(evt->txt,0,SID_ANY_LEN));
      }
    }
    else if (item==7)
    {
      wchar_t time[50];
      int before=evt->AskBefore;
      int after=evt->AskAfter;
      if (evt->isMinusInBefore)before=-before;
      if (evt->isMinusInAfter)after=-after;
      snwprintf(time, 50,L"%d/%d", before, after);
      SetMenuItemText1(msg,Str2ID(time,0,SID_ANY_LEN));
    }
    else if (item==8)
    {
      if (evt->gps)
      {
        wchar_t time[50];
        snwprintf(time, 50,L"%s", evt->gps);
        SetMenuItemText1(msg,Str2ID(time,0,SID_ANY_LEN));
      }
      else
      {
        SetMenuItemText1(msg,GetStrID(L"MSG_UI_EMPTY_MESSAGE_TXT"));
      }
    }
    if (item==7 && mbk->selev->isGps)
    {
      GUIonMessage_SetItemDisabled (msg,1);
    }
    if (item==3 && mbk->selev->dtype==1)
    {
      GUIonMessage_SetItemDisabled (msg,1);
    }
    if ((item==4 || item==5) && ((mbk->selev->dtype==0 || mbk->selev->isGps)||((mbk->selev->dtype==0 && mbk->selev->isGps))))
    {
      GUIonMessage_SetItemDisabled (msg,1);
    }
    if (item==1 && mbk->selev->ttype==0)
    {
      GUIonMessage_SetItemDisabled (msg,1);
    }
    if ((item==0 || item==1) && mbk->selev->isGps)
    {
      GUIonMessage_SetItemDisabled (msg,1);
    }
  }
  return(1);
};

GUI_LIST *create_ed(BOOK *book);
int isEvtControl(BOOK *bk)
{
  if (strcmp(bk->xbook->name,"EventControl")==0)
  {
    return 1;
  }
  return 0;
};
typedef void (*UPDATE_METHOD)(wchar_t *path, wchar_t *name);

typedef struct
{
  BOOK bk;
  UPDATE_METHOD update;
}EVTBOOK;

void OnYesExitGui(BOOK * bk, void *)
{
  
  MyBOOK * mbk=(MyBOOK *)bk;
  if (DestroyEvents(mbk, true)==0)
  {
    EVTBOOK *ek=(EVTBOOK*)FindBook(isEvtControl);
    if (ek)
    {
      ek->update(defPath,defName);
    }
    FREE_GUI(mbk->lst);
    FreeBook(bk);
  }
};

void OnNoExitGui(BOOK * bk, void *)
{
  MyBOOK * mbk=(MyBOOK *)bk;
  FREE_GUI(mbk->lst);
  DestroyEvents(mbk,false);
  FreeBook(bk);
};

void OnBackExitGui(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  FREE_GUI(myBook->yesno);
};

void Escape(MyBOOK *mbk, bool dontsave)
{
  if (!dontsave)
  {
    if (mbk->selev==0)
    {
      mbk->yesno=CreateYesNoQuestionVA(0,
                                            VAR_BOOK(&mbk->book),
                                            VAR_YESNO_QUESTION(mbk->save_before_exit),
                                            0);
      GUIObject_Softkey_SetAction(mbk->yesno,ACTION_YES,OnYesExitGui);
      GUIObject_Softkey_SetAction(mbk->yesno,ACTION_NO,OnNoExitGui);
      GUIObject_Softkey_SetAction(mbk->yesno,ACTION_BACK,OnBackExitGui);
    }
    else
    {
      FREE_GUI(mbk->lst);
      mbk->selev=0;
      mbk->opt_lastindex=0;
      create_ed(&mbk->book);
    }
  }
  else
  {
    BookObj_ReturnPage(&mbk->book, PREVIOUS_EVENT);
    FREE_GUI(mbk->lst);
    DestroyEvents(mbk, false);
    FreeBook(&mbk->book);
  }
};
void OnBackGui(BOOK * bk, void *)
{
  Escape((MyBOOK*)bk, false);
};

void EvRem(EVENT *event)
{
   if (event)
   {
      if (event->txt)
      {
        delete(event->txt);
      }
      if (event->param)
      {
        delete(event->param);
      }
      delete(event);
   }
};

void OnDelGui(BOOK *bk, void *)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem(mbk->lst);
  if (mbk->selev==0)
  {
    if (item>=0 && item<(events->FirstFree))
    {
      EVENT *ev=(EVENT*)ListElement_Remove(events,item);
      EvRem(ev);
      FREE_GUI(mbk->lst);
      if (mbk->events->FirstFree)
      {
        mbk->main_lastindex=item;
      }
      else
      {
        mbk->main_lastindex=0;
      }
      create_ed(bk);
    }
  }
};

void OnAuthor(BOOK *bk, void *)
{
  wchar_t ustr[64];
  snwprintf(ustr,MAXELEMS(ustr)-1,L"\nEvtEdit v1.0\n(c) UltraShot\n%s",BUILD);
  MessageBox(SID_NULL,Str2ID(ustr,0,MAXELEMS(ustr)-1),0, 1 ,5000,bk);
};

void OnSelect1Gui(BOOK *bk, void *)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem(mbk->lst);
  if (mbk->selev==0)
  {
    if (item>=0 && item<(events->FirstFree+1))
    {
      mbk->main_lastindex=item;
      if (item==events->FirstFree)
      {
        EVENT *ev = new EVENT;
        memset(ev,0,sizeof(EVENT));
        ListElement_Add(events, ev);
        ev->days[0]=1;ev->days[1]=1;ev->days[2]=1;ev->days[3]=1;ev->days[4]=1;ev->days[5]=1;ev->days[6]=1;
        ev->rem=0x55555555;
      }
      mbk->selev=(EVENT*)ListElement_GetByIndex(events,item);
      FREE_GUI(mbk->lst);
      create_ed(bk);
    }
  }
  else
  {
    mbk->opt_lastindex=item;
    if (item==0)
    {
      mbk->curtime=&mbk->selev->time;
      BookObj_CallPage(&mbk->book,&bk_time_input);
    }
    if (item==1)
    {
      mbk->curtime=&mbk->selev->time2;
      BookObj_CallPage(&mbk->book,&bk_time_input);
    }
    else if (item==2)
    {
      BookObj_CallPage(&mbk->book,&bk_selectevent);
    }
    else if (item==3)
    {
      mbk->curdays=mbk->selev->days;
      mbk->daystype=0;
      BookObj_CallPage(&mbk->book,&bk_days);
    }
    else if (item==4)
    {
      mbk->curdate=&mbk->selev->date;
      BookObj_CallPage(&mbk->book,&bk_date_input);
    }
    else if (item==5)
    {
      mbk->curdate=&mbk->selev->date2;
      BookObj_CallPage(&mbk->book,&bk_date_input);
    }
    else if (item==6)
    {
      mbk->inputType=IT_STRING;
      mbk->minint=-1;
      mbk->maxint=100;
      mbk->TextInputType=1;
      BookObj_CallPage(&mbk->book,&bk_StringInput);
    }
    else if (item==7)
    {
      mbk->daystype=1;
      BookObj_CallPage(&mbk->book,&bk_days);
    }
    else if (item==8)
    {
      mbk->TextInputType=4;
      mbk->inputType=IT_STRING;
      mbk->minint=-1;
      mbk->maxint=9;
      BookObj_CallPage(&mbk->book,&bk_StringInput);
    }
  }
};

void onDummy(BOOK *bk, void*)
{
  return;
};

void onDType0Press(BOOK *bk, void *)
{
  MyBOOK *mbk=(MyBOOK*)bk;
  mbk->selev->dtype=0;
  BookObj_CallPage(bk,&bk_main);
};

void onDType1Press(BOOK *bk, void *)
{
  MyBOOK *mbk=(MyBOOK*)bk;
  if (mbk->selev->date.year==0 && mbk->selev->date.mon==0 && mbk->selev->date.day==0)
  {
    DATETIME dt;
    REQUEST_DATEANDTIME_GET(SYNC, &dt);
    mbk->selev->date.year=dt.date.year;
    mbk->selev->date.mon=dt.date.mon;
    mbk->selev->date.day=dt.date.day;
  }
  if (mbk->selev->date2.year==0 && mbk->selev->date2.mon==0 && mbk->selev->date2.day==0)
  {
    DATETIME dt;
    REQUEST_DATEANDTIME_GET(SYNC, &dt);
    mbk->selev->date2.year=dt.date.year;
    mbk->selev->date2.mon=dt.date.mon;
    mbk->selev->date2.day=dt.date.day;
  }
  mbk->selev->dtype=1;
  BookObj_CallPage(bk,&bk_main);
};

void onTType0Press(BOOK *bk, void *)
{
  MyBOOK *mbk=(MyBOOK*)bk;
  mbk->selev->ttype=0;
  BookObj_CallPage(bk,&bk_main);
};

void onTType1Press(BOOK *bk, void *)
{
  MyBOOK *mbk=(MyBOOK*)bk;
  mbk->selev->ttype=1;
  BookObj_CallPage(bk,&bk_main);
};

void onEventClone(BOOK *bk, void *)
{
  MyBOOK *mbk=(MyBOOK*)bk;
  mbk->inputType=IT_UNSIGNED_DIGIT;
  mbk->TextInputType=8;
  mbk->minint=1;
  mbk->maxint=1000;
  BookObj_CallPage(bk,&bk_StringInput);
};

void onIndication(BOOK *bk, void *)
{
  extern const PAGE_DESC bk_indic;
  BookObj_CallPage(bk, &bk_indic);
};

GUI_LIST * CreateGuiList(BOOK * book)
{
  GUI_LIST * lo=0;
  if (lo=CreateListObject(book,0))
  {
    MyBOOK *bk=(MyBOOK*)FindBook(isEvtEditBook);
    if (!bk->selev)
    {
      GUIObject_Softkey_SetAction(lo,0, OnAuthor);
      GUIObject_Softkey_SetText(lo,0, GetStrID(L"JAVA_APP_NR_ABOUT"));
      GuiObject_SetTitleText(lo,Str2ID(L"EventControl v2.0",0,SID_ANY_LEN));
      if (events->FirstFree)
      {
        SetNumOfMenuItem(lo,events->FirstFree+1);
        SetCursorToItem(lo,bk->main_lastindex);
        ListMenu_SetOnMessages(lo,onLBMessage);
      }
      else
      {
        ListMenu_SetNoItemText(lo,GetStrID(L"MP_AUDIO_NO_ITEMS_TXT"));
        SetNumOfMenuItem(lo,1);
        SetCursorToItem(lo,0);
        ListMenu_SetOnMessages(lo,onLBMessage);
      }
    }
    else
    {
      GuiObject_SetTitleText(lo,Str2ID(items[bk->selev->type],0,SID_ANY_LEN));
      SetNumOfMenuItem(lo,9);
      SetCursorToItem(lo,bk->opt_lastindex);
      ListMenu_SetOnMessages(lo,onLBMessage2);
    }
    SetMenuItemStyle(lo,3);
    GUIObject_Softkey_SetAction(lo,ACTION_BACK, OnBackGui);
    GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,OnSelect1Gui);
    if (!bk->selev)
    {
      GUIObject_Softkey_SetAction(lo,ACTION_DELETE, OnDelGui);
      GUIObject_Softkey_SetAction(lo, 2, onIndication);
      GUIObject_Softkey_SetText(lo,2, Str2ID(lng[INDICATION],0,SID_ANY_LEN));
    }
    else
    {
      if (!bk->selev->isGps)
      {
        GUIObject_Softkey_SetAction(lo, 0, onDummy);
        GUIObject_Softkey_SetText(lo, 0, Str2ID(lng[REMIND_TYPE],0,SID_ANY_LEN));
        GUIObject_Softkey_SetAction(lo,1,onDType0Press);
        GUIObject_Softkey_SetText(lo,1,Str2ID(lng[REMIND_DAYS],0,SID_ANY_LEN));
        
        GUIObject_Softkey_SetAction(lo,2,onDType1Press);
        GUIObject_Softkey_SetText(lo,2,Str2ID(lng[REMIND_DATE],0,SID_ANY_LEN));
        SoftKey_SetItemAsSubItem(lo, 0, 1);
        SoftKey_SetItemAsSubItem(lo, 0, 2);
      }
      GUIObject_Softkey_SetAction(lo, 3, onDummy);
      GUIObject_Softkey_SetText(lo, 3, Str2ID(lng[REMIND_TTYPE],0,SID_ANY_LEN));
      
      GUIObject_Softkey_SetAction(lo,4,onTType0Press);
      GUIObject_Softkey_SetText(lo,4,Str2ID(lng[REMIND_TIME],0,SID_ANY_LEN));
      
      GUIObject_Softkey_SetAction(lo,5,onTType1Press);
      GUIObject_Softkey_SetText(lo,5,Str2ID(lng[REMIND_INTERVAL],0,SID_ANY_LEN));
      SoftKey_SetItemAsSubItem(lo, 3, 4);
      SoftKey_SetItemAsSubItem(lo, 3, 5);
      GUIObject_Softkey_SetAction(lo,6,onEventClone);
      GUIObject_Softkey_SetText(lo,6,Str2ID(lng[CLONE],0,SID_ANY_LEN));
      GUIObject_Softkey_SetAction(lo, 7, onIndication);
      GUIObject_Softkey_SetText(lo,7, Str2ID(lng[INDICATION],0,SID_ANY_LEN));
    }
  }
  return(lo);
};

GUI_LIST *create_ed(BOOK *book)
{
  MyBOOK *mbk=(MyBOOK *)book;
  mbk->lst=CreateGuiList(book);
  ShowWindow(mbk->lst);
  return(mbk->lst);
};

static int MainPageOnCreate(void *, BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  create_ed(&mbk->book);
  return (1);
};

const PAGE_MSG bk_msglst_main[] @ "DYN_PAGE"  =
{
  PAGE_ENTER_EVENT_TAG,    MainPageOnCreate,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_main = {"EvtEdit_Main_Page",0,bk_msglst_main};

void getfilesize(wchar_t *elfpath)
{
  wchar_t path[512];
  wchar_t name[512];
  wstrcpy(path,elfpath);
  *(wstrrchr(path,L'/'))=0;
  wstrcpy(name,wstrrchr(elfpath,L'/'));
  wchar_t *name2=name;
  name2++;
  FSTAT fst;
  if (fstat(path,name2,&fst)==0)
  {
    fsize=fst.fsize;
  }
};

int main(wchar_t *elfpath, wchar_t *path, wchar_t *fname)
{
  trace_init();
  InitializeFolders();
  getfilesize(elfpath);
  wchar_t str[50];
  char *st=lang_get_name(get_system_langID());
  str2wstr(str,st);
  wstrcat(str,L".cfg");
  if (lng_load(extFolder, str))
  {
    if (lng_load(intFolder, str))
    {
      if (strcmp(st,"ru"))
      {
        if (lng_load(extFolder, L"en.cfg"))
        {
          lng_load(intFolder, L"en.cfg");
        }
      }
    }
  }
  if (path&&fname)
  {
    defPath=new wchar_t[wstrlen(path)+1];
    wstrcpy(defPath,path);
    defName=new wchar_t[wstrlen(fname)+1];
    wstrcpy(defName,fname);
  }
  myBook=new MyBOOK;
  memset(myBook,0,sizeof(MyBOOK));
  if (!CreateBook(&myBook->book,onMyBookClose,&bk_base,"EvtEdit",-1,0))
  {
    delete myBook;
    destroy_innative_lng();
    delete(intFolder);
    delete(extFolder);
    SUBPROC(elf_exit);
    return (0);
  }
  int icon_id;
  iconidname2id(IDN_CHECKBOX_UNCHECKED_ICON,-1,&icon_id);
  myBook->check_box_unchecked=icon_id;
  iconidname2id(IDN_CHECKBOX_CHECKED_ICON,-1,&icon_id);
  myBook->check_box_checked=icon_id;
  textidname2id(IDN_CHANGES_HAVE_BEEN_MADE,-1,&myBook->changes_have_been_made);
  textidname2id(IDN_SAVE_BEFORE_EXIT,-1,&myBook->save_before_exit);
  InitializeEvents(&myBook->book, path, fname);
  BookObj_GotoPage(&myBook->book,&bk_main);
  return 0;
};
