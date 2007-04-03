#include "..\..\inc\swilib.h"
#include "..\conf_loader.h"
#include "..\mailclient.h"

//===============================================================================================
// ELKA Compatibility
#pragma inline
void patch_header(HEADER_DESC* head)
{
  head->rc.x=0;
  head->rc.y=YDISP;
  head->rc.x2=ScreenW()-1;
  head->rc.y2=HeaderH()+YDISP;
}
#pragma inline
void patch_input(INPUTDIA_DESC* inp)
{
  inp->rc.x=0;
  inp->rc.y=HeaderH()+1+YDISP;
  inp->rc.x2=ScreenW()-1;
  inp->rc.y2=ScreenH()-SoftkeyH()-1;
}

#define wslen(ws) ws->wsbody[0]
//===============================================================================================
int S_ICONS[8];
const char empty_str[]="";
const char eol[]="\r\n";
const char d_eol[]="\r\n\r\n";
// ------------------------------- Defines ----------------------------------------

const char wintranslation[128]=
{
  0x5F,0x5F,0x27,0x5F,0x22,0x3A,0xC5,0xD8,0x5F,0x25,0x5F,0x3C,0x5F,0x5F,0x5F,0x5F,
  0x5F,0x27,0x27,0x22,0x22,0x07,0x2D,0x2D,0x5F,0x54,0x5F,0x3E,0x5F,0x5F,0x5F,0x5F,
  0xFF,0xF6,0xF7,0x5F,0xFD,0x83,0xB3,0x15,0xF0,0x63,0xF2,0x3C,0xBF,0x2D,0x52,0xF4,
  0xF8,0x2B,'I' ,'i' ,0xA3,0xE7,0x14,0xFA,0xF1,0xFC,0xF3,0x3E,0x5F,0x5F,0x5F,0xF5,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
  0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF
};

const char koi8translation[128]=
{
  0x5F,0x5F,0x27,0x5F,0x22,0x3A,0xC5,0xD8,0x5F,0x25,0x5F,0x3C,0x5F,0x5F,0x5F,0x5F,
  0x5F,0x27,0x27,0x22,0x22,0x07,0x2D,0x2D,0x5F,0x54,0x5F,0x3E,0x5F,0x5F,0x5F,0x5F,
  0xFF,0xF6,0xF7,0xF1,0xF3,0x5F,'i' ,0xF5,0xF0,0x63,0xF2,0x3C,0xBF,0xA3,0x52,0xF4,
  0xF8,0x2B,0x5F,0xF0,0xF2,0xE7,'I' ,0xF4,0xF1,0xFC,0xF3,0x3E,0x5F,0x83,0x5F,0xF5,

  0xEE,0xA0,0xA1,0xE6,0xA4,0xA5,0xE4,0xA3,0xE5,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,
  0xAF,0xEF,0xE0,0xE1,0xE2,0xE3,0xA6,0xA2,0xEC,0xEB,0xA7,0xE8,0xED,0xE9,0xE7,0xEA,
  0x9E,0x80,0x81,0x96,0x84,0x85,0x94,0x83,0x95,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,
  0x8F,0x9F,0x90,0x91,0x92,0x93,0x86,0x82,0x9C,0x9B,0x87,0x98,0x9D,0x99,0x97,0x9A
};

const unsigned short dos2unicode[128]=
{
  0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,
  0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
  0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,
  0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
  0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,
  0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
  0x002D,0x002D,0x002D,0x00A6,0x002B,0x00A6,0x00A6,0x00AC,
  0x00AC,0x00A6,0x00A6,0x00AC,0x002D,0x002D,0x002D,0x00AC,
  0x004C,0x002B,0x0054,0x002B,0x002D,0x002B,0x00A6,0x00A6,
  0x004C,0x0433,0x00A6,0x0054,0x00A6,0x003D,0x002B,0x00A6,
  0x00A6,0x0054,0x0054,0x004C,0x004C,0x002D,0x0433,0x002B,
  0x002B,0x002D,0x002D,0x002D,0x002D,0x00A6,0x00A6,0x002D,
  0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,
  0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F,
  0x0401,0x0451,0x0404,0x0454,0x0407,0x0457,0x040E,0x045E,
  0x00B0,0x2022,0x00B7,0x0076,0x2116,0x00A4,0x00A6,0x00A0
};



unsigned int char8to16(int c, int type)
{
  if (c>=128)
  {
    switch(type)
    {
    case 1:
      //Win->Dos
      c=wintranslation[c-128];
      break;
    case 2:
      //Koi8->Dos
      c=koi8translation[c-128];
      break;
    case 3:
      break;
      //Dos
    }
    if (c<128) return(c);
    return(dos2unicode[c-128]);
  }
  return(c);
}


void ascii2ws(WSHDR *ws, const char *s)
{
  char c;
  CutWSTR(ws,0);
  while((c=*s++))
  {
    wsAppendChar(ws,char8to16(c,1));
  }
}

void koi2ws(WSHDR *ws, const char *s)
{
  char c;
  CutWSTR(ws,0);
  while((c=*s++))
  {
    wsAppendChar(ws,char8to16(c,2));
  }
}

