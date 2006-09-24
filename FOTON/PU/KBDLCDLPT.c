//--------------------------------------------
// ��������� �����-������
// ����������, ���, �������
//--------------------------------------------

//#pragma codeseg(RCODE)

//��������� ���������� �� ������� 10��
//__interrupt [TIMER2_OVF_vect]void MainIntSystem(void)
#pragma vector=TIMER2_OVF_vect
__interrupt void MainIntSystem(void)
{
  static char SoundTimer;
  char ckey;
  char tled;
  __enable_interrupt();
#ifdef PUA1
  if (PORTA&8) PORTA&=~8; else PORTA|=8;
#endif
  //������ ������ � ���� ����������� � �������
  if (!(--LEDflashcnt))
  {
    LEDflashcnt=20;
  }
  tled=LED;
  if (LEDflashcnt>=10) tled^=LEDflash;
#ifdef PUA1
  PORTD&=3;
  PORTD|=tled;
  //��������� ����������
  __disable_interrupt();
  ckey=~PINF;
  __enable_interrupt();
  switch (ckey&0xFC) //����������� ���� � �������� �������
  {
  case 0x80: ckey=kACK; break;
  case 0x40: ckey=kUP; break;
  case 0x10: ckey=kDW; break;
  case 0x20: ckey=kLT; break;
  case 0x08: ckey=kRT; break;
  case 0x28: ckey=kSECRET; break;
  case 0x04: ckey=kESC; break;
  default: ckey=0x00;
  }
#else
  __disable_interrupt();
  PORTA=0xFF;
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  DDRA=0x00;
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  ckey=~PINA;
  PORTA=tled;
  DDRA=0xFF;
  __enable_interrupt();
  switch (ckey)
  {
  case 0x20: ckey=kACK; break;
  case 0x10: ckey=kUP; break;
  case 0x04: ckey=kDW; break;
  case 0x08: ckey=kLT; break;
  case 0x02: ckey=kRT; break;
  case 0x0A: ckey=kSECRET; break;
  case 0x01: ckey=kESC; break;
  default: ckey=0x00;
  }
#endif
  if (ckey==OldKey && ckey!=0x00) //���� ������� ������ � �� ����������
  {
    KeyCounter++; //����������� �������
    //if (ckey!=kOK) //��� ������� ������
    {
      if (KeyCounter==0) {KEY=ckey; if (!SndMode) SndMode=4;} //������ 50��, ��������� ���, �������� ����
      if (KeyCounter==KEY_REPDEL) //������ ����� �� ������ �����������
      {
        KeyCounter=KEY_REPPER; //����� ����������
        KEY=ckey; if (!SndMode) SndMode=4; //��������� ���, �������� ����
      }
    }
    /*else  //��� ������� ����
    {
    if (KeyCounter==KEY_REPDEL) //������ ����� �� ������ ����������� (������ �������)
    {
    KEY=kESC; if (!SndMode) SndMode=4; //��������� ���, �������� ����
  }
   else
    {
    if (KeyCounter>KEY_REPDEL) KeyCounter=KEY_REPDEL; //������������ �������
  }
  }*/
  }
  else //��������� ������� ��� ��� ���������
  {
    /*if (OldKey==kOK && KeyCounter>0 && KeyCounter<(KEY_REPDEL>>1)) //��� ������� ����
    //���� ���� ������ ����� ��� 50�� � ����� ��� �������� ������� �� �����������
    {
    KEY=kOK; if (!SndMode) SndMode=4; //��������� ���
    KeyCounter=KEY_DELAY;             //�������� ����
    OldKey=ckey;                      //��������� ����� ���
  }
  else */
    //�������
    {
      OldKey=ckey; //��������� ����� ���
      KeyCounter=KEY_DELAY; //������������� ������� 50��
    }
  }
  if (!KEY) //���� ��� ������� ��������� ����� ������� ���������� � ������� ������ � KEY
  {
    if(ATKwp!=ATKrp) { KEY=ATKbuf[ATKrp]; ATKrp=(ATKrp+1)&7; if (!SndMode) SndMode=4;}
  }
  //��������� ��������� ������������ �����
  SoundTimer+=8;
  if (SndMode&&(COPY_PU_STAT&0x10)) SndMode=4;

  if (SndMode==0) PORTB&=0x7f; //������
  if (SndMode==1) PORTB|=0x80; //����� - ������� ������
  if (SndMode==2) PORTB=(PORTB&0x7F)|(SoundTimer&0x80); //����� - 50/50
  if (SndMode==3) {if (SoundTimer>192) PORTB|=0x80; else PORTB&=0x7f;} //�������������� 25/75
  if (SndMode==4) {PORTB|=0x80; SndMode=0;} //��������� - ��������� �� 10��
  //�������� ���������� �������� � ������ OffLine
#ifdef PUA1
  if (PINA&0x40) // ������� �����
#else
    if (PINB&0x20)
#endif
      PrinterOnLine=1000; //������� ��������������� �� 100�
    else //������� �� �����
      if (PrinterOnLine) //���� ��� �� � ������
      {
        PrinterOnLine--; //��������� �������
        if (!PrinterOnLine) Event2FIFO(0xFF,0xFF,19,0); //����� ������� - ��������� ��������� "������� � ������ OffLine"
      }
    if (CANtimeout) CANtimeout--;
}

