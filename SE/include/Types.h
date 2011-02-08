#ifndef _TYPES_H
#define _TYPES_H


#define SID_ANY_LEN 0xFFFF
#define NOIMAGE 0xFFFF
#define EMPTY_SID 0x6FFFFFFF
#define LIST_ERROR 0xFFFF
#define TEXT(__STR__) L##__STR__
#define _T(__STR__) L##__STR__
#define MAXELEMS(x) (sizeof(x)/sizeof(x[0]))
#define STR(__STR__) Str2ID(_T(__STR__),0,SID_ANY_LEN)

#define MINUTE_TICK_EVENT 999
#define ELF_SHOW_INFO_EVENT 998
#define ELF_TERMINATE_EVENT 997
#define ELF_RECONFIG_EVENT 996
#define SBY_REDRAW_RELEASE_EVENT 995

#define BLOCK_EVENT_GLOBALLY 666
#define BLOCK_EVENT_IN_THIS_SESSION 667

#define EMPTY_REDRAW_METHOD (void (*)(DISP_OBJ *,int ,int ,int))-1

//#define IID_PPV_ARG(Type, Expr) &IID_##Type, reinterpret_cast<void**>(static_cast<Type **>(Expr))

#ifdef __cplusplus
template<typename T> void** PPINTERFACE(T** pp)
{
  //static_cast<IUnknown*>(*pp);
  return reinterpret_cast<void**>(pp);
}
#else
#define PPINTERFACE(v) (void**)v
#endif


typedef unsigned short u16;

#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif

typedef int BOOL;
typedef int STRID;

union SIGNAL;

typedef unsigned char OSPRIORITY;
typedef unsigned char OSBOOLEAN;

typedef unsigned short OSVECTOR;

typedef unsigned long PROCESS;
typedef unsigned long SIGSELECT;
typedef unsigned long OSBUFSIZE;
typedef unsigned long OSUSER;
typedef unsigned long OSERRCODE;
typedef unsigned long OSADDRESS;
typedef unsigned long OSTIME;
typedef unsigned long OSTICK;
typedef unsigned long OSATTREF;
typedef unsigned long OSSEGMENT;

typedef signed long OSFSEMVAL;
typedef signed long OSSEMVAL;

typedef unsigned long LPARAM;
typedef unsigned long LRESULT;
#define OS_PROCESS(x) __interwork void x(void)

typedef void (OSENTRYPOINT)(void);

typedef OSADDRESS (OSERRH)(OSBOOLEAN, OSERRCODE, OSERRCODE);

struct PCOLOR
{
  int pen;
  int brush;
};

typedef int jmp_buf[11];
typedef unsigned int size_t;
typedef long time_t;

typedef struct XGUILIST_type XGUILIST;
typedef struct UI_APP_SESSION_type UI_APP_SESSION;
typedef struct GUI_type GUI;

typedef struct SURFACE
{
  int Ifc;
  u16 Flags;
  int Width;
  int Height;
  int Buffer;
  int BufferSize;
  int Tag;
  int RefCount;
  struct SURFACE *  next;
  int HWFlags;
  int BitsLock;
  int unk;
  int MemLocks;
  int unk1;
  int unk2;
  int unk3;
  int unk4;
  int unk5;
  int Display;
}SURFACE;

struct HELPER_SIGNAL
{
  SIGSELECT signo;
  void(*PROC)(int,void*);
  int   param1;
  void * param2;
};
typedef struct
{
  char mode;
  int  book_or_sess_id;
  unsigned short event;

  void *data;
  void (*mfree)(void*);
}UI_MESSAGE;


typedef struct
{
  int   count;
  int   signals[3];
}MYSIG;

typedef struct
{
  SIGSELECT signo;
  void (* HOOK)(void*);
}OSE_HOOK_ITEM;

typedef struct
{
  int  unk;
  wchar_t * path;
  wchar_t * fname;
  char * content_type;
  char unk1[0x2F0];
}FILELISTITEM;

typedef struct
{
  int unk_0;
  u16 unk_4;
  u16 unk_6;
  int unk_8; 
  char unk_C;    // �� 2010 ������ ����������...
  char unk_D;
  char ena_hidden;
} DIR_HANDLE;

typedef struct
{
  int StrID_FolderName;
  int StrID_SavedTo;
  wchar_t * Path;
  int isInternal;
}DB_EXT_FOLDERS;


