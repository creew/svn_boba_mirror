#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\var_arg.h"
#include "main.h"
//#include "log.h"
#include "revision.h"

MyBOOK * EEBook;
int is_changed=0;


#pragma segment="ELFBEGIN"
void elf_exit(void){
  kill_data(__segment_begin("ELFBEGIN"), (void(*)(void*))mfree_adr());
}

int isExtEditorBook(BOOK * struc);

void win12512unicode(wchar_t *ws, const char *s, int len)
{
  int c;
  while((c=*s++)&&((len--)>0))
  {
    if (c==0xA8) c=0x401;
    if (c==0xAA) c=0x404;
    if (c==0xAF) c=0x407;
    if (c==0xB8) c=0x451;
    if (c==0xBA) c=0x454;
    if (c==0xBF) c=0x457;
    if ((c>=0xC0)&&(c<0x100)) c+=0x350;
    *ws++=c;
  }
  *ws=0;
}
char *unicode2win1251(char *s, wchar_t *ws, int len)
{
  char *d=s;
  int c;
  while((c=*ws++)&&((len--)>0))
  {
    if (c==0x401) c=0xA8;
    if (c==0x404) c=0xAA;
    if (c==0x407) c=0xAF;
    if (c==0x451) c=0xB8;
    if (c==0x454) c=0xBA;
    if (c==0x457) c=0xBF;
    if ((c>=0x410)&&(c<0x450)) c-=0x350;
    *s++=c;
  }
  *s=0;
  return(d);
}

int strcmp_nocase(const char *s, const char *d)
{
  int cs;
  int ds;
  do
  {
    cs=*s++;
    if (cs&0x40) cs&=0xDF;
    ds=*d++;
    if (ds&0x40) ds&=0xDF;
    cs-=ds;
    if (cs) break;
  }
  while(ds);
  return(cs);
}

static int TerminateElf(void * ,BOOK* book)
{
  FreeBook(book);
  return(1);
}

static int ShowAuthorInfo(void *mess ,BOOK *book)
{
  MSG * msg = (MSG *)mess;
  wchar_t ustr[64];
  snwprintf(ustr,MAXELEMS(ustr)-1,L"\nExtEditor v1.0\nRevision %d\n(c) KreN",__SVN_REVISION__);
  MessageBox(LGP_NULL,Str2ID(ustr,0,MAXELEMS(ustr)-1),0, 1 ,5000,msg->book);
  return(1);
}