void dos2ws(WSHDR *ws, const char *s)
{
  int c;
  CutWSTR(ws,0);
  while((c=*s++))
  {
    wsAppendChar(ws,char8to16(c,3));
  }
}  

// --------------------------------------------------------------------------------
extern char *unmime_header(const char *encoded_str);
// --------------------------------------------------------------------------------
const char mailer_db_name[]="mails.db";
const char _percent_t[]="%t";
const int minus11=-11;
unsigned short maincsm_name_body[140];



int displace_config;
// --------------------------------------------------------------------------------
volatile int main_menu_id;
volatile int maillist_menu_id;
volatile int options_menu_id;
volatile int view_mail_id;
volatile int request_recount_mailmenu;
// -------------------------- Global Variables -------------------------------------
typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}
//-----------------------------------------------------------------------------------

int menusoftkeys[]={0,1,2};
SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Options"},
  {0x0001,0x0000,(int)"Close"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

// ----------------------------------------------------------------------------------
#pragma inline=forced
int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}

int strcmp_nocase(const char *s,const char *d)
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

int strncmp_nocase(const char *s1,const char *s2,unsigned int n)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))&&(--n)) if (!c) break;
  return(i);
}

volatile ML_VIEW *mails;

void FreeMailDB()
{
  ML_VIEW *ml_cur=(ML_VIEW *)mails;
  LockSched();
  mails=0;
  UnlockSched();
  ML_VIEW *ml_prev;
  while (ml_cur)
  {
    mfree(ml_cur->uidl);
    mfree(ml_cur->header);
    mfree(ml_cur->subject);
    mfree(ml_cur->from);
    mfree(ml_cur->to);
    mfree(ml_cur->content_type);
    mfree(ml_cur->content_encoding);
    ml_prev=ml_cur;
    ml_cur=ml_cur->next;
    mfree(ml_prev);
  }
}

void InitHeaders();
int InitMailDB()
{
  char fname[256];
  int f;
  MAIL_DB mail_db;
  char *uidl;
  int n=0;
  ML_VIEW *ml_cur=(ML_VIEW *)&mails;
  unsigned int err;
  strcpy(fname,EML_PATH);
  if (fname[strlen(fname)-1]!='\\')
  {
    strcat(fname,"\\");
  }
  strcat(fname,mailer_db_name);
  FreeMailDB();
  if ((f=fopen(fname,A_BIN+A_ReadOnly,P_READ,&err))==-1)
  {
    mails=0;
    return(n);
  }
  while (fread(f,&mail_db,sizeof(MAIL_DB),&err)==sizeof(MAIL_DB))
  {
    if(mail_db.magic!=MDB_MAGIC) break;
    ml_cur=ml_cur->next=malloc(sizeof(ML_VIEW));
    ml_cur->next=0;
    ml_cur->state=mail_db.state;
    ml_cur->is_read=mail_db.is_read;
    ml_cur->mail_size=mail_db.mail_size;
    if (mail_db.uidl_len)
    {
      uidl=malloc(mail_db.uidl_len+1);
      uidl[mail_db.uidl_len]=0;
      fread(f,uidl,mail_db.uidl_len,&err);
      ml_cur->uidl=uidl;
    }
    else ml_cur->uidl=0;
    ml_cur->subject=0;
    ml_cur->header=0;
    ml_cur->from=0;
    ml_cur->to=0;
    ml_cur->content_type=0;
    ml_cur->content_encoding=0;
    ml_cur->content_type=0;
    n++;
  }
  fclose(f,&err);
  InitHeaders();
  return (n);
}

void write_mail_DB()
{
  int f;
  unsigned int err;
  char fname[128];
  ML_VIEW *ml_list=(ML_VIEW *)&mails;
  MAIL_DB mail_db;
  strcpy(fname,EML_PATH);
  strcat(fname,mailer_db_name);
  if ((f=fopen(fname,A_WriteOnly+A_BIN+A_Create+A_Truncate,P_WRITE,&err))==-1)
    return;
  while((ml_list=ml_list->next))
  {
    mail_db.magic=MDB_MAGIC;
    mail_db.uidl_len=(ml_list->uidl)?strlen(ml_list->uidl):NULL;
    mail_db.state=ml_list->state;
    mail_db.is_read=ml_list->is_read;
    mail_db.mail_size=ml_list->mail_size;
    fwrite(f,&mail_db,sizeof(MAIL_DB),&err);
    if (mail_db.uidl_len) fwrite(f,ml_list->uidl,mail_db.uidl_len,&err);
  }
  fclose(f,&err);
}

ML_VIEW *find_mlist_ByID(int id)
{
  ML_VIEW *ml_cur=(ML_VIEW *)(&mails);
  int i=0;
  while ((ml_cur=ml_cur->next))
  {
    if (i++==id) return (ml_cur);
  }
  return (0);
}

#pragma inline
int get_mlist_N()
{
  ML_VIEW *ml_cur=(ML_VIEW *)(&mails);
  int i=0;
  while ((ml_cur=ml_cur->next))
    i++;
  return i;
}

int get_fsize(int f)
{
  int cur_pos;
  int fsize;
  unsigned int err;
  cur_pos=lseek(f,0,S_CUR,&err,&err);
  fsize=lseek(f,0,S_END,&err,&err);
  lseek(f,cur_pos,S_SET,&err,&err);
  return (fsize);
}