//���������� �� ������ 0 �� ����� CLK �� ����������
#pragma vector=INT0_vect
__interrupt void INT0_interrupt(void)
{
  //������� ������������� ����-���� � ��� ������� ��� ��������� ������� �����
  static __flash char NEKBD[]=
  {
    0x00,0x19,0x00,0x15,0x13,0x11,0x12,0x1c,0x00,0x1a,0x18,0x16,0x14,0x09,'`' ,0x00,
    0x00,0x00,0x00,0x00,0x00,'q' ,'1' ,0x00,0x00,0x00,'z' ,'s' ,'a' ,'w' ,'2' ,0x00,
    0x00,'c' ,'x' ,'d' ,'e' ,'4' ,'3' ,0x00,0x00,' ' ,'v' ,'f' ,'t' ,'r' ,'5' ,0x00,
    0x00,'n' ,'b' ,'h' ,'g' ,'y' ,'6' ,0x00,0x00,0x00,'m' ,'j' ,'u' ,'7' ,'8' ,0x00,
    0x00,',' ,'k' ,'i' ,'o' ,'0' ,'9' ,0x00,0x00,'.' ,'/' ,'l' ,';' ,'p' ,'-' ,0x00,
    0x00,0x00,0x27,0x00,'[' ,'=' ,0x00,0x00,0x00,0x00,0x0d,']' ,0x00,0x5c,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,'1' ,0x00,'4' ,'7' ,0x00,0x00,0x00,
    '0' ,'.' ,'2' ,'5' ,'6' ,'8' ,0x1b,0x00,0x1b,'+' ,'3' ,'-' ,'*' ,'9' ,0x00,0x00,
    0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
  };
  static __flash char SEKBD[]=
  {
    0x00,0x19,0x00,0x15,0x13,0x11,0x12,0x1c,0x00,0x1a,0x18,0x16,0x14,0x09,'~' ,0x00,
    0x00,0x00,0x00,0x00,0x00,'Q' ,'!' ,0x00,0x00,0x00,'Z' ,'S' ,'A' ,'W' ,'@' ,0x00,
    0x00,'C' ,'X' ,'D' ,'E' ,'$' ,'#' ,0x00,0x00,' ' ,'V' ,'F' ,'T' ,'R' ,'%' ,0x00,
    0x00,'N' ,'B' ,'H' ,'G' ,'Y' ,'^' ,0x00,0x00,0x00,'M' ,'J' ,'U' ,'&' ,'*' ,0x00,
    0x00,'<' ,'K' ,'I' ,'O' ,')' ,'(' ,0x00,0x00,'>' ,'?' ,'L' ,':' ,'P' ,'_' ,0x00,
    0x00,0x00,'"' ,0x00,'{' ,'+' ,0x00,0x00,0x00,0x00,0x0d,'}' ,0x00,'|' ,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x01,0x00,0x08,0x02,0x00,0x00,0x00,
    0x0e,0x0f,0x0c,0x00,0x0a,0x0b,0x1b,0x00,0x1b,'+' ,0x03,'-' ,'*' ,0x04,0x00,0x00,
    0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
  };
  static __flash char CEKBD[]=
  {
    0x00,0x19,0x00,0x15,0x13,0x11,0x12,0x1c,0x00,0x1a,0x18,0x16,0x14,0x09,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x1a,0x13,0x01,0x17,0x00,0x00,
    0x00,0x03,0x18,0x04,0x05,0x00,0x00,0x00,0x00,' ' ,0x16,0x06,0x14,0x12,0x00,0x00,
    0x00,0x0e,0x02,0x08,0x07,0x19,0x00,0x00,0x00,0x00,0x0d,0x0a,0x15,0x00,0x00,0x00,
    0x00,0x00,0x0b,0x09,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x00,0x10,0x1f,0x00,
    0x00,0x00,0x00,0x00,0x1b,0x00,0x00,0x00,0x00,0x00,0x07,0x1d,0x00,0x1c,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x01,0x00,0x08,0x02,0x00,0x00,0x00,
    0x0e,0x0f,0x0c,0x00,0x0a,0x0b,0x1b,0x00,0x1b,0x00,0x03,0x00,0x00,0x04,0x00,0x00,
    0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
  };

  static __flash char NRKBD[]=
  {
    0x00,0x19,0x00,0x15,0x13,0x11,0x12,0x1c,0x00,0x1a,0x18,0x16,0x14,0x09,'�' ,0x00,
    0x00,0x00,0x00,0x00,0x00,'�' ,'1' ,0x00,0x00,0x00,0xFF,'�' ,'�' ,'�' ,'2' ,0x00,
    0x00,'�' ,'�' ,'�' ,'�' ,'4' ,'3' ,0x00,0x00,' ' ,'�' ,'�' ,'�' ,'�' ,'5' ,0x00,
    0x00,'�' ,'�' ,'�' ,'�' ,'�' ,'6' ,0x00,0x00,0x00,'�' ,'�' ,'�' ,'7' ,'8' ,0x00,
    0x00,'�' ,'�' ,'�' ,'�' ,'0' ,'9' ,0x00,0x00,'�' ,'.' ,'�' ,'�' ,'�' ,'-' ,0x00,
    0x00,0x00,'�' ,0x00,'�' ,'=' ,0x00,0x00,0x00,0x00,0x0d,'�' ,0x00,0x5C,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,'1' ,0x00,'4' ,'7' ,0x00,0x00,0x00,
    '0' ,'.' ,'2' ,'5' ,'6' ,'8' ,0x1b,0x00,0x1b,'+' ,'3' ,'-' ,'*' ,'9' ,0x00,0x00,
    0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
  };
  static __flash char SRKBD[]=
  {
    0x00,0x19,0x00,0x15,0x13,0x11,0x12,0x1c,0x00,0x1a,0x18,0x16,0x14,0x09,'�' ,0x00,
    0x00,0x00,0x00,0x00,0x00,'�' ,'!' ,0x00,0x00,0x00,'�' ,'�' ,'�' ,'�' ,'"' ,0x00,
    0x00,'�' ,'�' ,'�' ,'�' ,';' ,'�' ,0x00,0x00,' ' ,'�' ,'�' ,'�' ,'�' ,'%' ,0x00,
    0x00,'�' ,'�' ,'�' ,'�' ,'�' ,':' ,0x00,0x00,0x00,'�' ,'�' ,'�' ,'?' ,'*' ,0x00,
    0x00,'�' ,'�' ,'�' ,'�' ,')' ,'(' ,0x00,0x00,'�' ,',' ,'�' ,'�' ,'�' ,'_' ,0x00,
    0x00,0x00,'�' ,0x00,'�' ,'+' ,0x00,0x00,0x00,0x00,0x0d,'�' ,0x00,'/' ,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x01,0x00,0x08,0x02,0x00,0x00,0x00,
    0x0e,0x0f,0x0c,0x00,0x0a,0x0b,0x1b,0x00,0x1b,'+' ,0x03,'-' ,'*' ,0x04,0x00,0x00,
    0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
  };


  static unsigned char data;                // �������� �������� ����-���
  if (ATKbitcount < 11 && ATKbitcount > 2)  // ���� �� 3 �� 10 - ������. ��������,
  {                                         // ��������� � �������� ���� ������������.
    data = (data >> 1);
    if (PIND & 2) data = data | 0x80;            // ���������� 1
  }
  EIMSK&=0xFE; //������ ���������� �� 0
  //TIMSK&=0xFB; //������ ���������� ������� �������� ������
  //__enable_interrupt();
  while ((PIND&1)==0); //�������� ������ ������� CLK
  //__disable_interrupt();
  //TIMSK=SaveTIMSK; //�������������� ���������� �� �������
  if(--ATKbitcount == 0)                    // ��� ���� �������
  {
    PORTD&=0xFE; //������������ �������� ������������
    DDRD|=0x01;
    delay(100);
    DDRD&=0xFE;
    PORTD|=0x01;
    if (ATKstat&1)
    {
      // ��������� ��� 0xf0 - ���������� ������
      switch (data)
      {
      case 0x12: ATKstat&=0xfd; break; //��������� Shift
      case 0x59: ATKstat&=0xfd; break;
      case 0x14: ATKstat&=0xfb; break; //��������� Ctrl
      }
      ATKstat&=0xfe; //�������� ����
    }
    else //������� ������
    {
      switch (data)
      {
      case 0x12: ATKstat|=2; break; //����� Shift
      case 0x59: ATKstat|=2; break;
      case 0x14: ATKstat|=4; break; //����� Ctrl
      case 0xf0: ATKstat|=1; break; //������� ���������� �������
      case 0x58: ATKstat^=8; break; //����� CapsLock, ����������� ����������/�������
      default: if (data<0x90) //������ ����-���
      {
        if (ATKstat&4)
          ATKbuf[ATKwp]=CEKBD[data]; //��� ������� ��� ������� Ctrl
        else
        {
          if (ATKstat&2) //��� �������� Shift
            ATKbuf[ATKwp]=ATKstat&8?SRKBD[data]:SEKBD[data]; //�������/����������
          else // ������� �������
            ATKbuf[ATKwp]=ATKstat&8?NRKBD[data]:NEKBD[data]; //�������/����������
        }
        ATKwp=(ATKwp+1)&7; //��������� ������ � ������
      } break;
      }
    }
    ATKbitcount = 11; //������������� �������� ���
  }
  EIMSK|=0x01; //���������� ���������� �� ����������
}

