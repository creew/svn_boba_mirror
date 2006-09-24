// ��������� ��� TIMER1
// ������ 3.2��
#define Time3200 (unsigned int)(-4*3200/8)
// ������ 2.2��
#define Time2200 (unsigned int)(-4*2200/8)
// ������ 1.6��
#define Time1600 (unsigned int)(-4*1600/8)
//#define Time1400 (unsigned int)(-4*1400/8)
// ������ 1.3��
//#define Time1300 (unsigned int)(-4*1300/8)
// ������ 1��
#define Time1000 (unsigned int)(-4*1000/8)
// ������ 20��
#define Time20ms (unsigned int)(-10000)
//#define Time20ms (unsigned int)(-4*20000/8)
// ������ 40��
#define Time40ms (unsigned int)(-20000)

//----------------------------------------------------
// ����� � ���� ��� ����. ��������
//----------------------------------------------------

#ifdef PUA1
// ���������� �������� �� ������ ���
#define EnaTX1 PORTE&=~0x04
//  ������ �������� �� ������ ���
#define DisTX1 PORTE|=0x04
// ���������� ������ �� ������ ���
#define EnaRX1 PORTE&=~0x08
// ������ ������ �� ������ ���
#define DisRX1 PORTE|=0x08
// ������� ���������/���������� ��������/������ � ������ ������ � ����
#define EnaTX0 PORTE&=~0x10
#define DisTX0 PORTE|=0x10
#define EnaRX0 PORTE&=~0x20
#define DisRX0 PORTE|=0x20
#define EnaTX2 PORTE&=~0x40
#define DisTX2 PORTE|=0x40
#define EnaRX2 PORTE&=~0x80
#define DisRX2 PORTE|=0x80
#else
#define EnaTX1 PORTE&=~0x10
#define DisTX1 PORTE|=0x10
#define EnaRX1 PORTE&=~0x20
#define DisRX1 PORTE|=0x20
#define EnaTX0 PORTE&=0xBF
#define DisTX0 PORTE|=0x40
#define EnaRX0 PORTE&=0x7F
#define DisRX0 PORTE|=0x80
#define EnaTX2 PORTE&=~0x04
#define DisTX2 PORTE|=0x04
#define EnaRX2 PORTE&=~0x08
#define DisRX2 PORTE|=0x08
#endif

#define    CONTEXT_SWT_vect SPM_RDY_vect

//char IN_CSWT;
#define IN_CSWT ACSR_ACIS1

jmp_buf main_task;
jmp_buf rs_task;
char rs_rstack[8];
char rs_cstack[64];

char rxbuf[16];

#pragma vector=USART0_RXC_vect
__interrupt __raw void rs_rx(void)
{
  UCSR0B_RXCIE0=0;
  if (!IN_CSWT)
  {
    IN_CSWT=1;
    __enable_interrupt();
    asm("call 0x88\n");
    __disable_interrupt();
    IN_CSWT=0;
  }
}

#pragma vector=USART0_UDRE_vect
__interrupt __raw void rs_tx(void)
{
  UCSR0B_UDRIE0=0;  //��������� ����������
  if (!IN_CSWT)
  {
    IN_CSWT=1;
    __enable_interrupt();
    asm("call 0x88\n");
    __disable_interrupt();
    IN_CSWT=0;
  }
}

#pragma vector=USART0_TXC_vect
__interrupt __raw void rs_txc(void)
{
  //UCSR0A=1<<TXC0; //�������� ����
  if (!IN_CSWT)
  {
    IN_CSWT=1;
    __enable_interrupt();
    asm("call 0x88\n");
    __disable_interrupt();
    IN_CSWT=0;
  }
}

#pragma vector=TIMER1_OVF_vect
__interrupt __raw void rs_timeout(void)
{
  UCSR0B_RXCIE0=0; //��������� ���������� �� ���������
  if (!IN_CSWT)
  {
    IN_CSWT=1;
    __enable_interrupt();
    asm("call 0x88\n");
    __disable_interrupt();
    IN_CSWT=0;
  }
}

