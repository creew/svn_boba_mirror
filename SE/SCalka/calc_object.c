#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "main.h"
#include "calc_menu.h"
#include "calc_object.h"
#include <math.h>

static const char CalcGuiName[]="CalcGui";
#define MESSAGE(__STR__) MessageBox(0x6fffffff,__STR__,0, 1 ,11000,(BOOK*)NULL);
extern "C" {
  double strtod(const char *_S, char **_Endptr);
  int abs(int);
};
  
#define PI_CONST 3.1415926535897932384626433832795

double vars['z'-'a'];
double d_answer=0;

char operation[256];
STRID op_str[256];
int op_len=0;
int op_pos=0;

int req_recalc;

int FONTID;
int FONTH;

#define GetFontStyle(a)  (a>>16)
#define GetFontSize(a)  (a&0xFFFF)

enum FONT_STYLES {
  FONT_R=1,
  FONT_B=2,
  FONT_I=4
};

// ��� ���� E_24BI
int GetFontSizeStyle(wchar_t *name)
{
  int size=0;
  int style=0;
  int res=-1;
  if (*name++=='E')
  {
    if (*name++=='_')
    {
      while(*name>='0' && *name<='9')  // ���� �����
      {
        size*=10;
        size+=*name-'0';
        name++;
      }
      int c;
      while((c=*name++))
      {
        if (c=='R') style|=FONT_R;
        if (c=='B') style|=FONT_B;
        if (c=='I') style|=FONT_I;
      }
      if (style!=0 && size!=0)
        res=size|(style<<16);
    }
  }
  return (res);
}

void InitFonts()
{
  int scr_h=Display_GetHeight(0);
  int total_fonts=*GetFontCount();
  FONT_DESC *fnt=GetFontDesc();
  int sfont=scr_h/12;
  int dy=100;
  for (int i=0; i<total_fonts; i++)
  {
    int fss=GetFontSizeStyle(fnt->name);
    if (fss!=-1)
    {
      if (GetFontStyle(fss)==FONT_R)
      {
        SetFont(fnt->id);
        int fh=GetImageHeight(L'A');
        int a=abs(fh-sfont);
        if (a<dy)
        {
          dy=a;
          FONTID=fnt->id;
          FONTH=fh;
        }
      }
    }
    fnt++;
  }
}

const char *const keydesc[48]=
{
    "1"    ,"2"   ,"3",
    "4"    ,"5"   ,"6",
    "7"    ,"8"   ,"9",
    "."    ,"0"   ,"#",
    
    "sin"  ,"*"   ,"/",
    "cos"  ,"+"   ,"-",
    "tan"  ,"("   ,")",
    "^2"   ,"sqrt"   ,"#",
  
    "asin" ,"ln"  ,"log",
    "acos" ,"e^"  ,"-",
    "atan" ,"10^" ,"pi",
    "^"    ,""    ,"#",
  
    "sh"   ,"ash" ,"ANS",
    "ch"   ,"ach" ,"abs",
    "th"   ,"ath" ,"!",
    "X"    ,"Y"   ,"#"
  
};


int GetOperIndexByKey(int key)
{
  if (key>KEY_DIGITAL_0&&key<=(KEY_DIGITAL_0+9))  return (key-KEY_DIGITAL_0-1);
  if (key==KEY_DIGITAL_0)   return (10);
  if (key==KEY_STAR)   return (9);
  return (11);
}

void insert_operation(int op)
{
  if (op_len<sizeof(operation))
  {
    for(int i=op_len; i>=op_pos; i--)
    {
      operation[i+1]=operation[i];
    }
    operation[op_pos++]=op;
    op_len++;    
  } 
}

void remove_operation(void)
{
  if (op_len && op_pos)
  {
    if (op_pos<=op_len)
    {
      int i=op_pos;
      while(i<op_len)
      {
        operation[i-1]=operation[i];
        i++;
      }
      op_pos--;
      op_len--;
    }    
  }  
}