const PAGE_MSG bk_msglst_base[] @ "DYN_PAGE"  = 
{
  ELF_TERMINATE_EVENT,     TerminateElf,
  ELF_SHOW_INFO_EVENT,     ShowAuthorInfo,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_base = {"ExtEditor_Base_Page",0,bk_msglst_base};

void SafeFreeString(MyBOOK *mbk, char *str)
{
  if (str<mbk->ebuf || str>=(mbk->ebuf+mbk->ebuf_len+1))
    delete str;
}

void DestroyStringList(MyBOOK *mbk)
{
  while(mbk->astr->FirstFree)
  {
    char *str=(char *)ListElement_Remove(mbk->astr,0);
    SafeFreeString(mbk, str);
  }
}

char *find_eol(char *s, int *skip)
{
  char *s1=NULL;
  int c;
  while((c=*s))
  {
    if ((c=='\r' && *(s+1)=='\n') || (c=='\n' && *(s+1)=='\r')) {
      s1=s; *skip=2; break;
    }
    if (c=='\r' || c=='\n') {
      s1=s; *skip=1; break;
    }
    s++;
  }
  return s1; 
}

char *strchr(char *s, int ch)
{
  char *ret=NULL;
  int c;
  if (s)
  {
    while((c=*s))
    {
      if (c==ch) return s;
      s++;
    }
  }
  return (ret);
}

#pragma inline
int IsDigit(int c)
{
  return (c>='0' && c<='9');  
}

int IsStringValid(char *s)
{
  int valid=0;
  int c;
  if (s)
  {
    while((c=*s)) 
    {
      if (c!=' ') break;
      s++;
    }    
    if (s[0]=='[' && s[1]=='E' && s[2]=='X' && s[3]=='T' &&
        IsDigit(s[4]) && IsDigit(s[5]) && s[6]==']')
    {
      s+=7;
      int param=0;
      while((s=strchr(s,',')))
      {
        s++;
        param++;
      }
      if (param>=3)
        valid=1;    
    }
  }
  return valid;  
}





int GetExtInfo(char *str, char *ext, char *elf, char *ismall, char *ibig)
{
  int ret=-1;
  char *s=str;
  int c;
  while((c=*s)) 
  {
    if (c!=' ') break;
    s++;
  }
  if (s[0]=='[' && s[1]=='E' && s[2]=='X' && s[3]=='T' &&
      IsDigit(s[4]) && IsDigit(s[5]) && s[6]==']')
  {
    int id=((s[4])-'0')*10+((s[5])-'0');
    s+=7;
    
    // Extension
    while((c=*s))
    {
      if (c!=' ' && c!=':') break;
      s++;
    }
    while((c=*s))
    {
      if (c==' ' || c==',') break;
      if(ext) *ext++=c;
      s++;
    }
    if(ext) *ext=0;
    
    //Big Icon
    while((c=*s))
    {
      if (c!=' ' && c!=',') break;
      s++;
    }
    while((c=*s))
    {
      if (c==' ' || c==',') break;
      if(ibig) *ibig++=c;
      s++;
    }
    if(ibig) *ibig=0;
    
    //  Small Icon
    while((c=*s))
    {
      if (c!=' ' && c!=',') break;
      s++;
    }
    while((c=*s))
    {
      if (c==' ' || c==',') break;
      if(ismall) *ismall++=c;
      s++;
    }    
    if(ismall) *ismall=0;
    
    // Elf
    while((c=*s))
    {
      if (c!=' ' && c!=',') break;
      s++;
    }
    while((c=*s))
    {
      if (c==' ' || c==',') break;
      if(elf) *elf++=c;
      s++;
    }
    if(elf) *elf=0;
    ret=id;
  }
  return (ret);
}



// ============================= INI ================================
void SaveIni(MyBOOK *mbk)
{
  wchar_t path[128];
  wstrcpy(path,GetDir(DIR_ELFS_CONFIG | MEM_INTERNAL));
  wstrcat(path,L"/Extension/ext.ini");
  int ihigh='0', ilow='0';
  int f;
  if ((f=w_fopen(path, WA_Write|WA_Create|WA_Truncate, 0x1FF, NULL)) >=0){
    for(int i=0; i<mbk->astr->FirstFree; i++)
    {
      char *str=(char *)ListElement_GetByIndex(mbk->astr,i);
      char *s=str;
      int c;
      while((c=*s))
      {
        if (c!=' ') break;
        s++;
      }
      if (s[0]=='[' && s[1]=='E' && s[2]=='X' && s[3]=='T' &&
        IsDigit(s[4]) && IsDigit(s[5]) && s[6]==']')
      {
        s[4]=ihigh;
        s[5]=ilow;
        ilow++;
        if (ilow>'9') {ihigh++; ilow='0';}
      }
      w_fwrite(f,str,strlen(str));
      w_fwrite(f,(void *)"\r\n",2);
    }
    w_fclose(f);
  }
}

void ReadIni(MyBOOK *mbk)
{
  wchar_t path[128];
  wstrcpy(path,GetDir(DIR_ELFS_CONFIG | MEM_INTERNAL));
  wstrcat(path,L"/Extension/ext.ini");
  int f;
  if ((f=w_fopen(path, WA_Read, 0x1FF, NULL)) >=0){
    mbk->ebuf_len=w_lseek(f,0,WSEEK_END);
    if (mbk->ebuf_len>0) {
      w_lseek(f,0,WSEEK_SET);
      if (mbk->ebuf) delete mbk->ebuf;
      mbk->ebuf=new char[mbk->ebuf_len+1];
      mbk->ebuf[w_fread(f,mbk->ebuf,mbk->ebuf_len)]=0;
    }
    w_fclose(f);
    DestroyStringList(mbk);
    char *s=mbk->ebuf;
    int skip=0;
    do {
      s+=skip;
      ListElement_Add(mbk->astr, s);
      s=find_eol(s, &skip);
      if (s) *s=0;
    } while(s);
    // �������� ����� ������ ��� ����� � ����� ������ ��������� ��� ����
  }
}

// ========================= DataBrowser ============================

int SelFileFilter(const wchar_t *ExtList, const wchar_t *ItemPath, const wchar_t *ItemName)
{
  if (DataBrowser_isFileInListExt(ExtList,ItemPath,ItemName)) return(1);
  if (EEBook->DB_State==0)
  { 
    FSTAT fs;
    fstat(ItemPath,ItemName,&fs);
    if ((fs.unk1&0x10000)) return(1);
  }
  return(0);
}


static int SelFilePageOnCreate(void *, BOOK *bk)
{
  FILEITEM *fi=NULL;
  MyBOOK *mbk=(MyBOOK *)bk;
  void * DB_Desc=DataBrowserDesc_Create();
  int folder_num;
  static wchar_t folderpics[128];
  const wchar_t * folder_list[2];
  wchar_t *filter;
  if (mbk->DB_State==0)
  {
    W_FSTAT wf;
    wchar_t path[256];
    int res=-1;
    snwprintf(path, MAXELEMS(path)-1, L"%ls%s", GetDir(DIR_ELFS|MEM_INTERNAL), mbk->el_elf);
    if ((res=w_fstat(path,&wf)))
    {
      snwprintf(path, MAXELEMS(path)-1, L"%ls%s", GetDir(DIR_ELFS|MEM_EXTERNAL), mbk->el_elf);
      res=w_fstat(path,&wf);
    }
    if (!res)
    {
      
      wchar_t *fname=wstrrchr(path, L'/');
      if (fname)
      {
        *fname++=0;
        fi=FILEITEM_Create();
        FILEITEM_SetPathAndContentType(fi, path);
        FILEITEM_SetFnameAndContentType(fi, fname);
      }
    }
    folder_list[0]=GetDir(DIR_ELFS|MEM_INTERNAL);
    folder_list[1]=GetDir(DIR_ELFS|MEM_EXTERNAL);
    folder_num=2;
    filter=L"*.elf";
  }
  else
  {
    wchar_t fname[64];
    char *s;
    fi=FILEITEM_Create();
    wstrcpy(folderpics,GetDir(DIR_ELFS_CONFIG | MEM_INTERNAL));
    wstrcat(folderpics,L"/Extension");
    FILEITEM_SetPathAndContentType(fi, folderpics);
    s=mbk->DB_State==1?mbk->el_smicon: mbk->el_bicon;
    win12512unicode(fname,s ,MAXELEMS(fname)-1);
    FILEITEM_SetFnameAndContentType(fi, fname);
    folder_list[0]=folderpics;
    folder_num=1;
    filter=L"*.png;*.gif;*.jpg;*.jpeg";
  }
  DataBrowserDesc_SetHeaderText(DB_Desc,Str2ID(L"SelectFile",0,SID_ANY_LEN));
  DataBrowserDesc_SetBookID(DB_Desc,BOOK_GetBookID(&mbk->book));
  DataBrowserDesc_SetFolders(DB_Desc,folder_list);
  DataBrowserDesc_SetFoldersNumber(DB_Desc,folder_num);

  DataBrowserDesc_SetSelectAction(DB_Desc,1);
  DataBrowserDesc_SetFileExtList(DB_Desc,filter);
  DataBrowserDesc_SetItemFilter(DB_Desc,SelFileFilter);
  if (fi){
    DataBrowserDesc_SetFocusToFILEITEM(DB_Desc, fi);
    FILEITEM_Destroy(fi);
  }
  DataBrowser_Create(DB_Desc);
  DataBrowserDesc_Destroy(DB_Desc);
  return(1);
}


static int SelFilePageOnAccept(void *data, BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  FILEITEM *file=(FILEITEM *)data;
  if (mbk->DB_State==1 || mbk->DB_State==2)
  {
    if (mbk->DB_State==1)
      unicode2win1251(mbk->el_smicon, FILEITEM_GetFname(file), sizeof(mbk->el_smicon)-1);
    else
      unicode2win1251(mbk->el_bicon, FILEITEM_GetFname(file), sizeof(mbk->el_bicon)-1);
  }
  else
  {
    wchar_t *p, *f;
    int pl;
    int len=(pl=wstrlen((p=FILEITEM_GetPath(file))))+wstrlen((f=FILEITEM_GetFname(file)))+2;
    wchar_t *path=new wchar_t[len];
    wstrcpy(path,p);
    path[pl++]='/';
    wstrcpy(path+pl, f);
    p=GetDir(DIR_ELFS|MEM_INTERNAL);
    f=GetDir(DIR_ELFS|MEM_EXTERNAL);
    if (!wstrcmpni(p, path, (pl=wstrlen(p))))
      unicode2win1251(mbk->el_elf, path+pl, sizeof(mbk->el_elf)-1);
    else if (!wstrcmpni(f, path, (pl=wstrlen(f))))
      unicode2win1251(mbk->el_elf, path+pl, sizeof(mbk->el_elf)-1);
    delete path;
  }
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
  return(1);
}

static int SelFilePageOnCancel(void *data, BOOK *bk)
{
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
  return(1);
}

const PAGE_MSG bk_msglst_selfile[] @ "DYN_PAGE"  =
{
  PAGE_ENTER_EVENT_TAG,    SelFilePageOnCreate,
  ACCEPT_EVENT_TAG,        SelFilePageOnAccept,
  CANCEL_EVENT_TAG,        SelFilePageOnCancel,
  PREVIOUS_EVENT_TAG,      SelFilePageOnCancel,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_selbcfg = {"ExtEditor_SelectFile_Page",0,bk_msglst_selfile};

// ========================= SI_EditExt =============================
void OnBackSI_EditExt(BOOK * bk, u16 *string, int len)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  FREE_GUI(myBook->si_editext);
}

void OnOkSI_EditExt(BOOK * bk, wchar_t *string, int len)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  unicode2win1251(myBook->el_ext,string,MAXELEMS(myBook->el_ext)-1);
  FREE_GUI(myBook->si_editext);
}

