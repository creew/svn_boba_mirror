#include "..\..\inc\swilib.h"
#include "image.h"
#include "..\lgp.h"

#define fon 1
#define st_off 2
#define st_on 3
#define wd_off 4
#define wd_on 5
#define logo 6

#ifdef NEWSGOLD
#define DEFAULT_DISK "4"
#else
#define DEFAULT_DISK "0"
#endif

unsigned int hour[6];
unsigned int min[6];
unsigned int weekdays[6][8];
unsigned int num_alarms;
unsigned int backup[3];
unsigned int ch[3];
unsigned int max;
unsigned int mode=1;
unsigned int X;
unsigned int Y;
unsigned int show_icon;

GBSTMR mytmr;
unsigned int input;

unsigned int status[6];
unsigned int num_alarm=1;
unsigned int edit_level=1;
unsigned int ch[3];
unsigned int set=1;
int lng;
char cfgfile[]=DEFAULT_DISK":\\zbin\\alarm\\alarm.cfg";

int scr_w;
int scr_h;
int font_size;

typedef struct
{
  char w;
  char h;
  unsigned short bpnum;
  char *bitmap;
}IMGHDR_;

const int minus11=-11;
unsigned short maincsm_name_body[140];
unsigned int MAINCSM_ID = 0;
unsigned int MAINGUI_ID = 0;
char colors[4][4]={{0xFF,0,0,0x64},{0,0xFF,0,0x64},{0,0,0xFF,0x64},{0xC6,0xAA,0xAF,0x32}};
const char Pointer[5]={0x27,0x27,0xFF,0x27,0x27};
const IMGHDR_ imgPointer = {5,5,0x1,(char *)Pointer};
GBSTMR mytmr;

WSHDR *ws;

typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  GUI gui;
  WSHDR *ws1;
  WSHDR *ws2;
  int i1;
}MAIN_GUI;

typedef struct
{
  char signature[16];
  unsigned short picnum;
  unsigned short unk1;
  char w; 
  char h; 
  char Compr_Bits;  
}PICHDR;

void DrwStr(WSHDR *ws, int x, int y,int w,int h, int font,int flag,char *pen, char *brush)
{
  RECT rc;
  DRWOBJ drwobj;
  StoreXYWHtoRECT(&rc,x,y,w,h);
  SetPropTo_Obj1(&drwobj,&rc,0,ws,font,flag);
  SetColor(&drwobj,pen,brush);
  DrawObject(&drwobj);
}

DrwImg(IMGHDR *img, int x, int y)
{
  RECT rc;
  DRWOBJ drwobj;
  StoreXYWHtoRECT(&rc,x,y,img->w,img->h);
  SetPropTo_Obj5(&drwobj,&rc,0,img);
  SetColor(&drwobj,GetPaletteAdrByColorIndex(1),GetPaletteAdrByColorIndex(0));
  DrawObject(&drwobj);
}
///////////////////////////////////////////////////////////////////////////////
void DrawGPF(char *fname, int x, int y)
{
  volatile int hFile;
  PICHDR Pic_Header;
  unsigned int io_error = 0;
  hFile = fopen(fname, A_ReadOnly + A_BIN, P_READ, &io_error);
  if(!io_error)
  {
    fread(hFile, &Pic_Header, sizeof(Pic_Header), &io_error);
    unsigned int buf_size = lseek(hFile, 0, S_END, &io_error, &io_error) - sizeof(PICHDR);
    lseek(hFile, sizeof(PICHDR), S_SET, &io_error, &io_error); 
    char *pic_buffer = malloc((buf_size+3)&(~3));
    fread(hFile, pic_buffer, buf_size, &io_error);    
    fclose(hFile, &io_error);
    IMGHDR img;
    img.w = Pic_Header.w;
    img.h = Pic_Header.h;
    img.bpnum = Pic_Header.Compr_Bits;
    //img.zero = 0;
    img.bitmap = pic_buffer;
    DrwImg(&img, x, y);
    mfree(pic_buffer);
  }
}