int getXXXXwidth(int font)
{
  int f=SetFont(font);
  STRID str=0x78000000|L'X';
  int w=Disp_GetStrIdWidth(str,1)*4;
  SetFont(f);
  return (w);
}

#pragma inline
int IsCharNumber(int c)
{
  return ((c>=DIG_1 && c<=DIG_0) || c==OP_NEG);
}

#pragma inline
int GetCharByIndex(int c)
{
  return (c==29?'-':c==9?'.':c==10?'0':c+'1');
}

#pragma inline 
int IsMathFunc(int c)
{
  return (c==OP_SIN ||
          c==OP_COS ||
          c==OP_TAN ||
          c==OP_ASIN ||
          c==OP_LN ||
          c==OP_LOG || 
          c==OP_ACOS ||
          c==OP_ATAN || 
          c==OP_SH ||
          c==OP_ASH || 
          c==OP_CH || 
          c==OP_ACH ||
          c==OP_ABS ||
          c==OP_TH ||
          c==OP_ATH ||
          c==OP_FAC);     
}

int IsPowFunc(int c)
{
  return (c==OP_SQUARE || c==OP_SQRT || c==OP_EPOW || c==OP_TENPOW || c==OP_POW);
}

/* ������� PRIOR ����p����� �p��p���� �p���. ���p���� */
int PRIOR(int a)
{
  if (IsPowFunc(a)) return 5;
  if (IsMathFunc(a)) return 4;
  switch(a)
  {    
  case 13:   // *
  case 14:   // /
    return 3;
    
  case 16:  // +
  case 17:  // -
    return 2;
    
  case 19:  // (
         return 1;
  }
  return (0);
}


typedef struct {
  jmp_buf jmp;
  double *stack;
  int sp;
  int st_size;
}DSTACK;

#define DSTACK_SIZE 16
void PushDoubleStack(DSTACK *dstack, double value)
{
  if (dstack->sp>=dstack->st_size)
  {
    dstack->st_size=dstack->sp+DSTACK_SIZE;
    double *d=new double[dstack->st_size];
    if (dstack->stack)
    {
      memcpy(d,dstack->stack,dstack->sp*sizeof(double));
      delete(dstack->stack);
    }
    dstack->stack=d;
  }
  dstack->stack[dstack->sp]=value;
  dstack->sp++;
}

double PopDoubleStack(DSTACK *dstack)
{
  double a=0;
  if (dstack->sp)
  {
    a=dstack->stack[--dstack->sp];    
  }
  else
  {
    longjmp(dstack->jmp, 1);
  }
  return a;  
}

void ConstructDStackStruct(DSTACK *dstack)
{
  dstack->stack=NULL;
  dstack->sp=0;
  dstack->st_size=0;
}

void DestructDStackStruct(DSTACK *dstack)
{
  delete (dstack->stack);
}

double FacN(double n)
{
  double ans=n;
  while(n>1 && ans!=INFINITY)
  {
    ans*=--n;   
  }
  return ans;  
}




double ConvertAngleToRadians(double angle)
{
  double a=0;
  switch(calc_set.drg)
  {
  case DEGREES:
  default:
    a=angle*PI_CONST/180;
    break;
  case RADIANS:
    a=angle;
    break;
  case GRADS:
    a=angle*PI_CONST/200;
    break;
  }
  return (a);
}


double ConvertRadiansToAngle(double radian)
{
  double a=0;
  switch(calc_set.drg)
  {
  case DEGREES:
  default:
    a=radian*180/PI_CONST;
    break;
  case RADIANS:
    a=radian;
    break;
  case GRADS:
    a=radian*200/PI_CONST;
    break;
  }
  return (a);
}

double asinh(double a)
{
  return log(a+sqrt(pow(a,2)+1));
}

