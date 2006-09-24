typedef struct
{
  unsigned long PP;
  unsigned long SecPP;
  unsigned long LongPP;
  unsigned long SZU;
  unsigned long SDU;
  char RelayCfg;
  /*
  ������� ������������ ���� �� �������� � ������������
  N31  N30  N21  N20  N11  N10  N01  N00
  */

  //  char NpCfg;
  struct
  {
    char AR: 1, nz: 1;
  };
  /*
  x  x  x  x  x  x  CS  AR
  \  \   ����. ���������� ���������� ������
  \  ��� ��

  */

  //  char LongCfg;
  struct
  {
    char Li: 5,L2en:1, Lnz:1;
  };

  /*
  x �� EN  I4 I3 I2 I1 I0
  \  \   ����� ����� ��� ��� �o���������� (0..31)
  \  ���������� ������� ������� � �������� �������������
  ���� ��
  */
  //  char WCfg;
  struct
  {
    char Wi: 5,Wen:1, Wnz:1;
  };
  /*
  x �� EN  I4 I3 I2 I1 I0
  \   \   ����� ����� ��� �������� (0..31)
  \   ���������� �����
  ���� ��
  */
  char Delay; //�������� �� ����� � �������� 192-234
  char Interval; //�������� ����� ��������� 193-245
  char DelayLong; //�������� �� ������������ 246-248
  char IntervalLong; //�������� ������������ 24
  char reserved[4];
}PKPCFG;

#define PKPCFG_BASE 1