void CreateSI_EditExt(MyBOOK *myBook)
{
  wchar_t ustr[64];
  STRID text, header_name;
  header_name=Str2ID(L"Input new ext",0,SID_ANY_LEN);
  win12512unicode(ustr,myBook->el_ext,MAXELEMS(ustr)-1);
  text=Str2ID(ustr,0,SID_ANY_LEN);
  myBook->si_editext=(GUI *)CreateStringInput(0,
                                              VAR_HEADER_TEXT(header_name),
                                              VAR_STRINP_MIN_LEN(0),
                                              VAR_STRINP_MAX_LEN(35),
                                              VAR_STRINP_MODE(1),
                                              VAR_BOOK(myBook),
                                              VAR_STRINP_TEXT(text),
                                              VAR_PREV_ACTION_PROC(OnBackSI_EditExt),
                                              VAR_OK_PROC(OnOkSI_EditExt),
                                              0);
}
// =========================== YesNoGui =============================
static void YSYes(BOOK * bk, void *)
{
  MyBOOK *mcb=(MyBOOK *)bk;
  mcb->YesNoFunc(bk, 1);
  FREE_GUI(mcb->yes_no);
}

static void YSNo (BOOK * bk, void *)
{
  MyBOOK *mcb=(MyBOOK *)bk;
  mcb->YesNoFunc(bk, 0);
  FREE_GUI(mcb->yes_no);
}