const char eml_not_found[]="Error! File not found!";

void InitHeaders()
{
  int f;
  unsigned int err;
  char fname[128];
  char *buf, *dec_str;
  int fsize;
  char *_eol;
  ML_VIEW *ml_cur=(ML_VIEW *)&mails;
  while((ml_cur=ml_cur->next))
  {
    if (ml_cur->header)
    {
      mfree(ml_cur->header);
      ml_cur->header=0;
    }
    snprintf(fname,127,"%s%s.eml",EML_PATH,ml_cur->uidl);
    if ((f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&err))==-1)
    {
      continue;
    }
    fsize=get_fsize(f);
    buf=malloc(fsize+1);
    fread(f,buf,fsize,&err);
    fclose(f,&err);
    buf[fsize]=0;
    _eol=strstr(buf,d_eol);
    if (_eol)
    {
      *_eol=0;
      buf=realloc(buf,strlen(buf)+1);
    }
    dec_str=unmime_header(buf);
    mfree(buf);
    ml_cur->header=dec_str;
  }
}
    
char *get_string_from_header(ML_VIEW *ml_list, char *str)
{
  char *p;
  char *end;
  char *subject;
  int len;
  if (!ml_list) return (0);
  if (ml_list->header)
  {
    len=strlen(str);
    p=ml_list->header;
    p-=2;
    do
    {
      p+=2;
      end=strstr(p,eol);
      if (!strncmp_nocase(p, str,len))
      {
        if (end)
        {
          subject=malloc((end-p)+1);
          strncpy(subject,p,end-p);
          subject[end-p]=0;
        }
        else 
        {
          subject=malloc(strlen(p)+1);
          strcpy(subject,p);
        }
        return (subject);
      }
      if (!end) return (0);
      p=end;
    }
    while(*p!=0);
  }
  return 0;
}

char *get_subject(ML_VIEW *ml_list)
{
  char *subject;
  if (ml_list->subject) return ml_list->subject;
  subject=get_string_from_header(ml_list, "subject:");
  return (ml_list->subject=subject);
}

char *get_from(ML_VIEW *ml_list)
{
  char *from;
  if (ml_list->from) return ml_list->from;
  from=get_string_from_header(ml_list, "from:");
  return (ml_list->from=from);
}

char *get_to(ML_VIEW *ml_list)
{
  char *to;
  if (ml_list->to) return ml_list->to;
  to=get_string_from_header(ml_list, "to:");
  return (ml_list->to=to);
}

char *get_content_type(ML_VIEW *ml_list)
{
  char *content_type;
  if (ml_list->content_type) return ml_list->content_type;
  content_type=get_string_from_header(ml_list, "content-type:");
  return (ml_list->content_type=content_type);
}

char *get_content_encoding(ML_VIEW *ml_list)
{
  char *content_encoding;
  if (ml_list->content_encoding) return ml_list->content_encoding;
  content_encoding=get_string_from_header(ml_list, "content-transfer-encoding:");
  return (ml_list->content_encoding=content_encoding);
}
//----------------------------------------------------------------------------------------------
typedef struct{
  void *next;
  int content_type;
  int transfer_encoding;
  int charset;
  int offset;
  int size;
  char ct[256];
  char te[256];
  int ec_n;
}MAIL_PART;

typedef struct{
  MAIL_PART *top;
  char *eml;
}MAIL_VIEW;

char *strstr_nocase(const char *s1, const char *s2);
extern char * base64_decode(const char *str, size_t *size);
extern char * quoted_printable_decode(const char *str, size_t *size);
extern void koi2win(char*d,char *s);
extern void iso885952win(char*d,char *s);

enum {BIT8, BASE64, QPRINTABLE, BIT7};
enum {MULTIPART, APPLICATION, TEXT};

void saveas_locret(void){}


int saveas_onkey(GUI *data, GUI_MSG *msg)
{
  MAIL_VIEW *mail_view;
  MAIL_PART *mail;
  int f;
  unsigned int err;
  EDITCONTROL ec;
  char *p, *decoded;
  size_t size;
  char fname[128];
  mail_view=EDIT_GetUserPointer(data);
  if (msg->keys==0xFFF)
  {
    if (!mail_view)
    {
      ShowMSG(1,(int)"Can't save attach!");
      return (1);
    }
    mail=mail_view->top;
    p=mail_view->eml+mail->offset;
    size=mail->size;
    if (mail->transfer_encoding!=BASE64)
    {
      ShowMSG(1,(int)"Unknown encoding!");
      return (1);
    } 
    decoded=base64_decode(p, &size);
    if (!decoded) 
    {
      ShowMSG(1,(int)"Can't decode attach!");
      return (1);
    } 
    ExtractEditControl(data,2,&ec);
    ws_2str(ec.pWS,fname,128);
    if ((f=fopen(fname,A_WriteOnly+A_BIN+A_Truncate+A_Create, P_WRITE, &err))==-1)
    {
      mfree(decoded);
      ShowMSG(1,(int)"Can't create file!");
      return (1);
    }
    fwrite(f, decoded, size, &err);
    mfree(decoded);
    fclose(f, &err);
    ShowMSG(1,(int)"Succesfully saved!");
    return(1);
  }
  return(0); //Do standart keys
  //1: close
}