void ConfigPKP(char ppkp)
{
  PKPCFG cnf;
  char writeflag=0;
  char i;
  char c;
  char pos=8;
  char adr=0;
  char *p;
  char val;
  ClrScr();

  do
  {
    adr&=3;
    ClrScr();
    sprintf_P(CON,(STR)"���%02d ��%01d",ppkp,adr+1);
    switch(pos>>5)
    {
    case 0:
      CursorPos=8;
      break;
    case 1: //32
      sprintf_P(CON+10,(STR)"���� ��������� ������:");
      p=(char *)(&(cnf.PP));
      goto L_PP;
    case 2: //64
      sprintf_P(CON+10,(STR)"���� ���������� ������:");
      p=(char *)(&(cnf.SecPP));
      goto L_PP;
    case 3: //96
      sprintf_P(CON+10,(STR)"���� ������������:");
      p=(char *)(&(cnf.LongPP));
      goto L_PP;
    case 4: //128
      sprintf_P(CON+10,(STR)"���� ���:");
      p=(char *)(&(cnf.SZU));
      goto L_PP;
    case 5: //160
      sprintf_P(CON+10,(STR)"���� ���:");
      p=(char *)(&(cnf.SDU));
      goto L_PP;
    case 6: //192
      if (pos<204)
      {
        c=pos-192;
        switch(c/3)
        {
        case 0:
          sprintf_P(CON+40,(STR)"%03d ������ �� ����� ���",cnf.Delay);
          break;
        case 1:
          sprintf_P(CON+40,(STR)"%03d ������ ����� ������ ��",cnf.Interval);
          break;
        case 2:
          sprintf_P(CON+40,(STR)"%03d ������ �� ����� ��� ������.",cnf.DelayLong);
          break;
        case 3:
          sprintf_P(CON+40,(STR)"%03d ������ ����� ������� �� ������.",cnf.IntervalLong);
          break;
        }
        CursorPos=c%3+40;
        break;
      }
      switch(pos)
      {
      case 204:
        sprintf_P(CON+40,cnf.AR?(STR)"�������������� ���� �������":
                    (STR)"������ ���� �������");
        CursorPos=40;
        break;

      case 205:
        sprintf_P(CON+40,cnf.nz?(STR)"��� - ��":
                    (STR)"��� - ��");
        CursorPos=40;
        break;
      case 206:
      case 207:
        sprintf_P(CON+40,(STR)"%02d - ���� ��� ��� ������������",cnf.Li+1);
        CursorPos=pos-206+40;
        break;
      case 208:
        sprintf_P(CON+40,cnf.Lnz?(STR)"��� ��� ������������ - ��":
                    (STR)"��� ��� ������������ - ��");
        CursorPos=40;
        break;
      case 209:
        sprintf_P(CON+40,cnf.L2en?(STR)"������ ������ ������������ ��������":
                    (STR)"������ ������ ������������ ��������");
        CursorPos=40;
        break;
      case 210:
        sprintf_P(CON+40,cnf.Wen?(STR)"��� ������������":
                    (STR)"��� �� ������������");
        CursorPos=40;
        break;
      case 211:
      case 212:
        sprintf_P(CON+40,(STR)"%02d - ���� ���",cnf.Wi+1);
        CursorPos=pos-211+40;
        break;
      case 213:
        sprintf_P(CON+40,cnf.Wnz?(STR)"��� - ��":
                    (STR)"��� - ��");
        CursorPos=40;
        break;
      }
      break;
    L_PP:
      c=0;
      do
      {
        CON[40+c]=p[c>>3]&(1<<(c&7))?(c+1)%10+'0':'-';
        c++;
      }
      while(c<32);
      sprintf_P(CON+40+33,(STR)"1-%02d-32",(pos&31)+1);
      CursorPos=40+(pos&31);
      break;
    }
    c=KBD(0); //���� �������
    if ((c>=32 && c<128)||(c>=192)) //���� ���������� �������
    {
      if (Edit_db(c,pos,192,&(cnf.Delay))) {c=kRT; writeflag=1;}
      if (Edit_db(c,pos,195,&(cnf.Interval))) {c=kRT; writeflag=1;}
      if (Edit_db(c,pos,198,&(cnf.DelayLong))) {c=kRT; writeflag=1;}
      if (Edit_db(c,pos,201,&(cnf.IntervalLong))) {c=kRT; writeflag=1;}
      val=cnf.Li;
      if (Edit_dc(c,pos,206,&val)) {cnf.Li=val; c=kRT; writeflag=1;}
      val=cnf.Wi;
      if (Edit_dc(c,pos,211,&val)) {cnf.Wi=val; c=kRT; writeflag=1; }
    }
    switch(c)
    {
    case kESC:
      goto LWR;
    case kLT:
      switch(pos)
      {
      case 8: c=kESC; goto LWR;
      case 32: pos=8;
      LWR:
        if (writeflag)
        {
          ClrScr();
          UpdateConsole();
          i=0;
          p=(char *)&cnf;
          do
          {
            sprintf_P(CON,(STR)"������ ������������ %03d...",i);
            UpdateConsole();
            _CLI();
            SysPPKP_rxsize=0;
            SysPPKP_TXB[0]=ppkp+96;
            SysPPKP_TXB[1]=0x48;
            SysPPKP_TXB[2]=adr*sizeof(cnf)+i+PKPCFG_BASE;
            SysPPKP_TXB[3]=*p++;
            SysPPKP_txsize=4;
            _SEI();
            while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
            i++;
          }
          while(i<sizeof(cnf));
        }
        writeflag=0;
        break;
      default: pos--; break;
      }
      break;
    case kRT:
      switch(pos)
      {
      case 8:
        writeflag=0;
        adr&=3;
        i=0;
        p=(char *)&cnf;
        do
        {
          sprintf_P(CON,(STR)"������ ������������ %03d...",i);
          UpdateConsole();
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp+96;
          SysPPKP_TXB[1]=0x49;
          SysPPKP_TXB[2]=adr*sizeof(cnf)+i+PKPCFG_BASE;
          SysPPKP_txsize=3;
          _SEI();
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
          if (SysPPKP_rxsize!=3) return;
          *p++=SysPPKP_RXB[1];
          i++;
        }
        while(i<sizeof(cnf));
        pos=32;
        break;
      case 213: break;
      default: pos++;
      }
      break;
    case kUP:
      switch(pos>>5)
      {
      case 0:
        Select_dc(pos,7,1,&adr);
        continue;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
        i=pos&31;
        p[i>>3]|=(1<<(i&7));
        break;
      }
      writeflag=1;
      Select_db(pos,192,1,&(cnf.Delay));
      Select_db(pos,195,1,&(cnf.Interval));
      Select_db(pos,198,1,&(cnf.DelayLong));
      Select_db(pos,201,1,&(cnf.IntervalLong));
      val=cnf.Li;
      Select_dc(pos,206,1,&val);
      cnf.Li=val;
      if (pos==204) cnf.AR=1;
      if (pos==205) cnf.nz=1;
      if (pos==208) cnf.Lnz=1;
      if (pos==209) cnf.L2en=1;
      val=cnf.Wi;
      Select_dc(pos,211,1,&val);
      cnf.Wi=val;
      if (pos==210) cnf.Wen=1;
      if (pos==213) cnf.Wnz=1;
      break;
    case kDW:
      switch(pos>>5)
      {
      case 0:
        Select_dc(pos,7,-1,&adr);
        continue;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
        i=pos&31;
        p[i>>3]&=~(1<<(i&7));
        break;
      }
      writeflag=1;
      Select_db(pos,192,-1,&(cnf.Delay));
      Select_db(pos,195,-1,&(cnf.Interval));
      Select_db(pos,198,-1,&(cnf.DelayLong));
      Select_db(pos,201,-1,&(cnf.IntervalLong));
      val=cnf.Li;
      Select_dc(pos,206,-1,&val);
      cnf.Li=val;
      if (pos==204) cnf.AR=0;
      if (pos==205) cnf.nz=0;
      if (pos==208) cnf.Lnz=0;
      if (pos==209) cnf.L2en=0;
      val=cnf.Wi;
      Select_dc(pos,211,-1,&val);
      cnf.Wi=val;
      if (pos==210) cnf.Wen=0;
      if (pos==213) cnf.Wnz=0;
      break;
    }
  }
  while(c!=kESC);
}