static void YSPrev (BOOK * bk, void *)
{
  MyBOOK *mcb=(MyBOOK *)bk;
  FREE_GUI(mcb->yes_no);
}

void MsgBoxYesNo(MyBOOK *mbk, wchar_t *qv, void(*f)(BOOK *, int))
{
  STRID q=Str2ID(qv,0,SID_ANY_LEN);
  mbk->YesNoFunc=f;
  mbk->yes_no=CreateYesNoQuestionVA(0,
                                       VAR_BOOK(mbk),
                                       VAR_YESNO_QUESTION(q),
                                       VAR_YESNO_YES_ACTION(YSYes),
                                       VAR_YESNO_NO_ACTION(YSNo),
                                       VAR_PREV_ACTION_PROC(YSPrev),
                                       0);
}
// ========================= EditExtList ============================
const wchar_t * menu_items[] =
{
  L"extension",
  L"elf",
  L"small img",
  L"big img",
};

STRID GetMenuItemName(int item)
{
  return (item<MAXELEMS(menu_items)?Str2ID(menu_items[item],0,SID_ANY_LEN):LGP_NULL);
}

void OnSelect1EditExtList(BOOK * bk, void *)
{
  MyBOOK * mbk=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem((GUI_LIST *)mbk->edit_list);
  switch(item)
  {
  case 0:  // ��������� ��������...
    CreateSI_EditExt(mbk);
    break;
    
  case 1:  // ���� �������
    mbk->DB_State=0;
    BookObj_CallPage(&mbk->book,&bk_selbcfg);
    break;
    
  case 2:  // ���..
    mbk->DB_State=1;
    BookObj_CallPage(&mbk->book,&bk_selbcfg);
    break;
    
  case 3:  // � ���..
    mbk->DB_State=2;
    BookObj_CallPage(&mbk->book,&bk_selbcfg);
    break;
  }
}