double acosh(double a)
{
  return a>=1?log(a+sqrt(pow(a,2)-1)):NAN;
}

double athh(double a)
{
  return log((1+a)/(1-a))/2;
}

void ParseOperation(DSTACK *dstack, int operation)
{
  double a, b, ans;
  switch (operation)
  {
  case 12:    // sin
    a=PopDoubleStack(dstack);
    ans=sin(ConvertAngleToRadians(a));
    PushDoubleStack(dstack, ans);
    break;
  case 13:    // *
    a=PopDoubleStack(dstack);
    b=PopDoubleStack(dstack);
    ans=b*a;
    PushDoubleStack(dstack, ans);
    break;
  case 14:    // /
    a=PopDoubleStack(dstack);
    b=PopDoubleStack(dstack);
    ans=b/a;
    PushDoubleStack(dstack, ans);
    break;
  case 15:    // cos
    a=PopDoubleStack(dstack);
    ans=cos(ConvertAngleToRadians(a));
    PushDoubleStack(dstack, ans);
    break;
  case 16:    // +
    a=PopDoubleStack(dstack);
    b=PopDoubleStack(dstack);
    ans=b+a;
    PushDoubleStack(dstack, ans);
    break;    
  case 17:    // -
    a=PopDoubleStack(dstack);
    b=PopDoubleStack(dstack);
    ans=b-a;
    PushDoubleStack(dstack, ans);
    break;
  case 18:    // tan
    a=PopDoubleStack(dstack);
    ans=tan(ConvertAngleToRadians(a));
    PushDoubleStack(dstack, ans);
    break;
  case 21:    // ^2
    a=PopDoubleStack(dstack);
    ans=pow(a, 2);
    PushDoubleStack(dstack, ans);
    break;
  case 22:    // sqrt
    a=PopDoubleStack(dstack);
    ans=sqrt(a);
    PushDoubleStack(dstack, ans);
    break;
  case 24:    // asin
    a=PopDoubleStack(dstack);
    ans=ConvertRadiansToAngle(asin(a));
    PushDoubleStack(dstack, ans);
    break;
  case 25:   // ln
    a=PopDoubleStack(dstack);
    ans=log(a);
    PushDoubleStack(dstack, ans);
    break;
  case 26:   // log 10
    a=PopDoubleStack(dstack);
    ans=log10(a);
    PushDoubleStack(dstack, ans);
    break;
  case 27:   // acos
    a=PopDoubleStack(dstack);
    ans=ConvertRadiansToAngle(acos(a));
    PushDoubleStack(dstack, ans);
    break;
  case 28:    // e^
    a=PopDoubleStack(dstack);
    ans=exp(a);
    PushDoubleStack(dstack, ans);
    break;
  case 30:    // atan
    a=PopDoubleStack(dstack);
    ans=ConvertRadiansToAngle(atan(a));
    PushDoubleStack(dstack, ans);
    break;
  case 31:    // 10^
    a=PopDoubleStack(dstack);
    ans=pow(10, a);
    PushDoubleStack(dstack, ans);
    break;
  case 33:     // ^
    a=PopDoubleStack(dstack);
    b=PopDoubleStack(dstack);
    ans=pow(b, a);
    PushDoubleStack(dstack, ans);
    break;
  case 36:    // sinh
    a=PopDoubleStack(dstack);
    ans=sinh(ConvertAngleToRadians(a));
    PushDoubleStack(dstack, ans);
    break;
  case 37:    // asinh
    a=PopDoubleStack(dstack);
    ans=ConvertRadiansToAngle(asinh(a));
    PushDoubleStack(dstack, ans);
    break;
  case 39:    // cosh
    a=PopDoubleStack(dstack);
    ans=cosh(ConvertAngleToRadians(a));
    PushDoubleStack(dstack, ans);
    break;
  case 40:    // acosh
    a=PopDoubleStack(dstack);
    ans=ConvertRadiansToAngle(acosh(a));
    PushDoubleStack(dstack, ans);
    break;
  case 42:    // tanh
    a=PopDoubleStack(dstack);
    ans=tanh(ConvertAngleToRadians(a));
    PushDoubleStack(dstack, ans);
    break;
  case 43:    // atanh
    a=PopDoubleStack(dstack);
    ans=ConvertRadiansToAngle(athh(a));
    PushDoubleStack(dstack, ans);
    break;
  case 41:    // abs
    a=PopDoubleStack(dstack);
    ans=fabs(a);
    PushDoubleStack(dstack, ans);
    break;
  case 44:   // !
    a=PopDoubleStack(dstack);
    ans=FacN(a);
    PushDoubleStack(dstack, ans);
    break;
  }
}