#pragma vector=CONTEXT_SWT_vect
__interrupt void CNTX_SWT(void)
{
  if (!setjmp(main_task)) //��������� �������� ���. ������
  {
    longjmp(rs_task,1); //������� � �������� RS_TRX
  }
}

void wait_int()
{
  if (!setjmp(rs_task)) //��������� �������� RS_TASK
  {
    longjmp(main_task,1); //������� � �������� ���. ������
  }
}

//txsize - ���������� ���� ��� ����������� �����
__x char DO_RS(char *out, char txsize, char n_rs, char CRC8RX)
{
  char CRC8;
  char c;
  UCSR0A=1<<TXC0; //�������� ���� �� ����������� ����������
  CRC8=CRC8tab[UDR0=(*out++)]; //�� ������ �������� � ����
  while(PINE_Bit1); //���� ������ ���������� ����
  if (n_rs) EnaTX2; else EnaTX0; //��������� ����������
  while(--txsize)
  {
    CRC8=CRC8tab[CRC8^(UDR0=(*out++))];
    UCSR0B_UDRIE0=1; //��������� ���������� �� UDRIE
    wait_int(); //���� ������������ ������
  }
  UDR0=CRC8;
  UCSR0B_TXCIE0=1; //��������� ���������� �� TXC
  wait_int(); //���� ����������� ����� - ��������� ����
  UCSR0B_TXCIE0=0; //��������� ���������� �� TXC
  DisTX2; //��������� ������ ��������
  DisTX0;
  if (n_rs) EnaRX2; else EnaRX0; //��������� �������� (���� ������ �� ��������)
  TCNT1=Time1000;
  TIFR=1<<(TOV1);
  TIMSK_TOIE1=1; //��������� ���������� �� �������
  wait_int(); //���� 1 �� ���� ���������� ��������
  UCSR0B_RXEN0=1; //��������� ��������
  //txsize ����� ����� 0
  CRC8RX=CRC8tab[CRC8RX]; //���. ��������
  out=rxbuf;
  TCNT1=Time2200; //����� �������� ������� �����
  for(;;)
  {
    UCSR0B_RXCIE0=1; //��������� ���������� �� ���������
    wait_int();
    if (!UCSR0A_RXC0)
    {
      //����� �� ��������
      if (CRC8RX) txsize=0; //�� �������� ����������� �����
      break; //������� �� �����
    }
    //������� ����
    if (UCSR0A_FE0||UCSR0A_DOR0)
    {
      txsize=0xFF; //������
    }
    c=UDR0;
    if (txsize!=sizeof(rxbuf))
    {
      CRC8RX=CRC8tab[CRC8RX^(*out=c)]; // ������ �������� ���� � ����� ������ � ���������� CRC
      txsize++;
      out++;
    }
    else
    {
      if (txsize==sizeof(rxbuf))
      {
        txsize=0xFF; //������� ����� ����
        break;
      }
    }
    TCNT1=Time1600;
  }
  UCSR0B_RXEN0=0; //��������� ��������
  TIMSK_TOIE1=0; //��������� ������
  DisRX0;
  DisRX2;
  if (txsize==0xFF) txsize=0;
  return(txsize); //���������� �������� ����, ���� 0 - ��� ������
}