// ���������� ��������� ����������
void WorkSecond(void)
{
  char *p;
  char i;
  char k;
  unsigned int j;
  __disable_interrupt();
  if (TIFR&0x01)
  {
    TIFR=1; //����� ����� ���������� ����������
    TimeDate=TimeDate+1; //���������� �������
    if ((k=DBLE_RS0))
    {
      k--;
      if (!k)
      {
        Event2FIFO(0xFF,0xFF,34,0); //��� ������ c ����. �������� �� RS0
      }
      DBLE_RS0=k;
    }
    if ((k=DBLE_RS1))
    {
      k--;
      if (!k)
      {
        Event2FIFO(0xFF,0xFF,34,1); //��� ������ � ����. �������� �� RS1
      }
      DBLE_RS1=k;
    }
    if (DBLS) DBLS--;
  }
  else
  {
    __enable_interrupt();
    return;
  }
  __enable_interrupt();
  i=0;
  if (!(TimeDate%3600)) NeedDecTrialTime=1; //���� ������ ���, ���������� ������� 1 �� ������� ������
  // ��������� �������� ���
  p=ZoneTF;
  do
  {
    if (*p) //���� ������� ������ ������ 0
    {
      (*p++)--; //��������� ������
    }
    else //�������
    {
      //���� ���� �� �������������
      p++;
      if ((*p)!=0xFF) *p=0; //����� ���������� ��������
    }
    p++; //��������� ����
    i++;
  }
  while (i!=0); //���� �� ���������� ��� ����
  p=ZoneDelays;
  j=1024;
  do
  {
    i=*p;
    if (i<0xFE) i--;
    *p=i;
    p++;
  }
  while(--j);
  p=MacroTimers;
  i=16;
  do
  {
    k=*p;
    if (k)
    {
      k--;
      if (!k)
      {
        char c=p[1];
        BitFlags[c>>3]|=(1<<(c&7));
      }
      *p=k;
    }
    p+=2;
  }
  while(--i);
  // UPScount=9; //����� ������ �� ���, ������ ����������
  // __watchdog_reset();
}

