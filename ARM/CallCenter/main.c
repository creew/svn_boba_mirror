#include "..\inc\swilib.h"
#include "conf_loader.h"
#include "ussd_process.h"

#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])

static const char progress_colors[MAX_CASH_SIZE][4]={{0xFF,0x00,0x00,0x32},
                                                 {0x00,0xFF,0x00,0x32},
                                                 {0x00,0xFF,0xFF,0x32},
                                                 {0xFF,0xFF,0x00,0x32}};

static GBSTMR tmr_scroll;

extern int CurrentCASH[MAX_CASH_SIZE];
extern int MaxCASH[MAX_CASH_SIZE];

extern const int ENA_VIBRA;
extern const unsigned int vibraPower;
extern const unsigned int vibraDuration;
extern const unsigned int IDLE_Y;
extern const char COLOR_MENU_BK[4];
extern const char COLOR_MENU_BRD[4];
extern const char COLOR_NOTSELECTED[4];
extern const char COLOR_SELECTED[4];
extern const char COLOR_SELECTED_BG[4];
extern const char COLOR_SELECTED_BRD[4];
extern const char COLOR_SEARCH_MARK[4];

#define TMR_SECOND 216


#pragma inline
static void patch_header(const HEADER_DESC* head)
{
  ((HEADER_DESC*)head)->rc.x=0;
  ((HEADER_DESC*)head)->rc.y=YDISP;
  ((HEADER_DESC*)head)->rc.x2=ScreenW()-1;
  ((HEADER_DESC*)head)->rc.y2=HeaderH()-1+YDISP;
}
#pragma inline
static void patch_input(const INPUTDIA_DESC* inp)
{
  ((INPUTDIA_DESC*)inp)->rc.x=0;
  ((INPUTDIA_DESC*)inp)->rc.y=HeaderH()+1+YDISP;
  ((INPUTDIA_DESC*)inp)->rc.x2=ScreenW()-1;
  ((INPUTDIA_DESC*)inp)->rc.y2=ScreenH()-SoftkeyH()-1;
}

#ifdef ELKA
#define MAX_ESTR_LEN 9
#else
#define MAX_ESTR_LEN 13
#endif

#ifdef NEWSGOLD
#define MAX_RECORDS 5000
#define LEVEL1_RN	(41*41)
#define LEVEL2_RN	(41)
#else 
#define MAX_RECORDS 1024
#define LEVEL1_RN	(0x20)
#endif

#define NUMBERS_MAX 5

#define wslen(ARG) (ARG->wsbody[0])

#ifdef NEWSGOLD
#else 
#define LAST_NAME 0x23
#define FIRST_NAME 0x24
#define PHONE_NUMBER 0x2D
#define PHONE_OFFICE 0x2A
#define PHONE_MOBILE 0x2C
#define PHONE_FAX 0x2B
#define PHONE_FAX2 0x5E
#endif

static CSM_DESC icsmd;

static int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
static void (*old_icsm_onClose)(CSM_RAM*);

static int (*old_ed_onkey)(GUI *gui, GUI_MSG *msg);
static void (*old_ed_ghook)(GUI *gui, int cmd);
static void (*old_ed_redraw)(void *data);

//0 - ���� ��������� �������
//1 - ������ ��������, ����������, ���� ���������� �� onRedraw
//2 - �������� onRedraw, idle-�����
//3 - ������ ������
//4 - ��������� ������
//5 - ������������ �����
static volatile int hook_state=0;

static volatile WSHDR *e_ws;

static volatile int curpos; //������� ������� � ������

typedef struct
{
  void *next;
  WSHDR *name;
  WSHDR *num[NUMBERS_MAX];
  WSHDR *icons;
}CLIST;

static volatile CLIST *cltop; //������

static char dstr[NUMBERS_MAX][40];

static int menu_icons[NUMBERS_MAX];
static int utf_symbs[NUMBERS_MAX];

#ifdef NEWSGOLD
#define USR_WIRE 0xE106
#define USR_MOBILE 0xE107
#define WORK_WIRE 0xE108
#define WORK_FAX 0xE109
#define USR_FAX 0xE10A
#define WORK_MOBILE 0xE10E

