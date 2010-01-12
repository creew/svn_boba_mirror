#include "..\\include\Target.h"
#ifndef _LIB_CLARA_H
#define _LIB_CLARA_H

#include "..\\include\Types.h"
#include "png.h"
#ifndef SWI_HOOK
#include "..\\include\DYN_ext.h"
#include "..\\include\DYN_tag.h"
#include "..\\include\Colors.h"
#endif

#include "..\include\obsolete.h"

extern int *SYNC;
extern int *ASYNC;
extern void *ELF_BEGIN;

#ifdef __cplusplus
extern "C" void kill_data(void *p, void (*func_p)(void *));
#else
extern void kill_data(void *p, void (*func_p)(void *));
#endif

#ifndef _NULL
#define _NULL 0
#endif

#ifndef NULL
#define NULL   _NULL
#endif /* NULL */

#define SID_ANY_LEN 0xFFFF
#define TEXT(__STR__) L##__STR__
#define _T(__STR__) L##__STR__
#define MAXELEMS(x) (sizeof(x)/sizeof(x[0]))
#define STR(__STR__) Str2ID(_T(__STR__),0,SID_ANY_LEN)

#define MINUTE_TICK_EVENT 999
#define ELF_SHOW_INFO_EVENT 998
#define ELF_TERMINATE_EVENT 997
#define ELF_RECONFIG_EVENT 996
#define SBY_REDRAW_RELEASE_EVENT 995


#pragma diag_suppress=Ta035
#pragma diag_suppress=Ta036

#pragma swi_number=0x100
__swi __arm  void IMB(void);

#pragma swi_number=0x101
__swi __arm  void * GetUserData (int size,void (*constr)(void *, void *), void *constr_param);

#pragma swi_number=0x102
__swi __arm  int RemoveUserData(void (*constr)(void *, void *));

#pragma swi_number=0x103
__swi __arm  void *malloc(int size);

#pragma swi_number=0x104
__swi __arm  void mfree(void *p);

#pragma swi_number=0x8104
__swi __arm void *mfree_adr(void);

#pragma swi_number=0x105
__swi __arm  wchar_t * GetDir (int DirIndex);

#pragma swi_number=0x106
__swi __arm  int fopen (const wchar_t * fname, int mode, int rights);

#pragma swi_number=0x107
__swi __arm  int ModifyKeyHook(int(*proc)(int,int,int),int mode);

#pragma swi_number=0x108
__swi __arm  void SUBPROC (void *, ...);


#ifdef __cplusplus
#pragma swi_number=0x108
__swi __arm  void SUBPROC (void(*PROC)(void));

#pragma swi_number=0x108
__swi __arm  void SUBPROC (void(*PROC)(int),int p1);

#pragma swi_number=0x108
__swi __arm  void SUBPROC (void(*PROC)(int,void*),int p1 , void * p2);
#endif

#pragma swi_number=0x109
__swi __arm  void MMIPROC (void(*PROC)(void));

#ifdef __cplusplus
#pragma swi_number=0x109
__swi __arm  void MMIPROC (void(*PROC)(int),int p1);
#pragma swi_number=0x109
__swi __arm  void MMIPROC (void(*PROC)(int,void*),int p1 , void * p2);
#endif

//#pragma swi_number=0x10A
//__swi __arm  int ModifyOSEHook(int msg , void(*proc)(void * msg),int mode);

//#pragma swi_number=0x10B
//__swi __arm void ELF_RemoveFromList(void *elf_begin);

#pragma swi_number=0x10C
__swi __arm int ModifyUIHook(int event , int (*PROC)(UI_MESSAGE*),int mode);

#ifndef SWI_HOOK
#pragma swi_number=0x10D
__swi __arm int elfload(const wchar_t* filename, void *param1, void *param2, void *param3);
#endif
//-------------------------------------------------------------------------------------------

#pragma swi_number=0x112
__swi __arm  void memset(void *mem,char chr,int size);

#pragma swi_number=0x113
__swi __arm  int memcpy(void *dest,const void *source,int cnt);

#pragma swi_number=0x114
__swi __arm  int sprintf(char *buf, const char *fmt, ...);

#pragma swi_number=0x115
__swi __arm  int snwprintf(wchar_t* buffer, int size, const wchar_t* fmt, ...);

#pragma swi_number=0x116
//0x204 - append
//0x004 - write begin

__swi __arm  int _fopen(const wchar_t *filpath, const wchar_t *filname,unsigned int mode , unsigned int rights, unsigned int __0);

#pragma swi_number=0x117
__swi __arm  int fclose(int  file);

#pragma swi_number=0x118
__swi __arm  int fread(int file , void *ptr, int size);

#pragma swi_number=0x119
__swi __arm  int fwrite(int file , const void *ptr, int size);

//#pragma swi_number=0x11A
//__swi __arm  int _wstat(short *path, void *statbuf);

//#pragma swi_number=0x11B
//__swi __arm  int fputs(const char *s, int file);

#pragma swi_number=0x11C
__swi __arm  int fstat(const wchar_t * path, const wchar_t *fname , FSTAT * fstat_stuct);

#pragma swi_number=0x11D
__swi __arm  DIR_HANDLE * AllocDirHandle(const wchar_t * path);

#pragma swi_number=0x11E
__swi __arm  FILELISTITEM *GetFname(DIR_HANDLE *, FILELISTITEM *);

#pragma swi_number=0x11F
__swi __arm  void * DataBrowserDesc_Create(void);

#pragma swi_number=0x120
__swi __arm  void DataBrowserDesc_SetStyle(void * db , int style);

#pragma swi_number=0x121
__swi __arm  void DataBrowserDesc_SetHeaderText(void * db , int StrID);

#pragma swi_number=0x122
__swi __arm  void DataBrowserDesc_SetFolders(void * db , wchar_t const ** FolderList);

#pragma swi_number=0x123
__swi __arm  void DataBrowserDesc_SetOKSoftKeyText(void * db , int StrID);

#pragma swi_number=0x124
__swi __arm  void DataBrowser_Create(void * db);

#pragma swi_number=0x125
__swi __arm  void DataBrowserDesc_Destroy(void * db);

#pragma swi_number=0x126
__swi __arm  wchar_t * getFileExtention(wchar_t * fnane);

#pragma swi_number=0x127
__swi __arm  int DataBrowser_isFileInListExt(const wchar_t * ext_table,const wchar_t * path ,const wchar_t *fname );
#pragma swi_number=0x8127
__swi __arm  DB_FILE_FILTER DataBrowser_isFileInListExt_adr(void);

#pragma swi_number=0x128
__swi __arm  void Timer_ReSet(u16 * timer ,int time, void (*onTimer)(u16 timerID, LPARAM lparam), LPARAM lparam);
#ifdef __cplusplus
#pragma swi_number=0x128
__swi __arm  void Timer_ReSet(u16 * timer ,int time, void (*onTimer)(u16 timerID, void *), void *);
#endif

#pragma swi_number=0x129
__swi __arm  u16 Timer_Set(int time, void (*onTimer)(u16 timerID, LPARAM lparam), LPARAM lparam);
#ifdef __cplusplus
#pragma swi_number=0x129
__swi __arm  u16 Timer_Set(int time, void (*onTimer)(u16 timerID,  void *),  void *);
#endif
#pragma swi_number=0x12A
__swi __arm  void Timer_Kill(u16 *timerID);

#pragma swi_number=0x12B
__swi __arm  int CreateBook(void * mem, void (* onClose)(BOOK*), const PAGE_DESC * bp, const char * name, int __0xFFFFFFFF,int __0);

#pragma swi_number=0x12C
__swi __arm  void BookObj_KillBook(void *Book);

#pragma swi_number=0x12D
__swi __arm  void BookObj_GotoPage(BOOK * book, const PAGE_DESC  * page);

#pragma swi_number=0x12E
__swi __arm  void BookObj_ReturnPage(BOOK * ,int);

#pragma swi_number=0x12F
__swi __arm  void FreeBook ( BOOK *book);


#pragma swi_number=0x130
__swi __arm  BOOK * FindBook(IS_NEEDED_BOOK);

#pragma swi_number=0x131
__swi __arm  BOOK * Find_StandbyBook(void);

#pragma swi_number=0x132
__swi __arm  void BookObj_SetFocus(void *,int focus);

#pragma swi_number=0x133
__swi __arm  void GuiObject_SetTitleText (void * gui , int StrID);

#pragma swi_number=0x134
__swi __arm  void ShowWindow (void * gui);

#pragma swi_number=0x135
__swi __arm  void SetCursorToItem ( void *obj , int item);

#pragma swi_number=0x136
__swi __arm  void GUI_SetStyle(GUI *,int style);

#ifdef __cplusplus
#pragma swi_number=0x136
__swi __arm  void GUI_SetStyle(GUI_LIST *,int style);
#pragma swi_number=0x136
__swi __arm  void GUI_SetStyle(GUI_TABMENUBAR *,int style);
#pragma swi_number=0x136
__swi __arm  void GUI_SetStyle(GUI_ONEOFMANY *,int style);
#pragma swi_number=0x136
__swi __arm  void GUI_SetStyle(GUI_NOFMANY *,int style);
#pragma swi_number=0x136
__swi __arm  void GUI_SetStyle(GUI_FEEDBACK *,int style);
#endif

#pragma swi_number=0x137
__swi __arm  int GUIonMessage_GetCreatedItemIndex ( void *msg);

#pragma swi_number=0x138
__swi __arm  char SetMenuItemText0 ( GUI_MESSAGE *, int StrID);

#pragma swi_number=0x139
__swi __arm  char SetMenuItemText1 ( GUI_MESSAGE *, int StrID);

#pragma swi_number=0x13A
__swi __arm  char SetMenuItemText2 ( GUI_MESSAGE *, int StrID);

#pragma swi_number=0x13B
__swi __arm  char SetMenuItemText3 ( GUI_MESSAGE *, int StrID);

#pragma swi_number=0x13C
__swi __arm  void SetListObjectItemIcon ( void *msg , int align , wchar_t iconID);

#pragma swi_number=0x13D
__swi __arm  void DispObject_SetTitleText(DISP_OBJ* DispObj , int StrID);

#pragma swi_number=0x13E
__swi __arm  int ListMenu_GetSelectedItem(GUI_LIST * );

#pragma swi_number=0x13F
__swi __arm  void SetMenuItemStyle( void *obj , int style);

#pragma swi_number=0x140
__swi __arm  void putchar( void *xx , int x, int y, int _zero, int zero1 , short wchar);