//��������� ������ ������ � ���
#ifdef PUA1
#define LCDstrobe {PORTA|=0x20;__no_operation();__no_operation();PORTA&=~0x20;}
#else
#define LCDstrobe {PORTD&=0x7F;__no_operation();__no_operation();PORTD|=0x80;}
#endif

//������������ ��� ��� ������������ �3
char RotateLCDdata(char c)
{
  char r=0;
  if (c&0x01) r|=0x08;
  if (c&0x02) r|=0x04;
  if (c&0x04) r|=0x02;
  if (c&0x08) r|=0x01;
  if (c&0x10) r|=0x80;
  if (c&0x20) r|=0x40;
  if (c&0x40) r|=0x20;
  if (c&0x80) r|=0x10;
  return(r);
}

//�������� � ��� ��������
void SendCmdLCD(char c)
{
  c=RotateLCDdata(c);
  PORTC=(c&0xF0)>>4;
  LCDstrobe;
  PORTC=(c&0x0F);
  LCDstrobe;
  delay(50);
}

//�������� � ��� ������
void SendDataLCD(char c)
{
  c=RotateLCDdata(c);
  PORTC=((c&0xF0)>>4)|0x10;
  LCDstrobe;
  PORTC=(c&0x0F)|0x10;
  LCDstrobe;
  delay(50);
}
//#pragma warnings=on