void ConfigBRAdelays(char ppkp)
{
  static __flash char mess[]="����: %01d ����� ���������: %03d ���";
  char c;
  char pos=6;
  char readflag=1;
  char writeflag=0;
  char adr=1;
  char val;
  memset(CON+40,32,40);
  do
  {
    adr--;
    adr&=7;
    adr++;
    if (readflag)
    {
      _CLI();
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=ppkp+96;
      SysPPKP_TXB[1]=0x49;
      SysPPKP_TXB[2]=adr;
      SysPPKP_txsize=3;
      _SEI();
      while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
      val=SysPPKP_RXB[1];
      readflag=0;
    }
    sprintf_P(CON+40,mess,adr,val); //�������� ���������
    CursorPos=pos+40; //���������� ������
    c=KBD(0); //���� �������
    UpdateConsole();
    if ((c>=32 && c<128)||(c>=192)) //���� ���������� �������
    {
      if (Edit_dc(c,pos,5,&adr)) { c=kRT;readflag=1;} //�������� ������������� ����� ����
      if (Edit_db(c,pos,25,&val)) {c=kRT;writeflag=1;}
    }
    switch(c)
    {
      //�������� ���������� ������ �����
    case kESC:
      goto L1;
    case kLT:
      switch (pos)
      {
      case 6: c=kESC;
      case 25:
        if (writeflag)
        {
        L1:
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp+96;
          SysPPKP_TXB[1]=0x48;
          SysPPKP_TXB[2]=adr;
          SysPPKP_TXB[3]=val;
          SysPPKP_txsize=4;
          _SEI();
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
          writeflag=0;
        }
        pos=6; break;
      default: pos--;
      }
      break;
      // �������� ���������� ������ ������
    case kRT:
      switch (pos)
      {
      case 27:
        break;
      case 6: pos=25; break;
      default: pos++;
      }
      break;
    case kUP:
      if (pos!=6)
        writeflag=1;
      else
        readflag=1;
      Select_dc(pos,5,1,&adr);
      Select_db(pos,25,1,&val);
      break;
    case kDW:
      if (pos!=6)
        writeflag=1;
      else
        readflag=1;
      Select_dc(pos,5,-1,&adr);
      Select_db(pos,25,-1,&val);
      break;
    }
  }
  while (c!=kESC); //����, ���� �� �����
}