void calc_answer()
{
  char *s;
  int c;
  int i=op_len;
  char stack[256];
  int stack_depth=0;
  s=operation;
  DSTACK dstack;
  char *d,value[260];
  double ans=0;
  ConstructDStackStruct(&dstack);
  if (i)
  {
    if (setjmp(dstack.jmp))
    {
      ans=NAN;
      goto L_ERROR;
    }
    do 
    {
      c=*s++;
      if (IsCharNumber(c))
      {
        double a;
        d=value;
        do {
          *d++=GetCharByIndex(c);
          i--;
          c=*s++;
        } while (IsCharNumber(c) && i);
        *d=0;
        a=strtod(value, NULL);
        PushDoubleStack(&dstack, a);
        if (!i) break;
      }
      if (c==32)  // ��
      {
        double a=PI_CONST;
        PushDoubleStack(&dstack, a);
      }
      else if (c==38)   // ANS
      {
        PushDoubleStack(&dstack, d_answer);
      }
      else if (c==45)   // X
      {
        PushDoubleStack(&dstack, calc_set.x);
      }
      else if (c==46)   // Y
      {
        PushDoubleStack(&dstack, calc_set.y);
      }
      else if (c>=VAR_A && c<=VAR_Z)
      {
        PushDoubleStack(&dstack, vars[c-VAR_A]);
      }
      else if (c==RIGHTBRACKET)   // ������������� ������
      {
        while(stack_depth && stack[--stack_depth]!=LEFTBRACKET)
        {
          ParseOperation(&dstack, stack[stack_depth]);
        }
      }
      else if (c==LEFTBRACKET)  // ������������� ������
      {
        stack[stack_depth++]=c;
      }
      else if (c==13 || c==14 || c==16 || c==17 || IsMathFunc(c) || IsPowFunc(c))  // ���� ���� ��������
      {
        if (!stack_depth)  // ���� ���� ����
        {
          stack[stack_depth++]=c;
        }
        else if (PRIOR(c)>PRIOR(stack[stack_depth-1]))  // ���� ��������� ����������� �������� ������ ���� ��� �� ������� �����
        {
          stack[stack_depth++]=c;
        }
        else
        {
          while(stack_depth && (PRIOR(stack[stack_depth-1])>=PRIOR(c)))
          {
            ParseOperation(&dstack, stack[--stack_depth]);
          }
          stack[stack_depth++]=c;
        }
      }
    }
    while(--i);
    while(stack_depth)
    {
      ParseOperation(&dstack, stack[--stack_depth]);
    }
    ans=PopDoubleStack(&dstack);
  }
L_ERROR:
  DestructDStackStruct(&dstack);
  d_answer=ans;
}