typedef struct
{
  wchar_t * __fname; //use FILEITEM_SetFname/FILEITEM_GetFname
  wchar_t * __path; //use FILEITEM_SetPath/FILEITEM_GetPath
  char *MIME_type;
  wchar_t * extension;
  int * FSTAT;
  int * DRMHandle;
  char DRM_Flag;
}FILEITEM;

typedef struct
{
  u16 unk;
  u16 FirstFree;
  void **listdata;
}LIST;

typedef struct
{
  LIST * listbook;
  int    name;
  int    unk;
  int    unk1;
  int    app_ID;
  int    unk2;
}LISTBOOK;


typedef struct
{
  char   isDestroyed;
  char * name;
  UI_APP_SESSION * app_session;
  int   parent_BookID;
  u16   unk2;
  int   unk3;
  int   unk4;
  char   unk5;
  void * unk6;
  int   unk7;
  u16   unk8;
}XBOOK;


struct PAGE_DESC;

typedef struct BOOK_
{
  XBOOK * xbook;
  void (*onClose)(struct BOOK_*);
  int    BookID;
  struct PAGE_DESC * current_page;
  struct PAGE_DESC  * base_page;
  XGUILIST * xguilist;
}BOOK;

typedef struct XGUILIST_type
{
  BOOK * book;
  LIST * guilist;
}_XGUILIST;

typedef struct UI_APP_SESSION_type
{
  LIST * listbook;
  int    name;
  int    W1;
  int    W2;
  int    SessionID;
  int    unk2;
}_UI_APP_SESSION;
//-------------------------------------------
typedef struct
{
  u16   year;
  char  mon;
  char  day;
}DATE;

typedef struct
{
  char  hour;
  char  min;
  char  sec;
  char  unk;
}TIME;

typedef struct
{
  DATE  date;
  TIME  time;
}DATETIME;

typedef struct
{
  int unk;
  int unk1;
  int fsize;
  int unk3;
  int unix_file_creation_time;
  int unk5;
  int unk6;
}FSTAT;


typedef struct
{
  u16 RecordID;
  char num[41];//2
  char xx[0x35];
}AB_NUM_ITEM;


typedef struct
{
  u16 len;
  char encodingID;
  char str[255];
}AB_STR_ITEM;






typedef struct
{
  int msg;
  int (*proc)(void * ,BOOK*);
}PAGE_MSG;

typedef struct
{
  const char *name;
  struct PAGE_DESC *page;
  void (*OnEnter)(void *, BOOK *);
  void (*OnExit)(void *, BOOK *);
}BOOK_SUBROUTINE;

typedef struct PAGE_DESC
{
  const char * name;
  BOOK_SUBROUTINE *subr;
  const PAGE_MSG * msg_lst;
}PAGE_DESC;



typedef struct
{
  int flag;
  int unk_NULL1;
  int unk_NULL2;
  int HeaderStrID;
  int unk_NULL3;
  int (*onMessage)(void *);
}PAGE_MENU_ITEMS_DESC;

typedef struct
{
  int flag;
  int unk_NULL1;
  int unk_NULL2;
  int HeaderStrID;
  int unk_NULL3;
  int (*onMessage)(void *);
  const PAGE_MENU_ITEMS_DESC * const *items;
}PAGE_MENU_DESC;

typedef struct
{
  int msg;
  int submsg;
  int param1;
  void * param2;
}HELPER_MSG;


typedef struct
{
  unsigned short event;
  //  int (* HOOK)(UI_MESSAGE*, BOOK *);
  int (* HOOK)(UI_MESSAGE*);
}UI_HOOK_ITEM;

typedef struct
{
  int (*PROC)(void *msg,BOOK*);
  int BookID;
  u16 event;
  void * APP_DESC;
  void * PAGE_DESC;
}ACTION;

struct SUB_EXECUTE;

typedef struct
{
  int cmd;
  union
  {
    int (*ON_CMD)(struct SUB_EXECUTE*);
    int (*ON_CMD_RUN)(struct SUB_EXECUTE*);
    int (*ON_CMD_ICON)(struct SUB_EXECUTE*, wchar_t * iconid);
    int (*ON_CMD_STRID)(struct SUB_EXECUTE*, STRID * strid);
  };
  int StrID;
  union
  {
    int (*ON_CHECK)(struct SUB_EXECUTE*, void*);
    int (*ON_CMD_RUN_CHECK)(struct SUB_EXECUTE*);
  };
}FILESUBROUTINE;