void saveas_ghook(GUI *data, int cmd)
{
  MAIL_VIEW *mail_view;
  static SOFTKEY_DESC sk={0x0FFF,0x0000,(int)"Save!"};
  mail_view=EDIT_GetUserPointer(data);
  if (cmd==3)
  {
    mfree(mail_view);
  }
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
  if (cmd==7)
  {
    SetSoftKey(data,&sk,SET_SOFT_KEY_N);
  }
}


HEADER_DESC saveas_hdr={0,0,0,0,NULL,(int)"����������",LGP_NULL};

INPUTDIA_DESC saveas_desc =
{
  1,
  saveas_onkey,
  saveas_ghook,
  (void *)saveas_locret,
  0,
  &menu_skt,
  {0,NULL,NULL,NULL},
  SMALL_FONT,
  100,
  101,
  0,
  //  0x00000001 - ��������� �� ������� ����
  //  0x00000002 - ��������� �� ������
  //  0x00000004 - �������� ���������
  //  0x00000008 - UnderLine
  //  0x00000020 - �� ���������� �����
  //  0x00000200 - bold
  0,
  //  0x00000002 - ReadOnly
  //  0x00000004 - �� ��������� ������
  //  0x40000000 - �������� ������� ����-������
  0x40000000
};






void create_save_as_dialog(MAIL_PART *top, char *eml)
{
  void *ma=malloc_adr();
  void *eq;  
  char *p;
  int c, i;
  char fname[256];
  EDITCONTROL ec;
  MAIL_VIEW *mail_view;
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  WSHDR *ews=AllocWS(128);
  wsprintf(ews,"%t","Path Save File:");  
  ConstructEditControl(&ec,ECT_HEADER,0x40,ews,wslen(ews));
  AddEditControlToEditQend(eq,&ec,ma);
  
  p=top->ct;
  p=strstr_nocase(p,"name=");
  p+=5;
  while(*p=='\"') p++;
  for (i=0; (c=p[i])!='\"';i++)
  {
    fname[i]=c;
  }
  fname[i]=0;
  p=unmime_header(fname);
  wsprintf(ews,"%s%t",SAVE_AS_FOLDER,p);
  mfree(p);
  ConstructEditControl(&ec,ECT_NORMAL_TEXT,0x40,ews,128);
  AddEditControlToEditQend(eq,&ec,ma);  
    
  
  mail_view=malloc(sizeof(MAIL_VIEW));
  mail_view->top=top;
  mail_view->eml=eml;
  patch_header(&saveas_hdr);
  patch_input(&saveas_desc);
  FreeWS(ews);
  CreateInputTextDialog(&saveas_desc,&saveas_hdr,eq,1,mail_view);  
}

  
HEADER_DESC ed1_hdr={0,0,0,0,NULL,(int)"��������",LGP_NULL};

void ed1_locret(void){}

int ed1_onkey(GUI *data, GUI_MSG *msg)
{
  int l;
  int i;
  MAIL_VIEW *view_list;
  MAIL_PART *top;
  view_list=EDIT_GetUserPointer(data);
  if (!view_list) return (0);
  if (!view_list->eml) return (0);
  
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    l=msg->gbsmsg->submess;
    if (l==LEFT_SOFT||l==ENTER_BUTTON)
    {
      i=EDIT_GetFocus(data);
      top=view_list->top;
      while(top)
      {
        if (top->ec_n==i)
        {
          if (top->content_type==APPLICATION) break;
        }
        top=top->next;
      }
      if (!top) return (0);
      create_save_as_dialog(top, view_list->eml);
      return (-1);
    }
  }
  return(0); 
}

void ed1_ghook(GUI *data, int cmd)
{
  int j;
  MAIL_VIEW *view_list;
  MAIL_PART *m, *prev;
  if (cmd==3)
  {
    view_list=EDIT_GetUserPointer(data);
    if (view_list)
    {
      m=view_list->top;
      while(m)
      {
        prev=m;
        m=m->next;
        mfree(prev);
      }
      mfree(view_list->eml);
      mfree(view_list);
    }
  }
  if (cmd==0x0C)
  {
    j=EDIT_GetFocus(data);
    if (EDIT_GetUnFocus(data)<j) EDIT_SetCursorPos(data,1);
  }
}
INPUTDIA_DESC ed1_desc=
{
  1,
  ed1_onkey,
  ed1_ghook,
  (void *)ed1_locret,
  0,
  &menu_skt,
  {0,0,0,0},
  4,
  100,
  101,
  0,
  0,
  0
};




int get_ctype_index(char *str)
{
  char *p;
  p=strstr_nocase(str, "Content-Type:");
  if (p)
  {
    p+=13;
    while(*p==' ') p++;
    if (!strncmp_nocase(p,"multipart",9))
      return MULTIPART;
    
    if (!strncmp_nocase(p,"application",11))
      return APPLICATION; 
    
    if (!strncmp_nocase(p,"text",4))
      return TEXT;
  }
  return TEXT;
}