#pragma swi_number=0x8141
__swi __arm IS_NEEDED_BOOK isScreenSaverBook(void);

#pragma swi_number=0x142
__swi __arm  void StatusIndication_SetItemText(void *gui,int item,int StrID);

#pragma swi_number=0x143
__swi __arm  void StatusIndication_ShowNotes(int StrID);

#pragma swi_number=0x144
__swi __arm  GUI * CreateStringInput(int,...);

#pragma swi_number=0x145
__swi __arm  GUI_ONEOFMANY * CreateOneOfMany(BOOK *);

#pragma swi_number=0x146
__swi __arm  void OneOfMany_SetItemCount(GUI_ONEOFMANY * , int count);

#pragma swi_number=0x147
__swi __arm  void OneOfMany_SetChecked(GUI_ONEOFMANY * , int checked);

#pragma swi_number=0x148
__swi __arm  void OneOfMany_SetTexts(GUI_ONEOFMANY * , int * StrID , int Count);

#pragma swi_number=0x149
__swi __arm  int OneOfMany_GetSelected(GUI_ONEOFMANY * rb);

#pragma swi_number=0x14A
__swi __arm  void StatusIndication_Item8_SetText(int StrID);

#pragma swi_number=0x14B
__swi __arm  void GUIObject_Softkey_SetAction(void * gui,u16 actionID,void (*proc)(BOOK *,void*));

#pragma swi_number=0x14C
__swi __arm  void GUIObject_Softkey_SetText(void * gui,u16 actionID , int StrID);

#pragma swi_number=0x14D
__swi __arm  void GUIObject_SoftKey_SetEnable(void *gui,u16 actionID,int ena);

#pragma swi_number=0x14E
__swi __arm  void GUIObject_SoftKey_AddErrorStr(void *gui,u16 actionID,int StrID);

#pragma swi_number=0x14F
__swi __arm  void GUIObject_SoftKey_RemoveItem(void *gui,u16 actionID);

#pragma swi_number=0x150
__swi __arm  void GUIObject_SoftKey_SetVisible(void *gui,u16 actionID,int visible);

#pragma swi_number=0x151
__swi __arm  void GUIObject_SoftKey_SuppressDefaultAction(void *gui,u16 actionID);

#pragma swi_number=0x152
__swi __arm  wchar_t* wstrcpy(wchar_t * dest, const wchar_t * source);

#pragma swi_number=0x153
__swi __arm  wchar_t* wstrncpy(wchar_t * dest, const wchar_t * source, int maxlen);

#pragma swi_number=0x154
__swi __arm  wchar_t* wstrcat(wchar_t * wstr, const wchar_t * subwstr);

#pragma swi_number=0x155
__swi __arm  wchar_t *wstrncat(wchar_t *wstr, const wchar_t *subwstr , int maxlen);

#pragma swi_number=0x156
__swi __arm  int wstrcmp(const wchar_t *wstr1, const wchar_t *wstr2);

#pragma swi_number=0x157
__swi __arm  int wstrlen(const wchar_t * wstr);

#pragma swi_number=0x158
__swi __arm  wchar_t * str2wstr(wchar_t * wstr, const char * str);

#pragma swi_number=0x159
__swi __arm  int strcmp(const char * str1, const char * str2);

#pragma swi_number=0x15A
__swi __arm  int strlen(const char *str);

#pragma swi_number=0x15B
__swi __arm  char * wstr2strn (char * str , const wchar_t * wstr, int maxlen);

#pragma swi_number=0x15C
__swi __arm  int int2strID (int num);

#pragma swi_number=0x15D
__swi __arm  int Str2ID(const void * wstr , int  flag , int len);

#pragma swi_number=0x15E
__swi __arm  void StrID2Str(int StrID,char * str,int maxlen);

#pragma swi_number=0x15F
__swi __arm  int TextID2wstr(int StrID,wchar_t * str,int maxlen);

#pragma swi_number=0x160
__swi __arm  int TextGetLength(int StrID);

#pragma swi_number=0x161
__swi __arm  void TextFree(int StrID);

#pragma swi_number=0x162
__swi __arm  int AB_DEFAULTNBR_GET(int rec_num, void * mem_0x30, int * unk);

#pragma swi_number=0x163
__swi __arm  int AB_READSTRING(AB_STR_ITEM *,int rec_num, int field_ID);

#pragma swi_number=0x164
__swi __arm  int AB_READPHONENBR(AB_NUM_ITEM *,int rec_num, int field_ID);

#pragma swi_number=0x165
__swi __arm  int AB_GETNBROFITEMS(int get_from,int unk_0);

#pragma swi_number=0x166
__swi __arm  int PNUM_len(void *pnum);

#pragma swi_number=0x167
__swi __arm  void PNUM2str(char *str,void *pnum,int len, int max_len);

#pragma swi_number=0x168
__swi __arm  DISP_OBJ * GUIObj_GetDISPObj (void * gui);

#pragma swi_number=0x169
__swi __arm  LIST *List_New(void);

#pragma swi_number=0x16A
__swi __arm  void List_Free(LIST *lst);

#pragma swi_number=0x16B
__swi __arm  void ListElement_AddtoTop(LIST *lst,void *newElement);

#pragma swi_number=0x16C
__swi __arm  int Gif2ID(u16 IMAGEHANDLE,const wchar_t * path, const wchar_t * fname,wchar_t * ID);

#pragma swi_number=0x16D
__swi __arm  int REQUEST_IMAGEHANDLER_INTERNAL_GETHANDLE(const int * __zero,u16 * IMAGEHANDLE,char * unk);

#pragma swi_number=0x16E
__swi __arm  void REQUEST_DATEANDTIME_GET(const int * __zero , DATETIME *dt);

#pragma swi_number=0x16F
__swi __arm  void IndicationDevice_Backlight_FadeToLevel(int unk_zero ,int bl_level);

#pragma swi_number=0x170
__swi __arm  int GetFreeBytesOnHeap(void);

#pragma swi_number=0x171
__swi __arm  void BookObj_Hide(BOOK *,int display_type);

#pragma swi_number=0x172
__swi __arm  void BookObj_Show(BOOK *,int display_type);

#pragma swi_number=0x173
__swi __arm  void StartAPP (const wchar_t * appname);

#pragma swi_number=0x174
__swi __arm  void ListMenu_SetOnMessages (GUI_LIST * , int (*proc)(GUI_MESSAGE *));

#pragma swi_number=0x175
__swi __arm  char * manifest_GetParam(const char *buf, const char *param_name,int unk);

#pragma swi_number=0x176
__swi __arm  int lseek(int file,int offset,int mode);

#pragma swi_number=0x8177
__swi __arm  int get_VBUFFER(void);

#pragma swi_number=0x178
__swi __arm  void SetLampLevel (int level);

#pragma swi_number=0x179
__swi __arm  void * ListElement_Remove(LIST *lst,int index);

#pragma swi_number=0x17A
__swi __arm  int GetCurrentPID(void);

#pragma swi_number=0x17B
__swi __arm  int ListElement_Prt2NumElement(LIST *lst,void *ptr);


#pragma swi_number=0x17C
__swi   __arm union SIGNAL *alloc(OSBUFSIZE size,SIGSELECT signo);
#pragma swi_number=0x17D
__swi   __arm union SIGNAL *receive(const SIGSELECT *sigsel);
#pragma swi_number=0x17E
__swi   __arm void send(union SIGNAL **sig,PROCESS to);
#pragma swi_number=0x17F
__swi   __arm PROCESS sender(union SIGNAL **sig);
#pragma swi_number=0x180
__swi   __arm void free_buf(union SIGNAL **sig);
#pragma swi_number=0x181
__swi   __arm PROCESS create_process(int proc_type,char *name,OSENTRYPOINT *entrypoint,OSADDRESS stack_size,OSPRIORITY priority,OSTIME timeslice,PROCESS pid_block,void *redir_table,OSVECTOR vector,OSUSER user);
#pragma swi_number=0x182
__swi   __arm void start(PROCESS pid);
#pragma swi_number=0x183
__swi   __arm int get_ptype(PROCESS pid);
#pragma swi_number=0x184
__swi   __arm PROCESS current_process(void);
#pragma swi_number=0x185
__swi   __arm void delay(OSTIME timeout);
#pragma swi_number=0x186
__swi   __arm OSBOOLEAN hunt(const char *name, OSUSER user, PROCESS *name_, union SIGNAL **hunt_sig);
#pragma swi_number=0x187
__swi   __arm void kill_proc(PROCESS pid);
//#pragma swi_number=0x188
//__swi   __arm OSBOOLEAN set_bp (PROCESS pid,OSADDRESS address,OSADDRESS attribute,union SIGNAL **trapsig);
//#pragma swi_number=0x8188
//__swi   __arm int set_bp_adr (void);
#pragma swi_number=0x189
__swi   __arm union SIGNAL *receive_w_tmo (OSTIME timeout, SIGSELECT *sel);
//#pragma swi_number=0x18A
//__swi   __arm OSBOOLEAN clear_bp (PROCESS pid, OSADDRESS addr);
//#pragma swi_number=0x818A
//__swi   __arm int clear_bp_adr (void);
#pragma swi_number=0x18B
__swi   __arm void stop(PROCESS pid);
#pragma swi_number=0x18C
__swi   __arm OSBOOLEAN get_mem(PROCESS pid, OSADDRESS from, void* to, OSADDRESS size);
#pragma swi_number=0x18D
__swi   __arm OSADDRESS get_envp (PROCESS pid, char *name);
#pragma swi_number=0x18E
__swi   __arm OSBOOLEAN set_envp (PROCESS pid, char *name, OSADDRESS value);
#pragma swi_number=0x18F
__swi   __arm PROCESS get_bid (PROCESS pid);

#pragma swi_number=0x1A0
__swi __arm  char* strcpy(char * dest, const char * source);

#pragma swi_number=0x1A1
__swi __arm  void *CreateMessage(int size,int ev,char *name);
#pragma swi_number=0x1A2
__swi __arm  void SendMessage(void **signal,int PID);
#pragma swi_number=0x1A3
__swi __arm  void * WaitMessage(void *SIGSEL);
#pragma swi_number=0x1A4
__swi __arm  int FreeMessage(void **Mess);
#pragma swi_number=0x1A5
__swi __arm  void SendDispatchMessage(int id, int unk_zero, int size,void *mess);

#pragma swi_number=0x1A6
__swi __arm  char * UIEventName(int event);

#pragma swi_number=0x81A7
__swi __arm int * MissedEvents(void);

