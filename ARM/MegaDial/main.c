#include "..\swilib\swilib.h"

#define MAX_RECORDS 5000

#define LEVEL1_RN	(41*41)
#define LEVEL2_RN	(41)

#define wslen(ARG) (ARG->wsbody[0])

CSM_DESC icsmd;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);

int (*old_ed_onkey)(GUI *gui, GUI_MSG *msg);
void (*old_ed_ghook)(GUI *gui, int cmd);
void (*old_ed_redraw)(void *data);

//0 - ���� ��������� �������
//1 - ������ ��������, ����������, ���� ���������� �� onRedraw
//2 - �������� onRedraw, idle-�����
//3 - ������ ������
//4 - ��������� ������
//5 - ������������ �����
volatile int hook_state=0;

volatile WSHDR *e_ws;

volatile int curpos; //������� ������� � ������

typedef struct
{
  void *next;
  WSHDR *name;
  WSHDR *num[4];
}CLIST;

volatile CLIST *cltop; //������
volatile CLIST *clbot; //�����

char dstr[4][40];

//���������� ������
void FreeCLIST(void)
{
  LockSched();
  CLIST *cl=(CLIST*)cltop;
  cltop=0;
  clbot=0;
  UnlockSched();
  while(cl)
  {
    CLIST *p;
    FreeWS(cl->name);
    FreeWS(cl->num[0]);
    FreeWS(cl->num[1]);
    FreeWS(cl->num[2]);
    FreeWS(cl->num[3]);
    p=cl;
    cl=(CLIST*)(cl->next);
    mfree(p);
  }
}

//-----------------------------------------------------
//����� ��������� � ������ �� ������ �9
//-----------------------------------------------------
int CompareStrT9(WSHDR *ws, WSHDR *ss)
{
  int spos=1;
  int wpos=1;
  int c;

  //������� ������ ��� ������ ������
  static const char key[256]=
    "11111111111111111111111111111111"
      "10001**0***0000*012345678900***0"
	"0222333444555666777788899991*110"
	  "122233344455566677778889999111*1"
	    "11111111111111111111111111111111"
	      "11111111111111111111111111111111"
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
      c&=0xFF;
      c=key[c];
      if (c==ss->wsbody[spos])
      {
	spos++;
	if (spos>wslen(ss)) return(1); //��� �������
      }
      else spos=0; //���� ����� �����
    }
    wpos++;
  }
  while(wpos<=wslen(ws));
  return(0);
}