typedef struct
{
  char **content_type;
  wchar_t **ext_list;
  void (*GetMethods)(struct SUB_EXECUTE*);
  int sub_execute_size;
  const DB_EXT_FOLDERS * dbf;
  char type_group;
  int drm_flags;
  char unk2;
  char unk3;
}DB_EXT;

typedef struct
{
  char **content_type;
  wchar_t **ext_list;
  void (*GetMethods)(struct SUB_EXECUTE*);
  int sub_execute_size;
  const DB_EXT_FOLDERS * dbf;
  int drm_flags;
  char unk2;
  char unk3;
}DB_EXT_2010;

typedef struct
{
  LIST *UserDataList;
  LIST *gKbdHookList;
  PROCESS HPID;
  LIST *UIHookList;
  LIST *OseHookList;
  LIST *DBExtList;
  FILESUBROUTINE* elf_ext_m;
  LIST *DLLList;
  LIST *UIPageHook_Before;
  DB_EXT * (*CreateDbExt)(void);
  wchar_t IconSmall;
  wchar_t IconBig;
  void ** LibraryCache;  //�� ����������!!!!!
  void *LibraryDLL;
  wchar_t LastKey;
  LIST *UIPageHook_After;
}EP_DATA;

typedef struct
{
  int base;
  int size;
  wchar_t * fname;
}ELF_DESC;

typedef int (*IS_NEEDED_BOOK)(BOOK *);

typedef struct
{
#ifdef __cplusplus
  int (*SigHandler)(SIGNAL **sig);
#else
  int (*SigHandler)(void **sig);
#endif
  PAGE_DESC     *default_page;
  char  * name;
  int (*MsgHandler)(void*msg);
  char * (*GetMsgName)(int);
}APP_DESC;

typedef struct
{
  char __msg; //use GUIonMessage_GetMsg()
  GUI* __gui; //use GUIonMessage_GetGui()
  BOOK* __book; //use GUIonMessage_GetBook()
}GUI_MESSAGE;

struct DISP_DESC;

struct WINDOW;


typedef struct SYS_WINDOW
{
  int   magic;
  u16   x1;
  u16   x2;
  u16   y1;
  u16   y2;
}SYS_WINDOW;


typedef struct WINDOW
{
  struct SYS_WINDOW * swin;
  int unk_int;
  void * unk_win1;
  u16   x1;
  u16   x2;
  u16   y1;
  u16   y2;
  char  isVisible;
  char  unk_char;
  u16   unk_u16;
  u16   unk_u161;
  u16   unk_u162;
  struct DISP_DESC * desc;
}WINDOW;

typedef struct
{
/*
  struct DISP_DESC * desc; //use DispObject_GetDESC
  struct WINDOW *window; //use DispObject_GetWindow
*/
  char dummy[0x16C];
}DISP_OBJ;

typedef struct GUI_type
{
/*
  DISP_OBJ * __DISP_OBJ; //use GUIObject_GetDispObject
  BOOK * __book; //use GUIObject_GetBook
*/
  char dummy[0x90];
}_GUI;

//2do: remove these types
typedef struct GUI_type GUI_LIST;
typedef struct GUI_type GUI_TABMENUBAR;
typedef struct GUI_type GUI_ONEOFMANY;
typedef struct GUI_type GUI_NOFMANY;
typedef struct GUI_type GUI_FEEDBACK;


typedef struct DISP_DESC
{
  char *name        ;
  u16 DataSize    ;
  u16 field_6     ;
  void (*Constructor)(DISP_OBJ *);
  void (*onCreate)(DISP_OBJ *);
  void (*onClose)(DISP_OBJ *);
  void (*onRedraw)(DISP_OBJ *);
  void (*field_18)(DISP_OBJ *);
  void (*field_1C)(DISP_OBJ *);
  void (*onKey)(DISP_OBJ *,int scan_code,int unk,int repeat_num,int key_mode);
  void (*field_24)(DISP_OBJ *);
  void (*field_28)(DISP_OBJ *);
  void (*field_2C)(DISP_OBJ *);
  void (*field_30)(DISP_OBJ *);
  void (*field_34)(DISP_OBJ *);
  void (*field_38)(DISP_OBJ *);
  void (*field_39)(DISP_OBJ *);
  void (*field_3A)(DISP_OBJ *);
  char field_3B;
}DISP_DESC;

typedef struct
{
  u16 x1;
  u16 x2;
  u16 y1;
  u16 y2;
}RECT;