//���������� ����� ���� "������������ ���"
void ConfigBRA(void)
{
  //
  static __flash char Message_EBRA[]="���: %02d ";
  static __flash char messOFF[]=                "��������";
  static __flash char  messON[]=                "�������";

  static __flash char messPRESENT[]="����";
  static __flash char messABSENT[]="��� ";

  static char ppkp;
  static char pos=5;
  char c;
  TestPWD(40); //��������� ������
  do
  {
    ppkp&=31;
    ClrScr();
    sprintf_P(CON,Message_EBRA,ppkp); //�������� ���������
    //�������� ���������
    sprintf_P(CON+8,(BRA_AREA[ppkp]&0x80)?messON:messOFF);
    sprintf_P(CON+17,(BRA_AREA[ppkp]&0x40)?messPRESENT:messABSENT);
    CursorPos=pos; //���������� ������
    c=KBD(0); //���� �������
    UpdateConsole();
    if ((c>=32 && c<128)||(c>=192)) //���� ���������� �������
    {
      if (Edit_dc(c,pos,5,&ppkp)) { c=kRT;} //�������� ������������� ����� ����
    }
    switch(c)
    {
      //�������� ���������� ������ �����
    case kLT:
      switch (pos)
      {
      case 5: c=kESC; break;
      case 8: pos=6; break;
      case 17: pos=8; break;
      default: pos--;
      }
      break;
      // �������� ���������� ������ ������
    case kRT:
      switch (pos)
      {
      case 17:
        if ((PWDmode==3)&&(ABONS[ppkp+96]&0x80))
        {
          //������, ��� �� ������
          _CLI();
          //NeedNoError=1;
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp+96;
          SysPPKP_TXB[1]=0x49;
          SysPPKP_TXB[2]=0;
          SysPPKP_txsize=3;
          _SEI();
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
          if (SysPPKP_rxsize!=3) break;
          if (SysPPKP_RXB[1]&0x80)
          {
            ConfigPKP(ppkp);
          }
          else
          {
            ConfigBRAdelays(ppkp);
          }
        }
        break;
      case 8: pos=17; break;
      case 6: pos=8; break;
      default: pos++;
      }
      break;
    case kUP:
      if (pos==8&&PWDmode>=2) //���� � ������ ������� � ������ �������, �������� ����
      {
        if (BRA_AREA[ppkp]&0x40)
        {
          BRA_AREA[ppkp]|=0x80;
          ABONS[ppkp+96]=0x80; //��������
          Event2FIFO(ppkp+96,0xC0,0xFE,BRA_AREA[ppkp]);
        }
      }
      if (pos==17&&PWDmode==3)//���� � ������ ������� � ������ �������, �������� ����� ����������
      {
        BRA_AREA[ppkp]=0xC0;
        ABONS[ppkp+96]=0x80; //��������
        Event2FIFO(ppkp+96,0xC0,0xFE,BRA_AREA[ppkp]);
      }
      Select_dc(pos,5,1,&ppkp); //�������� ���������� ����� ����
      break;
    case kDW:
      if (pos==8&&PWDmode>=2) //���� � ������ ������� � ������ �������, �������� ����
      {
        if (((BRA_AREA[ppkp]&=~0x80)&0xC0)==0x40) IncOfflineCounter();
        ABONS[ppkp+96]=0; //���������
        Event2FIFO(ppkp+96,0xC0,0xFD,BRA_AREA[ppkp]);
      }
      if (pos==17&&PWDmode==3)//���� � ������ ������� � ������ �������, �������� ����� ����������
      {
        BRA_AREA[ppkp]=0;
        ABONS[ppkp+96]=0; //���������
        Event2FIFO(ppkp+96,0xC0,0xFD,BRA_AREA[ppkp]);
      }
      Select_dc(pos,5,-1,&ppkp);//�������� ���������� ����� ����
      break;
    }
  } while (c!=kESC); //����, ���� �� �����
}