//=====================================================
// ����������� ������
//=====================================================
void ConstructList(void)
{
  int fin;
  unsigned int ul;
  char recname[128];

  AB_UNPRES ur;
  void *buffer;

#pragma pack(1)
  struct
  {
    long dummy1;
    short dummy2;
    char bitmap[MAX_RECORDS/8];
  } ABmain;
#pragma pack()

  int rec=0;
  int fsz;
  int total=0;
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
    if ((fin=fopen("0:\\System\\apo\\addr\\main",A_ReadOnly+A_BIN,0,&ul))!=-1)
    {
      if (fread(fin,&ABmain,sizeof(ABmain),&ul)==sizeof(ABmain))
      {
	fclose(fin,&ul);
	do
	{
	  if (ABmain.bitmap[rec>>3]&(0x80>>(rec&7)))
	  {
	    //������ ���� � �������
	    unsigned int rl1;
	    unsigned int rl2;
	    unsigned int rl3;
	    rl1=rec/LEVEL1_RN;
	    rl2=(rec%LEVEL1_RN)/LEVEL2_RN;
	    rl3=rec%LEVEL2_RN;
	    snprintf(recname,128,"0:\\System\\apo\\addr\\data\\%02d\\%02d\\%02d",rl1,rl2,rl3);
	    if ((fin=fopen(recname,A_ReadOnly+A_BIN,0,&ul))!=-1)
	    {
	      zeromem(&ur,sizeof(ur));
	      fsz=fread(fin,buffer,65536,&ul);
	      fclose(fin,&ul);
	      UnpackABentry(&ur,((char *)buffer+8),fsz,0x28);
	      int i=0;
	      zeromem(&contact,sizeof(contact));
	      while(i<ur.number_of_records)
	      {
		AB_UNPRES_ITEM *r=ur.record_list+i;
		if (r->no_data!=1)
		{
		  switch(GetTypeOfAB_UNPRES_ITEM(r->item_type))
		  {
		  case 0x05:
		    if (r->item_type==0x81)
		    {
		      if (r->data)
		      {
			wstrcpy(contact.name=AllocWS(50),(WSHDR *)(r->data));
			*((int *)(&contact.next))|=CompareStrT9(contact.name,sws);
		      }
		    }
		    break;
		  case 0x01:
		    {
		      PKT_NUM *p=(PKT_NUM*)r->data;
		      unsigned int n=r->item_type;
		      int j;
		      int c;
		      WSHDR *ws;
		      n-=0x62;
		      if (n<4)
		      {
			if (p)
			{
			  ws=contact.num[n]=AllocWS(50);
			  j=0;
			  if (p->format==0x91) wsAppendChar(ws,'+');
			  while(j<p->data_size)
			  {
			    c=(p->data[j])&0x0F;
			    if (c!=0x0F) wsAppendChar(ws,c+'0'); else break;
			    c=(p->data[j]>>4)&0x0F;
			    if (c!=0x0F) wsAppendChar(ws,c+'0'); else break;
			    j++;
			  }
			  *((int *)(&contact.next))|=CompareStrT9(ws,sws);
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
		FreeWS(contact.num[0]);
		FreeWS(contact.num[1]);
		FreeWS(contact.num[2]);
		FreeWS(contact.num[3]);
	      }
	      FreeUnpackABentry(&ur,mfree_adr());
	      if (hook_state!=5) goto L_STOP;
	      LockSched();
	      if ((hook_state==5)&&(contact.next))
	      {
		//��������� � ������
		CLIST *p=malloc(sizeof(contact));
		CLIST *b=(CLIST *)clbot;
		contact.next=0;
		memcpy(p,&contact,sizeof(contact));
		if (b)
		{
		  //�� ������
		  b->next=p;
		  clbot=p;
		}
		else
		{
		  //������
		  cltop=p;
		  clbot=p;
		}
		if (curpos<2)
		{
		  if (total<5) REDRAW();
		}
		else
		{
		  if ((unsigned int)(total-(curpos-2))<5) REDRAW();
		}
		total++;
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
    FreeWS(contact.num[0]);
    FreeWS(contact.num[1]);
    FreeWS(contact.num[2]);
    FreeWS(contact.num[3]);
  }
  LockSched();
  if (hook_state==5) hook_state=2; else FreeCLIST();
  UnlockSched();
  FreeWS(sws);
}

#pragma optimize=no_inline
void f_dummy(void){}

void my_ed_redraw(void *data)
{
  //  WSHDR *ews=(WSHDR*)e_ws;
  int i=curpos-2;
  int cp;
  CLIST *cl=(CLIST *)cltop;
  old_ed_redraw(data);

  if (!cl) return;

  if (!e_ws) return;

  if (e_ws->wsbody[0]<13) //�� ����� <13
  {

    DrawRoundedFrame(1,63,130,152,0,0,0,GetPaletteAdrByColorIndex(1),GetPaletteAdrByColorIndex(7));

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
      if (!cl) break;
      if (i!=cp)
      {
	DrawString(cl->name,3,67+(i*17),128,67+13+(i*17),5,0x80,GetPaletteAdrByColorIndex(1),GetPaletteAdrByColorIndex(23));
      }
      else
      {
	DrawRoundedFrame(2,65+(i*17),129,65+17+(i*17),0,0,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(3));
	DrawString(cl->name,3,67+(i*17),128,67+13+(i*17),5,0x80,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      }
      cl=(CLIST *)cl->next;
      i++;
    }
    while(i<5);
  }
}

void ChangeRC(GUI *gui)
{
  static const RECT rc={6,40,126,100};
  if (e_ws)
  {
    if (e_ws->wsbody[0]>12) return;
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

void goto_1(void)
{
  MakeVoiceCall(dstr[0],0x10,0x20C0);
}

void goto_2(void)
{
  MakeVoiceCall(dstr[1],0x10,0x20C0);
}

void goto_3(void)
{
  MakeVoiceCall(dstr[2],0x10,0x20C0);
}

void goto_4(void)
{
  MakeVoiceCall(dstr[3],0x10,0x20C0);
}

int menusoftkeys[]={0,1,2};

SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Select"},
  {0x0001,0x0000,(int)"Back"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

void *gotomenu_HNDLS[4]=
{
  (void *)goto_1,
  (void *)goto_2,
  (void *)goto_3,
  (void *)goto_4
};

MENUITEM_DESC gotomenu_ITEMS[9]=
{
  {NULL,(int)dstr[0],0x7FFFFFFF,0,NULL,0,0x59D},
  {NULL,(int)dstr[1],0x7FFFFFFF,0,NULL,0,0x59D},
  {NULL,(int)dstr[2],0x7FFFFFFF,0,NULL,0,0x59D},
  {NULL,(int)dstr[3],0x7FFFFFFF,0,NULL,0,0x59D},
};

HEADER_DESC gotomenu_HDR={0,0,131,21,/*icon*/0,(int)"Select number...",0x7FFFFFFF};

MENU_DESC gotomenu_STRUCT=
{
  8,NULL,NULL,NULL,
  menusoftkeys,
  &menu_skt,
  0,
  NULL,
  gotomenu_ITEMS,
  gotomenu_HNDLS,
  4
};

int my_ed_onkey(GUI *gui, GUI_MSG *msg)
{
  int key=msg->gbsmsg->submess;
  int m=msg->gbsmsg->msg;
  int r;
  int i=0;

  CLIST *cl=(CLIST *)cltop;

  if (key==GREEN_BUTTON)
  {
    if (!cl) goto L_OLDKEY;
    while(i!=curpos)
    {
      i++;
      cl=(CLIST *)cl->next;
      if (!cl) goto L_OLDKEY;
    }
    //������ cl ��������� �� ����
    i=0;
    r=0;
    do
    {
      if (cl->num[r]) {ws_2str(cl->num[r],dstr[i],39);i++;}
      r++;
    }
    while(r<4);
    if (i==1)
    {
      MakeVoiceCall(dstr[0],0x10,0x20C0);
      return(1); //������� ���
    }
    if (!i) goto L_OLDKEY;
    //���������� ������� ������ 1, ������ ����
    gotomenu_STRUCT.n_items=i;
    CreateMenu(0,0,&gotomenu_STRUCT,&gotomenu_HDR,0,i,0,0);
    return(0);
  }
  if ((key==UP_BUTTON)||(key==DOWN_BUTTON))
  {
    //�� ������������ ���������� �����/����
    msg->keys=0;
    if (m==KEY_DOWN)
    {
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
    if ((key>='0'&&key<='9')||(key=='*')||(key=='#')||(key==RIGHT_SOFT))
    {
      if (m==KEY_DOWN)
      {
	if (hook_state>=2) //�������� ��������� ������ �����, ��������� �����
	{
	  hook_state=3;
	  FreeCLIST();
	}
      }
    }
  L_OLDKEY:
    r=old_ed_onkey(gui,msg);
  }
  ChangeRC(gui);
  return(r);
}

void my_ed_ghook(GUI *gui, int cmd)
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
    if ((e_ws=ec.pWS)->wsbody[0]<13) //�� ����� <13
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

void DoSplices(GUI *gui)
{
  static INPUTDIA_DESC my_ed;
  memcpy(&my_ed,gui->definition,sizeof(INPUTDIA_DESC));
  old_ed_onkey=my_ed.onKey;
  old_ed_ghook=my_ed.global_hook_proc;
  my_ed.onKey=my_ed_onkey;
  my_ed.global_hook_proc=my_ed_ghook;
  gui->definition=&my_ed;
}

int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
#define edialgui_id (((int *)data)[DISPLACE_OF_EDGUI_ID/4])
  int csm_result;

  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==edialgui_id)
    {
      hook_state=0;
      e_ws=0;
      FreeCLIST();
    }
  }
  csm_result=old_icsm_onMessage(data,msg); //�������� ������ ���������� �������
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

int main(void)
{
  LockSched();
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  icsmd.onMessage=MyIDLECSM_onMessage;
  icsm->constr=&icsmd;
  UnlockSched();
  return 0;
}
