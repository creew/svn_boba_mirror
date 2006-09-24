//---------------------------------------------------
//  ����� � �� ��� ��������� ������������ �����������
//---------------------------------------------------

static char CRC8;

//����� ����� �� �� � ��������� �������� � ������������ CRC
char rxpc(void)
{
  char c;
  //������������� ������
  _CLI();
  TIFR=4;
  TCNT1=Time3200;
  _SEI();
  while(!((USR&0x80)||(TIFR&4))); //���� ��� ���� ��� �������
  c=UDR;
  CRC8=CRC8tab[CRC8^c]; //��������������� CRC
  return(c); //���������� �������� ����
}

//�������� ����� � �� � ������������ ���
void txpc(char c)
{
  while (!(USR&0x20)); //���� ����������
  CRC8=CRC8tab[CRC8^(UDR=c)]; //��������
}

//���������� ����� � ��
void DoConfigPUpk(void)
{

  char __farflash *fp;
  char c;
  unsigned int page;
  char *p;

  EnaRX1; //��������� �����
  ClrScr();
  sprintf_P(CON,(STR)"����� �� RS2...");
  UpdateConsole();
  do
  {
    UCR=0x10; //��������� ��������
    while(!((USR&0x80)||(KEY!=0))); //������ ���� ����������� ��� ��������
    c=UDR;
    CRC8=CRC8tab[c]; //������� CRC
    if (KEY) //���� ������ ������, ��������� � ������
    {
    }
    else //������� ������ ����
    {
      switch(c) //����� ��������
      {
      case 1: //������ �������
        p=(char *)&page;
        *p++=rxpc(); //������� ���� ������ �������
        if (TIFR&4) break; //���� ������� - �����
        *p=rxpc();   //������� ���� ������ �������
        if (TIFR&4) break; //���� ������� - �����
        rxpc();     //CRC
        if (TIFR&4) break; //���� ������� - �����
        if (CRC8) break; //���� �� ������� CRC - �����
        //�������� ��������
        UCR=0x08;
        DisRX1;
        while(TIFR&4); //���� �������� ��� ����������� ������������
        EnaTX1;
        if (page<0xFE00)
        {
          //������ ������ �� 45??021 � ����� 1
          WaitFlash;
          FlashOn;
          SPDR=0x53;
          WaitSPI;
          SPDR=page>>7;
          WaitSPI;
          SPDR=page<<1;
          WaitSPI;
          SPDR=0;
          WaitSPI;
          FlashOff;
          //������ �� ������ 1
          WaitFlash;
          FlashOn;
          SPDR=FlashCMDbuf1read;
          WaitSPI;
          SPDR=0x00;
          WaitSPI;
          SPDR=0x00;
          WaitSPI;
          SPDR=0x00;
          WaitSPI;
          SPDR=0x00;
          WaitSPI;
          PulseSCK;
          CRC8=0; //��������� �������� CRC
          c=0;
          //�������� �������
          do
          {
            //������ ��������� ����
            SPDR=0xFF;
            WaitSPI;
            txpc(SPDR); //�������� � ��
            c++;
          }
          while (c); //����, ���� �� ��� 256 ���� ��������
          FlashOff;
        }
        else
        {
#pragma diag_suppress=Pe1053
          fp=(char __farflash *) (((unsigned long)(page-0xFE00))<<8);
#pragma diag_default=Pe1053
          CRC8=0; //��������� �������� CRC
          c=0;
          //�������� �������
          do
          {
            //������ ��������� ����
            txpc(*fp++); //�������� � ��
            c++;
          }
          while (c); //����, ���� �� ��� 256 ���� ��������
        }
        txpc(CRC8); //�������� CRC
        delay(2000);
        //��������� ��������
        UCR=0;
        DisTX1;
        EnaRX1;
        break;
      case 2: //������ �������
        p=(char *)&page;
        *p++=rxpc(); //������� ���� ������ �������
        if (TIFR&4) break; //���� ������� - �����
        *p=rxpc(); //������� ���� ������ �������
        if (TIFR&4) break; //���� ������� - �����
        //������ � ����� 1
        WaitFlash;
        FlashOn;
        SPDR=0x84;
        WaitSPI;
        SPDR=0x00;
        WaitSPI;
        SPDR=0x00;
        WaitSPI;
        SPDR=0x00;
        WaitSPI;
        c=0;
        do
        {
          SPDR=rxpc(); //��������� ��������� ���� � ���������� ��� � ����� 1
          if (TIFR&4) break; //���� ������� - �����
          c++;
          WaitSPI; //������� ��������� ������
        }
        while (c); //����, ���� �� ��� 256 ���� �������
        FlashOff;
        if (TIFR&4) break; //���� ����� �� �������� - �� ���������� ������
        rxpc(); //������� CRC
        if (TIFR&4) break; //���� ������� - �����
        if (CRC8) break; //���� �� ������� CRC - �����
        //��������� ��������
        UCR=0x08;
        DisRX1;
        while(TIFR&4); //���� �������� ��� ����������� ������������
        EnaTX1;
        AddFlashSectorCRC(); //��������� �����. �����
        WaitFlash;
        //������ ������� �� ������ 1 � 45??021
        FlashOn;
        SPDR=0x83;
        WaitSPI;
        SPDR=page>>7;
        WaitSPI;
        SPDR=page<<1;
        WaitSPI;
        SPDR=0;
        WaitSPI;
        FlashOff;
        WaitFlash;
        //������ �����������, �������� �������������
        CRC8=0;
        txpc(0);
        txpc(CRC8);
        delay(2000);
        //��������� ��������
        UCR=0;
        DisTX1;
        EnaRX1;
        break;
      case 3: //������ ����� �� EEPROM
        p=(char *)&page;
        *p++=rxpc(); //������� ���� ������ �������
        if (TIFR&4) break; //���� ������� - �����
        *p=rxpc();   //������� ���� ������ �������
        if (TIFR&4) break; //���� ������� - �����
        rxpc();     //CRC
        if (TIFR&4) break; //���� ������� - �����
        if (CRC8) break; //���� �� ������� CRC - �����
        //�������� ��������
        UCR=0x08;
        DisRX1;
        while(TIFR&4); //���� �������� ��� ����������� ������������
        EnaTX1;
        CRC8=0; //��������� �������� CRC
        txpc(*((__eeprom char *)page)); //�������� � ��
        txpc(CRC8); //�������� CRC
        delay(2000);
        //��������� ��������
        UCR=0;
        DisTX1;
        EnaRX1;
        break;
      case 4: //������ ����� �� EEPROM
        p=(char *)&page;
        *p++=rxpc(); //������� ���� ������ �������
        if (TIFR&4) break; //���� ������� - �����
        *p=rxpc();   //������� ���� ������ �������
        if (TIFR&4) break; //���� ������� - �����
        c=rxpc();
        if (TIFR&4) break; //���� ����� �� �������� - �� ���������� ����
        rxpc(); //������� CRC
        if (TIFR&4) break; //���� ������� - �����
        if (CRC8) break; //���� �� ������� CRC - �����
        //��������� ��������
        UCR=0x08;
        DisRX1;
        while(TIFR&4); //���� �������� ��� ����������� ������������
        EnaTX1;
        *((__eeprom char *)page)=c;
        CRC8=0;
        txpc(0);
        txpc(CRC8);
        delay(2000);
        //��������� ��������
        UCR=0;
        DisTX1;
        EnaRX1;
        break;
      }
    }
  }
  while (KEY==0); //����, ���� �� ������ ����� �������
  KEY=0;
  //��������� �����
  UCR=0;
  DisRX1;
  UCSR0B_TXEN0=1; //��������� ����������
}