static void InitIcons(void)
{
  menu_icons[0]=GetPicNByUnicodeSymbol(utf_symbs[0]=USR_WIRE); //12345
  menu_icons[1]=GetPicNByUnicodeSymbol(utf_symbs[1]=WORK_WIRE); //093
  menu_icons[2]=GetPicNByUnicodeSymbol(utf_symbs[2]=USR_MOBILE); //651
  menu_icons[3]=GetPicNByUnicodeSymbol(utf_symbs[3]=WORK_MOBILE); //884
  menu_icons[4]=GetPicNByUnicodeSymbol(utf_symbs[4]=USR_FAX);
}

#else
#define USR_WIRE 0xE100
#define WORK_WIRE 0xE102
#define USR_MOBILE 0xE101
#define USR_FAX1 0xE103
#define USR_FAX2 0xE104

static void InitIcons(void)
{
  menu_icons[0]=GetPicNByUnicodeSymbol(utf_symbs[0]=USR_WIRE); 
  menu_icons[1]=GetPicNByUnicodeSymbol(utf_symbs[1]=WORK_WIRE); 
  menu_icons[2]=GetPicNByUnicodeSymbol(utf_symbs[2]=USR_MOBILE); 
  menu_icons[3]=GetPicNByUnicodeSymbol(utf_symbs[3]=USR_FAX1);
  menu_icons[4]=GetPicNByUnicodeSymbol(utf_symbs[4]=USR_FAX2);
}
#endif

//���������� ������
static void FreeCLIST(void)
{
  LockSched();
  CLIST *cl=(CLIST*)cltop;
  cltop=0;
  UnlockSched();
  while(cl)
  {
    CLIST *p;
    FreeWS(cl->name);
    for(int i=0;i<NUMBERS_MAX;i++) FreeWS(cl->num[i]);
    FreeWS(cl->icons);
    p=cl;
    cl=(CLIST*)(cl->next);
    mfree(p);
  }
}

//-----------------------------------------------------
//����� ��������� � ������ �� ������ �9
//-----------------------------------------------------
static unsigned int us_reverse(unsigned int v)
{
  return((v>>8)|((v&0xFF)<<8));
}

static int CompareStrT9(WSHDR *ws, WSHDR *ss, int need_insert_color)
{
  int spos=1;
  int wpos=1;
  int c;
  
  int first_pos=-1;

  //������� ������ ��� ������ ������
  static const char key[256]=
    "11111111111111111111111111111111"   
      "10001**0***0000*012345678900***0"
	"0222333444555666777788899991*110"
	  "122233344455566677778889999111*1"
	    "11111111111111111111111111111111"
	      "11111111311111111111111131111111"
		"22223333444455566677778888899999"
		  "22223333444455566677778888899999";

  if (!wslen(ss)) return(1); //������ ������ ������ ������ ;)
  if (!wslen(ws)) return(0); //� ������ �������� �� �������� ;)
  do
  {
    c=ws->wsbody[wpos];
    if ((wpos==1)&&(c=='+'))
    {
      wpos=2; //������� ������� '+'
      continue;
    }
    if (!spos)
    {
      //���� ������
      if (c==' ') spos=1; //�������� ������
    }
    else
    {
      //����������� � ��� ������
      if ((c>=0x410)&&(c<0x450)) c-=0x350;
      if ((c==0x401)) c=0xA8;
      if ((c==0x451)) c=0xB8;
      c&=0xFF;
      c=key[c];
      if (c==ss->wsbody[spos])
      {
	if (first_pos<0) first_pos=wpos;
	spos++;
	if (spos>wslen(ss))
	{
	  if (need_insert_color&&(first_pos>0))
	  {
	    wsInsertChar(ws,us_reverse(((unsigned short *)COLOR_NOTSELECTED)[1]),wpos+1);
	    wsInsertChar(ws,us_reverse(((unsigned short *)COLOR_NOTSELECTED)[0]),wpos+1);
	    wsInsertChar(ws,0xE006,wpos+1);
	    wsInsertChar(ws,us_reverse(((unsigned short *)COLOR_SEARCH_MARK)[1]),first_pos);
	    wsInsertChar(ws,us_reverse(((unsigned short *)COLOR_SEARCH_MARK)[0]),first_pos);
	    wsInsertChar(ws,0xE006,first_pos);
	  }
	  return(1); //��� �������
	}
      }
      else 
      {
	first_pos=-1;
	spos=0; //���� ����� �����
      }
    }
    wpos++;
  }
  while(wpos<=wslen(ws));
  return(0);
}