#pragma swi_number=0x1A8
__swi __arm  void  UI_Event(int event);
#pragma swi_number=0x1A9
__swi __arm  void  UI_Event_wData(int event,void *message,void (*mfree)(void*));
#pragma swi_number=0x1AA
__swi __arm  void  UI_Event_toBookID(int event,int BookID);
#pragma swi_number=0x1AB
__swi __arm  void  UI_Event_toBookIDwData(int event,int BookID,void *message,void (*mfree)(void*));
#pragma swi_number=0x1AC
__swi __arm  int ListElement_Find(LIST *lst,void *element, int (*cmp_proc)(void *,void *));


#pragma swi_number=0x1AD
__swi __arm  void * ListElement_GetByIndex(LIST * , int index);
#pragma swi_number=0x1AE
__swi __arm  wchar_t* wstrrchr(const wchar_t* wstr, wchar_t wchar);
#pragma swi_number=0x1AF
__swi __arm  void BookObj_CallSubroutine(BOOK * , BOOK_SUBROUTINE *subr);
#pragma swi_number=0x1B0
__swi __arm  void ListElement_Add(LIST *lst,void *newElement);

#pragma swi_number=0x1B1
__swi __arm  void debug_printf(const char * fmt,...);

#pragma swi_number=0x1B2
__swi __arm  int PlayFile(const wchar_t * path, const wchar_t * fname);

#pragma swi_number=0x1B4
__swi __arm int REQUEST_PROFILE_GETACTIVEPROFILE(const int *__zero , int *level);

#pragma swi_number=0x1B5
__swi __arm  int Profile_SetActive(int NUMprof,int set_in_gdfs_flag);

#pragma swi_number=0x1B7
__swi __arm  int  isKeylocked(void);

#pragma swi_number=0x81B8
__swi __arm IS_NEEDED_BOOK isMediaPlayerVideoBook(void);

#pragma swi_number=0x1BC
__swi __arm int REQUEST_DATEFORMAT_GET(const int *SYNC, char *DateFormat);

#pragma swi_number=0x1BD
__swi __arm int REQUEST_TIMEFORMAT_GET(const int *SYNC, char *TimeFormat);

#pragma swi_number=0x1BE
__swi __arm int Date2ID(DATE *, int TimeFormat,int);

#pragma swi_number=0x1BF
__swi __arm int Time2ID(TIME *, char TimeFormat,int isSec);

#pragma swi_number=0x1C0
__swi __arm  GUI_LIST * CreateListObject(BOOK * , void * unk);
#pragma swi_number=0x1C1
__swi __arm  void SetNumOfMenuItem(void * , int  item_count);

#ifndef SWI_HOOK
#pragma swi_number=0x81C2
__swi __arm LIST ** ROOT_APP(void);
#endif
#pragma swi_number=0x1C3
__swi __arm  void SoftKey_AddHelpStr(void *gui,int msg,int StrID);
#pragma swi_number=0x1C4
__swi __arm  void SoftKey_SetItemAsSubItem(void *gui,int item,int subitem);
#pragma swi_number=0x1C5
__swi __arm  int REQUEST_SYSTEM_SHUTDOWN(void);
#pragma swi_number=0x1C6
__swi __arm  int REQUEST_SYSTEM_RESTART(void);
#pragma swi_number=0x1C7
__swi __arm  void SoftKeys_SetText(void *gui,int msg,int StrID,int StrID1);

#pragma swi_number=0x1C8
__swi __arm  int isRightNowBook(BOOK *);
#pragma swi_number=0x1C9
__swi __arm  int isVolumeControllerBook(BOOK *);


#pragma swi_number=0x1CA
__swi __arm  GUI_TABMENUBAR * CreateTabMenuBar(BOOK *);
#pragma swi_number=0x1CB
__swi __arm  void TabMenuBar_SetTabCount(GUI_TABMENUBAR * , int count);
#pragma swi_number=0x1CC
__swi __arm  void TabMenuBar_AssignGuiObj(GUI_TABMENUBAR * , int tab , GUI *);

#ifdef __cplusplus
#pragma swi_number=0x1CC
__swi __arm  void TabMenuBar_AssignGuiObj(GUI_TABMENUBAR * , int tab , GUI_LIST *);
#pragma swi_number=0x1CC
__swi __arm  void TabMenuBar_AssignGuiObj(GUI_TABMENUBAR * , int tab , GUI_ONEOFMANY *);
#pragma swi_number=0x1CC
__swi __arm  void TabMenuBar_AssignGuiObj(GUI_TABMENUBAR * , int tab , GUI_NOFMANY *);
#endif

#pragma swi_number=0x1CD
__swi __arm  void TabMenuBar_SetTabIcon(GUI_TABMENUBAR *,int tab, wchar_t icon , int for_state);

#pragma swi_number=0x1CE
__swi __arm  GUI *GUI_Free(GUI*);
#ifdef __cplusplus
#pragma swi_number=0x1CE
__swi __arm  GUI_LIST *GUI_Free(GUI_LIST *);
#pragma swi_number=0x1CE
__swi __arm  GUI_ONEOFMANY *GUI_Free(GUI_ONEOFMANY *);
#pragma swi_number=0x1CE
__swi __arm  GUI_NOFMANY *GUI_Free(GUI_NOFMANY *);
#pragma swi_number=0x1CE
__swi __arm  GUI_FEEDBACK *GUI_Free(GUI_FEEDBACK *);
#endif

#pragma swi_number=0x1CF
__swi __arm void InputFeedback_SetIcon(GUI *gui, wchar_t icon);

#pragma swi_number=0x1D0
__swi __arm int StringInput_GetStringAndLen (GUI*,wchar_t**,u16*);

#pragma swi_number=0x81D1
__swi __arm PAudioControl* GetAudioControlPtr();
#pragma swi_number=0x1D2
__swi __arm int AudioControl_Vibrate(PAudioControl pIAudioControl, int vibratime, int pausetime, int totaltime);

#pragma swi_number=0x1D4
__swi __arm  GUI_NOFMANY * CreateNOfMany(BOOK *);

#pragma swi_number=0x1D5
__swi __arm  void NOfMany_SetTexts(GUI_NOFMANY * , int * StrIDtable, int items_count);

#pragma swi_number=0x1D6
__swi __arm  void NOfMany_SetChecked(GUI_NOFMANY * , u16 * checked_table, int items_count);

#pragma swi_number=0x1D7
__swi __arm  void NOfMany_SetCursor(GUI_NOFMANY * , int item);

#pragma swi_number=0x1D8
__swi __arm  int NOfMany_GetCheckedCount(GUI_NOFMANY *);

#pragma swi_number=0x1D9
__swi __arm  int NOfMany_GetChecked(GUI_NOFMANY * , u16 * buffer );

#pragma swi_number=0x1DA
__swi __arm  void NOfMany_SetonMessage(GUI_NOFMANY * , void * proc );

#pragma swi_number=0x1DF
__swi __arm  PAudioControl AudioControl_Init();

#pragma swi_number=0x1E0
__swi __arm  void OneOfMany_SetFocused(GUI_ONEOFMANY * , int item);
#pragma swi_number=0x1E1
__swi __arm  void OneOfMany_SetonMessage(GUI_ONEOFMANY * , int (*proc)(GUI_MESSAGE *));



#pragma swi_number=0x1E2
__swi __arm GUI_FEEDBACK * CreateFeedBack (int,...);
#pragma swi_number=0x1E3
__swi __arm void  FeedBack_SetText(GUI_FEEDBACK*,int);
#pragma swi_number=0x1E4
__swi __arm int GetBattaryState(const int * , BATT *);
#pragma swi_number=0x1E5
__swi __arm void * get_DisplayGC (void);
#pragma swi_number=0x1E6
__swi __arm void GetGuiRect(void*,RECT*);
#pragma swi_number=0x1E7
__swi __arm  void get_GC_RECT(void*,RECT*);
#pragma swi_number=0x1E8
__swi __arm int get_GC_xx(void * GC);
#pragma swi_number=0x1E9
__swi __arm void *set_GC_xx(void * GC, int);
#pragma swi_number=0x1EA
__swi __arm void GC_validate_RECT(void * GC , RECT*);
#pragma swi_number=0x1EB
__swi __arm void addGui2book(BOOK*,GUI*);
#pragma swi_number=0x1EC
__swi __arm void DrawRect(int x1, int y1,int x2 ,int y2,int pen_color,int brush_color);
#pragma swi_number=0x1ED
__swi __arm void DrawString(int StrID,int align,int x1,int y1,int x2,int y2,int unk,int unk1,int pen_color ,int brush_color);
#pragma swi_number=0x1EE
__swi __arm  void addGui2XGuiList (void* xguilist , GUI*);
#pragma swi_number=0x1EF
__swi __arm void InvalidateRect(DISP_OBJ *,RECT*);
#pragma swi_number=0x1F0
__swi __arm int CreateObject(GUI*,void (*)(DISP_DESC *),void (*)(DISP_DESC *),BOOK *,void(*)(),int,int size_obj);
#pragma swi_number=0x1F1
__swi __arm int SetFont(int);
#pragma swi_number=0x81F2
__swi __arm DISP_OBJ ** StatusRow_p(void);

#pragma swi_number=0x1F3
__swi __arm int root_list_get_session_count(void);
#pragma swi_number=0x1F4
__swi __arm UI_APP_SESSION *root_list_get_session(int num_session);
#pragma swi_number=0x1F5
__swi __arm BOOK *SESSION_GetTopBook(UI_APP_SESSION *);

//15.11.07
#pragma swi_number=0x1F6
__swi __arm int MainInput_getVisible(GUI *);
#pragma swi_number=0x1F7
__swi __arm int MainInput_strlen(GUI *);
#pragma swi_number=0x1F8
__swi __arm char * MainInput_getPNUM(GUI *);
#pragma swi_number=0x1F9
__swi __arm int MainInput_isPlus(GUI *);

#pragma swi_number=0x1FA
__swi __arm int BOOK_GetBookID(BOOK * );
#pragma swi_number=0x1FB
__swi __arm int wstrncmp(const wchar_t*,const wchar_t*,int);
#pragma swi_number=0x1FC
__swi __arm  int DISP_OBJ_GetAbsoluteXPos(DISP_OBJ *);
#pragma swi_number=0x1FD
__swi __arm int DISP_OBJ_GetAbsoluteYPos(DISP_OBJ *);
#pragma swi_number=0x1FE
__swi __arm WINDOW * DISP_OBJ_GetWindow(DISP_OBJ *);
#pragma swi_number=0x1FF
__swi __arm int GetThemeColor(int , int);
#pragma swi_number=0x200
__swi __arm int REQUEST_SETTING_SILENCE_SET(const int * __zero,u16 profile,u16 value);
#pragma swi_number=0x201
__swi __arm int REQUEST_SETTING_SILENCE_GET(const int * __zero,u16 profile, char * silence_mode);

