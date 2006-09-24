//--------------------------------------
// ���������� �������
//--------------------------------------

void DoBoot(char cmd)
{
  char i;
  unsigned int zp;
  unsigned int zone;
  static __flash char mess[]="����� �������: ...";
  //static __flash char mess2[]="DBG1: %02X";

  RebootMode=0xFF; //������ ������ � ��-�

  ClrScr();
  sprintf_P(CON,mess);
  UpdateConsole();
  i=0;
  do
  {
    VMPC[i]=0;
    MacroTimers[i]=0;
    i++;
  }
  while(i<16);
  ConstructVMPC();
  i=0;
  do
  {
    BitFlags[i]=0;
    i++;
  }
  while(i<32);
  ConstructABONS(); //��������� ������ �� ��-� � ��� �� ������� ������ ������ ������ � ��-�
  TotalFires=0; //����� ���������� ������� = 0
  SndMode=0; //��������� ����
  IsPreAtt=0;
  LED=0; //��������� ����������
  LEDflash=0; //��������� �������
  IsSuprDZ=0; //��� ��� � ����. ������
  FlagsCAN=0;
  //�������� ������� � ���������� �������� ��� ���� ���
  i=0;
  do
  {
    _CLI();
    ZoneTF[i<<1]=0; //�������� ������
    ZoneTF[(i<<1)+1]=0; //�������� ���������� ��������
    _SEI();
    i++; //��������� ����
  }
  while (i!=0); //����, ���� �� ��������� ��� ����
  _CLI();
#ifdef PUA1
  PORTB&=~0x10; //��������� ���� "�����"
  PORTB&=~0x40; //��������� ���� "����� 120�"
#else
  PORTD&=0xCF;
#endif
  if (!(PU_STAT&4))
  {
#ifdef PUA1
        PORTB|=0x40; //��������� ���� "�����"
#else
        PORTD|=0x10;
#endif
  }
  Fire120sec=0xFFFFFFFF; //�������� ������ 120�
  _SEI();
  mrv1relay=0;
  mrv2relay=0;
  for (i=0;i<16;i++)
  {
    WriteI2C(0x92,1,0);
    WriteI2C(0x94,1,0);
  }
  TADEVoff=0; //�������� ����� ������ ������ � ��� � ��
  // ���������� ������� EPUS �� ������ ���
  zone=0;
  do
  {
    zp=ZONEPOINT_AREA[zone++];
    if ((zp&0xC000)==0x8000)
    {
      EPUS[(zp>>8)&0x3F]=0x80;
    }
  }
  while (zone<(256*4));
  zone=0;
  do
  {
    ZoneDelays[zone]=0xFE;
    zone++;
  }
  while (zone<1024);
  //--------------------------------
  //�������� ���� ��-� �������� ��������
  RebootMode=cmd;
  while(RebootMode) //���� ��������� ������
  {
    //   ClrScr();
    //   sprintf_P(CON,mess2,DBG1);
    //   UpdateConsole();
  }
}

//���������� ����������
//�� ����� �������� ��������, ������� ���������� � ��-�
void WarmBoot(char cmd)
{
  ClrScr();
  sprintf_P(CON,(STR)"������� ��� ��� ������ �������!");
  if (XTestPWD(40)<2) return; //�������� ���
  WarmBoot_cmd=cmd;
  if (ISMASTER) Event2FIFO(0xff,0xff,4,cmd); //��������� "�����"
}

//������ �������� �������
void FullCheck(void)
{
  unsigned long td;
  char s1;
  char s2;
  char s3;
  _CLI();
  td=TimeDate+2; //������ 2 ���
  s1=LEDflash;
  s2=LED;
  s3=SndMode;
  for(;;)
  {
    _SEI();
    SndMode=1; //�������� ����
    SetLEDon(LEDfire|LEDmfire|LEDfail|LEDoff|LEDsndoff|LEDsysf); //�������� ��� ����������
    _CLI();
    if (td<TimeDate) break; //�����, ���� ������ 2�
  }
  LEDflash=s1;
  LED=s2;
  SndMode=s3;
  _SEI();
  //WarmBoot(0x4E); //������ ����������
}

//������� "���������� �������"
void Boot(void)
{
  char c,i=0;
  do
  {
    //�������� ����
    ClrScr();
    sprintf_P(CON,Message_MainMenu[2]);
    sprintf_P(CON+40,Message_Boot[i]);
    sprintf_P(CON+66,(STR)"<������>-�����");
    c=KBD(0); //���� �������
    if (c>=0x11 && c<=0x14) //���������� �������������� ������
    {
      i=c-0x11;
      c=kRT;
    }
    switch(c)
    {
    case kRT:
      switch(i)
      {
      case 0: WarmBoot(0x4E); break; //���������� � ��������� "������� ���������� ��-�"
      case 1: WarmBoot(0x4F); break; //���������� � ��������� "�������� ���������� ��-�"
      case 2: FullCheck(); break;
      case 3: if (DOUBLE_PU) ISMASTER=0; break;
      } ClrScr(); break;
    case kUP: if (i>0) i--; break;
    case kDW: if (i<3) i++; break;
    }
  } while (c!=kLT); //����, ���� �� �����
}