int CalcGuiOnCreate(DISP_OBJ_CALC *db)
{
  InitFonts();
  db->answer_sid=LGP_NULL;
  int font=SetFont(FONTID);
  for (int x=0; x<VAR_A; x++)
  {
    STRID text;
    const char *txt=keydesc[x];
    if (!txt[1])
      text=0x78000000|txt[0];
    else
      text=Str2ID(txt,6,SID_ANY_LEN);
    db->yx[x]=text;
    db->names_len[x]=Disp_GetStrIdWidth(text,TextGetLength(text));
  }
  for (int x=VAR_A, a='a'; x<TOTAL_OPS; x++, a++)
  {
    STRID text=0x78000000|a;
    db->yx[x]=text;
    db->names_len[x]=Disp_GetStrIdWidth(text,TextGetLength(text));
  }
  SetFont(font);
  db->current_tab=0;
  db->cur_base=0;
  req_recalc=1;
  for (int i=0; i<MAXELEMS(vars);i++) vars[i]=NAN;
  return (1);
}

void CalcGuiOnClose(DISP_OBJ_CALC *db)
{
  TextFree(db->answer_sid);
  for (int x=0; x<TOTAL_OPS; x++)
  {
    TextFree(db->yx[x]); 
  }
}

extern "C" void DrawUnk(void *gc, int x, int y, int unk, int unk2, int pic);

enum TEXT_ALIGN {
  TA_LEFT=0,
  TA_RIGHT=1,
  TA_CENTER=2,
};

enum GC_XX {
  GC_XX_WITH_BRUSH=1,
  GC_XX_WITHOUT_BRUSH=2,
};

#define COLOR(R,G,B,A) ((B&0xFF)|((G&0xFF)<<8)|((R&0xFF)<<16)|((A&0xFF)<<24))


#define SB_W            (scr_w>>6)
#define SB_X1(x2abs)    (x2abs-(scr_h>>6)-SB_W)
#define SB_X2(x2abs)    (x2abs-(scr_h>>6))

#define ANS_B 1
#define ANS_H (FONTH+ANS_B*2)
#define ANS_X (scr_h>>6)
#define ANS_Y (scr_h>>6)

#define INP_H          (FONTH+ANS_B*2)
#define INP_YD         (scr_h>>6)
#define INP_Y          (ANS_Y+ANS_H+(scr_h>>5))
#define INP_X1(x1abs)  (x1abs+(scr_h>>6))
#define INP_X2(x2abs)  (x2abs-(scr_h>>6)-1-SB_W)
#define INP_Y1(y1abs)  (y1abs+INP_Y)
#define INP_Y2(y2abs)  (y2abs-INP_YD)



#define INP_MAXITEM(rc) ((INP_Y2(rc->y2)-INP_Y1(rc->y1))/INP_H)

int GetMaxCurRow(DISP_OBJ_CALC *db, int *max, int *cur)
{
  int scr_h=db->y2-db->y1;
  int scr_w=db->x2-db->x1;
  int inp_w=INP_X2(db->x2)-INP_X1(db->x1)-4;
  int tx=0;
  int f=0;
  int drow=0;
  int pos=0;
  if (max) *max=0;
  if (cur) *cur=0;
  for (int i=0; i<=op_len;i++)
  {
    if (i==op_pos)
    {
      if (cur) *cur=drow;
      if (!max) break;
      pos=tx;
      f=2;
    }
    if (i<op_len)
    {
      int tlen=db->names_len[operation[i]];
      if (tx+tlen>inp_w+f)
      {
        drow++;
        tx=0;
        f=0;
      }
      tx+=tlen;
    }
  }
  if (max) *max=drow;
  return (pos);
}

void SetCursPos(DISP_OBJ_CALC *db, int cur, int pos)
{
  int scr_h=db->y2-db->y1;
  int scr_w=db->x2-db->x1;
  int inp_w=INP_X2(db->x2)-INP_X1(db->x1)-4;
  int tx=0;
  int f=0;
  int drow=0;
  for (int i=0; i<=op_len;i++)
  {
    if ((drow==cur && tx>=pos) || i==op_len)
    {
      op_pos=i;
      f=2;
      break;
    }
    if (i<op_len)
    {
      int tlen=db->names_len[operation[i]];
      if (tx+tlen>inp_w+f)
      {
        if (cur==drow) 
        {
          op_pos=i;
          break;
        }
        drow++;
        tx=0;
        f=0;
      }
      tx+=tlen;
    }
  }
}