void YesNoOnBackExt(BOOK * bk, int i)
{
  MyBOOK * mbk=(MyBOOK *)bk;
  
  if (i==1)
  {
    char tmp[256];
    int len=sprintf(tmp, "[EXT99]: %s, %s, %s, %s", mbk->el_ext,mbk->el_bicon,mbk->el_smicon,mbk->el_elf);
    char *newstr=new char[len+1];
    strcpy(newstr,tmp);
    if (mbk->edit_id!=-1)
    {
      char *str=(char *)ListElement_Remove(mbk->astr,mbk->edit_id);
      SafeFreeString(mbk, str);
      ListElement_Insert(mbk->astr, mbk->edit_id, newstr);
    }
    else
    {
      ListElement_Add(mbk->astr, newstr);
    }
    is_changed=1;
    CreateGuiList(mbk);
  }
  FREE_GUI(mbk->edit_list);
}

void OnBackEditExtList(BOOK * bk, void *)
{
  MyBOOK * mbk=(MyBOOK *)bk;
  int cmp=0;
  if (mbk->edit_id!=-1)
  {
    char *str=(char *)ListElement_GetByIndex(mbk->astr,mbk->edit_id);
    char el_ext[36];
    char el_elf[128];
    char el_smicon[64];
    char el_bicon[64];
    GetExtInfo(str, el_ext, el_elf, el_smicon, el_bicon);
    cmp=strcmp_nocase(el_ext,mbk->el_ext);
    if (!cmp) cmp=strcmp_nocase(el_elf,mbk->el_elf);
    if (!cmp) cmp=strcmp_nocase(el_smicon,mbk->el_smicon);
    if (!cmp) cmp=strcmp_nocase(el_bicon,mbk->el_bicon);
  }
  else
  {
    cmp=*mbk->el_ext;
    if (!cmp) cmp=*mbk->el_elf;
    if (!cmp) cmp=*mbk->el_smicon;
    if (!cmp) cmp=*mbk->el_bicon;
  }
  if (cmp)
  {
    MsgBoxYesNo(mbk, L"Do you want to save changes?",YesNoOnBackExt);
  }
  else
    FREE_GUI(mbk->edit_list);
}