typedef struct
{
  char ChargingState;
  char ChargingMethod;
  int BatteryVoltage;
  int ChargerVoltage;
  int ChargerCurrent;
  int CurrentToBattery;
  int BatteryTemperature;
  int SystemTemperature;
  int RemainingCapacity;
  char RemainingCapacityInPercent;
  int PowerDissipation;
  int NrOfChargingCycles;
  int NrOfSecondsOnSafetyTimer;
  char SuspensionCause;
}BATT;

typedef struct
{
  int magic;
  u16 x1;
  u16 x2;
  u16 y1;
  u16 y2;
}C_INTERFACE;

typedef struct
{
  u16 unk_02;
  int rec_num;
}AB_GET_STRUCT;

typedef struct
{
  u16 num;
  u16 icon;
  int name;
  int name2;
  u16 field_C;
  u16 field_D;
  u16 big_icon;
  u16 field_10;
  u16 ID;
  u16 null;
}AB_ITEM_DESC_A2;

typedef struct
{
  u16 num;
  u16 tab;
  u16 num_in_tab;
  u16 icon;
  int name;
  char field_C;
  char field_D;
  u16 big_icon;
  u16 field_10;
  u16 ID;
}AB_ITEM_DESC_2020;

typedef struct
{
  u16 num;
  u16 field_2;
  u16 field_4;
  u16 icon;
  int name;
  u16 field_C;
  u16 field_E;
  u16 field_10;
  char field_12;
  char field_13;
  char field_14;
  char field_15;
  u16 field_16l;
  char ID;
  char field_19;
  char field_1A;
  char field_1B;
}AB_ITEM_DESC;

typedef struct {
  short ch;
  unsigned char bsic;
  unsigned char unk3;
  unsigned short cid;
  char unk6[2];
  int unk8;
  char index;
  char unkD[3];
  int unk10;
  char unk14;
  char unk15;
  short unk16;
  int unk18;
  char unk1C[2];
  char cc[2];
  char mnc[2];
  char lac;
  signed char rxam;
  int unk24;
  int unk28;
  char unk2C;
  char cro;
  char unk2E[2];
  unsigned short unk30;
  unsigned short unk32;
  unsigned short unk34;
  unsigned short unk36;
  signed char rxlvl;
  char unk39[3];
  int unk3C;
  int unk40;
} NEIGHBOUR_CELLDATA;


typedef struct {
  unsigned short cid;
  char unk2[2];
  char cc[2];
  char mnc[2];
  char lac;
  char unk09[3];
  int unk0C;
  int unk10;
  char unk14[2];
  signed char unk16;
  char unk17;
  int unk18;
  int unk1C;
  char unk20[2];
  signed char rxlev;
} CUR_CELLDATA;


typedef struct
{
  char CallID;
  char CallType;
  char CallState;
  int  *PNUM;
  char unk1;
  char unk2;
  char CallCause;
}CALLMANAGER_EVENT_DATA;

typedef struct
{
  char CallID;
  int unk1;
  char unk2;
  char CallType;
  char CallState;
  int  *PNUM;
  char unk3;
  char unk4;
  char CallCause;
}CALLMANAGER_EVENT_DATA_A2;

typedef struct
{
  int SesTime;
  int SentBytes;
  int RcvBytes;
}GPRS_SESSION_INFO;

typedef struct
{
  u16 item_name_len;
  char const_2;
  wchar_t * item_name;
}MIDP_DESC_ITEM;

typedef struct
{
  MIDP_DESC_ITEM * name;
  MIDP_DESC_ITEM * vendor;
  MIDP_DESC_ITEM * point;
}MIDP_DESC;

#pragma pack(2)
typedef struct
{
  u16 id;
  wchar_t name[10];
} FONT_DESC;
#pragma pack()



typedef struct SUB_EXECUTE
{
  FILESUBROUTINE * filesub;
  FILEITEM * file_item;
  BOOK * BrowserItemBook;
  int DB_BookID;
  void * unk2;
  void * unk3;
  GUI * gui;
  void * strinp_struct;
}SUB_EXECUTE;

typedef struct
{
  int attr;
  int unk_0x4;
  int unk_0x8;
  int unk_0xC;
  int unk_0x10;
  int unk_0x14;
  int st_size;
  unsigned int unk_0x1C;
  unsigned int unk_0x20;
  unsigned int unk_0x24;  
} W_FSTAT;

enum W_SEEK_SET {
  WSEEK_CUR=0,
  WSEEK_END=1,
  WSEEK_SET=2
};

enum _SEEK_SET {
  SEEK_SET=0,
  SEEK_CUR,
  SEEK_END
};