void CalcGuiOnRedraw(DISP_OBJ_CALC *db,int ,RECT *cur_rc,int)
{
  void *gc=get_DisplayGC();
  int gc_xx, font;
  int scr_h, scr_w;
  db->x1=cur_rc->x1;
  db->x2=cur_rc->x2;
  db->y1=cur_rc->y1;
  db->y2=cur_rc->y2;
  scr_h=db->y2-db->y1;
  scr_w=db->x2-db->x1;

  gc_xx=get_GC_xx(gc);
  set_GC_xx(gc,2);
  DrawRect(db->x1,db->y1,db->x2,db->y2,clWhite,COLOR(0xC0,0xC0,0xC0,0xFF));
  
  if (req_recalc)
  {
    char revpn[32];
    req_recalc=0;
    calc_answer();
    sprintf(revpn, "%1.10lg", d_answer);
    TextFree(db->answer_sid);
    db->answer_sid=Str2ID(revpn,6,SID_ANY_LEN);
  }
  RECT rt;
  //RECT *rtp=&rt;
  rt.x1=db->x1+ANS_X;
  rt.y1=db->y1+ANS_Y;
  rt.x2=db->x2-ANS_X-1;
  rt.y2=rt.y1+ANS_H;
  GC_SetPenColor(gc, clBlack);
  GC_SetBrushColor(gc, clWhite);
  DrawRect(rt.x1,rt.y1,rt.x2,rt.y2,GC_GetPenColor(gc),GC_GetBrushColor(gc));
  //GC_DrawRoundRect(gc ,&rt,4,4,2,1);  // ����� ��� ������
    
  rt.x1=INP_X1(db->x1);
  rt.y1=INP_Y1(db->y1);
  rt.x2=INP_X2(db->x2);
  rt.y2=INP_Y2(db->y2);
  GC_SetPenColor(gc, clBlack);
  GC_SetBrushColor(gc, clWhite);
  DrawRect(rt.x1,rt.y1,rt.x2,rt.y2,GC_GetPenColor(gc),GC_GetBrushColor(gc));
  //GC_DrawRoundRect(gc ,&rt,4,4,2,1);  // ����� ��� �������� �����
  
  font=SetFont(FONTID);
  DrawString(db->answer_sid,0,db->x1+ANS_X+1,db->y1+ANS_Y+ANS_B, db->x2-ANS_X-2, db->y1+ANS_Y+ANS_H+ANS_B*2,0,0,clBlack,0);  // �����
  
  // �������
  int drow=0;
  int cur_row, max_row;
  int cur_base, cur_count;
  int maxitem=INP_MAXITEM(cur_rc);
  GetMaxCurRow(db,&max_row,&cur_row);
  max_row++;
  if (max_row<=maxitem)
  {
    cur_base=0;
    cur_count=max_row;
  }
  else
  {
    cur_count=maxitem;
    cur_base=db->cur_base;
    
    int idist=cur_row-db->cur_base;
    if (idist >= maxitem-1) cur_base += idist - maxitem + 2;
    else if (cur_row <= db->cur_base) cur_base += idist - 1;
    
    if (cur_base < 0) cur_base = 0;
    else if (cur_base > max_row - maxitem) cur_base = max_row - maxitem;
  }
  db->cur_base=cur_base;
  {
    SetFont(FONTID);
    int tx=0;
    int inp_w=INP_X2(db->x2)-INP_X1(db->x1)-4;
    int x_offs=INP_X1(db->x1)+1;
    int y_offs=INP_Y1(db->y1);
    int f=0;
    for (int i=0; i<=op_len;i++)
    {
      if (i==op_pos)
      {
        DrawRect(x_offs+tx,y_offs+INP_H*(drow-cur_base),x_offs+tx+2,y_offs+INP_H*(drow-cur_base)+FONTH,clBlack,clBlack);
        tx+=2;
        f=2;
      }
      if (i<op_len)
      {
        STRID text=db->yx[operation[i]];
        int tlen=db->names_len[operation[i]];
        if (tx+tlen>(inp_w+f))
        {
          drow++;
          tx=0;
          f=0;
        }
        if (drow>=cur_base && drow<(cur_base+cur_count))
          DrawString(text,0,x_offs+tx,y_offs+INP_H*(drow-cur_base), x_offs+tx+tlen, y_offs+INP_H*(drow-cur_base+1),0,0,clBlack,0);  // �����
        tx+=tlen;
      }
    }
  }
  
  // ������ ���
  if (max_row>maxitem)
  {
    int inp_h=INP_Y2(db->y2)-INP_Y1(db->y1);
    int sb_h=maxitem*inp_h/(max_row-1);
    int sb_y=(inp_h-sb_h)*cur_row/(max_row-1);
    DrawRect(SB_X1(db->x2),INP_Y1(db->y1)+sb_y,SB_X2(db->x2),INP_Y1(db->y1)+sb_y+sb_h,clBlack,clBlack);
  }
  
  
  
  if (db->current_tab!=0) // ���� ��� �� ������ ��� ������ ���������
  {
    int XXXXwidth=getXXXXwidth(FONTID);
    int need_height=FONTH*4+5*2*3+5*2;
    int need_width=XXXXwidth*3+5*2*2+5*2;
    
    int start_y=need_height<scr_h?(scr_h-need_height)>>1:0;
    int start_x=need_width<scr_w?(scr_w-need_width)>>1:0;
    
    rt.x1=db->x1+start_x;
    rt.y1=db->y1+start_y;
    rt.x2=db->x1+start_x+need_width;
    rt.y2=db->y1+start_y+need_height;
    GC_SetPenColor(gc, clBlack);
    GC_SetBrushColor(gc, clWhite);
    DrawRect(rt.x1,rt.y1,rt.x2,rt.y2,GC_GetPenColor(gc),GC_GetBrushColor(gc));
    //GC_DrawRoundRect(gc ,&rt,4,4,2,1);  // ����� ��� �������� �����
    for (int y=0; y<4; y++)
    {
      for (int x=0; x<3; x++)
      {
        unsigned int x_frame=start_x+XXXXwidth*x+5*2*x;
        unsigned int y_frame=start_y+FONTH*y+5*2*y;
        STRID text=db->yx[db->current_tab*12+y*3+x];
        SetFont(FONTID);    
        //unsigned int str_width=db->names_len[db->current_tab][y*3+x];
        rt.x1=db->x1+x_frame;
        rt.y1=db->y1+y_frame;
        rt.x2=db->x1+x_frame+XXXXwidth+5*2;
        rt.y2=db->y1+y_frame+FONTH+5*2;
        GC_SetPenColor(gc, clBlack);
        GC_SetBrushColor(gc, COLOR(0xC0,0xC0,0xC0,0xFF));
        DrawRect(rt.x1,rt.y1,rt.x2,rt.y2,GC_GetPenColor(gc),GC_GetBrushColor(gc));
        //GC_DrawRoundRect(gc ,&rt,3,3,2,1);  // ����� ��� �������� �����
        DrawString(text,2,x_frame,y_frame+5,x_frame+XXXXwidth+10,y_frame+FONTH+5,
                   0,0,clBlack,0);
      }
    }
  }
  SetFont(font);
  set_GC_xx(gc,gc_xx);
}