void draw_pic(int num,int x, int y)
{
  switch(num)
  {
  case fon:
    {
      DrawGPF(DEFAULT_DISK":\\zbin\\alarm\\fon.gpf",0,0);
    } break;
  case st_off:
    {
      IMGHDR img;
      img.w = 52;
      img.h = 52;
      img.bpnum = 0x88;
      //img.zero = 0;
      img.bitmap = _st_off;
      DrwImg(&img,x,y);
    } break;
  case st_on:
    {
      IMGHDR img;
      img.w = 52;
      img.h = 52;
      img.bpnum = 0x88;
      //img.zero = 0;
      img.bitmap = _st_on;
      DrwImg(&img,x,y);
    } break;
  case wd_off:
    {
      IMGHDR img;
      img.w = 14;
      img.h = 14;
      img.bpnum = 0x85;
      //img.zero = 0;
      img.bitmap = _wd_off;
      DrwImg(&img,x,y);
    } break;
  case wd_on:
    {
      IMGHDR img;
      img.w = 14;
      img.h = 14;
      img.bpnum = 0x85;
      //img.zero = 0;
      img.bitmap = _wd_on;
      DrwImg(&img,x,y);
    } break;
  case logo:
    {
      IMGHDR img;
      img.w = 26;
      img.h = 14;
      img.bpnum = 0x85;
      //img.zero = 0;
      img.bitmap = _logo;
      DrwImg(&img,x,y);
    } break;
  }
}

void load_settings(void)
{
  unsigned int err;
  int handle=fopen(cfgfile, A_ReadOnly, P_READ,&err);
  if(handle!=-1)
  {
    char *data=malloc(128);
    if(data)
    {
      fread(handle,data,128,&err);
      
status[1]=data[2];
hour[1]=data[3];
min[1]=data[4];
weekdays[1][1]=data[5];
weekdays[1][2]=data[6];
weekdays[1][3]=data[7];
weekdays[1][4]=data[8];
weekdays[1][5]=data[9];
weekdays[1][6]=data[10];
weekdays[1][7]=data[11];
status[2]=data[12];
hour[2]=data[13];
min[2]=data[14];
weekdays[2][1]=data[15];
weekdays[2][2]=data[16];
weekdays[2][3]=data[17];
weekdays[2][4]=data[18];
weekdays[2][5]=data[19];
weekdays[2][6]=data[20];
weekdays[2][7]=data[21];
status[3]=data[22];
hour[3]=data[23];
min[3]=data[24];
weekdays[3][1]=data[25];
weekdays[3][2]=data[26];
weekdays[3][3]=data[27];
weekdays[3][4]=data[28];
weekdays[3][5]=data[29];
weekdays[3][6]=data[30];
weekdays[3][7]=data[31];
status[4]=data[32];
hour[4]=data[33];
min[4]=data[34];
weekdays[4][1]=data[35];
weekdays[4][2]=data[36];
weekdays[4][3]=data[37];
weekdays[4][4]=data[38];
weekdays[4][5]=data[39];
weekdays[4][6]=data[40];
weekdays[4][7]=data[41];
status[5]=data[42];
hour[5]=data[43];
min[5]=data[44];
weekdays[5][1]=data[45];
weekdays[5][2]=data[46];
weekdays[5][3]=data[47];
weekdays[5][4]=data[48];
weekdays[5][5]=data[49];
weekdays[5][6]=data[50];
weekdays[5][7]=data[51];
//skin=data[52];
show_icon=data[53];
X=data[54];
Y=data[55];

      mfree(data);
    }
  }
  fclose(handle,&err);
}