int get_ctencoding_index(char *str)
{
  char *p;
  p=strstr_nocase(str, "Content-Transfer-Encoding:");
  if (p)
  {
    p+=26;
    while(*p==' ') p++;
    if (!strncmp_nocase(p,"8bit",4))
      return BIT8;
    
    if (!strncmp_nocase(p,"base64",6))
      return BASE64; 
    
    if (!strncmp_nocase(p,"quoted-printable",16))
      return QPRINTABLE;
    
    if (!strncmp_nocase(p,"7bit",4))
      return BIT8;
  }
  return BIT8;
}

enum {WIN_1251, KOI8_R, ISO_8859_5};
int get_charset(char *charset)
{
  if (!strncmp_nocase(charset,"windows-1251",12))
    return WIN_1251;
  
  if (!strncmp_nocase(charset,"\"windows-1251\"",14))
    return WIN_1251;
    
  if (!strncmp_nocase(charset,"koi8-r",6))
    return KOI8_R; 
  
  if (!strncmp_nocase(charset,"\"koi8-r\"",8))
    return KOI8_R; 
    
  if (!strncmp_nocase(charset,"ISO-8859-5",10))
    return ISO_8859_5;
  
  if (!strncmp_nocase(charset,"\"ISO-8859-5\"",12))
    return ISO_8859_5;
  
  return WIN_1251;
}
#pragma inline 
char *strchr_nocase(const char *s, int c) /* Copied from strchr.c */
{ 
  int ch = toupper(c);
  
  for (; toupper(*s) != ch; ++s)
  if (*s == '\0')
    return (0);
  return ((char *)s);
}

#pragma inline 
char *strstr_nocase(const char *s1, const char *s2) /* Copied from strstr.c */
{  
  if (*s2 == '\0')
    return ((char *)s1);
  
  for (; (s1 = strchr_nocase(s1, *s2)) != 0; ++s1)
  {    
    const char *sc1, *sc2;
    
    for (sc1 = s1, sc2 = s2; ; )
    if (*++sc2 == '\0')
      return ((char *)s1);
    else if (toupper(*++sc1) != toupper(*sc2))
      break;
  }
  return (0);
}

char *cstrcpy(char *s1, const char *s2)
{
  char *s = s1;
  int c=*s2++;
  while(c!=0 && c!='\r' && c!='\n')
  {
    *s++=c;
    c=*s2++;
  }
  *s=0;
  return (s1);
}
extern char * base64_decode(const char *str, size_t *size);
extern char * quoted_printable_decode(const char *str, size_t *size);
extern void koi2win(char*d,char *s);
extern void iso885952win(char*d,char *s);

const char default_ctype[]="Content-Type: text/plain; charset=\"windows-1251\"";
const char default_transfere[]="Content-Transfer-Encoding: 8bit";