void UpdateConsole(void)
{
  char *p;
  char i;
  char c;

  if (PU_STAT&1) //���� ������ ������� �����
  {
    p=CON;
    i=80;
    do
    {
      if (*p&64) *p&=0xDF;
      p++;
    }
    while(--i);
  }
  SendCmdLCD(0x0C); //��������� ������
  SendCmdLCD(128); //������� 0
  p=CON;
  i=80;
  do
  {
    if (i==40) SendCmdLCD(192); //������� 40 (������ ������)
    c=*p++;
    if (c==0) c=32;
    if (c&128) c=Win2LCDTable[c-192];
    SendDataLCD(c);
  }
  while(--i);
  if (CursorPos<80)
  {
    SendCmdLCD(CursorPos>=40?152+CursorPos:128+CursorPos);
    SendCmdLCD(0x0F); //���� ����, �������� ������
  }
}

//������� ������
void ClrScr(void)
{
  memset(CON,' ',80);
  CursorPos=80;
}

//��������� ������������� ���
void InitLCD(void)
{
  delay(0);
  PORTC=0x0C;
  LCDstrobe;
  delay(6000);
  PORTC=0x0C;
  LCDstrobe;
  delay(5000);
  PORTC=0x0C;
  LCDstrobe;
  delay(5000);
  PORTC=0x04;
  LCDstrobe;
  delay(50);
  SendCmdLCD(0x28);
  SendCmdLCD(0x08);
  ClrScr();
  SendCmdLCD(0x06);
  CursorPos=80;
}

//������ ������� �� �������
//�������� ����������� putchar �� ����������
int putchar(int i)
{
  char s,c;
  s=i;
  if (s>=0xc0&&s<0xf0) s-=0x40; //������������� ��������� ������� ����
  if (s>=0xf0) s-=0x10;
  c=s;
#ifdef PUA1
  while ((!(PINA&0x40))&&PrinterOnLine); //������� ���������� ��� ��������
  PORTC=~c; //������� ������
  delay(5);
  PORTA&=0x7F; //���������� �����
  delay(5);
  PORTA|=0x80;
#else
  c=0;
  if (s&0x01) c|=0x40;
  if (s&0x02) c|=0x80;
  if (s&0x04) c|=0x02;
  if (s&0x08) c|=0x08;
  if (s&0x10) c|=0x20;
  if (s&0x20) c|=0x01;
  if (s&0x40) c|=0x04;
  if (s&0x80) c|=0x10;
  while ((!(PINB&0x20))&&PrinterOnLine);
  PORTC=~c;
  delay(5);
  PORTB&=0xEF;
  delay(5);
  PORTB|=0x10;
#endif
  if (PrinterOnLine) PrinterOnLine=1000; //������� ��������������� �� 100�
  return(i);
}


//������ 80-�� �������� "-"
void PrintSeparator(void)
{
  char c;
  for(c=0;c<79;c++) putchar('-');
  PRINT_CRLF;
}

//������� ������ ������ �� ���
int putstr(char *p)
{
  while (*p) putchar(*p++);
  return(1);
}
//������� ������ ������ �� Flash
//int putstr_P(PGM_P p)
//{
// while (*p) putchar(*p++);
// return(1);
//}

//�������� ��������� ��� �������
void SetLEDon(char c)
{
  LEDflash&=(~c);
  LED|=c;
}

//�������� ��������� � ��������
void SetLEDflash(char c)
{
  LEDflash|=c;
  LED|=c;
}

//��������� ���������
void SetLEDoff(char c)
{
  LEDflash&=(~c);
  LED&=(~c);
}