void save_settings(void)
{
  unsigned int err; 
  int handle=fopen(cfgfile,A_WriteOnly+A_Create,P_WRITE,&err);
  if(handle!=-1)
  {
    char *data=malloc(128);
    if(data!=0)
    {
      
//data[0]=10;
//data[1]=1;
data[2]=status[1];
data[3]=hour[1];
data[4]=min[1];
data[5]=weekdays[1][1];
data[6]=weekdays[1][2];
data[7]=weekdays[1][3];
data[8]=weekdays[1][4];
data[9]=weekdays[1][5];
data[10]=weekdays[1][6];
data[11]=weekdays[1][7];
data[12]=status[2];
data[13]=hour[2];
data[14]=min[2];
data[15]=weekdays[2][1];
data[16]=weekdays[2][2];
data[17]=weekdays[2][3];
data[18]=weekdays[2][4];
data[19]=weekdays[2][5];
data[20]=weekdays[2][6];
data[21]=weekdays[2][7];
data[22]=status[3];
data[23]=hour[3];
data[24]=min[3];
data[25]=weekdays[3][1];
data[26]=weekdays[3][2];
data[27]=weekdays[3][3];
data[28]=weekdays[3][4];
data[29]=weekdays[3][5];
data[30]=weekdays[3][6];
data[31]=weekdays[3][7];
data[32]=status[4];
data[33]=hour[4];
data[34]=min[4];
data[35]=weekdays[4][1];
data[36]=weekdays[4][2];
data[37]=weekdays[4][3];
data[38]=weekdays[4][4];
data[39]=weekdays[4][5];
data[40]=weekdays[4][6];
data[41]=weekdays[4][7];
data[42]=status[5];
data[43]=hour[5];
data[44]=min[5];
data[45]=weekdays[5][1];
data[46]=weekdays[5][2];
data[47]=weekdays[5][3];
data[48]=weekdays[5][4];
data[49]=weekdays[5][5];
data[50]=weekdays[5][6];
data[51]=weekdays[5][7];
//data[52]=1;
data[53]=show_icon;
data[54]=X;
data[55]=Y;

      fwrite(handle,data,56,&err);
      mfree(data);
    }
    fclose(handle,&err);
  }
}

void edit()
{
  if (input)
  {
    switch(set)
    {
    case 1: if(ch[1])
        hour[num_alarm]=backup[1];
    case 2: if(ch[2])
      min[num_alarm]=backup[2];
    }
    }
}

void status_on_off()
{
  if (status[num_alarm]==1) status[num_alarm]=0; else status[num_alarm]=1;
}

void weekday_on_off()
{
  if (weekdays[num_alarm][set]==1) weekdays[num_alarm][set]=0; else weekdays[num_alarm][set]=1;
}