MAIL_VIEW *ParseMailBody(void *eq,ML_VIEW *ml_list, void *ma, int ed_toitem)
{
  WSHDR *ws;
  MAIL_PART *top=0, *bot, *prev;
  MAIL_VIEW *view_list;
  EDITCONTROL ec;
  EDITC_OPTIONS ec_options;
  char *content_type, *content_encoding;
  int f;
  unsigned int err;
  char fname[128];
  char *eml, *end_header;
  int size;
  
  top=malloc(sizeof(MAIL_PART));
  bot=top;
  top->next=0;
  top->offset=0;
  top->ec_n=-1;
  
  content_type=get_content_type(ml_list);
  strcpy(top->ct,content_type?content_type:default_ctype);
  top->content_type=get_ctype_index(content_type);
  
  content_encoding=get_content_encoding(ml_list);
  strcpy(top->te,content_encoding?content_encoding:default_transfere);
  top->transfer_encoding=get_ctencoding_index(content_encoding);
  
  snprintf(fname,127,"%s%s.eml",EML_PATH,ml_list->uidl);
  f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&err);
  if (f==-1)
  {
    mfree(top);
    return (0);
  }
  
  size=get_fsize(f);
  if (!size)
  {
    fclose(f, &err);
    mfree(top);
    return (0);
  }
  eml=malloc(size+1);
  fread(f, eml, size, &err);
  fclose(f, &err);
  eml[size]=0;
  end_header=strstr(eml, d_eol);
  if (!end_header || (end_header+4)==(eml+size))
  {
    mfree(eml);
    mfree(top);
    return (0);
  }
  strcpy(eml,end_header+4);
  size=strlen(eml);
  top->size=size;
  eml=realloc(eml, size+1);
  while(bot)
  {
    char boundary[32];
    size_t buf_size;
    char *p, *buf, *b_end;
    int c, i;
    switch(bot->content_type)
    {
    case TEXT:
      p=bot->ct;
      p=strstr_nocase(p,"charset=");

      if (p)
      {
        bot->charset=get_charset(p+8);
      }
      else bot->charset=WIN_1251;
      switch(bot->transfer_encoding)
      {
      default:
      case BIT8:
        buf=malloc(bot->size+1);
        strncpy(buf,eml+bot->offset,size);
        buf[size]=0;
        break;
         
      case BASE64:
        buf_size=bot->size;
        buf=base64_decode(eml+bot->offset, &buf_size);
        break;
          
      case QPRINTABLE:
        buf_size=bot->size;
        buf=quoted_printable_decode(eml+bot->offset, &buf_size);
        break;  
      }
      switch(bot->charset)
      {
      case WIN_1251:
      default:
        break;
        
      case KOI8_R:
        koi2win(buf, buf);
        break;
   
      case ISO_8859_5:
        iso885952win(buf, buf);
        break;
      }
      ws=AllocWS(strlen(buf));
      ascii2ws(ws, buf);
        
      PrepareEditControl(&ec);
      ConstructEditControl(&ec,3,0x40,ws,wslen(ws));
      PrepareEditCOptions(&ec_options);
      SetFontToEditCOptions(&ec_options,1);
      CopyOptionsToEditControl(&ec,&ec_options);
      AddEditControlToEditQend(eq,&ec,ma); 
      ed_toitem++;
      bot->ec_n=ed_toitem;
      FreeWS(ws);
      mfree(buf);
      break;
      
    case MULTIPART:
      p=ml_list->header;
      if (!p) break;
      p=strstr_nocase(p,"boundary=");
      if (!p) break;
      p+=9;
      if (*p=='\"')
      {
        while(*p=='\"') p++;
      }
      for (i=0; (c=p[i])!='\"';i++)
      {
        boundary[i]=c;
      }
      boundary[i]=0;
      p=strstr_nocase(eml,boundary);
      while(p)
      {
        p+=i;
        if (!strncmp(p,"--\r\n",4)) break;

        buf=strstr(p,d_eol);
        buf+=4;
        
        b_end=strstr_nocase(p,boundary);
        if (!b_end)  continue;
        
        prev=bot;
        while(prev->next) prev=prev->next;
        prev=prev->next=malloc(sizeof(MAIL_PART));
        prev->next=0;
        prev->ec_n=-1;
        prev->offset=buf-eml;
        prev->size=b_end-buf;
        
        content_type=strstr_nocase(p,"content-type:");
        if (content_type && content_type<buf)
        {
          cstrcpy(prev->ct,content_type);          
        }
        else strcpy(prev->ct,default_ctype);
        prev->content_type=get_ctype_index(prev->ct);
        
        content_encoding=strstr_nocase(p,"Content-Transfer-Encoding:");
        if (content_encoding && content_encoding<buf)
        {
          cstrcpy(prev->te,content_encoding);          
        }
        else strcpy(prev->te,default_transfere);
        prev->transfer_encoding=get_ctencoding_index(prev->te);
        p=b_end;
      }      
    case APPLICATION:
      p=bot->ct;
      p=strstr_nocase(p,"name=");
      if (!p) break;
      p+=5;
      if (*p=='\"')
      {
        while(*p=='\"') p++;
      }
      for (i=0; (c=p[i])!='\"';i++)
      {
        fname[i]=c;
      }
      fname[i]=0;
      p=unmime_header(fname);
      
      ws=AllocWS(strlen(p));
      ascii2ws(ws, p);
      mfree(p);
      PrepareEditControl(&ec);
      ConstructEditControl(&ec,8,0x40,ws,wslen(ws));
      PrepareEditCOptions(&ec_options);
      SetFontToEditCOptions(&ec_options,1);
      CopyOptionsToEditControl(&ec,&ec_options);
      AddEditControlToEditQend(eq,&ec,ma); 
      ed_toitem++;
      bot->ec_n=ed_toitem;
      FreeWS(ws);
    }
    bot=bot->next;
  }
  view_list=malloc(sizeof(MAIL_VIEW));
  view_list->top=top;
  view_list->eml=eml;
  return (view_list);
}
  

int create_view(ML_VIEW *ml_list)
{
  MAIL_VIEW *view_list;
  void *ma=malloc_adr();
  void *eq;
  int ed_toitem=0;
  EDITCONTROL ec;
  EDITC_OPTIONS ec_options;
  char *from, *to, *subject;
  WSHDR *ws, *headers;
  
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  headers=AllocWS(100);
  from=get_from(ml_list);
  if (from)
  {
    from=strchr(from, ':')+1;
    while (*from==' ') from++;
    ws=AllocWS(strlen(from));
    ascii2ws(ws,from);
    ascii2ws(headers,"From:");
    
    ConstructEditControl(&ec,1,0x40,headers,wslen(headers));
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma);
    ed_toitem++;
    
    ConstructEditControl(&ec,3,0x40,ws,wslen(ws));
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma); 
    ed_toitem++;
    FreeWS(ws);
  }
  
  to=get_to(ml_list);
  if (to)
  {
    to=strchr(to, ':')+1;
    while (*to==' ') to++;
    ws=AllocWS(strlen(to));
    ascii2ws(ws,to);
    ascii2ws(headers,"To:");
    
    ConstructEditControl(&ec,1,0x40,headers,wslen(headers));
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma);
    ed_toitem++;
    
    ConstructEditControl(&ec,3,0x40,ws,wslen(ws));
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma); 
    ed_toitem++;
    FreeWS(ws);
  }
  
  subject=get_subject(ml_list);
  if (subject)
  {
    subject=strchr(subject, ':')+1;
    while (*subject==' ') subject++;
    ws=AllocWS(strlen(subject));
    ascii2ws(ws,subject);
    ascii2ws(headers,"Subject:");
    
    ConstructEditControl(&ec,1,0x40,headers,wslen(headers));
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma);
    ed_toitem++;
    
    ConstructEditControl(&ec,3,0x40,ws,wslen(ws));
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma); 
    ed_toitem++;
    FreeWS(ws);
  }
  ascii2ws(headers,"---------------------");
  ConstructEditControl(&ec,1,0x40,headers,wslen(headers));
  PrepareEditCOptions(&ec_options);
  SetFontToEditCOptions(&ec_options,1);
  CopyOptionsToEditControl(&ec,&ec_options);
  AddEditControlToEditQend(eq,&ec,ma);  
  ed_toitem++;
  
  view_list=ParseMailBody(eq,ml_list,ma,ed_toitem);    
  FreeWS(headers);
  patch_header(&ed1_hdr);
  patch_input(&ed1_desc);  
  return CreateInputTextDialog(&ed1_desc,&ed1_hdr,eq,1,view_list);
}  
  