//��������� ������� �����
int wstrcmp(WSHDR *ws1, WSHDR *ws2)
{
  int l1=wslen(ws1);
  int l2=wslen(ws2);
  int pos=1;
  int c;
  while((pos<=l1)&&(pos<=l2))
  {
    c=ws1->wsbody[pos]-ws2->wsbody[pos];
    if (c) return c;
    pos++;
  }
  return(l1-l2);
}

//=====================================================
// ����������� ������
//=====================================================
static void ConstructList(void)
{
  int fin;
  unsigned int ul;
  char recname[128];

  AB_UNPRES ur;
  void *buffer;
  
#pragma pack(1)
  struct {
#ifdef NEWSGOLD
    long dummy1;
    short dummy2;
    char bitmap[MAX_RECORDS/8];
#else
    long dummy1;
    char bitmap[MAX_RECORDS/8];    
#endif    
  } ABmain;
#pragma pack()

  unsigned int rec=0;
  int fsz;
  CLIST contact;

  WSHDR *sws=AllocWS(50);

  FreeCLIST(); //���������� ����. ������
  LockSched();
  if (e_ws) wstrcpy(sws,(WSHDR *)e_ws);
  if (hook_state==4) hook_state=5;
  UnlockSched();
  zeromem(&contact,sizeof(contact));
  if ((buffer=malloc(65536)))
  {
    zeromem(&ABmain,sizeof(ABmain));
    if ((fin=fopen("0:\\System\\apo\\addr\\main",A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
    {
#ifdef ELKA
      if (fread(fin,&ABmain,sizeof(ABmain),&ul)>=194)
#else
      if (fread(fin,&ABmain,sizeof(ABmain),&ul)==sizeof(ABmain))
#endif
      {
	fclose(fin,&ul);
	do
	{
          #ifdef NEWSGOLD
	  if (ABmain.bitmap[rec>>3]&(0x80>>(rec&7)))
          #else
          if (ABmain.bitmap[rec>>3]&(1<<(rec&7)))
          #endif   
	  {
            #ifdef NEWSGOLD
	    //������ ���� � �������
            unsigned int rl1;
	    unsigned int rl2;
	    unsigned int rl3;
	    rl1=rec/LEVEL1_RN;
	    rl2=(rec%LEVEL1_RN)/LEVEL2_RN;
	    rl3=rec%LEVEL2_RN;
	    snprintf(recname,128,"0:\\System\\apo\\addr\\data\\%02d\\%02d\\%02d",rl1,rl2,rl3);
            #else
	    unsigned int rl1=rec/LEVEL1_RN;
	    unsigned int r12=rec%LEVEL1_RN;
	    snprintf(recname,128,"0:\\System\\apo\\addr\\%02x\\%02x",rl1,r12);            
            #endif             
	    if ((fin=fopen(recname,A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
	    {
	      zeromem(&ur,sizeof(AB_UNPRES));
              fsz=lseek(fin,0,S_END,&ul,&ul);
              lseek(fin,0,S_SET,&ul,&ul);
  	      fread(fin,buffer,fsz,&ul);
	      fclose(fin,&ul);
              #ifdef NEWSGOLD
	      UnpackABentry(&ur,((char *)buffer+8),fsz,0x28);
              #else
              UnpackABentry(&ur,((char *)buffer+4),fsz-4,0x28);
              #endif
	      int i=0;
	      zeromem(&contact,sizeof(contact));
	      contact.icons=AllocWS(NUMBERS_MAX);
	      while(i<ur.number_of_records)
	      {
		AB_UNPRES_ITEM *r=ur.record_list+i;
		if (r->no_data!=1)
		{
		  switch(GetTypeOfAB_UNPRES_ITEM(r->item_type))
		  {
		  case 0x05:
                    #ifdef NEWSGOLD
                    if (r->item_type==0x81)
                    #else
                    if (r->item_type==LAST_NAME||r->item_type==FIRST_NAME)
                    #endif   
		    {
                      #ifdef NEWSGOLD
		      if (r->data)
		      {
			wstrcpy(contact.name=AllocWS(150),(WSHDR *)(r->data));
			*((int *)(&contact.next))|=CompareStrT9(contact.name,sws,0);
		      }
                      #else
                      if (r->data)
		      { 
                        if (!contact.name)
                        {
			 wstrcpy(contact.name=AllocWS(150),(WSHDR *)(r->data));
			 *((int *)(&contact.next))|=CompareStrT9(contact.name,sws,0);
                        }
                        else
                        {
                         wsAppendChar(contact.name,',');
                         wsAppendChar(contact.name,' ');
                         wstrcat(contact.name,(WSHDR *)(r->data));
                         *((int *)(&contact.next))|=CompareStrT9(contact.name,sws,0);
                        }
		      }
                      #endif 
		    }
		    break;
		  case 0x01:
		    {
		      PKT_NUM *p=(PKT_NUM*)r->data;
		      unsigned int n=r->item_type;
		      int j;
		      unsigned int c;
		      WSHDR *ws;
                      #ifdef NEWSGOLD
		      if (n!=106)
			n-=0x62;
		      else
			n=4;
                      if (n<5)
                      #else
                      switch(r->item_type)
                      {
                      case PHONE_NUMBER:
                        n=0;    break;      
                      case PHONE_OFFICE:
                        n=1;    break;
                      case PHONE_MOBILE:
                        n=2;    break;
                      case PHONE_FAX:
                        n=3;    break;
                      case PHONE_FAX2:
                        n=4;    break;
                      default:
                        continue;
                      }
                      #endif 
		      {
			if (p)
			{
                          unsigned int c1;
                          int m;
			  ws=contact.num[n]=AllocWS(50);
			  //��������� ������ ���������
			  wsAppendChar(contact.icons,utf_symbs[n]);
			  j=0;
                          m=0;
			  if (p->format==0x91) wsAppendChar(ws,'+');
			  while(j<p->data_size)
			  {
                            if (m&1) {c1=c>>4; j++;}
                            else c1=(c=p->data[j])&0x0F;
			    if (c1==0x0F) break;
                            
                            if (c1==0xA) c1='*';
                            else if (c1==0xB) c1='#';
                            else if (c1==0xC) c1='+';
                            else if (c1==0xD) c1='?';
                            else c1+='0';
                            wsAppendChar(ws,c1);
			    m++;
			  }
			  *((int *)(&contact.next))|=CompareStrT9(ws,sws,0);
			}
		      }
		      break;
		    }
		  }
		}
		i++;
	      }
	      if (!contact.next)
	      {
		FreeWS(contact.name);
		for(int i=0;i<NUMBERS_MAX;i++) FreeWS(contact.num[i]);
		FreeWS(contact.icons);
	      }
	      FreeUnpackABentry(&ur,mfree_adr());
	      if (hook_state!=5) goto L_STOP;
	      LockSched();
	      if ((hook_state==5)&&(contact.next))
	      {
		//��������� � ������
		int n=0;
		CLIST *p=malloc(sizeof(contact));
		CLIST *t;
		CLIST *b=(CLIST *)(&cltop);
		contact.next=0;
		memcpy(p,&contact,sizeof(contact));
		for(;;)
		{
		  if (t=b->next)
		  {
		    //���� ���������
		    if (wstrcmp(contact.name,t->name)<0)
		    {
		      //��������� ������ ������������, ������� �����
		      p->next=t;
		      b->next=p;
		      break;
		    }
		    b=t; //���������
		    n++; //����� ������������, ��� �������, ����� �� REDRAW
		  }
		  else
		  {
		    //������ ��� ������
		    b->next=p;
		    break;
		  }
		}
		if (curpos<2)
		{
		  if (n<5) REDRAW();
		}
		else
		{
		  if ((unsigned int)(n-(curpos-2))<5) REDRAW();
		}
	      }
	      UnlockSched();
	    }
	  }
	  rec++;
	}
	while(rec<MAX_RECORDS);
      }
      else
      {
	fclose(fin,&ul);
      }
    }
  L_STOP:
    mfree(buffer);
  }
  if (contact.next)
  {
    FreeWS(contact.name);
    for(int i=0;i<NUMBERS_MAX;i++) FreeWS(contact.num[i]);
    FreeWS(contact.icons);
  }
  LockSched();
  if (hook_state==5) hook_state=2; else FreeCLIST();
  UnlockSched();
  REDRAW();
  FreeWS(sws);
}

static volatile int scroll_disp;
static volatile int max_scroll_disp;

static void scroll_timer_proc(void)
{
  int i=max_scroll_disp;
  if (i)
  {
    if (scroll_disp>=i)
    {
      scroll_disp=0;
      GBS_StartTimerProc(&tmr_scroll,TMR_SECOND,scroll_timer_proc);
    }
    else
    {
      scroll_disp++;
      GBS_StartTimerProc(&tmr_scroll,scroll_disp!=i?TMR_SECOND>>3:TMR_SECOND,scroll_timer_proc);
    }
    REDRAW();
  }
}

static void DisableScroll(void)
{
  GBS_DelTimer(&tmr_scroll);
  max_scroll_disp=0;
  scroll_disp=0;
}

static void my_ed_redraw(void *data)
{
  //  WSHDR *ews=(WSHDR*)e_ws;
  int i=curpos-2;
  int cp;
  CLIST *cl=(CLIST *)cltop;
  old_ed_redraw(data);
  
  WSHDR *prws=AllocWS(256);

  if (!cl) return;

  if (!e_ws) return;

  if (e_ws->wsbody[0]<MAX_ESTR_LEN) //�� ����� <MAX_ESTR_LEN
  {
    int y=ScreenH()-SoftkeyH()-(GetFontYSIZE(FONT_MEDIUM)+1)*5-5;

    DrawRectangle/*DrawRoundedFrame*/(1,y,ScreenW()-2,ScreenH()-SoftkeyH()-2,0/*,0,0*/,COLOR_MENU_BRD,COLOR_MENU_BK);

    if (i<0) cp=curpos; else cp=2;
    while(i>0)
    {
      if (!cl) break;
      cl=(CLIST *)(cl->next);
      i--;
    }
    i=0;
    do
    {
      int dy=i*(GetFontYSIZE(FONT_MEDIUM)+1)+y;
      if (!cl) break;
      if (i!=cp)
      {
	wstrcpy(prws,cl->name);
	if (e_ws) CompareStrT9(prws,(WSHDR *)e_ws,1);
	DrawString(prws,3,dy+4,ScreenW()-4,dy+3+GetFontYSIZE(FONT_MEDIUM),FONT_MEDIUM,0x80,COLOR_NOTSELECTED,GetPaletteAdrByColorIndex(23));
	//DrawScrollString(cl->name,3,dy+4,ScreenW()-4,dy+3+GetFontYSIZE(FONT_MEDIUM),1,FONT_MEDIUM,0x80,COLOR_NOTSELECTED,GetPaletteAdrByColorIndex(23));
      }
      else
      {
	int icons_size=Get_WS_width(cl->icons,FONT_MEDIUM_BOLD);
	{
	  int i=Get_WS_width(cl->name,FONT_MEDIUM_BOLD);
	  i-=(ScreenW()-7-icons_size);
	  if (i<0)
	  {
	    DisableScroll();
	  }
	  else
	  {
	    if (!max_scroll_disp)
	    {
	      GBS_StartTimerProc(&tmr_scroll,TMR_SECOND,scroll_timer_proc);
	    }
	    max_scroll_disp=i;
	  }
	}
	DrawRectangle/*DrawRoundedFrame*/(2,dy+2,ScreenW()-3,dy+3+GetFontYSIZE(FONT_MEDIUM_BOLD),0/*,0,0*/,COLOR_SELECTED_BRD,COLOR_SELECTED_BG);
	DrawScrollString(cl->name,3,dy+4,ScreenW()-5-icons_size,dy+3+GetFontYSIZE(FONT_MEDIUM_BOLD),scroll_disp+1,FONT_MEDIUM_BOLD,0x80,COLOR_SELECTED,GetPaletteAdrByColorIndex(23));
	DrawString(cl->icons,ScreenW()-4-icons_size,dy+4,ScreenW()-5,dy+3+GetFontYSIZE(FONT_MEDIUM_BOLD),FONT_MEDIUM_BOLD,0x80,COLOR_SELECTED,GetPaletteAdrByColorIndex(23));
      }
      cl=(CLIST *)cl->next;
      i++;
    }
    while(i<5);
  }
  FreeWS(prws);
}

static void ChangeRC(GUI *gui)
{
#ifdef ELKA
  static const RECT rc={6,80,234,140};
#else
  static const RECT rc={6,40,126,100};
#endif
  if (e_ws)
  {
    if (wslen(e_ws)>=MAX_ESTR_LEN) return;
  }
  if (!gui) return;
  char *p=(char *)gui;
  p+=DISPLACE_OF_EDGUI;
  gui=*((GUI **)p);
  if (!gui) return;
  void **m=GetDataOfItemByID(gui,4);
  if (gui->canvas) memcpy(gui->canvas,&rc,sizeof(rc));
  if (!m) return;
  if (m[0]) memcpy(m[0],&rc,sizeof(rc));
}


static const int menusoftkeys[]={0,1,2};

static const SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Select"},
  {0x0001,0x0000,(int)"Back"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

static const SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

static int is_sms_need=0;
static WSHDR *ews;
//static WSHDR *dbg_ws;

static void edsms_locret(void){}

static int edsms_onkey(GUI *data, GUI_MSG *msg)
{
  EDITCONTROL ec;
  const char *snum=EDIT_GetUserPointer(data);
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    if (msg->gbsmsg->submess==GREEN_BUTTON)
    {
      ExtractEditControl(data,2,&ec);
      WSHDR *sw=AllocWS(ec.pWS->wsbody[0]);
      wstrcpy(sw,ec.pWS);
      SendSMS(sw,snum,MMI_CEPID,MSG_SMS_RX-1,6);
      return(1);
    }
  }
  //-1 - do redraw
  return(0); //Do standart keys
  //1: close
}

static void edsms_ghook(GUI *data, int cmd)
{
}

static const HEADER_DESC edsms_hdr={0,0,0,0,NULL,(int)"Write SMS",LGP_NULL};

static const INPUTDIA_DESC edsms_desc=
{
  1,
  edsms_onkey,
  edsms_ghook,
  (void *)edsms_locret,
  0,
  &menu_skt,
  {0,0,0,0},
  4,
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
  0x40000000 // �������� ������� ����-������
};

static void VoiceOrSMS(const char *num)
{
  if (!is_sms_need)
  {
//    #ifdef NEWSGOLD
//    MakeVoiceCall(num,0x10,0x20C0);
//    #else
    MakeVoiceCall(num,0x10,0x2FFF);
//    #endif
  }
  else
  {
    void *ma=malloc_adr();
    void *eq;
    EDITCONTROL ec;
    PrepareEditControl(&ec);
    eq=AllocEQueue(ma,mfree_adr());
    wsprintf(ews,"SMS to %s:",num);
    ConstructEditControl(&ec,1,0x40,ews,64);
    AddEditControlToEditQend(eq,&ec,ma);
    //wsprintf(ews,percent_t,"");
    CutWSTR(ews,0);
    ConstructEditControl(&ec,3,0x40,ews,1024);
    AddEditControlToEditQend(eq,&ec,ma);
    patch_header(&edsms_hdr);
    patch_input(&edsms_desc);
    CreateInputTextDialog(&edsms_desc,&edsms_hdr,eq,1,(void *)num);
  }
}

typedef struct
{
  void *next;
  int index;
}NUMLIST;

static int gotomenu_onkey(void *data, GUI_MSG *msg)
{
  int i;
  NUMLIST *nltop=MenuGetUserPointer(data);
  if ((msg->gbsmsg->msg==KEY_DOWN)&&(msg->gbsmsg->submess==GREEN_BUTTON))
  {
    msg->keys=0x18;
  }
  if (msg->keys==0x18 || msg->keys==0x3D)
  {
    i=GetCurMenuItem(data);
    for(int d=0; d!=i && nltop; d++) nltop=nltop->next;
    if (nltop)
    {
      VoiceOrSMS(dstr[nltop->index]);
      return (0);
    }
  }
  return(0);
}

static void gotomenu_ghook(void *data, int cmd)
{
  NUMLIST *nltop=MenuGetUserPointer(data);
  if(cmd==3)
  {
    while(nltop)
    {
      NUMLIST *nl=nltop;
      nltop=nltop->next;
      mfree(nl);
    }
  }
}
    

static void gotomenu_itemhandler(void *data, int curitem, void *user_pointer)
{
  WSHDR *ws;
  NUMLIST *nltop=user_pointer;
  void *item=AllocMenuItem(data);
  for(int d=0; d!=curitem && nltop; d++) nltop=nltop->next;
  if (nltop)
  {
    ws=AllocMenuWS(data,40);
    str_2ws(ws,dstr[nltop->index],39);
    SetMenuItemIconArray(data, item, menu_icons+nltop->index);
    SetMenuItemText(data, item, ws, curitem);
//    SetMenuItemIcon(data,curitem,nltop->index);
  }
}

static const HEADER_DESC gotomenu_HDR={0,0,131,21,/*icon*/0,(int)"Select number...",LGP_NULL};

static const MENU_DESC gotomenu_STRUCT=
{
  8,gotomenu_onkey,gotomenu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  1|0x10,
  gotomenu_itemhandler,
  NULL,
  NULL,
  0
};

static int my_ed_onkey(GUI *gui, GUI_MSG *msg)
{
  int key=msg->gbsmsg->submess;
  int m=msg->gbsmsg->msg;
  int r;
  int i=0;
  int n=0;
  
  NUMLIST *nltop=0, *nlbot;
  CLIST *cl=(CLIST *)cltop;
  nlbot=(NUMLIST *)&nltop;
  
  is_sms_need=0;
  if ((key==RIGHT_BUTTON)&&(m==KEY_DOWN))
  {
    EDITCONTROL ec;
    ExtractEditControl(gui,1,&ec);
    if (ec.pWS->wsbody[0]==EDIT_GetCursorPos(gui)-1)
    {
//      ShowMSG(1,(int)"Try to write SMS!");
      is_sms_need=1;
    }
  }
  if (key==GREEN_BUTTON||is_sms_need)
  {
    DisableScroll();
    if (!cl) goto L_OLDKEY;
    while(i!=curpos)
    {
      i++;
      cl=(CLIST *)cl->next;
      if (!cl) goto L_OLDKEY;
    }
    //������ cl ��������� �� ����
    r=0;
    do
    {
      if (cl->num[r])
      {
        nlbot=nlbot->next=malloc(sizeof(NUMLIST));
        nlbot->next=0;
        nlbot->index=r;
        ws_2str(cl->num[r],dstr[r],39);
        n++;
      }
      r++;
    }
    while(r<NUMBERS_MAX);
    if (n==1) //������ ���� �����
    {
      VoiceOrSMS(dstr[nltop->index]);
      mfree(nltop);
      return(1); //������� ���
    }
    if (n==0) goto L_OLDKEY; //��� ������ ���������
    //���������� ������� ������ 1, ������ ����
    patch_header((HEADER_DESC *)&gotomenu_HDR);
    CreateMenu(0,0,&gotomenu_STRUCT,&gotomenu_HDR,0,n,nltop,0);
    return(0);
  }
  if ((key==UP_BUTTON)||(key==DOWN_BUTTON))
  {
    //�� ������������ ���������� �����/����
    msg->keys=0;
    if ((m==KEY_DOWN)||(m==LONG_PRESS))
    {
      DisableScroll();
      if (key==UP_BUTTON)
      {
	if (curpos) curpos--;
      }
      if (key==DOWN_BUTTON)
      {
	if (cl)
	{
	  do
	  {
	    cl=(CLIST *)cl->next;
	    if (!cl) break;
	    i++;
	  }
	  while(i<=curpos);
	}
	curpos=i;
      }
    }
    r=-1; //������������
  }
  else
  {
    #ifdef NEWSGOLD
    if ((key>='0'&&key<='9')||(key=='*')||(key=='#')||(key==RIGHT_SOFT))
    #else
    if ((key>='0'&&key<='9')||(key=='*')||(key=='#')||(key==LEFT_SOFT))
    #endif  
    {
      if (m==KEY_DOWN)
      {
	DisableScroll();
	if (hook_state>=2) //�������� ��������� ������ �����, ��������� �����
	{
	  hook_state=3;
	}
      }
    }
  L_OLDKEY:
    r=old_ed_onkey(gui,msg);
  }
  ChangeRC(gui);
  return(r);
}

static void my_ed_ghook(GUI *gui, int cmd)
{
  static void *methods[16];
  void **m=GetDataOfItemByID(gui,4);
  if ((hook_state==1)/*&&(cmd==7)*/)
  {
    //������� ����� onRedraw
    if (m)
    {
      if (m[1])
      {
	memcpy(methods,m[1],sizeof(methods));
	old_ed_redraw=(void (*)(void *))(methods[0]);
	methods[0]=(void *)my_ed_redraw;
	m[1]=methods;
	hook_state=3; //������ ������
      }
    }
  }
  old_ed_ghook(gui, cmd);
  if (cmd==7)
  {
    EDITCONTROL ec;
    ExtractEditControl(gui,1,&ec);
    //����� ������ ������
    if ((e_ws=ec.pWS)->wsbody[0]<MAX_ESTR_LEN) //�� ����� <MAX_ESTR_LEN
    {
      if (hook_state==3)
      {
	curpos=0;
	hook_state=4;
	SUBPROC((void *)ConstructList);
      }
    }
  }
  ChangeRC(gui);
}

static void DoSplices(GUI *gui)
{
  static INPUTDIA_DESC my_ed;
  memcpy(&my_ed,gui->definition,sizeof(INPUTDIA_DESC));
  old_ed_onkey=my_ed.onKey;
  old_ed_ghook=my_ed.global_hook_proc;
  my_ed.onKey=my_ed_onkey;
  my_ed.global_hook_proc=my_ed_ghook;
  gui->definition=&my_ed;
  scroll_disp=0;
}

static GBSTMR vibra_tmr;

static void vibra_tmr_proc(void)
{
  SetVibration(0);
}

#pragma inline=forced
static int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}

int strcmp_nocase(const char *s1,const char *s2)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))) if (!c) break;
  return(i);
}

static void DrawMyProgress(int y, int cur, int max, const char *color)
{
  int s=cur*127/max;
  DrawRectangle(1,y,130,y+6,0,"\xFF\xFF\xFF\xFF","\0\0\0\0");
  DrawRectangle(2,y+1,s+2,y+5,0,color,color);
}

static int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
#define edialgui_id (((int *)data)[DISPLACE_OF_EDGUI_ID/4])
  int csm_result;
  
  if (msg->msg==MSG_USSD_RX)
  {
    if (ProcessUSSD(data,(GBS_USSD_MSG *)msg)) return 0; //�����������
  }
  if (msg->msg==MSG_END_CALL)
  {
    SendCashReq();
  }
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==edialgui_id)
    {
      hook_state=0;
      e_ws=0;
      FreeCLIST();
      DisableScroll();
    }
  }
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"CallCenter config updated!");
      InitConfig();
      LoadCash();
    }
  }
  #ifdef NEWSGOLD
  if ((msg->msg==MSG_STATE_OF_CALL)&&(msg->submess==1)&&((int)msg->data0==2)&&(ENA_VIBRA))
  #else
  if ((msg->msg==MSG_STATE_OF_CALL)&&(msg->submess==1)&&((int)msg->data0==0)&&(ENA_VIBRA))
  #endif   
  {
    SetVibration(vibraPower);
    GBS_StartTimerProc(&vibra_tmr,vibraDuration*216/1000,vibra_tmr_proc);
  }
  csm_result=old_icsm_onMessage(data,msg); //�������� ������ ���������� �������
  if (IsGuiOnTop(idlegui_id)) //���� IdleGui �� ����� �����
  {
    GUI *igui = GetTopGUI();
    if (igui) //� �� ����������
    {
      extern int CASH_SIZE;               //by BoBa 4.07.07
      void *canvasdata = BuildCanvas();
      DrawCanvas(canvasdata, 1, IDLE_Y, 130, IDLE_Y+8*CASH_SIZE-1, 1);
      extern const char * const patterns[];
      int n=0; //�����                        
      do {
//       if (!*patterns[n]) break; //������ ��������� ���
       DrawMyProgress(IDLE_Y+n*8,CurrentCASH[n],MaxCASH[n],progress_colors[n]);
//       DrawMyProgress(95+0*8,CurrentCASH[1],MaxCASH[1],"\xFF\x00\x00\x64");
//       DrawMyProgress(95+1*8,CurrentCASH[1],MaxCASH[1],"\x00\xFF\x00\x64");
//       DrawMyProgress(95+2*8,CurrentCASH[2],MaxCASH[2],"\x00\xFF\xFF\x64");
//       DrawMyProgress(95+3*8,CurrentCASH[3],MaxCASH[3],"\xFF\xFF\x00\x64");
       n++;
      } while(n<CASH_SIZE);
       
    }
  }
  if (IsGuiOnTop(edialgui_id)) //���� EDialGui �� ����� �����
  {
    GUI *igui=GetTopGUI();
    if (igui) //� �� ���������� (� �� � ������� ;))
    {
      if (!hook_state)
      {
	//�� ���� �������
	DoSplices(igui);
	hook_state=1;
      }
    }
  }
  return(csm_result);
}

void CallCenter_Destructor(void)
{
  SaveCash();
  EndUSSDtimer();
//  FreeWS(dbg_ws);
  FreeWS(ews);
}

static void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  CallCenter_Destructor();
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

int main(void)
{
  InitConfig();
  LoadCash();
  LockSched();
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  old_icsm_onClose=icsmd.onClose;
  icsmd.onClose=MyIDLECSM_onClose;
  icsmd.onMessage=MyIDLECSM_onMessage;
  icsm->constr=&icsmd;
  UnlockSched();
  ews=AllocWS(1024);
//  dbg_ws=AllocWS(256);
  InitIcons();
  return 0;
}