enum W_OPEN_ATTR {
  WA_Read=1,
  WA_Write=2,
  WA_Append=4,
  WA_Create=8,
  WA_Truncate=0x40
};

typedef struct
{
  void (*OldonRedraw)(DISP_OBJ *,int ,int ,int);
  void (*NewonRedraw)(DISP_OBJ *,int ,int ,int);
}SBY_REDRAW_RELEASE_MESSAGE;

typedef struct
{
  void (*SB_OldOnRedraw)(DISP_OBJ *,int ,int ,int);
  void (*SB_NewOnRedraw)(DISP_OBJ *,int ,int ,int);
  void (*SI_OldOnRedraw)(DISP_OBJ *,int ,int ,int);
  void (*SI_NewOnRedraw)(DISP_OBJ *,int ,int ,int);
  void (*SK_OldOnRedraw)(DISP_OBJ *,int ,int ,int);
  void (*SK_NewOnRedraw)(DISP_OBJ *,int ,int ,int);
}REDRAW_RELEASE_MESSAGE;

typedef void* PAudioControl;

typedef struct
{
  STRID text;
  wchar_t unk1;
  char unk2;
}SKLABEL;

typedef struct
{
  wchar_t name[0x31];
  wchar_t null;
  wchar_t shortcut_state;
  wchar_t erasable_flag;
}SHORTCUT_DESC;


typedef struct
{
  wchar_t name[0xC8];
  wchar_t name1[0xC8];
  wchar_t name2[0xC8];
  char shortcut_state;
  char shortcutID;
  char press_mode;
  char is_name1_present;
  char name_type;
  char isEditable;
}SHORTCUT_DESC_A2;


enum shortcut_state {
  SC_State_Absent=0,
  SC_State_Inactive=1,
  SC_State_Present=2,
  SC_State_MainMenu=3
};

enum name_type {
  SC_Name_StandName=0,
  SC_Name_NameAbsent=1,
  SC_Name_MainMenu=2,
  SC_Name_AddShortcut=3
};

typedef struct
{
  wchar_t fullpath_len;
  char unk_2;
  wchar_t * fullpath;
}JavaAppFullpath;

enum schemeID {
  file=0,
  http=1,
  https=2,
  rtsp=3,
  mailto=4,
  smsto=5,
  mmsto=6,
  sms=7,
  mms=8,
  tel=9,
  vcard=10,
  vnote=11,
  vcal=12
};

typedef struct {
  wchar_t lenght;
  char flag;
  void* ptr;
} STRID_DATA;

typedef int (*DB_FILE_FILTER)(const wchar_t *ExtList, const wchar_t *ItemPath, const wchar_t *ItemName);

enum LISTMSGS {
  LISTMSG_HighlightChanged=0,
  LISTMSG_GetItem=1,
  LISTMSG_SubItemHighlightChanged=2,
  LISTMSG_GetSubItem=3,
  LISTMSG_Edit=4,
  LISTMSG_Delete=5,
  LISTMSG_SearchIndex=7,
  LISTMSG_GetKeyboard=8,
  LISTMSG_ItemSelectionChanged=9,
  LISTMSG_DrawComplete=10,
  LISTMSG_ListBusy=11,

  LISTMSG_KeyRepeat = 12,
  LISTMSG_StringInputCharProtYesCB = 13,
  LISTMSG_StringInputCharProtNoCB = 14,
  LISTMSG_Minipopup = 15
};

typedef struct{
  wchar_t fname[255];
  wchar_t fpath[255];  
}BT_FILE_2020;

typedef struct{
  wchar_t fname[252];
  wchar_t fpath[252];  
}BT_FILE_2010;

typedef struct {
  char is_multiple;
  int Book_ID;
  STRID send;
  STRID sent;
  u16 obex_flag;
  union 
  {
    LIST *lst;
    union
    {
      BT_FILE_2020 btf2020;
      BT_FILE_2010 btf2010;
    };
  };
} SEND_OBEX_STRUCT;

typedef void* PCANVAS;

typedef struct
{
  PCANVAS* pcanvas;
  char unk[0x20];
  PCANVAS* pcanvas_text;
  int unk2;
  PCANVAS* pcanvas_icon;
}GC;

typedef void* GVI_OBJ;
typedef GVI_OBJ GVI_GC;
typedef GVI_OBJ GVI_BMP;
typedef GVI_OBJ GVI_PEN;
typedef GVI_OBJ GVI_BRUSH;

typedef struct 
{
  int x;
  int y; 
}POINT;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} GVI_XYWH;