// ������������� ������ � ������� ����
int onLBMessageEditExtList(GUI_MESSAGE * msg)
{
  MyBOOK * myBook = (MyBOOK *) FindBook(isExtEditorBook);
  int item;
  char *str;
  STRID first=LGP_NULL, second=LGP_NULL;
  switch(msg->msg)
  {
    // onCreateListItem
  case 1:
    item=GUIonMessage_GetCreatedItemIndex(msg);
    first=GetMenuItemName(item);
    switch(item)
    {
    case 0:
      str=myBook->el_ext;
      break;
    case 1:
      str=myBook->el_elf;
      break;
    case 2:
      str=myBook->el_smicon;
      break;
    case 3:
      str=myBook->el_bicon;
      break;
    }
    second=Str2ID(str,6,SID_ANY_LEN);
    SetMenuItemText0(msg,first);
    
    SetMenuItemText2(msg,second);
    SetMenuItemText1(msg,second);
  }
  return(1);
};

GUI_LIST *CreateEditExtList(MyBOOK *mbk, int list_id)
{
  GUI_LIST * lo=CreateListObject(&mbk->book,0);
  wchar_t *title, ustr[32];
  mbk->edit_id=list_id;
  if (list_id!=-1)
  {
    char *str=(char *)ListElement_GetByIndex(mbk->astr,list_id);
    GetExtInfo(str, mbk->el_ext, mbk->el_elf, mbk->el_smicon, mbk->el_bicon);
    snwprintf(ustr,MAXELEMS(ustr)-1,L"Edit ext: %s",mbk->el_ext);
    title=ustr;
  }
  else
  {
    title=L"Add new ext";
    *mbk->el_ext=0;
    *mbk->el_elf=0;
    *mbk->el_smicon=0;
    *mbk->el_bicon=0;
  }
  GuiObject_SetTitleText(lo,Str2ID(title, 0, SID_ANY_LEN));
  SetNumOfMenuItem(lo,4);
  SetMenuItemStyle(lo,3);
  SetCursorToItem(lo,0);
  ListMenu_SetOnMessages(lo,onLBMessageEditExtList);
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,OnBackEditExtList);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,OnSelect1EditExtList);
  return (lo);
}


// ======================= GuiList ==================================
// ������������� ������ � ������� ����
int onLBMessage(GUI_MESSAGE * msg)
{
  MyBOOK * myBook = (MyBOOK *) FindBook(isExtEditorBook);
  wchar_t ustr[32];
  int item;
  char *str;
  switch(msg->msg)
  {
    // onCreateListItem
  case 1:
    item=GUIonMessage_GetCreatedItemIndex(msg);
    if (item>0)
    {
      char ext[20], elf[100];
      for (int i=0, icount=1; i<myBook->astr->FirstFree; i++)
      {
      
        str=(char *)ListElement_GetByIndex(myBook->astr,i);
        if (IsStringValid(str))
        {
          if (icount==item)
            break;
          else 
            icount++;
        }
      }
      GetExtInfo(str, ext, elf, 0, 0);
      win12512unicode(ustr,ext,MAXELEMS(ustr)-1);
      SetMenuItemText0(msg,Str2ID(ustr,0,SID_ANY_LEN));
    
      SetMenuItemText2(msg,Str2ID(ustr,0,SID_ANY_LEN));
      win12512unicode(ustr,elf,MAXELEMS(ustr)-1);;
      SetMenuItemText1(msg,Str2ID(ustr,0,SID_ANY_LEN));
    }
    else
    {
      SetMenuItemText0(msg,Str2ID(L"add new ext",0,SID_ANY_LEN));
    }
  }
  return(1);
};

void YesNoOnBackGuiList(BOOK * bk, int i)
{
  MyBOOK * mbk=(MyBOOK *)bk;
  if (i==1)
  {
    SaveIni(mbk);
  }
  FreeBook(&mbk->book);
}


void OnBackGuiList(BOOK * bk, void *)
{
  MyBOOK * mbk=(MyBOOK *)bk;
  if (is_changed)
    MsgBoxYesNo(mbk, L"Do you want to save ext.ini?",YesNoOnBackGuiList);
  else
    FreeBook(&mbk->book);
}