//����� ������ 0x55 ��� ��������� ������
void Do55(char n_rs, char c)
{
  char txsize=10;
  while(c)
  {
    __watchdog_reset();
    TCNT1=Time40ms;
    TIFR=1<<(TOV1);
    TIMSK_TOIE1=1; //��������� ���������� �� �������
    wait_int(); //���� 40 �� ���� ���������� ��������
    TIMSK_TOIE1=0; //��������� ������
    c--;
  }
  __watchdog_reset();
  UCSR0A=1<<TXC0; //�������� ���� �� ����������� ����������
  UDR0=0x55;
  while(PINE_Bit1); //���� ������ ���������� ����
  if (n_rs) EnaTX2; else EnaTX0; //��������� ����������
  while(--txsize)
  {
    UDR0=0x55;
    UCSR0B_UDRIE0=1; //��������� ���������� �� UDRIE
    wait_int(); //���� ������������ ������
  }
  UDR0=0x55;
  UCSR0B_TXCIE0=1; //��������� ���������� �� TXC
  wait_int(); //��������� ����
  UCSR0B_TXCIE0=0; //��������� ���������� �� TXC
  DisTX2; //��������� ������ ��������
  DisTX0;
  TCNT1=Time3200;
  TIFR=1<<(TOV1);
  TIMSK_TOIE1=1; //��������� ���������� �� �������
  wait_int(); //���� 3.2 �� ���� ���������� ��������
  TIMSK_TOIE1=0; //��������� ������
}

//�������� ������� � ������ slave
char SLAVE_RS(char n_rs)
{
  char *out;
  char txsize=0;
  char CRC8RX=0;
  char c;

  if (n_rs)
  {
    DisRX0;
    EnaRX2;
  }
  else
  {
    DisRX2;
    EnaRX0; //��������� �������� (���� ������ �� ��������)
  }
  out=rxbuf;
  TCNT1=Time40ms; //����� �������� ������� �����
  TIFR=1<<(TOV1);
  TIMSK_TOIE1=1; //��������� ���������� �� �������
  UCSR0B_RXEN0=1; //��������� ��������
  for(;;)
  {
    UCSR0B_RXCIE0=1; //��������� ���������� �� ���������
    wait_int();
    if (!UCSR0A_RXC0)
    {
      //����� �� ��������
      if (!txsize) txsize=0xFE; //��� �� ������ �����!
      if (CRC8RX) txsize=0; //�� �������� ����������� �����
      break; //������� �� �����
    }
    //������� ����
    if (UCSR0A_FE0||UCSR0A_DOR0) //���� ������
    {
      txsize=0xFF; //������
    }
    c=UDR0;
    if (txsize<sizeof(rxbuf))
    {
      CRC8RX=CRC8tab[CRC8RX^(*out=c)]; // ������ �������� ���� � ����� ������ � ���������� CRC
      txsize++;
      out++;
    }
    else
    {
      if (txsize==sizeof(rxbuf))
      {
        txsize=0xFF; //������� ����� ����
        break;
      }
    }
    TCNT1=Time1600;
  }
  TIMSK_TOIE1=0; //��������� ������
  if (txsize==0xFF) txsize=0;
  return(txsize);
}

__x void SLAVE_TX(char *out, char txsize, char n_rs)
{
  char CRC8;
  UCSR0A=1<<TXC0; //�������� ���� �� ����������� ����������
  CRC8=CRC8tab[UDR0=(*out++)]; //�� ������ �������� � ����
  while(PINE_Bit1); //���� ������ ���������� ����
  if (n_rs) EnaTX2; else EnaTX0; //��������� ����������
  while(--txsize)
  {
    CRC8=CRC8tab[CRC8^(UDR0=(*out++))];
    UCSR0B_UDRIE0=1; //��������� ���������� �� UDRIE
    wait_int(); //���� ������������ ������
  }
  UDR0=CRC8;
  UCSR0B_TXCIE0=1; //��������� ���������� �� TXC
  wait_int(); //���� ����������� ����� - ��������� ����
  UCSR0B_TXCIE0=0; //��������� ���������� �� TXC
  DisTX2; //��������� ������ ��������
  DisTX0;
}

char isFreeEv(void)
{
  char free=(EventFIFOrp-EventFIFOwp)&0x3f; // ��������� ���������� ���������� ����� � ����� �������
  if (free==8 || free==4)  // ���� � ����� �������� ����� 3 �������
  {
    return(0);
  }
  return(1);
}