void CalcGuiOnKey(DISP_OBJ_CALC *db,int key,int a,int b,int type)
{
  int reset_pos=0;
  if (type==KBD_SHORT_RELEASE || type==KBD_REPEAT)
  {
    if (key==KEY_DIEZ)
    {
      if (++db->current_tab>3) db->current_tab=0;
    }
    else if ((key>=KEY_DIGITAL_0 && key<=(KEY_DIGITAL_0+9)) || key==KEY_STAR)
    {
      insert_operation(GetOperIndexByKey(key)+((db->current_tab)*12));
      if (db->current_tab!=0) db->current_tab=0;
      if (calc_set.auto_recalc) req_recalc=1;
      reset_pos=1;
    }
    else if (key==KEY_DEL)
    {
      remove_operation();
      if (calc_set.auto_recalc) req_recalc=1;
      reset_pos=1;
    }
    else if (key==KEY_LEFT)
    {
      if (op_pos) op_pos--;
      reset_pos=1;
    }
    else if (key==KEY_RIGHT)
    {
      if (op_pos<op_len) op_pos++;
      reset_pos=1;
    }
    else if (key==KEY_UP || key==KEY_DOWN)
    {
      int max, cur, pos;
      pos=GetMaxCurRow(db,&max,&cur);
      if (db->curx_pos==-1) db->curx_pos=pos;

      if (key==KEY_UP)
      {
        if (cur>0) cur--;
      }
      else
      {
        if (cur<max) cur++;
      }
      SetCursPos(db,cur,db->curx_pos);
    }
    else if (key==KEY_RIGHT_SOFT)
    {
      CreateCalcMenu();
    }
    else if (key==KEY_LEFT_SOFT)
    {
      req_recalc=1;
    }
    else if (key==KEY_ESC)
    {
      if (db->current_tab!=0) db->current_tab=0;
      else if (op_len || op_pos)
      {
        op_len=op_pos=0;
        req_recalc=1;
        reset_pos=1;
      }
      else
      {
        FreeBook(FindBook(isScalkaBook));
        return;
      }
    }
  }
  if (reset_pos) db->curx_pos=-1;
  InvalidateRect(&db->dsp_obj,0);
}