//
#pragma swi_number=0x202
__swi __arm void VCALL_Init (void* vc);
#pragma swi_number=0x203
__swi __arm void VCALL_SetName (void * vc , wchar_t * name ,unsigned short name_len);
#pragma swi_number=0x204
__swi __arm void VCALL_SetNameIcon (void * vc , wchar_t icon);
#pragma swi_number=0x205
__swi __arm void VCALL_SetNumber (void * vc , wchar_t * number , unsigned short num_len);
#pragma swi_number=0x206
__swi __arm void VCALL_SetHZ1 (void * vc, int , u16 );
#pragma swi_number=0x207
__swi __arm void VCALL_SetHZ2 (void * vc , u16 );
#pragma swi_number=0x208
__swi __arm void MakeVoiceCall (int SessioID , void * vc ,int flag);

#pragma swi_number=0x209
__swi __arm int isDirectory (wchar_t * path ,wchar_t * filename , int * error );
#pragma swi_number=0x20A
__swi __arm void FileDelete (wchar_t * path ,wchar_t * filename , int * error );
#pragma swi_number=0x20B
__swi __arm void SetFocus (GUI * , int);

#pragma swi_number=0x20C
__swi __arm int CreateSMSCont (int, void*);
#pragma swi_number=0x20D
__swi __arm int FreeSMSCont (void*);
#pragma swi_number=0x20E
__swi __arm int PrepareSMSCont (void*,wchar_t*,wchar_t*,int,int);
#pragma swi_number=0x20F
__swi __arm int CteateNewMessage (int,void*,int);
#pragma swi_number=0x8210
__swi __arm  void *  get_APP_DESC_TABLE(void);


#pragma swi_number=0x211
__swi __arm  C_INTERFACE *Window_GetComponentInterface(WINDOW*);

#pragma swi_number=0x212
__swi __arm  int AB_POSITIONNBR_GET(void * ab_name,char * ,u16 * ,void * ab_num);
#pragma swi_number=0x213
__swi __arm  int AB_NAME_ITEM2StrID(AB_STR_ITEM * ab_name);
#pragma swi_number=0x214
__swi __arm  int AB_NUM_ITEM2StrID(AB_NUM_ITEM * ab_num_item);
#pragma swi_number=0x215
__swi __arm  int GetABRecNum(AB_GET_STRUCT *);
#pragma swi_number=0x8216
__swi __arm  AB_ITEM_DESC * get_AB_ITEMS_DESC();


#pragma swi_number=0x217
__swi __arm  GUI * SBY_GetMainInput(BOOK * StandBy);              // !!!!!  �� ��������� !!!!!!
#pragma swi_number=0x218
__swi __arm  GUI * SBY_GetStatusIndication(BOOK * StandBy);       // !!!!!  �� ��������� !!!!!!

#pragma swi_number=0x219
__swi __arm  int REQUEST_PHONEBOOK_ACCESSSTATUS_TOTAL_GET(const int * __zero, int *);

#pragma swi_number=0x821A
__swi __arm  int * PID_MMI(void);

#pragma swi_number=0x21C
__swi __arm GUI * DISP_OBJ_GetGUI(DISP_OBJ *);

#pragma swi_number=0x21D
__swi __arm DISP_OBJ* DISPBASE_GetFocused(int display);
#pragma swi_number=0x21E
__swi __arm char DISP_OBJ_getVisible(DISP_OBJ*);


typedef int (*DISP_OBJ_ONCREATE_METHOD)(DISP_OBJ *);
typedef void (*DISP_OBJ_ONCLOSE_METHOD)(DISP_OBJ *);
typedef void (*DISP_OBJ_ONREDRAW_METHOD)(DISP_OBJ *,int ,int,int);
typedef void (*DISP_OBJ_ONKEY_METHOD)(DISP_OBJ *,int ,int,int,int);


typedef void (*DISP_OBJ_METHOD)(DISP_OBJ *);

#pragma swi_number=0x21F
__swi __arm DISP_DESC* DISP_OBJ_GetDESC (DISP_OBJ* obj);                          // !!!!!  �� ��������� !!!!!!

#pragma swi_number=0x220
__swi __arm void DISP_DESC_SetName (DISP_DESC* , const char * name);
#pragma swi_number=0x221
__swi __arm char* DISP_OBJ_GetName (DISP_OBJ *);
#pragma swi_number=0x222
__swi __arm void DISP_DESC_SetSize (DISP_DESC* , u16 size);
#pragma swi_number=0x223
__swi __arm u16 DISP_DESC_GetSize (DISP_OBJ *);
//
//
#pragma swi_number=0x224
__swi __arm void DISP_DESC_SetOnCreate (DISP_DESC* , DISP_OBJ_ONCREATE_METHOD);
#pragma swi_number=0x225
__swi __arm DISP_OBJ_ONCREATE_METHOD DISP_OBJ_GetOnCreate (DISP_OBJ *);
#pragma swi_number=0x226
__swi __arm void DISP_DESC_SetOnClose (DISP_DESC* , DISP_OBJ_ONCLOSE_METHOD);
#pragma swi_number=0x227
__swi __arm DISP_OBJ_ONCLOSE_METHOD DISP_OBJ_GetOnClose (DISP_OBJ *);
#pragma swi_number=0x228
__swi __arm void DISP_DESC_SetOnRedraw (DISP_DESC* , DISP_OBJ_ONREDRAW_METHOD);
#pragma swi_number=0x229
__swi __arm DISP_OBJ_ONREDRAW_METHOD DISP_OBJ_GetOnRedraw (DISP_OBJ *);
#pragma swi_number=0x22A
__swi __arm void DISP_DESC_SetOnKey (DISP_DESC* , DISP_OBJ_ONKEY_METHOD);
#pragma swi_number=0x22B
__swi __arm DISP_OBJ_ONKEY_METHOD DISP_OBJ_GetOnKey (DISP_OBJ *);