//----------------------------------------------------------------------------------------------
      
int GetIconIndex(ML_VIEW *m_list)
{
  if (!m_list) return (7);
  int l=m_list->state;
  switch (l)
  {
  case M_FULL_LOADED:
    if (m_list->is_read)
      return 1;
    else return 0;
  case M_HEADERS_LOADED:
    return 2;
  case M_LOAD_FULL:
    return 5;
  case M_DELETE:
    return 6;
  }
  return 0;
}

// ---------------------------------------------------------------------------------


void set_state_for_all(int state)
{
  ML_VIEW *mail_cur=(ML_VIEW *)&mails;
  while((mail_cur=mail_cur->next))
  {
    mail_cur->state=state;
  }
}

void set_state_download(void *gui)
{
  ML_VIEW *cur_menu;
  cur_menu=MenuGetUserPointer(gui);
  cur_menu->state=M_LOAD_FULL;
  GeneralFuncF1(2);
}

void set_state_delete(void *gui)
{
  ML_VIEW *cur_menu;
  cur_menu=MenuGetUserPointer(gui);
  cur_menu->state=M_DELETE;
  GeneralFuncF1(2);
}

void set_state_download_all()
{
  set_state_for_all(M_LOAD_FULL);
  GeneralFuncF1(2);
}

void set_state_delete_all()
{
  set_state_for_all(M_DELETE);
  GeneralFuncF1(2);
}

void delete_record(void *gui)
{ 
  ML_VIEW *cur_menu;
  cur_menu=MenuGetUserPointer(gui);
  ML_VIEW *mail_cur=(ML_VIEW *)&mails;
  ML_VIEW *ml_prev=mail_cur;
  while((mail_cur=mail_cur->next))
  {
    if(!(strcmp(mail_cur->uidl,cur_menu->uidl)))
    {
      ml_prev->next=mail_cur->next;
      mfree(mail_cur->uidl);
      mfree(mail_cur->header);
      mfree(mail_cur->subject);
      mfree(mail_cur->from);
      mfree(mail_cur->to);
      mfree(mail_cur->content_type);
      mfree(mail_cur->content_encoding);
      mfree(mail_cur);
      request_recount_mailmenu=1;
      break;        
    }
    ml_prev=mail_cur;
  }
  GeneralFuncF1(2);
}
    
  

void back()
{
  GeneralFuncF1(1);
}



HEADER_DESC options_menuhdr={0,0,0,0,NULL,(int)"�����",LGP_NULL};

MENUITEM_DESC options_menu_ITEMS[6]=
{
  {NULL,(int)"���������",     LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"�������",       LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"������� ���",   LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"������� ������",LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"��������� ���", LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"�����",         LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
};

void *options_menu_HNDLS[6]=
{
  (void *)set_state_download,
  (void *)set_state_delete,
  (void *)set_state_delete_all,
  (void *)delete_record,
  (void *)set_state_download_all,
  (void *)back,  
};

MENU_DESC options_menu_STRUCT=
{
  0,(void *)NULL,(void*)NULL,NULL,
  menusoftkeys,
  &menu_skt,
  8,
  NULL,
  options_menu_ITEMS,   //Items
  options_menu_HNDLS,   //Procs
  6 //n
};

void create_options_menu(ML_VIEW *i)
{
  options_menuhdr.rc.x=3;
  options_menuhdr.rc.y=0x18;
  options_menuhdr.rc.x2=ScreenW()-6;
  options_menuhdr.rc.y2=0x18+0x13;
  
  options_menu_id=CreateMenu(1,0,&options_menu_STRUCT,&options_menuhdr,0,6,i,0);
}

// ----------------------------------------------------------------------------------

HEADER_DESC maillist_menuhdr={0,0,0,0,NULL,(int)"��������",LGP_NULL};


int maillist_menu_onkey(void *data, GUI_MSG *msg)
{
  ML_VIEW *mail_cur;
  int i;
  int keycode;
  keycode=msg->keys;
  i=GetCurMenuItem(data);
  mail_cur=find_mlist_ByID(i);
  if (!mail_cur) return (0);
  switch(keycode)
  {
  case 0x18:
    //    GeneralFunc_F1(1);
    create_options_menu(mail_cur);
    return(-1);
    
  case 0x3D:
    if (mail_cur->state==M_FULL_LOADED)
    {
      if(!mail_cur->is_read)
      {
        mail_cur->is_read=1;
        SUBPROC((void *)write_mail_DB);
      }
    }
    view_mail_id=create_view(mail_cur);
    return(-1);
  } 
  return(0);
}

void maillist_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
    if (request_recount_mailmenu)
    {
      request_recount_mailmenu=0;
      Menu_SetItemCountDyn(data,get_mlist_N());
    }
  }
}