void CalcGui_constr(DISP_DESC *desc)
{
  DISP_DESC_SetName(desc,CalcGuiName);
  DISP_DESC_SetSize(desc,sizeof(DISP_OBJ_CALC));
  DISP_DESC_SetOnCreate(desc,(DISP_OBJ_ONCREATE_METHOD)CalcGuiOnCreate);
  DISP_DESC_SetOnClose(desc,(DISP_OBJ_ONCLOSE_METHOD)CalcGuiOnClose);
  DISP_DESC_SetOnRedraw(desc,(DISP_OBJ_ONREDRAW_METHOD)CalcGuiOnRedraw);
  DISP_DESC_SetOnKey(desc,(DISP_OBJ_ONKEY_METHOD)CalcGuiOnKey);
}

void CalcGui_destr(DISP_DESC *desc)
{
}

enum GUI_STYLES {
  NO_ICONBAR=3,
  NO_SOFTKEYS_ICONBAR=4,
  SMALL_WINDOW=5,
  SMALL_WITH_FRAME=7,
  SMALL_WITHOUT_BGROUND=8,
  SMALL_WITHOUT_BGROUND_HEADER=9,
};

enum TITLE_TYPE {
  TITLE_NO=1,
  TITLE_SMALL=3,
  TITLE_LARGE=5
};

GUI_CALC *CreateCalkGUI(BOOK *bk)
{
  GUI_CALC *gui_calc=new GUI_CALC;
  if (!CreateObject((GUI *)gui_calc,CalcGui_destr,CalcGui_constr, bk,0,0,0))
  {
    delete gui_calc;
    return 0;    
  }
  if (bk) addGui2book(bk,(GUI*)gui_calc);
  GUI_SetStyle((GUI *)gui_calc,4);
  GuiObject_SetTitleType((GUI *)gui_calc, 1);
  GUIObject_HideSoftkeys((GUI *)gui_calc);
  return gui_calc;
}