typedef struct {
    char count;
    char *langs;
}LANGUAGE;

typedef struct {
    char dummy[0x424];
    LANGUAGE *lng;
    char cur_langID;
}DISP_OBJ_STRING_INPUT;

typedef struct
{
  int   TotalSpace;
  int   FreeSpace;
  int   BytesPerCluster;
  int   TotalClusters;
  int   TotalFreeClusters;
  int   unk0;
  int   unk1;
  int   unk2;
}VOLUMESIZE;

typedef struct
{
  long long   TotalSpace;
  long long   FreeSpace;
  int   BytesPerCluster;
  int   TotalClusters;
  int   TotalFreeClusters;
  int   unk0;
  int   unk1;
  int   unk2;
}VOLUMESIZE_A2;

typedef int (*DISP_OBJ_ONCREATE_METHOD)(DISP_OBJ *);
typedef void (*DISP_OBJ_ONCLOSE_METHOD)(DISP_OBJ *);
typedef void (*DISP_OBJ_ONREDRAW_METHOD)(DISP_OBJ *,int ,int,int);
typedef void (*DISP_OBJ_ONKEY_METHOD)(DISP_OBJ *,int ,int,int,int);

typedef void (*DISP_OBJ_METHOD)(DISP_OBJ *);

typedef int (*KEYHOOKPROC)(int key, int repeat_count, int mode, void*);
typedef int (*LISTFINDCALLBACK)(void* listitem, void* itemtofind);

typedef enum _LISTMENU_HOTKEY_MODE
{
  LKHM_SEARCH = 0,
  LKHM_SHORTCUT = 1,
  LKHM_FOCUS = 2,
  LKHM_PRESS = 3
} LISTMENU_HOTKEY_MODE;


typedef struct
{
  char unk1;		//�� 2010 ����� ����, �� ���� rat ������ �� �����, ����� ����� ���
  char RAT;
  int CI;
  char dummy[0x10];	//��� 3150 ���������� 0x4, ��� 2020 ���� 0x10, ��� 2010 ������ �� ����
}RAT_CI_DESC;


typedef struct
{
  char MCC[3];
  char MNC[2];
  char PLMN_x;
  char unk1[2];
  char LAC[2];
  char unk2[2];
}PLMN_LAC_DESC;


typedef struct
{
  unsigned char data[16];
}UUID, *PUUID;

// if( GetChipID()&CHIPID_MASK == CHIPID_DB... )
#define CHIPID_MASK 0xFE00
#define CHIPID_DB2000 0x7000
#define CHIPID_DB2010 0x8000
#define CHIPID_DB2020 0x9800
#define CHIPID_PNX5230 0xD000
#define CHIPID_DB3150 0xC800
#define CHIPID_DB3200 0xD800
#define CHIPID_DB3210 0xE800
#define CHIPID_DB3350 0xF000

typedef enum
{
  UI_OverlayStyle_NotDefined = 0,
  UI_OverlayStyle_Default,                      // Use original frame settings
  UI_OverlayStyle_FullScreen,                   // Fullscreen
  UI_OverlayStyle_FullScreenNoStatus,           // Fullscreen with no statusRow
  UI_OverlayStyle_TrueFullScreen,               // True fullscreen without softkeys and statusrow
  UI_OverlayStyle_InternalFrameSmall,           // Fullscreen with small internal frame
  UI_OverlayStyle_InternalFrameLarge,           // Fullscreen with large internal frame
  UI_OverlayStyle_InternalFrameOutput,          // Informationtext style
  UI_OverlayStyle_PopupFrame,                   // Popup with SW rendered frame
  UI_OverlayStyle_PopupNoFrame,                 // Transparent popup window. Default popup style.
  UI_OverlayStyle_FullScreenNoSoftkeys,         // Fullscreen without softkeys but with status row
  UI_OverlayStyle_Last
} UI_OverlayStyle_t;

typedef enum
{
  UI_TitleMode_Uninitated = 0, // Uninitiated
  UI_TitleMode_None,       // Show no title
  UI_TitleMode_Normal,     // Normal title size
  UI_TitleMode_Small,      // Small title
  UI_TitleMode_Tab,        // Show tabs instead of title.
  UI_TitleMode_Large,      // Large two row title (different fonts for 1:st and 2:nd line)
  UI_TitleMode_Desktop,    // Only for desktop title. Will be removed after hb107.
  UI_TitleMode_Last
} UI_TitleMode_t;

#endif