void maillist_menu_iconhndl(void *data, int curitem, int *unk)
{
  ML_VIEW *mail_cur;
  WSHDR *ws;
  char *subject;
  mail_cur=find_mlist_ByID(curitem);
  if (!mail_cur) return;
  void *item=AllocMenuItem(data);
  subject=get_subject(mail_cur);

  if (subject)
  {
    subject=strchr(subject,':')+1;
    while(*subject==' ') subject++;
    ws=AllocMenuWS(data,strlen(subject));
    wsprintf(ws,_percent_t,subject);
  }
  else    
  {
    ws=AllocMenuWS(data,10);
    wsprintf(ws,_percent_t,"������");
  }
  SetMenuItemIconArray(data,item,S_ICONS);
  SetMenuItemText(data,item,ws,curitem);
  SetMenuItemIcon(data,curitem,GetIconIndex(mail_cur));
}


MENU_DESC maillist_menu=
{
  8,(void *)maillist_menu_onkey,(void*)maillist_menu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  0x11,
  (void *)maillist_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};




//------------------------------------------------------------------------------------

void main_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

int main_menu_onkey(void *data, GUI_MSG *msg)
{
  if (msg->keys==0x18)
  {
    return(-1);
  }

  return(0);
}


void CreateMailList(void)
{
  InitMailDB();
  int mails_num=get_mlist_N();
  if (mails_num)
  {
    patch_header(&maillist_menuhdr);
    maillist_menu_id=CreateMenu(0,0,&maillist_menu,&maillist_menuhdr,0,mails_num,0,0);
  }
  else ShowMSG(1,(int)"Nothing loaded!");
}

void Incoming(void)
{
  CreateMailList();
}

void Outgoing(void)
{
}


void Options(void)
{
  WSHDR* ws;
  ws=AllocWS(128);
  str_2ws(ws,successed_config_filename,128);
  ExecuteFile(ws,0,0);
  FreeWS(ws);
}

void About()
{
  ShowMSG(2, (int)"MailClient v0.1\n(C) by Kren\nRst7/CBSIE");
}

void Exit(void)
{
  GeneralFuncF1(1);
}

HEADER_DESC mainmenu_HDR={0,0,0,0,NULL,(int)"MailViewer",LGP_NULL};


MENUITEM_DESC mainmenu_ITEMS[5]=
{
  {NULL,(int)"��������",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"���������",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"���������",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"�� �����",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"�����",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};

void *mainmenu_HNDLS[5]=
{
  (void *)Incoming,
  (void *)Outgoing,
  (void *)Options,
  (void *)About,
  (void *)Exit,
};


MENU_DESC mainmenu_STRUCT=
{
  8,(void *)main_menu_onkey,(void*)main_menu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  0x11,
  NULL,
  mainmenu_ITEMS,   //Items
  mainmenu_HNDLS,   //Procs
  5   //n
};

// ------------------------  Creating CSM -------------------------------- //
void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  patch_header(&mainmenu_HDR);
  csm->gui_id=main_menu_id=CreateMenu(0,0,&mainmenu_STRUCT,&mainmenu_HDR,0,5,0,0);
}

void maincsm_onclose(CSM_RAM *csm)
{
  FreeMailDB();
  SUBPROC((void *)ElfKiller);
}


int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      csm->csm.state=-3;
    }
    if ((int)msg->data0==maillist_menu_id)
    {
      maillist_menu_id=0;
    }
    if ((int)msg->data0==options_menu_id)
    {
      options_menu_id=0;
      if ((int)msg->data1==2)
      {
        SUBPROC((void *)write_mail_DB);
      }   
    }
  }
  return(1);
}



const struct
{
  CSM_DESC maincsm;
  WSHDR maincsm_name;
}MAINCSM =
{
  {
  maincsm_onmessage,
  maincsm_oncreate,
#ifdef NEWSGOLD
  0,
  0,
  0,
  0,
#endif
  maincsm_onclose,
  sizeof(MAIN_CSM),
  1,
  &minus11
  },
  {
    maincsm_name_body,
    NAMECSM_MAGIC1,
    NAMECSM_MAGIC2,
    0x0,
    139
  }
};


void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"MailViewer");
}


int main()
{
  MAIN_CSM main_csm;
  InitConfig();
  
  S_ICONS[0]=(int)I_UNREAD;
  S_ICONS[1]=(int)I_READ;
  S_ICONS[2]=(int)I_HEADER;
  S_ICONS[3]=(int)I_HEADER_ATT;
  S_ICONS[4]=(int)I_MES_ATT;
  S_ICONS[5]=(int)I_MES_DOWN;
  S_ICONS[6]=(int)I_MES_DEL;

  LockSched();
  UpdateCSMname();
  CreateCSM(&MAINCSM.maincsm,&main_csm,0);
  UnlockSched();
}
