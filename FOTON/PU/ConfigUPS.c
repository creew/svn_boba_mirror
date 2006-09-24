// ���������� ����� ��������� ���� "������������ ���"
void ConfigUPS(void)
{
  //
  static __flash char Message_EUPS[]="���: %02d ";
  static __flash char messOFF[]=                "��������";
  static __flash char  messON[]=                "������� ";
  static __flash char messL1[]="�1:";
  static __flash char messL2[]="�2:";

  static __flash char messPRESENT[]="����";
  static __flash char messABSENT[]="��� ";

  static __flash char messU[]="U���=%02d.%02d� I���=%04d�� ������ %04X%04X";

  char state=255; //��������� �������� ������

  unsigned int umax;
  unsigned int usens;
  unsigned int isens;
  unsigned long tmr;

  char sec;

  static char ppkp;
  static char pos=5;
  char c=kREDRAW;
  TestPWD(40); //��������� ������
  ClrScr();
  sec=TimeDate;
  for(;;)
  {
    if (c==kREDRAW)
    {
      memset(CON,' ',40);
      ppkp&=31;
      sprintf_P(CON,Message_EUPS,ppkp); //�������� ���������
      //�������� ���������
      sprintf_P(CON+8,(UPS_AREA[ppkp]&0x80)?messON:messOFF);
      sprintf_P(CON+17,(UPS_AREA[ppkp]&0x40)?messPRESENT:messABSENT);
      sprintf_P(CON+23,messL1);
      sprintf_P(CON+26,(UPS_AREA[ppkp]&0x01)?messPRESENT:messABSENT);
      sprintf_P(CON+32,messL2);
      sprintf_P(CON+35,(UPS_AREA[ppkp]&0x02)?messPRESENT:messABSENT);
      CursorPos=pos; //���������� ������
      UpdateConsole();
    }
    if (FIFO2EventLog())
    {
      Timer30sec=0;
      goto KEY_READY;
    }
    switch(state)
    {
    case 0:
      if (sec==(char)TimeDate) break;
      sec=TimeDate;
      //�������� ������ �� ������ umax
      _CLI();
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=ppkp+64;
      SysPPKP_TXB[1]=0x49;
      SysPPKP_TXB[2]=3;
      SysPPKP_txsize=3;
      _SEI();
      state++;
      break;
    case 1:
      if (SysPPKP_rxsize)
      {
        if (SysPPKP_rxsize==4)
        {
          umax=*((unsigned int *)(SysPPKP_RXB+1));
          //�������� ������ �� ������ usens
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp+64;
          SysPPKP_TXB[1]=0x49;
          SysPPKP_TXB[2]=1;
          SysPPKP_txsize=3;
          _SEI();
          state++;
        }
        else goto LERR;
      }
      break;
    case 2:
      if (SysPPKP_rxsize)
      {
        if (SysPPKP_rxsize==4)
        {
          usens=*((unsigned int *)(SysPPKP_RXB+1));
          //�������� ������ �� ������ isens
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp+64;
          SysPPKP_TXB[1]=0x49;
          SysPPKP_TXB[2]=2;
          SysPPKP_txsize=3;
          _SEI();
          state++;
        }
        else goto LERR;
      }
      break;
    case 3:
      if (SysPPKP_rxsize)
      {
        if (SysPPKP_rxsize==4)
        {
          isens=*((unsigned int *)(SysPPKP_RXB+1));
          //�������� ������ �� ������ ������� �������
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp+64;
          SysPPKP_TXB[1]=0x49;
          SysPPKP_TXB[2]=4;
          SysPPKP_txsize=3;
          _SEI();
          state++;
        }
        else goto LERR;
      }
      break;
    case 4:
      if (SysPPKP_rxsize)
      {
        if (SysPPKP_rxsize==6)
        {
          unsigned long l=usens*2800L;
          tmr=*((unsigned long *)(SysPPKP_RXB+1));
          l/=umax;
          usens=l;
          l=isens*0x14000L; //1.25*65536
          isens=l>>16;
          //��������, ��� ���������
          sprintf_P(CON+40,messU,
                    usens/100,
                    usens%100,
                    isens,
                    (unsigned int)(tmr>>16),
                    (unsigned int)tmr
                      ); //�������� ���������
        }
        else
        {
        LERR:
          memset(CON+40,' ',40);
        }
        state=255;
      }
      break;
    default:
      if (SysPPKP_rxsize)
      {
        state=0;
        CursorPos=pos; //���������� ������
        UpdateConsole();
      }
      break;
    }
    c=KEY;
    if ((c=KEY)==0) continue; //��� �������
  KEY_READY:
    CursorPos=pos; //���������� ������
    c=KBD(0); //���� �������
    state=255;
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
      case 26: pos=17; break;
      case 35: pos=26; break;
      default: pos--;
      }
      break;
      // �������� ���������� ������ ������
    case kRT:
      switch (pos)
      {
      case 35: break;
      case 26: pos=35; break;
      case 17: pos=26; break;
      case 8: pos=17; break;
      case 6: pos=8; break;
      default: pos++;
      }
      break;
    case kUP:
      if (pos==8&&PWDmode>=2) //���� � ������ ������� � ������ �������, �������� ����
      {
        if (UPS_AREA[ppkp]&0x40)
        {
          UPS_AREA[ppkp]|=0x80;
          ABONS[ppkp+64]=0x80; //��������
          Event2FIFO(ppkp+64,0xC0,0xFE,UPS_AREA[ppkp]);
        }
      }
      if (pos==17&&PWDmode==3)//���� � ������ ������� � ������ �������, �������� ����� ����������
      {
        UPS_AREA[ppkp]=0xC0;
        ABONS[ppkp+64]=0x80; //��������
        Event2FIFO(ppkp+64,0xC0,0xFE,UPS_AREA[ppkp]);
      }
      Select_dc(pos,5,1,&ppkp); //�������� ���������� ����� ����
      if (pos==26&&PWDmode==3)
      {
        //����� 1
        UPS_AREA[ppkp]|=1;
      }
      if (pos==35&&PWDmode==3)
      {
        //����� 2
        UPS_AREA[ppkp]|=2;
      }
      break;
    case kDW:
      if (pos==8&&PWDmode>=2) //���� � ������ ������� � ������ �������, �������� ����
      {
        if (((UPS_AREA[ppkp]&=~0x80)&0xC0)==0x40) IncOfflineCounter();
        ABONS[ppkp+64]=0; //���������
        Event2FIFO(ppkp+64,0xC0,0xFD,UPS_AREA[ppkp]);
      }
      if (pos==17&&PWDmode==3)//���� � ������ ������� � ������ �������, �������� ����� ����������
      {
        UPS_AREA[ppkp]=0;
        ABONS[ppkp+64]=0; //���������
        Event2FIFO(ppkp+64,0xC0,0xFD,UPS_AREA[ppkp]);
      }
      if (pos==26&&PWDmode==3)
      {
        //����� 1
        UPS_AREA[ppkp]&=~1;
      }
      if (pos==35&&PWDmode==3)
      {
        //����� 2
        UPS_AREA[ppkp]&=~2;
      }
      Select_dc(pos,5,-1,&ppkp);//�������� ���������� ����� ����
      break;
    }
    if (c==kESC) break;
    c=kREDRAW;
  }
}