void OnRedraw()
{
  switch(mode)
  {
  case 1:
    {
      DrawRoundedFrame(0,0,scr_w,scr_h,0,0,0,GetPaletteAdrByColorIndex(2),GetPaletteAdrByColorIndex(2));
      draw_pic(fon,0,0);
      draw_pic(logo,2,2);
      
      wsprintf(ws, "%t",alarm_name);
      DrwStr(ws,30,3,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      
      wsprintf(ws, "%t",change);
      DrwStr(ws,8,scr_h-font_size-3,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      
      wsprintf(ws, "%t",save);
      DrwStr(ws,scr_w/1.5,scr_h-font_size-3,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      
      char *stat=malloc(16);
      int tmp=scr_h/7.3;
      for (int i=1;i<6;i++)
      {
        if (status[i]) strcpy(stat,on);
          else strcpy(stat,off);
        wsprintf(ws, "%t %d: %d:%02d %t",alarm_name,i,hour[i],min[i],stat);
        if (num_alarm==i) DrwStr(ws,5,1+tmp*i,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(3));
          else DrwStr(ws,5,1+tmp*i,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      }
      mfree(stat);
    } break;

  case 2:
    {
      DrawRoundedFrame(0,0,scr_w,scr_h,0,0,0,GetPaletteAdrByColorIndex(2),GetPaletteAdrByColorIndex(2));
      draw_pic(fon,0,0);
      draw_pic(logo,2,2);
      
      wsprintf(ws, "%t %d",alarm_name,num_alarm);
      DrwStr(ws,30,3,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      
      //////////////////////////////////  SL65  ////////////////////////////////
#ifdef SL65
      if(status[num_alarm]==1) draw_pic(st_on,5,HeaderH());
        else draw_pic(st_off,5,HeaderH());
#else
      //////////////////////////////////////////////////////////////////////////
      if(status[num_alarm]==1) draw_pic(st_on,scr_w/2-25,HeaderH());
        else draw_pic(st_off,scr_w/2-25,HeaderH());
#endif
      if ((edit_level==1)||(edit_level==3))
        {
          wsprintf(ws,"%t",change);
          DrwStr(ws,8,scr_h-font_size-3,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
        }
      
      wsprintf(ws, "%t",ok);
      DrwStr(ws,scr_w/1.5,scr_h-font_size-3,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      
      int a=scr_w/2-GetSymbolWidth('n',FONT_SMALL)*2;
      if (status[num_alarm]) wsprintf(ws, "%t",on);
          else 
          {
            wsprintf(ws, "%t",off);
            a-=3;
          }
      //////////////////////////////////  SL65  ////////////////////////////////
#ifdef SL65
      if (edit_level==1) DrwStr(ws,67,HeaderH()+18,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(3));
        else DrwStr(ws,67,HeaderH()+18,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
#else
      //////////////////////////////////////////////////////////////////////////
      if (edit_level==1) DrwStr(ws,a,HeaderH()+57,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(3));
        else DrwStr(ws,a,HeaderH()+57,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
#endif
      
      //////////////////////////////////  SL65  ////////////////////////////////
#ifdef SL65
      a=67;
        int b=HeaderH()+21+font_size;
      wsprintf(ws, "%02d",hour[num_alarm]);
      if ((edit_level==2)&&(set==1))
        DrwStr(ws,a,b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(3));
        else DrwStr(ws,a,b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
        
      a+=(GetSymbolWidth((backup[1]/10)+'0',FONT_SMALL)+GetSymbolWidth((backup[1]%10)+'0',FONT_SMALL));
      wsprintf(ws, ":");
      DrwStr(ws,a,b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      
      a+=GetSymbolWidth(':',FONT_SMALL);
      wsprintf(ws, "%02d",min[num_alarm]);
      if ((edit_level==2)&&(set==2))
        DrwStr(ws,a,b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(3));
        else DrwStr(ws,a,b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));      
#else
      //////////////////////////////////////////////////////////////////////////
      a=scr_w/2-GetSymbolWidth((backup[1]/10)+'0',FONT_SMALL)-GetSymbolWidth((backup[1]%10)+'0',FONT_SMALL);      
        int b=HeaderH()+60+font_size;
      wsprintf(ws, "%02d",hour[num_alarm]);
      if ((edit_level==2)&&(set==1))
        DrwStr(ws,a,b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(3));
        else DrwStr(ws,a,b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
        
      a=scr_w/2;
      wsprintf(ws, ":");
      DrwStr(ws,a,b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      
      a+=GetSymbolWidth(':',FONT_SMALL);
      wsprintf(ws, "%02d",min[num_alarm]);
      if ((edit_level==2)&&(set==2))
        DrwStr(ws,a,b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(3));
        else DrwStr(ws,a,b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
#endif
      a=scr_w/7.3;
      b=scr_h-SoftkeyH()-font_size-24;
      for (int i=1;i<8;i++)
      {
        wsprintf(ws, "%t",wd[i]);
        if ((edit_level==3)&&(set==i)) DrwStr(ws,4+a*(i-1),b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(3));
          else DrwStr(ws,4+a*(i-1),b,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      }
      b+=(font_size+3);
      for (int i=1;i<8;i++)
      {
        if (weekdays[num_alarm][i]) draw_pic(wd_on,5+a*(i-1),b);
          else draw_pic(wd_off,5+a*(i-1),b);
      }
    } break;
  case 3:
    {
      DrawRoundedFrame(0,0,scr_w,scr_h,0,0,0,
                   GetPaletteAdrByColorIndex(0),
                   GetPaletteAdrByColorIndex(0));
      for (int y_0=0; y_0< scr_h;y_0+=10)
        {
          DrawLine(0,y_0,scr_w,y_0,1,colors[3]);
        }
      for (int x_0=0; x_0<scr_w;x_0+=10)
        {
          DrawLine(x_0,0,x_0,scr_h,1,colors[3]);
        }
      DrwImg((IMGHDR *)&imgPointer,X-2,Y-2);
      
      wsprintf(ws, "%03d,%03d", X, Y);
      DrwStr(ws,scr_w/1.5,scr_h-font_size-3,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(1),GetPaletteAdrByColorIndex(23));
      
      if (show_icon) wsprintf(ws, "%t",on);
        else wsprintf(ws, "%t",off);
      DrwStr(ws,8,scr_h-font_size-3,scr_w,scr_h,FONT_SMALL,1,GetPaletteAdrByColorIndex(1),GetPaletteAdrByColorIndex(23));
    }
  }
}

void onCreate(MAIN_GUI *data, void *(*malloc_adr)(int))
{
  ws = AllocWS(128);
  data->gui.state=1;
}

void onClose(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  FreeWS(ws);
  data->gui.state=0;
}

void onFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  DisableIDLETMR();
  data->gui.state=2;
}

void onUnfocus(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}

void open_bcfg()
{
  char *elf=DEFAULT_DISK":\\Zbin\\alarm\\alarm_melody.bcfg";
  WSHDR *_elf=AllocWS(256);
  wsprintf(_elf,"%s",elf);
  ExecuteFile(_elf,0,0);
  FreeWS(_elf);
}

int onkey(unsigned char keycode, int pressed)
{
  switch(mode)
  {
    case 1:
      {
        switch(pressed)
          {
          case KEY_UP: break;
          case LONG_PRESS:
          case KEY_DOWN:
            switch(keycode)
            {
            case RED_BUTTON: return(1);
            case LEFT_SOFT:  mode=2; OnRedraw(); break;
            case RIGHT_SOFT:
              {
                save_settings();
                GBS_SendMessage(MMI_CEPID, MSG_RECONFIGURE_REQ,0,"alarm");
                return(1);
              }
            case UP_BUTTON:
            case LEFT_BUTTON:
              {
                if (num_alarm>1) num_alarm--;
                  else num_alarm=5;
                break;
              }
            case RIGHT_BUTTON:
            case DOWN_BUTTON:
              {
                if (num_alarm<5) num_alarm++;
                  else num_alarm=1;
                  break;
              }
            case GREEN_BUTTON: open_bcfg(); break;
            case '1': num_alarm=1; break;
            case '2': num_alarm=2; break;
            case '3': num_alarm=3; break;
            case '4': num_alarm=4; break;
            case '5': num_alarm=5; break;
            case '#': mode=3; break;
            case '*': ShowMSG(1,(int)"Alarm cfg editor\n(c)Geka"); break;
            }
          }
      } break;
  case 2:
    {
      switch(pressed)
      {
      case KEY_UP: break;
      case LONG_PRESS:
      case KEY_DOWN:
        switch(keycode)
        {
        case RED_BUTTON:  mode=1; break;
        case RIGHT_SOFT:  mode=1; OnRedraw(); break;
        case LEFT_SOFT:
          {
            if (edit_level==1) status_on_off();
            if (edit_level==3)
            {
              weekday_on_off();
              if (set<7) set++;
               else set=1;
            }
            break;
          }
          case UP_BUTTON:
            {
              if (edit_level>1) edit_level--;
                else edit_level=3;
              set=1;
              input=1;
              edit();
              break;
            }
        case DOWN_BUTTON:
          {
            if (edit_level<3) edit_level++;
              else edit_level=1;
            set=1;
            input=1;
            edit();
            break;
          }
        case LEFT_BUTTON:
          {
            if(edit_level==1) status_on_off();
            if(edit_level==2)
            {
              if (set>0) set--;
                else set=2;
              lng=0;
            }
            if(edit_level==3)
            {
              if (set>1) set--;
                else set=7;
            }
            break;
          }
        case RIGHT_BUTTON:
          {
            if(edit_level==1) status_on_off();
            if(edit_level==2)
            {
              if (set<2) set++;
                else set=1;
              lng=0;
            }
            if(edit_level==3)
            {
              if (set<7) set++;
                else set=1;
            }
            break;
          }
        case ENTER_BUTTON:
          {
            if(edit_level==3)
            {
              weekday_on_off();
              if (set<7) set++;
                else set=1;
            }
            break;
          }
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
          {
            if(edit_level==2)
            {
              ++lng;
              if (set==1) max=3;
                else max=6;
              if (backup[set]<max) backup[set]=backup[set]*10+keycode-'0';
                else
                {
                  backup[set]=keycode-'0';
                  lng=1;
                }
                ch[set]=1;
                input=1;
                edit();
                if (lng==2) {if(set==1) {set=2; lng=0;} else {edit_level=1; lng=1;}}
            }
            if(edit_level==3)
            {
              int key=keycode-'0';
              if (key<8&&key>0) set=key;
            }
            break;
          }
        }
      }
    } break;
  case 3:
    {
      switch(pressed)
      {
      case KEY_UP: break;
      case LONG_PRESS:
        switch(keycode)
          {
          case LEFT_BUTTON: case '4': if (X>0) X-=5; else X=132; break;
          case RIGHT_BUTTON: case '6': if (X<132) X+=5; else X=0; break;
          case UP_BUTTON: case '2': if (Y>0) Y-=5; else Y=176; break;
          case DOWN_BUTTON: case '8': if (Y<176) Y+=5; else Y=0; break;
          }
      case KEY_DOWN:
        switch(keycode)
        {
        case RED_BUTTON:  mode=1; break;
        case LEFT_SOFT: if (show_icon==1) show_icon=0; else show_icon=1; break;
        case LEFT_BUTTON: case '4': if (X!=0) X=X-1; else X=132; break;
        case RIGHT_BUTTON: case '6': if (X!=132) X=X+1; else X=0; break;
        case UP_BUTTON: case '2': if (Y!=0) Y=Y-1; else Y=176; break;
        case DOWN_BUTTON: case '8': if (Y!=176) Y=Y+1; else Y=0; break;
        default: mode=1; break;
        }
      }
    } break;
  }
  DirectRedrawGUI();
  return(0);
}

int OnKey(MAIN_GUI *data, GUI_MSG *msg)
{
  return onkey(msg->gbsmsg->submess, msg->gbsmsg->msg);
}

  extern void kill_data(void *p, void (*func_p)(void *));
#ifdef NEWSGOLD
void onDestroy(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  kill_data(data,mfree_adr);
}
#endif

int method8(void){return(0);}
int method9(void){return(0);}
const void * const gui_methods[11]={
  (void *)OnRedraw,
  (void *)onCreate,
  (void *)onClose,
  (void *)onFocus,
  (void *)onUnfocus,
  (void *)OnKey,
  0,
  #ifdef NEWSGOLD
  (void *)onDestroy,
  #else
  (void *)kill_data,
  #endif
  (void *)method8,
  (void *)method9,
  0
};

const RECT Canvas={0,0,129,129};
void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  main_gui->gui.canvas=(void *)(&Canvas);
  main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=CreateGUI(main_gui);
  MAINGUI_ID=csm->gui_id;
}

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

void maincsm_onclose(CSM_RAM *csm)
{
  SUBPROC((void *)ElfKiller);
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if ((msg->msg==MSG_GUI_DESTROYED)&&((int)msg->data0==csm->gui_id))
  {
    csm->csm.state=-3;
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
  WSHDR *ws=AllocWS(256);
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"Alarm cfg editor");
  FreeWS(ws);
}

int main(void)
{
  font_size=GetFontYSIZE(FONT_SMALL);
  load_settings();
  scr_w=ScreenW()-1;
  scr_h=ScreenH()-1;
  LockSched();
  char dummy[sizeof(MAIN_CSM)];
  UpdateCSMname();
  MAINCSM_ID = CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
  return 0;
}