#pragma swi_number=0x22C
__swi __arm void DISP_DESC_SetonRefresh (DISP_DESC* ,DISP_OBJ_METHOD);
#pragma swi_number=0x22D
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetonRefresh (DISP_OBJ *);
#pragma swi_number=0x22E
__swi __arm void DISP_DESC_SetMethod04 (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x22F
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod04 (DISP_OBJ *);
#pragma swi_number=0x230
__swi __arm void DISP_DESC_SetMethod05 (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x231
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod05 (DISP_OBJ *);
#pragma swi_number=0x232
__swi __arm void DISP_DESC_SetMethod06 (DISP_DESC* ,DISP_OBJ_METHOD);
#pragma swi_number=0x233
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod06 (DISP_OBJ *);
#pragma swi_number=0x234
__swi __arm void DISP_DESC_SetMethod07 (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x235
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod07 (DISP_OBJ *);
#pragma swi_number=0x236
__swi __arm void DISP_DESC_SetMethod08 (DISP_DESC* ,DISP_OBJ_METHOD);
#pragma swi_number=0x237
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod08 (DISP_OBJ *);
#pragma swi_number=0x238
__swi __arm void DISP_DESC_SetMethod09 (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x239
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod09 (DISP_OBJ *);
#pragma swi_number=0x23A
__swi __arm void DISP_DESC_SetMethod0A (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x23B
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod0A (DISP_OBJ *);
#pragma swi_number=0x23C
__swi __arm void DISP_DESC_SetMethod0B (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x23D
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod0B (DISP_OBJ *);
#pragma swi_number=0x23E
__swi __arm void DISP_DESC_SetMethod0C (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x23F
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod0C (DISP_OBJ *);

#pragma swi_number=0x240
__swi __arm  void JavaSession_Manager (int cmd);
#pragma swi_number=0x241
__swi __arm  int JavaSession_GetName(void);
#pragma swi_number=0x242
__swi __arm  int TextCopyId(int);
#pragma swi_number=0x243
__swi __arm  UI_APP_SESSION *Book_GetSession(BOOK*);
#pragma swi_number=0x244
__swi __arm  void List2lineSetAsSubitem (void* msg,int unk,int n_sub_items);
#pragma swi_number=0x245
__swi __arm  void List2lineSubitemAddText(void* msg,int StrID);
#pragma swi_number=0x246
__swi __arm  int GUIonMessage_GetCreatedSubItemParrentIndex(void * msg);
#pragma swi_number=0x247
__swi __arm void GoMusic(void);

#pragma swi_number=0x24A
__swi __arm int PlayFileV(const wchar_t * path,const wchar_t * fname, int vol);
#pragma swi_number=0x24B
__swi __arm int GetSilent(void);
#pragma swi_number=0x24C
__swi __arm int GetVibrator(int ignorevibrator,int ignoresilent);
#pragma swi_number=0x24D
__swi __arm int GetAudioSettings(int what, char* retvalue);

#pragma swi_number=0x24E
__swi __arm int GetChipID(void);

#pragma swi_number=0x252
__swi __arm  int List2LineGetCurrentIndex(void * msg);
#pragma swi_number=0x253
__swi __arm  int GUIonMessage_GetCurrentItemIndex(void * msg);
#pragma swi_number=0x254
__swi __arm  void DestroyDirHandle(DIR_HANDLE *handle);
#pragma swi_number=0x8255
__swi __arm  DB_EXT *LastExtDB(void);
#pragma swi_number=0x256
__swi __arm  void GetOtherExtMethods(FILESUBROUTINE **);
#pragma swi_number=0x257
__swi __arm  void MessageBox(int HeaderStrID,int MessageStrID,wchar_t IconID, int style /*1 or 2*/,int time,BOOK * unk);
#pragma swi_number=0x258
__swi __arm  int Display_GetHeight(int Display);
#pragma swi_number=0x259
__swi __arm  int Display_GetWidth(int Display);
#pragma swi_number=0x25A
__swi __arm  void PlaySystemSound (int SndNumber);


#pragma swi_number=0x25D
__swi __arm  int TabMenuBar_GetFocusedTabIndex(GUI_TABMENUBAR * );
#ifdef __cplusplus
#pragma swi_number=0x25D
__swi __arm  int TabMenuBar_GetFocusedTabIndex(GUI * );
#endif
#pragma swi_number=0x25E
__swi __arm  void TabMenuBar_SetTabFocused(GUI_TABMENUBAR * , int tab_num);
#pragma swi_number=0x25F
__swi __arm  int GC_GetPenColor (void* GC);
#pragma swi_number=0x260
__swi __arm  void GC_SetPenColor(void* GC , int pen_color);

#pragma swi_number=0x261
__swi __arm  void GC_SetPixel (void* GC,int x1,int y1,int color);
#pragma swi_number=0x262
__swi __arm  void GC_DrawLine(void* GC,int x1, int y1, int x2, int y2);
#pragma swi_number=0x263
__swi __arm  void GC_DrawFRect (void* GC,int color,int x1,int y1,int x2 ,int y2);
#pragma swi_number=0x264
__swi __arm  int GC_DrawBitmap(void *GC, int x1, int y1, int x2, int y2, int * bmp);

#pragma swi_number=0x265
__swi __arm  png_structp png_create_read_struct_2(png_const_charp user_png_ver, png_voidp error_ptr,png_error_ptr error_fn, png_error_ptr warn_fn, png_voidp mem_ptr,png_malloc_ptr malloc_fn, png_free_ptr free_fn);
#pragma swi_number=0x266
__swi __arm  void png_destroy_read_struct(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);
#pragma swi_number=0x267
__swi __arm  png_infop png_create_info_struct(png_structp png_ptr);
#pragma swi_number=0x268
__swi __arm  void png_destroy_info_struct(png_structp png_ptr, png_infopp info_ptr_ptr);
#pragma swi_number=0x269
__swi __arm  void png_set_progressive_read_fn(png_structp png_ptr, png_voidp progressive_ptr, png_progressive_info_ptr info_fn, png_progressive_row_ptr row_fn, png_progressive_end_ptr end_fn);
#pragma swi_number=0x26A
__swi __arm  void png_process_data(png_structp png_ptr, png_infop info_ptr, png_bytep buffer, png_size_t buffer_size);
#pragma swi_number=0x26B
__swi __arm  void png_read_update_info(png_structp png_ptr, png_infop info_ptr);
#pragma swi_number=0x26C
__swi __arm  png_voidp png_get_progressive_ptr(png_structp png_ptr);
#pragma swi_number=0x26D
__swi __arm  png_uint_32 png_get_IHDR(png_structp png_ptr, png_infop info_ptr, png_uint_32 *width, png_uint_32 *height, int *bit_depth, int *color_type, int *interlace_method, int *compression_method, int *filter_method);
#pragma swi_number=0x26E
__swi __arm  void png_set_strip_16(png_structp png_ptr);
#pragma swi_number=0x26F
__swi __arm  void png_set_gray_1_2_4_to_8(png_structp png_ptr);
#pragma swi_number=0x270
__swi __arm  void png_set_gray_to_rgb(png_structp png_ptr);
#pragma swi_number=0x271
__swi __arm  png_uint_32 png_get_valid(png_structp png_ptr, png_infop info_ptr, png_uint_32 flag);
#pragma swi_number=0x272
__swi __arm  void png_set_tRNS_to_alpha(png_structp png_ptr);
#pragma swi_number=0x273
__swi __arm  void png_set_filler(png_structp png_ptr, png_uint_32 filler, int flags);
#pragma swi_number=0x274
__swi __arm  void png_set_palette_to_rgb(png_structp png_ptr);
#pragma swi_number=0x275
__swi __arm  png_uint_32 png_get_rowbytes(png_structp png_ptr, png_infop info_ptr);
#pragma swi_number=0x276
__swi __arm  void png_progressive_combine_row(png_structp png_ptr, png_bytep old_row, png_bytep new_row);
#pragma swi_number=0x277
__swi __arm  int png_sig_cmp(png_bytep sig, png_size_t start, png_size_t num_to_check);
#pragma swi_number=0x278
__swi __arm int setjmp(jmp_buf jmpbuf);
#pragma swi_number=0x279
__swi __arm void longjmp(jmp_buf, int);
#pragma swi_number=0x27A
__swi __arm int inflateInit2_ (z_streamp strm, int windowBits,const char *version, int stream_size);
#pragma swi_number=0x27B
__swi __arm int inflate (z_streamp strm, int flush);
#pragma swi_number=0x27C
__swi __arm int inflateEnd (z_streamp strm);
#pragma swi_number=0x27D
__swi __arm int memcmp (const void *m1,const void *m2,int n);
#pragma swi_number=0x27E
__swi __arm int strncmp(const char *s1, const char *s2, int len);
#pragma swi_number=0x27F
__swi __arm char *strncpy (char *dest,const char *source,int maxlen);
#pragma swi_number=0x280
__swi __arm char *strcat(char *dest, const char *src);
#pragma swi_number=0x281
__swi __arm void GuiObject_SetTitleType(GUI *gui, int type);

#pragma swi_number=0x282
__swi __arm void GUIonMessage_SetItemDisabled (void * msg, int Disabled);
#pragma swi_number=0x283
__swi __arm int REQUEST_IMAGEHANDLER_INTERNAL_REGISTER(const int * __zero,u16  ImageHandle,wchar_t *path,wchar_t *fname,int unk, wchar_t * ImageID ,char * error);
#pragma swi_number=0x284
__swi __arm int REQUEST_IMAGEHANDLER_INTERNAL_UNREGISTER (const int * __zero,u16 ImageHandle,u16* ,u16* ,wchar_t ImageID, int unk_1,char * error);
#pragma swi_number=0x8285
__swi __arm SURFACE ** get_Surfaces(void);

#pragma swi_number=0x286
__swi __arm int iconidname2id(const wchar_t* idname,int maxnamelen,int *id);
#pragma swi_number=0x287
__swi __arm int textidname2id(const wchar_t* idname,int maxnamelen,int* id);

#pragma swi_number=0x288
__swi __arm void ListMenu_SetNoItemText(GUI_LIST *, int str);

#pragma swi_number=0x828A
__swi __arm IS_NEEDED_BOOK isFmRadioBook(void);
#pragma swi_number=0x828B
__swi __arm IS_NEEDED_BOOK isAudioPlayerBook(void);
#pragma swi_number=0x28C
__swi __arm void PlayerControl(BOOK *AudioPlayerBook, int);
#pragma swi_number=0x28D
__swi __arm void SwitchRadioStationFromList(BOOK *FmRadioBook, int);

#pragma swi_number=0x28E
__swi __arm  void Shortcut_Run (wchar_t * shortcut_name);

#pragma swi_number=0x28F
__swi __arm int mkdir(const wchar_t* path);
#pragma swi_number=0x290
__swi __arm int rmdir(const wchar_t* path, int zero);
#pragma swi_number=0x291
__swi __arm int chmod(const wchar_t* path, const wchar_t* fname, int amode);
#pragma swi_number=0x292
__swi __arm int rename(const wchar_t* oldpath, const wchar_t* oldfname, const wchar_t* newpath, const wchar_t* newfname, int zero);

#pragma swi_number=0x293
__swi __arm int GetImageWidth(wchar_t ImageID);
#pragma swi_number=0x294
__swi __arm int GetImageHeight(wchar_t ImageID);

#pragma swi_number=0x295
__swi __arm void * CallID_GetCallStatusDesc(int CallID);
#pragma swi_number=0x296
__swi __arm wchar_t * CallStatusDesc_GetName(void * CallStatusDesc);
#pragma swi_number=0x297
__swi __arm void List_FreeElements(LIST *,int (*cmp_proc)(void * elem_from_list),void (*freefunc)(void * elem_from_list));

#pragma swi_number=0x298
__swi __arm char * strstr(const char * str1,const char * str2);

#pragma swi_number=0x299
__swi __arm int GPRS_GetLastSessionInfo(int,GPRS_SESSION_INFO *);

#pragma swi_number=0x29A
__swi __arm int wstrcmpni(const wchar_t * wstr1,const wchar_t * wstr2,int len);

#pragma swi_number=0x29B
__swi __arm int wtoi(const wchar_t * wstr,int len,int * dest);

#pragma swi_number=0x29C
__swi __arm void DATE_GetWeekDay(DATE *,char * dest);

#pragma swi_number=0x29D
__swi __arm void Cale_GetSettings(int setID,void *);

#pragma swi_number=0x29E
__swi __arm wchar_t * wstrwstr(const wchar_t * wstr1,const wchar_t * wstr2);

#pragma swi_number=0x29F
__swi __arm int wstrcmpi(const wchar_t *ws1, const wchar_t *ws2);

#pragma swi_number=0x2A0
__swi __arm wchar_t * wstrchr(const wchar_t * source,wchar_t chr);

#pragma swi_number=0x2A1
__swi __arm void GUIObject_HideSoftkeys(void *gui);

#pragma swi_number=0x2A2
__swi __arm void GUIObject_ShowSoftkeys(void *gui);

#pragma swi_number=0x2A3
__swi __arm DISP_OBJ* DispObject_SoftKeys_Get(void);
#pragma swi_number=0x2A4
__swi __arm int StandbyBackground_SetImage(int type,int,int,const wchar_t* path,const wchar_t* fname,int);

#pragma swi_number=0x2A5
__swi __arm GUI *CreateYesNoQuestionVA(int zero, ...);
#pragma swi_number=0x2A6
__swi __arm void ListMenu_SetSecondLineText(GUI_LIST *,int elem_num,int StrID);

#pragma swi_number=0x2A7
__swi __arm void _REQUEST_OAF_START_APPLICATION(const int * __one,int __zero,MIDP_DESC *,int ___zero);

#pragma swi_number=0x2A8
__swi __arm int FSX_isNameInvalid(wchar_t * name);

#pragma swi_number=0x2A9
__swi __arm void DataBrowserDesc_SetFoldersNumber(void * DataBrowserDesc,int state);
#pragma swi_number=0x2AA
__swi __arm void DataBrowserDesc_SetActions(void * DataBrowserDesc,char * actions);
#pragma swi_number=0x2AB
__swi __arm void DataBrowserDesc_SetSelectAction(void * DataBrowserDesc,int state);
#pragma swi_number=0x2AC
__swi __arm void DataBrowserDesc_SetSelectActionOnFolders(void * DataBrowserDesc,int state);
#pragma swi_number=0x2AD
__swi __arm void DataBrowserDesc_SetBookID(void * DataBrowserDesc,int BookID);
#pragma swi_number=0x2AE
__swi __arm void DataBrowserDesc_SetItemFilter(void * DataBrowserDesc,DB_FILE_FILTER);
#pragma swi_number=0x2AF
__swi __arm void DataBrowserDesc_SetOption(void * DataBrowserDesc,char * option);
#pragma swi_number=0x2B0
__swi __arm void DataBrowserDesc_SetOpenEmptyFolder(void * DataBrowserDesc,int state);
#pragma swi_number=0x2B1
__swi __arm void DataBrowserDesc_SetFileExtList(void * DataBrowserDesc,const wchar_t * ExtList);

#pragma swi_number=0x2B2
__swi __arm void FSX_RemoveIllegalSymbolsName(FILEITEM *);
#pragma swi_number=0x2B3
__swi __arm FILEITEM * FILEITEM_Create(void);
#pragma swi_number=0x2B4
__swi __arm void FILEITEM_Destroy(FILEITEM *);
#pragma swi_number=0x82B4
__swi __arm  void *FILEITEM_Destroy_adr (void);

#pragma swi_number=0x2B5
__swi __arm GUI *CreateDateInputVA(int zero, ...);
#pragma swi_number=0x2B6
__swi __arm GUI *CreateTimeInputVA(int zero, ...);

#pragma swi_number=0x2B7
__swi __arm void StringInput_SetCursorPosition(GUI *,u16 curs_pos,char unk);

#pragma swi_number=0x2B8
__swi __arm void BookObj_CallPage(BOOK * book, const PAGE_DESC  * page);

#pragma swi_number=0x2B9
__swi __arm unsigned int DateInput_GetDateInt(void *dateinp_msg);

#pragma swi_number=0x2BA
__swi __arm unsigned int TimeInput_GetTimeInt(void *timeinp_msg);

#pragma swi_number=0x82BB
__swi __arm FONT_DESC *GetFontDesc(void);

#pragma swi_number=0x82BC
__swi __arm int *GetFontCount();

#pragma swi_number=0x2BD
__swi __arm  int GC_GetBrushColor(void* GC);
#pragma swi_number=0x2BE
__swi __arm  void GC_SetBrushColor(void* GC , int brush_color);
#pragma swi_number=0x2BF
__swi __arm  void GC_DrawRoundRect(void* GC ,RECT *,int arcWidth,int arcHeight,int border_flag,int fill_flag);

#pragma swi_number=0x2C0
__swi __arm  BOOK * MenuBook_Desktop(int mode,int BookID);
#pragma swi_number=0x2C1
__swi __arm  wchar_t * MenuBook_Desktop_GetSelectedItemID(BOOK * MenuBook_Desktop);
#pragma swi_number=0x2C2
__swi __arm  void BookObj_Softkey_SetAction(BOOK *,int actionID,void (*proc)(BOOK *,void*));
#pragma swi_number=0x2C3
__swi __arm  void BookObj_Softkey_SetText(BOOK *,int actionID,int StrID);
#pragma swi_number=0x2C4
__swi __arm  BOOK * FindBookByID(int BookID);

#pragma swi_number=0x2C5
__swi __arm  GUI_FEEDBACK *TextFeedbackWindow(BOOK *bk, int zero);
#pragma swi_number=0x2C6
__swi __arm  void Feedback_SetKeyHook(GUI_FEEDBACK *, void (*hook)(BOOK *bk, int key, int unk, int unk2));
#pragma swi_number=0x2C7
__swi __arm  void Feedback_SetOnClose(GUI_FEEDBACK *, void (*OnClose)(BOOK *bk));
#pragma swi_number=0x2C8
__swi __arm  void Feedback_SetTimeout(GUI_FEEDBACK *, int time_ms);
#pragma swi_number=0x2C9
__swi __arm  void Feedback_SetTextExtended(GUI_FEEDBACK *, STRID text, int where);

#pragma swi_number=0x2CA
__swi __arm  void TabMenuBar_SetOnTabSwitch(GUI_TABMENUBAR *,void (*proc)(BOOK *,int act_tab));

#pragma swi_number=0x2CB
__swi __arm  STRID KeyCode2Name(int key_code);

#pragma swi_number=0x2CC
__swi __arm  int ImageID_Get(const wchar_t * fpath,const wchar_t * fname,wchar_t * imageID);
#pragma swi_number=0x2CD
__swi __arm  void ImageID_Free(wchar_t imageID);

#pragma swi_number=0x2CE
__swi __arm  SUB_EXECUTE *DataBrowser_CreateSubExecute(int BookID, FILEITEM *f);
#pragma swi_number=0x2CF
__swi __arm  int DataBrowser_ExecuteSubroutine(SUB_EXECUTE *sub, int action, u16 *unk);
#pragma swi_number=0x2D0
__swi __arm  int FILEITEM_SetFname(FILEITEM *f, const wchar_t *fname);
#pragma swi_number=0x2D1
__swi __arm  int FILEITEM_SetPath(FILEITEM *f, const wchar_t *fpath);
#pragma swi_number=0x2D2
__swi __arm  wchar_t *FILEITEM_GetFname(FILEITEM *f);
#pragma swi_number=0x2D3
__swi __arm  wchar_t *FILEITEM_GetPath(FILEITEM *f);
#pragma swi_number=0x2D4
__swi __arm  int FILEITEM_SetFnameAndContentType(FILEITEM *f, const wchar_t *fname);
#pragma swi_number=0x2D5
__swi __arm  int FILEITEM_SetPathAndContentType(FILEITEM *f, const wchar_t *fpath);

#pragma swi_number=0x2D6
__swi __arm  int JavaDialog_Open(int unk1,char * unk2,void ** JavaDesc);
#pragma swi_number=0x2D7
__swi __arm  void JavaDialog_Close(int unk1);
#pragma swi_number=0x2D8
__swi __arm  int JavaAppDesc_Get(int unk1,void ** JavaDesc);
#pragma swi_number=0x2D9
__swi __arm  int JavaAppDesc_GetFirstApp(void * JavaDesc);
#pragma swi_number=0x2DA
__swi __arm  int JavaAppDesc_GetNextApp(void * JavaDesc);
#pragma swi_number=0x2DB
__swi __arm  int JavaAppDesc_GetJavaAppInfo(void * JavaDesc,int ID,wchar_t ** wstr);

#pragma swi_number=0x2DC
__swi __arm  void OrangeLED_Control(int _free_val, int or_LED_ID, int level, int fade_time);

#pragma swi_number=0x2DD
__swi __arm  void DispObject_SetRefreshTimer(DISP_OBJ *,int time);
#pragma swi_number=0x2DE
__swi __arm  void DispObject_KillRefreshTimer(DISP_OBJ *);

#pragma swi_number=0x2DF
__swi __arm int isDataBrowserBook(BOOK *);

#pragma swi_number=0x2E0
__swi __arm SUB_EXECUTE * BrowserItem_Get_SUB_EXECUTE(BOOK * BrowserItemBook);

#pragma swi_number=0x2E1
__swi __arm int Shortcut_Get_MenuItemName(SHORTCUT *);
#pragma swi_number=0x2E2
__swi __arm int Shortcut_Get_MenuItemIconID(SHORTCUT *);

#pragma swi_number=0x2E3
__swi __arm BOOK * FindBookEx(int (*cmp_proc)(BOOK* book_from_list ,int * param), int * param);
#pragma swi_number=0x2E4
__swi __arm FILEITEM *FILEITEM_CreateCopy (FILEITEM *);

#pragma swi_number=0x2E5
__swi __arm ACTION *ActionCreate(int (*PROC)(void *msg,BOOK*), int BookID, u16 event, APP_DESC *app_desc, PAGE_DESC *pag_desc);
#pragma swi_number=0x2E6
__swi __arm int BOOK_GetSessionID(BOOK *);
#pragma swi_number=0x2E7
__swi __arm void UI_Event_toSID(int event, int sess_id);
#pragma swi_number=0x2E8
__swi __arm char *GetIMSI(void);
#pragma swi_number=0x2E9
__swi __arm void UI_CONTROLLED_SHUTDOWN_RESPONSE(int book_id);

#pragma swi_number=0x2EA
__swi __arm int MainInput_getCurPos(GUI*);
#pragma swi_number=0x2EB
__swi __arm void MainInput_Hide(GUI*);
#pragma swi_number=0x2EC
__swi __arm void DispObject_SoftKey_RestoreDefaultAction(DISP_OBJ*,int action);
#pragma swi_number=0x2ED
__swi __arm STRID PNUM2Name(void *pnum, int isIconNeeded, int only_full_number_flag);
#pragma swi_number=0x2EE
__swi __arm int SpeedDial_GetPNUM(int _zero,char charter0__9, void * PNUM);

#pragma swi_number=0x2EF
__swi __arm void * MetaData_Desc_Create(wchar_t * path,wchar_t * name);
#pragma swi_number=0x2F0
__swi __arm void MetaData_Desc_Destroy(void * MetaData_Desc);
#pragma swi_number=0x2F1
__swi __arm wchar_t * MetaData_Desc_GetTags(void * MetaData_Desc,int tagID);
#pragma swi_number=0x2F2
__swi __arm int MetaData_Desc_GetTrackNum(void * MetaData_Desc,int __NULL);

#pragma swi_number=0x2F3
__swi __arm int DISP_OBJ_GetWindowWidth(DISP_OBJ *);
#pragma swi_number=0x2F4
__swi __arm int DISP_OBJ_GetWindowHeight(DISP_OBJ *);
#pragma swi_number=0x2F5
__swi __arm int get_system_langID(void);
#pragma swi_number=0x2F6
__swi __arm char * lang_get_name(int langID);

#pragma swi_number=0x2F7
__swi __arm int w_chdir(const wchar_t *path);
#pragma swi_number=0x2F8
__swi __arm int w_fopen(const wchar_t *name, int attr, int rights, int err);
#pragma swi_number=0x2F9
__swi __arm int w_fstat(const wchar_t *name, W_FSTAT *);
#pragma swi_number=0x2FA
__swi __arm int w_fwrite(int f, void *buf, int size);
#pragma swi_number=0x2FB
__swi __arm int w_fread(int f, void *buf, int size);
#pragma swi_number=0x2FC
__swi __arm int w_lseek(int f, int offs, int mode);
#pragma swi_number=0x2FD
__swi __arm int w_fclose(int f);
#pragma swi_number=0x2FE
__swi __arm int w_mkdir(const wchar_t *path, int mode);
#pragma swi_number=0x2FF
__swi __arm int DirHandle_SetFilterStr(DIR_HANDLE *, const wchar_t *filter);
#pragma swi_number=0x300
__swi __arm int Disp_GetStrIdWidth(STRID , int len);

#pragma swi_number=0x301
__swi __arm int MetaData_Desc_GetCoverInfo(void * MetaData_Desc,char * cover_type,int * size,int * cover_offset);
#pragma swi_number=0x302
__swi __arm int ImageID_GetIndirect(void * buf_image,int size,int __NULL,wchar_t * image_type,wchar_t * imageID);

#pragma swi_number=0x303
__swi __arm void unixtime2datetime(int, DATETIME * );
#pragma swi_number=0x304
__swi __arm void ListElement_Insert(LIST *lst, int i, void *new_item);
#pragma swi_number=0x305
__swi __arm int FileCopy(wchar_t *src_path, wchar_t *src_name, wchar_t *dest_path, wchar_t *dest_name, int);
#pragma swi_number=0x306
__swi __arm int FileMove(wchar_t *src_path, wchar_t *src_name, wchar_t *dest_path, wchar_t *dest_name, int);

#pragma swi_number=0x307
__swi __arm void RedLED_On(int __NULL);
#pragma swi_number=0x308
__swi __arm void RedLED_Off(int __NULL);

#pragma swi_number=0x309
__swi __arm void DispObject_SetLayerColor(DISP_OBJ *,int color);

#pragma swi_number=0x30A
__swi __arm int JavaAppDesc_GetJavaAppID(void * JavaDesc);
#pragma swi_number=0x30B
__swi __arm void REQUEST_UI_OAF_START_APPLICATION(const int *ASYNC,int appID,char * flag);

#pragma swi_number=0x30C
__swi __arm int JavaApp_LogoImageID_Get(wchar_t * fullpath,wchar_t * imageID);

#pragma swi_number=0x30D
__swi __arm GUI * CreateProgressBar(BOOK*,int);
#pragma swi_number=0x30E
__swi __arm void ProgressBar_SetText(GUI*,STRID text);
#pragma swi_number=0x30F
__swi __arm void ProgressBar_SetIcon(GUI*,u16 icon_id);
#pragma swi_number=0x310
__swi __arm void ProgressBar_SetPercentValue(GUI*,int value);
#pragma swi_number=0x311
__swi __arm void ProgressBar_SetBarDisabled(GUI*,int disabled);

#pragma swi_number=0x312
__swi __arm void StringInput_DispObject_SetText(DISP_OBJ *,int StrID);

#pragma swi_number=0x313
__swi __arm void GuiObject_SetTitleImage(void *gui, wchar_t imageID);
#pragma swi_number=0x314
__swi __arm void GuiObject_SetTitleIcon(void *gui, wchar_t imageID);
#pragma swi_number=0x315
__swi __arm void GuiObject_SetHighlightImage(void *gui, wchar_t imageID);
#pragma swi_number=0x316
__swi __arm void GuiObject_SetBackgroundImage(void *gui, wchar_t imageID);

#pragma swi_number=0x317
__swi __arm void *DispObject_Softkeys_GetParams(DISP_OBJ *);
#pragma swi_number=0x318
__swi __arm LIST *DispObject_Softkeys_GetList(DISP_OBJ *, BOOK *bk, char __zero);

#pragma swi_number=0x319
__swi __arm int BookObj_GetDisplayOrientation(BOOK *bk);
#pragma swi_number=0x31A
__swi __arm void BookObj_SetDisplayOrientation(BOOK *bk, int orientation);

#pragma swi_number=0x31B
__swi __arm BOOK *DISPLAY_GetTopBook(int display);
#pragma swi_number=0x31C
__swi __arm void DISPLAY_SetBrightness(int display, int brightness);
#pragma swi_number=0x31D
__swi __arm int DISPLAY_GetBrightness(int display);

#pragma swi_number=0x31E
__swi __arm void DataBrowserDesc_Menu_AddFSFunctions(void * db , int);
#pragma swi_number=0x31F
__swi __arm void DataBrowserDesc_Menu_AddNewFolder(void * db , int);
#pragma swi_number=0x320
__swi __arm void DataBrowserDesc_Menu_AddMarkFiles(void * db , int);

#pragma swi_number=0x321
__swi __arm void SetTheme(wchar_t*path, wchar_t*name, int bookid, int unk_1);

#pragma swi_number=0x322
__swi __arm void Softkeys_GetLabel(DISP_OBJ *softkeys, SKLABEL *lbl, int id);
#pragma swi_number=0x323
__swi __arm void Softkeys_Update(DISP_OBJ *softkeys);

#pragma swi_number=0x8324
__swi __arm void * get_CellData(void);

#pragma swi_number=0x325
__swi __arm void * IncommingCall_Reject(BOOK* book);
#pragma swi_number=0x326
__swi __arm void * IncommingCall_Mute(BOOK* book);

#pragma swi_number=0x327
__swi __arm int GetRadioState(char* level, char* flag);

#pragma swi_number=0x328
__swi __arm int Bluetooth_GetState(void);
#pragma swi_number=0x329
__swi __arm int Bluetooth_GetPhoneVisibility(void);
#pragma swi_number=0x32A
__swi __arm int Bluetooth_isBusy(void);

#pragma swi_number=0x32B
__swi __arm int USB_isConnected(void);

#pragma swi_number=0x32C
__swi __arm void *w_diropen(const wchar_t *dir);
#pragma swi_number=0x32D
__swi __arm wchar_t *w_dirread(void *);
#pragma swi_number=0x32E
__swi __arm int w_dirclose(void *);
#pragma swi_number=0x32F
__swi __arm int w_rename(const wchar_t *dst, const wchar_t *src);
#pragma swi_number=0x330
__swi __arm int w_remove(const wchar_t *dir);

#pragma swi_number=0x331
__swi __arm int ConnectionManager_Connection_GetState(void);

#pragma swi_number=0x332
__swi __arm int GetSignalQuality(char *rssi,char *ber);

#pragma swi_number=0x8333
__swi __arm IS_NEEDED_BOOK isCameraBook(void);
#pragma swi_number=0x8334
__swi __arm IS_NEEDED_BOOK isSoundRecorderBook(void);
#pragma swi_number=0x335
__swi __arm void StringInput_MenuItem_SetPriority(GUI * strinp, int prio, int actionID);

#pragma swi_number=0x336
__swi __arm void VideoPlayerControl(BOOK *VideoPlayerBook, int);
#pragma swi_number=0x337
__swi __arm void Video_ActionBack(BOOK *VideoPlayerBook, void*);
#pragma swi_number=0x338
__swi __arm void Video_ExtractFrame(BOOK *VideoPlayerBook, void*);
#pragma swi_number=0x339
__swi __arm void Video_ZoomOn(BOOK *VideoPlayerBook, void*);

#pragma swi_number=0x33A
__swi __arm void RightNow_SetActive(int flag);

#pragma swi_number=0x33B
__swi __arm void VideoResize_AutomateMode(BOOK *VideoPlayerBook, u16 VideoWidth, u16 VideoHeight, u16* NewWidth, u16* NewHeight);
#pragma swi_number=0x33C
__swi __arm void VideoResize_AllScreenMode(BOOK *VideoPlayerBook, u16 VideoWidth, u16 VideoHeight, u16* NewWidth, u16* NewHeight);

#pragma swi_number=0x33D
__swi __arm int Video_GetCurrentSkinID(BOOK *VideoPlayerBook);
#pragma swi_number=0x33E
__swi __arm void Video_SetSkin(GUI* VideoPlayerGUI, int skinID);
#pragma swi_number=0x33F
__swi __arm int Video_SetPermit(BOOK *VideoPlayerBook, u16 NewWidth, u16 NewHeight);
#pragma swi_number=0x340
__swi __arm int Video_Refresh(BOOK *VideoPlayerBook, GUI* VideoPlayerGUI);

#pragma swi_number=0x341
__swi __arm void Video_Play(BOOK *VideoPlayerBook, void*);
#pragma swi_number=0x342
__swi __arm  void Video_Pause(BOOK *VideoPlayerBook, void*);
#pragma swi_number=0x343
__swi __arm void Video_Stop(BOOK *VideoPlayerBook, void*);

#pragma swi_number=0x344
__swi __arm void Audio_Pause(BOOK *);
#pragma swi_number=0x345
__swi __arm void Audio_Play(BOOK *);

#pragma swi_number=0x346
__swi __arm GUI * CreatePercentInputVA(int zero, ...);
#pragma swi_number=0x347
__swi __arm int PercentInput_GetPercent(GUI * PercentInput);

#pragma swi_number=0x348
__swi __arm void GUIonMessage_SetLineSeparator(GUI_MESSAGE *,int _unk1);

/*
#pragma swi_number=0x348
__swi __arm
#pragma swi_number=0x349
__swi __arm
#pragma swi_number=0x34A
__swi __arm
*/

#pragma swi_number=0x34B
__swi __arm void GUIObject_Softkeys_RemoveBackground(void *gui);
#pragma swi_number=0x34C
__swi __arm void GUIObject_Softkeys_RestoreBackground(void *gui);

#pragma swi_number=0x34D
__swi __arm void SetTrayIcon(wchar_t iconID, char mode);

#pragma swi_number=0x34E
__swi __arm int REQUEST_SETTING_RINGVOLUME_SET(const int *sync, int unk, int null, int vol);
#pragma swi_number=0x34F
__swi __arm int REQUEST_SETTING_VIBRATOR_SET(const int *sync, int unk, int mode);
#pragma swi_number=0x350
__swi __arm int REQUEST_SETTING_RINGTYPESOUNDFILE_SET(const int *sync, int null, wchar_t *path, wchar_t *name);
#pragma swi_number=0x351
__swi __arm int REQUEST_SETTING_ANSWERINGMODE_SET(const int *sync, int unk, int mode);
#pragma swi_number=0x352
__swi __arm int REQUEST_SETTING_INCREASINGRING_SET(const int *sync, int unk, int unk2, int mode);
#pragma swi_number=0x353
__swi __arm int REQUEST_SETTING_ALLOWEDCALLERS_SET(const int *sync, int unk, int mode);

#pragma swi_number=0x354
__swi __arm int JavaAppDesc_GetJavaAppFullpath(void * JavaDesc,JavaAppFullpath *);

#pragma swi_number=0x355
__swi __arm void *SoundRecorderDesc_Create(void);
#pragma swi_number=0x356
__swi __arm void SoundRecorderDesc_Destroy(void * desc);
#pragma swi_number=0x357
__swi __arm int SoundRecorderDesc_SetBookID(void * desc,int BookID);
#pragma swi_number=0x358
__swi __arm int SoundRecorderDesc_SetType(void * desc,const int type);
#pragma swi_number=0x359
__swi __arm int SoundRecorderDesc_SetFolder(void * desc,const wchar_t * fpath);
#pragma swi_number=0x35A
__swi __arm int SoundRecorderDesc_SetFname(void * desc,const wchar_t * fname);
#pragma swi_number=0x35B
__swi __arm int SoundRecorderDesc_SetRecordSize(void * desc,const int size);
#pragma swi_number=0x35C
__swi __arm int SoundRecorder_Create(void * desc);

#pragma swi_number=0x35D
__swi __arm int SoundRecorder_RecordCall(BOOK * OngoingCallBook);

#pragma swi_number=0x35E
__swi __arm int Browser_OpenURI(BOOK *, char * URI, int mode);
#pragma swi_number=0x35F
__swi __arm char * GetURIScheme (int schemeID);
#pragma swi_number=0x360
__swi __arm char * CreateURI (wchar_t * fpath, wchar_t * fname, char * URIScheme);

#pragma swi_number=0x361
__swi __arm void DataBrowserDesc_SetViewModeAndSortOrder(void * db,int view_sort_ID);

#pragma swi_number=0x362
__swi __arm int Alarm_GetCurrentTypeAndAlarmID(char *type, wchar_t *AlarmID);
#pragma swi_number=0x363
__swi __arm int Alarm_GetState(char *state, int AlarmID);
#pragma swi_number=0x364
__swi __arm int Alarm_GetTIME(TIME *AlarmTime, int AlarmID);
#pragma swi_number=0x365
__swi __arm int Alarm_GetWeekSettings(void* week,int AlarmID);
#pragma swi_number=0x366
__swi __arm int IrDa_GetState(char *state);
#pragma swi_number=0x367
__swi __arm void REQUEST_SETTING_ALLOWEDCALLERS_GET(const int *sync, int ProfileNum, char *state);
#pragma swi_number=0x368
__swi __arm void * IncommingCall_Accept(BOOK* book);

#pragma swi_number=0x369
__swi __arm void MediaPlayer_SoftKey_SetText(void *player_gui,int actionID , int StrID);
#pragma swi_number=0x36A
__swi __arm void MediaPlayer_SoftKey_SetAction(void *player_gui,int actionID ,void (*proc)(BOOK *,void*));
#pragma swi_number=0x36B
__swi __arm void MediaPlayer_SoftKey_SetItemAsSubItem(void *player_gui,int item,int subitem);
#pragma swi_number=0x36C
__swi __arm void MediaPlayer_SoftKey_AddHelpStr(void *player_gui,int item,int StrID);
#pragma swi_number=0x36D
__swi __arm void MediaPlayer_ShowNowPlaying(DISP_OBJ*disp_obj, int);

#pragma swi_number=0x36E
__swi __arm void DispObject_Show(DISP_OBJ *disp_obj, int mode);
#pragma swi_number=0x36F
__swi __arm int Request_EventChannel_Subscribe(const int *SYNC, int mode, int event);

#pragma swi_number=0x370
__swi __arm void GUIObject_Softkey_ExecuteAction(void *gui, u16 actionID);

#pragma swi_number=0x371
__swi __arm int Request_ICA_ShutdownAllConnections(const int *sync);

#pragma swi_number=0x372
__swi __arm int REQUEST_PROFILE_GETPROFILENAME(const int *SYNC,int unk,STRID_DATA *,char *error);

#pragma swi_number=0x373
__swi __arm int DataBrowserDesc_SetFocusToFILEITEM(void *DataBrowserDesc, FILEITEM *fi);

#pragma swi_number=0x374
__swi __arm  int GUIonMessage_GetMsg ( void *msg);
#pragma swi_number=0x375
__swi __arm  BOOK *GUIonMessage_GetBook ( void *msg);
#pragma swi_number=0x376
__swi __arm  GUI *GUIonMessage_GetGui ( void *msg);
#pragma swi_number=0x377
__swi __arm  int GUIonMessage_GetSelectedItem ( void *msg);
#pragma swi_number=0x378
__swi __arm  int GUIonMessage_GetPrevSelectedItem ( void *msg);

#pragma swi_number=0x379
__swi __arm  void ObexSendFile (SEND_OBEX_STRUCT *);


#pragma swi_number=0x37A
__swi __arm  void Feedback_SetManualScrollingText(DISP_OBJ* feedback_disp_obj,int);

#pragma swi_number=0x37B
__swi __arm  int EqualizerGain_Set (int AudioSessionID,int Band,int Level);

#pragma swi_number=0x37C
__swi __arm  int CANVAS_Get_GviGC(PCANVAS pcanvas, GVI_GC* dest);
#pragma swi_number=0x37D
__swi __arm  void GVI_BitBlt(GVI_GC destgc, int destx, int desty, int destxsize, int destysize, GVI_GC srcgc, int srcx, int srcy, int _204, int _0, int _0_2, int _0_3);
#pragma swi_number=0x37E
__swi __arm  void GVI_StretchBlt(GVI_GC destgc, int destx, int desty, int destxsize, int destysize, GVI_GC srcgc, int srcx, int srcy, int srcxsize, int srcysize, int _204, int _0, int _0_2, int _0_3);
#pragma swi_number=0x37F
__swi __arm  void GVI_TransformBlt(GVI_GC destgc, int destx, int desty, GVI_GC srcgc, int srcx, int srcy, int srcxsize, int srcysize, int orientation, int _0);

#pragma swi_number=0x380
__swi __arm  GVI_PEN GVI_CreatePen(char thikness, int color);
#pragma swi_number=0x381
__swi __arm  GVI_PEN GVI_CreateDashedPen(char thikness, int color, int bitmask, int step, int offset);
#pragma swi_number=0x382
__swi __arm  GVI_BRUSH GVI_CreateSolidBrush(int color);
#pragma swi_number=0x383
__swi __arm  BOOL GVI_Delete_GVI_Object(GVI_OBJ*);

#pragma swi_number=0x384
__swi __arm  void GVI_DrawEllipse(GVI_GC destgc, int x1, int y1, int x2, int y2, GVI_PEN pen);
#pragma swi_number=0x385
__swi __arm  void GVI_DrawLine(GVI_GC, int x1, int y1, int x2, int y2, GVI_PEN pen);
#pragma swi_number=0x386
__swi __arm  void GVI_DrawLines(GVI_GC gc, POINT* points, int count, GVI_PEN pen);
#pragma swi_number=0x387
__swi __arm  void GVI_DrawPolyLine(GVI_GC gc, POINT* points, int count, GVI_PEN pen, int is_closed);
#pragma swi_number=0x388
__swi __arm  void GVI_DrawRectangle(GVI_GC gc, int x, int y, int w, int h, GVI_PEN pen);
#pragma swi_number=0x389
__swi __arm  void GVI_DrawRoundRect(GVI_GC destgc, int x, int y, int w, int h, int arcw, int arch, GVI_PEN pen);

#pragma swi_number=0x38A
__swi __arm  void GVI_DrawSolidLines(GVI_GC gc, POINT* points, int count, int color, int width);
#pragma swi_number=0x38B
__swi __arm  void GVI_DrawSolidPolyLine(GVI_GC gc, POINT* points, int count, int is_closed, int color, int width);
#pragma swi_number=0x38C
__swi __arm  void GVI_DrawSolidRectangle(GVI_GC gc, int x, int y, int w, int h, int color, int width);

#pragma swi_number=0x38D
__swi __arm  void GVI_FillEllipse(GVI_GC destgc, int x, int y, int w, int h, GVI_BRUSH brush);
#pragma swi_number=0x38E
__swi __arm  void GVI_FillPolygon(GVI_GC destgc, POINT *pp, int count, GVI_BRUSH brush); 
#pragma swi_number=0x38F
__swi __arm  void GVI_FillRectangle(GVI_GC , int x, int y, int w, int h, GVI_BRUSH brush);
#pragma swi_number=0x390
__swi __arm  void GVI_FillRectangles(GVI_GC , GVI_XYWH *rcs, int count, GVI_BRUSH brush); 

#pragma swi_number=0x391
__swi __arm  void GVI_FillSolidEllipse(GVI_GC destgc, int x1, int y1, int x2, int y2, int color);
#pragma swi_number=0x392
__swi __arm  void GVI_FillSolidRectangle(GVI_GC , int x, int y, int w, int h, int color);
#pragma swi_number=0x393
__swi __arm  int GVI_FillSolidRoundRect(GVI_GC destgc, int x, int y, int w, int h, int arcx, int arcy, int color);
#pragma swi_number=0x394
__swi __arm  void GVI_FillSolidTriangle(GVI_GC destgc, int x1, int y1, int x2, int y2, int x3, int  y3, int color); 

#pragma swi_number=0x395
__swi __arm  GC* GC_CreateMemoryGC(int xsize, int ysize, int bpp, int unk, void* somefn , int unk2);
#pragma swi_number=0x396
__swi __arm  void GC_FreeGC(GC* gc);

#pragma swi_number=0x397
__swi __arm  void REQUEST_ALARMCLOCKSTATUS_GET(const int *SYNC, char * alarm_status);
#pragma swi_number=0x398
__swi __arm  void REQUEST_ALARMCLOCKTIME_GET(const int *SYNC, TIME *t);
#pragma swi_number=0x399
__swi __arm  void REQUEST_ALARMDATEANDTIMESTATUS_GET(const int *SYNC, DATETIME *);

#pragma swi_number=0x39A
__swi __arm  int Settings_ShowNumber_Get(char *state);

#pragma swi_number=0x39B
__swi __arm  GVI_BMP GVI_CreateBitmap(int xsize, int ysize, int bpp);
#pragma swi_number=0x39C
__swi __arm  GVI_GC GVI_CreateMemoryGC(GVI_BMP bitmap);
#pragma swi_number=0x39D
__swi __arm  void GVI_DeleteMemoryGC(GVI_GC srcgc);

#pragma swi_number=0x39E
__swi __arm  int StringInput_DispObject_SetLanguage(DISP_OBJ_STRING_INPUT *,int langID,int flag);

#pragma swi_number=0x39F
__swi __arm  int ListElement_InsertSorted(LIST * list, void * pelem, int (*compare)(void *, void *));

#pragma swi_number=0x3A0
__swi __arm  int ListMenu_SetItemIcon(GUI_LIST *,wchar_t ItemID,wchar_t unk_FFFF,int mode,wchar_t ImageID);

#pragma swi_number=0x3A1
__swi __arm  void wstrnupr(wchar_t * wstr, int maxlen);
#pragma swi_number=0x3A2
__swi __arm  void wstrnlwr(wchar_t * wstr, int maxlen);

#pragma swi_number=0x3A3
__swi __arm  int ListMenu_SetItemTextScroll(GUI_LIST *, int scroll);
#endif