void OnSelect1GuiList(BOOK * bk, void *)
{
  MyBOOK * mbk=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem((GUI_LIST *)mbk->main_gui);
  int listitem;
  if (item>0)
  {
    listitem=0;
    for (int icount=1; listitem<mbk->astr->FirstFree; listitem++)
    {
      char *str=(char *)ListElement_GetByIndex(mbk->astr,listitem);
      if (IsStringValid(str))
      {
        if (icount==item)
          break;
        else 
          icount++;
      }
    }
  }
  else
    listitem=-1;
  mbk->edit_list=CreateEditExtList(mbk, listitem);
  ShowWindow(mbk->edit_list);
}

void OnDeleteGuiList(BOOK * bk, void *)
{
  MyBOOK * mbk=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem((GUI_LIST *)mbk->main_gui);
  if (item>0)
  {
    int listitem=0;
    for (int icount=1; listitem<mbk->astr->FirstFree; listitem++)
    {
      char *str=(char *)ListElement_GetByIndex(mbk->astr,listitem);
      if (IsStringValid(str))
      {
        if (icount==item)
          break;
        else 
          icount++;
      }
    }
    char *str=(char *)ListElement_Remove(mbk->astr,listitem);
    SafeFreeString(mbk, str);
    is_changed=1;
    CreateGuiList(mbk);
  }
}

GUI_LIST * CreateGuiList(MyBOOK * mbk)
{
  int count=1;
  FREE_GUI(mbk->main_gui);
  mbk->main_gui=CreateListObject(&mbk->book,0);
  GuiObject_SetTitleText(mbk->main_gui,Str2ID(L"ExtEditor", 0, SID_ANY_LEN));
  
  for (int i=0; i<mbk->astr->FirstFree; i++)
  {
    char *str=(char *)ListElement_GetByIndex(mbk->astr, i);
    if (IsStringValid(str))
    {
      count++;
    }
  }
  SetNumOfMenuItem(mbk->main_gui,count);
  ListMenu_SetOnMessages(mbk->main_gui,onLBMessage);
  SetMenuItemStyle(mbk->main_gui,3);
  SetCursorToItem(mbk->main_gui,0);
  GUIObject_Softkey_SetAction(mbk->main_gui,ACTION_BACK, OnBackGuiList);
  GUIObject_Softkey_SetAction(mbk->main_gui,ACTION_SELECT1,OnSelect1GuiList);
  GUIObject_Softkey_SetAction(mbk->main_gui,ACTION_DELETE,OnDeleteGuiList);
  ShowWindow(mbk->main_gui);
  return(mbk->main_gui);
};

// ==================================================================

GUI *CreateEditGui(MyBOOK *bk)
{
  GUI *gui=NULL;
  ReadIni(bk);
  gui=(GUI *)CreateGuiList(bk);  
  return (gui);
}


static int MainPageOnEnter(void *, BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  mbk->astr=List_New();
  CreateEditGui(mbk);
  return (1);
}

static int MainPageOnExit(void *, BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  FREE_GUI(mbk->si_editext);
  FREE_GUI(mbk->edit_list);
  FREE_GUI(mbk->main_gui);
  FREE_GUI(mbk->yes_no);
  DestroyStringList(mbk);
  List_Free(mbk->astr);
  delete mbk->ebuf;
  mbk->ebuf=NULL;
  mbk->ebuf_len=0;
  return (1);
}

const PAGE_MSG bk_msglst_main[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,    MainPageOnEnter,
  PAGE_EXIT_EVENT_TAG,     MainPageOnExit,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_main = {"ExtEditor_Main_Page",0,bk_msglst_main};


// ��� �������� �����
static void onMyBookClose(BOOK * book)
{
  SUBPROC(elf_exit);
}

int isExtEditorBook(BOOK * struc)
{
  return(struc->onClose==(void*)onMyBookClose);
}

int main(wchar_t *elfname, wchar_t *path, wchar_t *fname)
{
  EEBook=new MyBOOK;
  memset(EEBook,0,sizeof(MyBOOK));
  if (!CreateBook(EEBook,onMyBookClose,&bk_base,"ExtEditor",-1,0))
  {
    delete EEBook;
    SUBPROC(elf_exit);
    return (0);    
  }
  BookObj_GotoPage(&EEBook->book,&bk_main);
  return 0;
}