//������ ������ ������ � ��
void ConfigPUpk(void)
{
  void ConstructVMPC(void);
  CommunicateWithPC=1; //������������� ���� ������������� ������ � ��
  // delay(10000);
  while (CommunicateWithPC!=2); //������� ����� ������������
  //��������� ���������� ������
  _CLI();
  __watchdog_reset();
  WDTCR=0x18;
  WDTCR=0x10;
  __watchdog_reset();
  _SEI();
  DoConfigPUpk(); //���������� ����� � ��
  //��������� ���������� ������
  _CLI();
  __watchdog_reset();
  WDTCR=0x08;
  WDTCR=0x18;
  WDTCR=0x0B; //��������� ���������� ������
  __watchdog_reset();
  TCNT1=Time1000;
  TIFR=1<<(TOV1);
  TIMSK_TOIE1=1; //��������� ���������� �� �������
  CommunicateWithPC=0;
  _SEI();
  if (TSTS==17)
  {
    unsigned long crcflash=0;
    unsigned int sector=0;
    //��������� ������������ - �������� �����. ����� ���� ��������
    ClrScr();
    sprintf_P(CON,(PGM_P)"������� CRC...");
    UpdateConsole();
    do
    {
      crcflash+=FlashSectorCRC(sector,256); //��� ���������� ����������� �����
      sector++;
    }
    while(sector<1024);
    //                   012345678901234567890123
    if (crcflash==0x22de66e)
    {
      TSTS=18;
      TEST_STAT=18;
    }
  }
  ConstructVMPC();
  Event2FIFO(0xff,0xff,4,0x4F);
}