void check5(void)
{
  //��� ������ ��������� 5 ������
#ifdef PUA1
#else
  char tled,ckey;
#endif
  __disable_interrupt();
#ifdef PUA1
  if (!(PINF&0xF8)) for (;;); // ���� ������ ��� ���� ������ - ������ ����, �� ����������� ������� - �����
#else
  tled=PORTA;
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
  ckey=PINA;
  PORTA=tled;
  DDRA=0xFF;
  if (!(ckey&0x3E)) for (;;);
#endif
  __enable_interrupt();
}

__task __noreturn RS_TRX(void)
{
  struct
  {
    char PPKP;
    char Cmd;
  };

  static struct
  {
    unsigned long FF;
    unsigned long Second;
    unsigned char SEQ;
  }DPU_TPKT={0xFFFFFFFF,0,0};

  char CurRS=0;
  char abon;
  char c;
  char ca;
  char syserr;
  char sysrs;
//  char dblerr;

  unsigned int tstcount;

  abon=0;
//  dblerr=0;
  if (DOUBLE_PU)
  {
    char slave_seq;
    if (!IS_DOUBLER_DEFAULT) DBLS=40; else DBLS=80; //���� ������ �� ���������, ������� � ��� ���� �������
  L_RECONNECT:
    PPKP=0; //�������� ������ �������� ������
    slave_seq=0;
    do
    {
      check5();
      if ((RebootMode==0x4E)||(RebootMode==0x4F)) RebootMode=0; //����� ��������
      //���������, ��� �� �� ���������� ����. ��
      CurRS^=1; //������ ���������
      do
      {
        __watchdog_reset();
        c=SLAVE_RS(CurRS);
        DBG1=c;
        if (c==10)
        {
          //��� �����
          DBLS=80;
          ca=1; //�������
          if (*((long *)rxbuf)!=-1)
          {
            if (slave_seq==rxbuf[8])
            {
              //SEQ ������ � ���������
              if (isFreeEv())
              {
                //���� ����� � ����� �������
                Event2FIFO(rxbuf[0],rxbuf[1],rxbuf[2],rxbuf[3]);
              }
              else
              {
                ca=2; //�� �������
              }
            }
          }
          else
          {
            //�������� �����
            __disable_interrupt();
            TimeDate=*((unsigned long *)(rxbuf+4)); //��� ������������� ���. �������
            __enable_interrupt();
          }
          if (slave_seq==rxbuf[8])
          {
            //����. ������������� ��������, ������ ����� ������� ������ ��������
            PPKP=0;
          }
          slave_seq=rxbuf[8]^1; //����� ��������� �����
          c=WarmBoot_cmd;
          if ((c==0x4E)||(c==0x4F))
          {
            //���� ������� �����
            PPKP=c; //������ ��������
            WarmBoot_cmd=0; //����� ������� ��������
          }
          UCSR0B_RXEN0=0; //��������� ��������
          DisRX0;
          DisRX2;
          SLAVE_TX(&PPKP,ca,CurRS);
        }
      }
      while(c!=0xFE); //���� ��������� ������ (���� ����� ������)
    }
    while(DBLS);
  }
  ISMASTER=1; //������ ����� - ������
  KEY=kREDRAW; //�������� ���
//  dblerr=0;
  if (DOUBLE_PU)
  {
    DBLE_RS0=32;
    DBLE_RS1=32;
  }
  else
  {
    DBLE_RS0=0;
    DBLE_RS1=0;
  }
  //�����, ����� �� - ������
  tstcount=0; //������� ��� ������������
  for(;;)
  {
    check5();
    if (CommunicateWithPC==1)
    {
      CommunicateWithPC=2;
      wait_int();
    }
    if (TSTS!=16) tstcount=0;
    __watchdog_reset();
    if (!ISMASTER)
    {
      DBLS=100;
      DBLE_RS0=0;
      DBLE_RS1=0;
      goto L_RECONNECT; //���� �������� ������� ������� � slave
    }
    WorkSecond(); // ���� ���� ���� ������� 1�, �������� ����������
    __watchdog_reset();
    if (!abon) //���� ������� - 0
    {
      CurRS^=1; //�������� ����� ���������
      Do55(CurRS,2);
    }
    if (SysPPKP_txsize)
    {
      //���� ������� ��������� �����
      syserr=16;
      c=ABONS[SysPPKP_TXB[0]];
      if ((c&7)>((c>>3)&7)) sysrs=0;
      Do55(sysrs,2);
      do
      {
        if (syserr==8)
        {
          sysrs^=1;
          Do55(sysrs,2);
        }
        c=DO_RS(SysPPKP_TXB,SysPPKP_txsize,sysrs,SysPPKP_TXB[0]);
        __watchdog_reset();
        if (c) break;
      }
      while(--syserr);
      SysPPKP_txsize=0; //��������� �����
      if (!c)
      {
        SysPPKP_rxsize=0xFF; //������ ������
      }
      else
      {
        if (c<8) memcpy(SysPPKP_RXB,rxbuf,c); //�������� �������� ������
        SysPPKP_rxsize=c;
      }
      Do55(CurRS,2); //��������������� ���. RS
    }
    if ((RebootMode==0xFF)||(!isFreeEv()))
    {
      //�� ��������� ����� (��� ������ ��� ����� ��� ����� ��� ���������)
      TCNT1=Time1000;
      TIFR=1<<(TOV1);
      TIMSK_TOIE1=1; //��������� ���������� �� �������
      wait_int(); //���� 1 �� ���� ���������� ��������
      TIMSK_TOIE1=0; //��������� ������
      abon=0;
      continue; //��������� ���� � ������
    }
    if ((RebootMode==0x4E)||(RebootMode==0x4F))
    {
      //����� ������ � �������
//      dblerr=0;
      Cmd=RebootMode;
      syserr=16;
      do
      {
        abon=0;
        ca=0; //����, ��� ���� ������������
        CurRS^=1;
        Do55(CurRS,2);
        do
        {
          if ((ABONS[abon]&0xC0)==0x80)
          {
            //���� ������� ��� �� �������
            PPKP=abon;
            if (DO_RS(&PPKP,2,CurRS,abon)==2)
              ABONS[abon]|=0x40; //������ �����
            else
              ca=1; //���� ������������
            __watchdog_reset();
          }
          abon++;
        }
        while(abon<128); //�� ���� ���������
        if (ca==0) break; //��� ��������
      }
      while(--syserr);
      if (DOUBLE_PU)
      {
        DBLE_RS0=32;
        DBLE_RS1=32;
      }
      RebootMode=0; //�����
      abon=0;
      continue; //C ������
    }
    ca=ABONS[abon];
    if (ca&0x80)
    {
      PPKP=abon|((ca&0x40)<<1); //����� �������� � ACK
      c=DO_RS(&PPKP,1,CurRS,abon);
      ca=ABONS[abon];
      if (!(ca&0x80)) goto L1; //��� ���������
      if ((rxbuf[0]&0x7F)!=abon) c=0; //�� ��� ������� �������, �����!
      switch(c)
      {
      case 4:
        //���� �������
        //���������� ������� � ����
        Event2FIFO(abon,rxbuf[1],rxbuf[2],0);
      case 2:
        //��� �������
        ca^=0x40; //�������� ACK
        if (CurRS)
        {
          //����� ������ ��� RS1
          if ((ca&0x38)!=0x38) ca&=0xC7; //���� �� 7, �����
          if (INDEPENDED_RS)
          {
            //���� ����������� ����������, ����� ������ � � ������ RS
            if ((ca&0x07)!=0x07) ca&=0xF8; //���� �� 7, �����
          }
        }
        else
        {
          //����� ������ ��� RS0
          if ((ca&0x07)!=0x07) ca&=0xF8; //���� �� 7, �����
          if (INDEPENDED_RS)
          {
            //���� ����������� ����������, ����� ������ � � ������ RS
            if ((ca&0x38)!=0x38) ca&=0xC7; //���� �� 7, �����
          }
        }
        if (c==4)
        {
          ABONS[abon]=ca;
          continue;
        }
        tstcount++; //���� �������, ����������� ������� ������
        break;
      default:
        //������
        tstcount=0;
        if (CurRS)
        {
          //������ � RS1
          if ((ca&0x38)!=0x38)
          {
            //�� �������������
            ca+=8;
            if ((ca&0x38)==0x38) Event2FIFO(abon,0xC0,0xFF,1);
          }
        }
        else
        {
          //������ � RS0
          if ((ca&0x07)!=0x07)
          {
            //�� �������������
            ca+=1;
            if ((ca&0x07)==0x07) Event2FIFO(abon,0xC0,0xFF,0);
          }
        }
        if ((ca&0x3F)==0x3F)
        {
          //������ �����, ��������� �� ������������
          ca=0;
        }
        break;
      }
      ABONS[abon]=ca;
    }
    else
    {
      //������� ��������, ���������� ���
      PPKP=abon;
      Cmd=0x4F; //�������� �����
      DO_RS(&PPKP,2,CurRS,CRC8tab[abon]);
    }
    //��������� �������
  L1:
    abon++;
    //���������, �� ���� �� ���������� � ����. ������
    if (DOUBLE_PU&&(abon>=128))
    {
      if (DPUmess_ready)
      {
        //�������� �����
      L_FASTSEND:
        c=DO_RS((char *)(&DPU_MESS),9,CurRS,0); //������ � SEQ
      }
      else
      {
        //�������� ����� � ��������
        __disable_interrupt();
        DPU_TPKT.Second=TimeDate;
        __enable_interrupt();
        DPU_TPKT.SEQ=DPU_SEQ;
        c=DO_RS((char *)(&DPU_TPKT),9,CurRS,0); //������ � SEQ
      }
      if ((c==2)||(c==3))
      {
        //�������� �������� ������
        if (c==2) DPUmess_ready=0; //�������
        DPU_SEQ^=1; //��. ����� ������
        if (INDEPENDED_RS||(CurRS==0)) if (DBLE_RS0) DBLE_RS0=32;
        if (INDEPENDED_RS||(CurRS==1)) if (DBLE_RS1) DBLE_RS1=32;
        c=rxbuf[0];
        if ((c==0x4E)||(c==0x4F))
        {
          //�������� �����
          Event2FIFO(0xff,0xff,4,c); //��������� "�����", �������� �����
        }
        //������ �����, ���� ~40��
        Do55(CurRS,0);
        Do55(CurRS,0);
        Do55(CurRS,0);
        Do55(CurRS,0);
        if (DPUmess_ready) goto L_FASTSEND;
      }
    }
    if (abon>=128)
    {
      abon=0;
      if (TSTS==16)
      {
#ifdef PUA1
        if ((char)tstcount>8)
#else
          if ((char)tstcount>0)
#endif
          {
            tstcount=(tstcount&0xFF00)+0x100;
          }
          else tstcount=0;
        if (tstcount>0x4000)
        {
          TSTS=17; //��������� ����
        }
        TSTDBG=tstcount>>8;
        KEY=kREDRAW; //�����������
      }
    }
  }
}

void InitRS(void)
{
  __disable_interrupt();
  DDRE=0xFE;
  PORTE=0xFF;
  UBRR0L=25;
  UCSR0B_TXEN0=1; //��������� ����������
  TCCR1B=2;
  ((unsigned int *)rs_task)[10]=((unsigned int)rs_rstack)+7; //SP
  ((unsigned int *)rs_task)[8]=((unsigned int)rs_cstack)+64; //Y
  ((unsigned int *)rs_task)[9]=(unsigned int)RS_TRX; //����� ��������
  if (!setjmp(main_task)) longjmp(rs_task,1); //��������� � RS_TRX
  __enable_interrupt();
}
