//---------------------------------------------------------
// ������ � 45??021
//---------------------------------------------------------

#define FlashTypeDB
//��� ���������� 45DB021
#ifdef FlashTypeDB
//�������������� ������� CLK
#define PulseSCK  {}
//�������� ������ ������ 1
#define FlashCMDbuf1read 0xD4
//�������� ������ ������ 2
#define FlashCMDbuf2read 0xD6
//�������� ������ �������
#define FlashCMDstatusread 0xD7
#else //��� ���������� 45D021
//�������������� ������� CLK
#define PulseSCK  {SPCR=0x00;PORTB&=0xFD;PORTB|=0x02;SPCR=0x5c;}
//�������� ������ ������ 1
#define FlashCMDbuf1read 0x54
//�������� ������ ������ 2
#define FlashCMDbuf2read 0x56
//�������� ������ �������
#define FlashCMDstatusread 0x57
#endif

//������ �������
char ReadFlashStatus(void)
{
  FlashOn;
  SPDR=FlashCMDstatusread;
  WaitSPI;
  PulseSCK;
  SPDR=0xFF;
  WaitSPI;
  FlashOff;
  return(SPDR);
}

//�������� ����������� ����� � ����� �������
void AddFlashSectorCRC(void)
{
  char c=0;
  char crc=0;
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
  //��������� �������
  do
  {
    //������ ��������� ����
    SPDR=0xFF;
    WaitSPI;
    crc+=SPDR;
    c++;
  }
  while (c); //����, ���� �� ��� 256 ����
  FlashOff;
  //������ � �����
  WaitFlash;
  FlashOn;
  SPDR=0x84;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=0x01; //���������� ���� ����� 256
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=-crc; //��������, �������� CRC
  WaitSPI;
  FlashOff;
}

//������ ������������ ����������
//�� ����� ����� ����, ����� ����������, ��������� �� ���������
//������ � ��������� 0-0x1FFFF (128�����=64*64*32)

void ReadSensorConfig(char ppkp, char sensor, struct TSensorConfig *s)
{
  int page;
  char disp;
  char i;
  char *p;

  //���������� ������
  p=(char*)s;
  page=(ppkp<<4)+(sensor>>2);
  disp=sensor<<5;
  //�������� ������ �������
  WaitFlash;
  FlashOn;
  SPDR=0x53;
  WaitSPI;
  SPDR=page>>8;
  WaitSPI;
  SPDR=page;
  WaitSPI;
  SPDR=0;
  WaitSPI;
  FlashOff;
  WaitFlash;
  //�������� ������ ������
  FlashOn;
  SPDR=FlashCMDbuf1read;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=disp;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  PulseSCK;
  for (i=0;i<32;i++)
  {
    SPDR=0xff;
    WaitSPI;
    *p++=SPDR;
  }
  FlashOff;
  //�������� �� ���������� �������
  for (i=0;i<24;i++)
  {
    if (s->GeoPos[i]<32||(s->GeoPos[i]>=128&&s->GeoPos[i]<192)) s->GeoPos[i]='?';
  }
  s->Type&=0x9f; //�������� �� ���������� ���
}

//������ ������������ ����������
//�� ����� ����� ����, ����� ����������, ��������� �� ���������
//������ � ��������� 0-0x1FFFF (128�����=64*64*32)
void WriteSensorConfig(char ppkp, char sensor, struct TSensorConfig *s)
{
  int page;
  char disp;
  char i;
  char *p;

  //���������� �������
  p=(char*)s;
  page=(ppkp<<4)+(sensor>>2);
  disp=sensor<<5;
  //������ �������
  WaitFlash;
  FlashOn;
  SPDR=0x53;
  WaitSPI;
  SPDR=page>>8;
  WaitSPI;
  SPDR=page;
  WaitSPI;
  SPDR=0;
  WaitSPI;
  FlashOff;
  //������ � �����
  WaitFlash;
  FlashOn;
  SPDR=0x84;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=disp;
  WaitSPI;
  for (i=0;i<32;i++)
  {
    SPDR=*p++;
    WaitSPI;
  }
  FlashOff;
  //������ ����������� �����
  AddFlashSectorCRC();
  WaitFlash;
  //������ �������
  FlashOn;
  SPDR=0x83;
  WaitSPI;
  SPDR=page>>8;
  WaitSPI;
  SPDR=page;
  WaitSPI;
  SPDR=0;
  WaitSPI;
  FlashOff;
}

//������ ������� �� ������
//�� ����� - ��������� �� ���������
//����� ������� ����������� �� LogFileRP
//�������� ������� 0x20000-0x28000 (32������=4096*8)

void ReadEvent(int LogFileRP ,struct TLogEvent *e)
{
  int page;
  char disp;
  char i;
  char *p;

  //���������� ������
  p=(char*)e;
  LogFileRP&=0xFFF;
  page=(LogFileRP>>4)+0x400;
  disp=LogFileRP<<3;
  /*if ((LogFileRP&0x0FE0)==(LogFileWP&0x0FE0))   //�������� �������� � ������� ����� ��� ������
  {
  //������ �� ������ ��� ������
  WaitFlash;
  FlashOn;
  SPDR=FlashCMDbuf2read;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=disp;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  PulseSCK;
  for (i=0;i<8;i++)
  {
  SPDR=0xff;
  WaitSPI;
  *p++=SPDR;
}
  FlashOff;
}
 else
  */
  //������ ����� ����� 1
  {
    //������ �������
    WaitFlash;
    FlashOn;
    SPDR=0x53;
    WaitSPI;
    SPDR=page>>8;
    WaitSPI;
    SPDR=page;
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
    SPDR=disp;
    WaitSPI;
    SPDR=0x00;
    WaitSPI;
    PulseSCK;
    for (i=0;i<8;i++)
    {
      SPDR=0xff;
      WaitSPI;
      *p++=SPDR;
    }
    FlashOff;
  }
}

//������ ������� � �����
//�� ����� - ��������� �� ���������
//����� ������� ����������� �� LogFileWP � ���������� ���������������
//�������� ������� 0x20000-0x28000 (32������=4096*8)

void WriteEvent(struct TLogEvent *e)
{
  int page;
  char disp;
  char i;
  char *p;

  //���������� �������
  p=(char*)e;
  LogFileWP&=0xFFF;
  page=(LogFileWP>>4)+0x400;
  disp=LogFileWP<<3;
  //������ � ����� 2
  WaitFlash;
  FlashOn;
  SPDR=0x87;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  //SPDR=disp;
  SPDR=0; //� ������ �������
  WaitSPI;
  i=0;
  do
  {
    if ((i&0xF8)==disp)
      SPDR=*p++; //����������� �����
    else
      SPDR=0xFF; //��������� - �� ��������
    WaitSPI;
  }
  while(++i);
  FlashOff;
  //���������� ����� 2
  WaitFlash;
  FlashOn;
  SPDR=0x89; //������ ��� ��������
  WaitSPI;
  SPDR=page>>8;
  WaitSPI;
  SPDR=page;
  WaitSPI;
  SPDR=0;
  WaitSPI;
  FlashOff;

  LogFileWP=(LogFileWP+1)&0xFFF; //������� �� ��������� �����
  if (!(LogFileWP & 0x1f))     //������� ����� ������� �������
  {
    //������������� ����� �������
    page=(LogFileWP>>4)+0x400;
    //������� ��������� ������
    WaitFlash;
    FlashOn;
    SPDR=0x81; //��������
    WaitSPI;
    SPDR=page>>8;
    WaitSPI;
    SPDR=page;
    WaitSPI;
    SPDR=0;
    WaitSPI;
    FlashOff;
  }
}

//������ ������ 2 � ������
//������������ ��� ���������� ������������ ������ ���
//���������� �������
void FlushLogFile(void)
{
  /* int page;
  page=(LogFileWP>>4)+0x400;
  WaitFlash;
  FlashOn;
  SPDR=0x86;
  WaitSPI;
  SPDR=page>>8;
  WaitSPI;
  SPDR=page;
  WaitSPI;
  SPDR=0;
  WaitSPI;
  FlashOff;
  WaitFlash; */
}

// ��������� ������ ��������� ������ � ������
// ����� ������ ������������� LogFileRP �� �������� ������,
// � LogFileWP �� ������ ���������
void SearchTopEventLog(void)
{
  struct TLogEvent event;
  unsigned int LogFileRP;
  //int page;
  //����� ������� ��������
  for (LogFileRP=0;LogFileRP<4096;LogFileRP++)
  {
    //LogFileWP=(LogFileRP+32)&0xFFF; //��������� ��������� ������ �� ������ ������
    ReadEvent(LogFileRP,&event); //������
    if (event.Second!=0xffffffff) break; //���� ����� - �����
  }
  //����� ������� ����������
  for (;LogFileRP<4096;LogFileRP++)
  {
    //LogFileWP=(LogFileRP+32)&0xFFF; //��������� ��������� ������ �� ������ ������
    ReadEvent(LogFileRP,&event);
    if (event.Second==0xffffffff) break; //���� ����� - �����
  }
  LogFileWP=LogFileRP&0xFFF; //��������� ��������� ������ �� ������ ���������
  LogFileRP=(LogFileRP-1)&0xFFF; //��������� ��������� ������ �� ���������� - �������� �������
  /*

  //������ ������
  page=(LogFileWP>>4)+0x400;
  //������ ������� � ����� 2
  WaitFlash;
  FlashOn;
  SPDR=0x55;
  WaitSPI;
  SPDR=page>>8;
  WaitSPI;
  SPDR=page;
  WaitSPI;
  SPDR=0;
  WaitSPI;
  FlashOff;
  */
}

/*void SearchLastNormal(int *LogFileRP)
{
struct TLogEvent ev;

*LogFileRP=LogFileWP;
do
{
*LogFileRP=(*LogFileRP-1)&0xFFF;
ReadEvent(*LogFileRP,&ev);
if (ev.Second==0xFFFFFFFF) { *LogFileRP=(*LogFileRP+1)&0xFFF; break; }
 }
while (!(ev.PPKP==0xFF&&ev.Sensor==0xFF&&ev.Event==0x04));
}*/
unsigned int SearchLastNormal(void)
{
  struct TLogEvent ev;
  unsigned int LogFileRP;
  LogFileRP=LogFileWP;
  do
  {
    LogFileRP=(LogFileRP-1)&0xFFF;
    ReadEvent(LogFileRP,&ev);
    if (ev.Second==0xFFFFFFFF) { LogFileRP=(LogFileRP+1)&0xFFF; break; }
  }
  while (!(ev.PPKP==0xFF&&ev.Sensor==0xFF&&ev.Event==0x04));
  return(LogFileRP);
}


void FlashReadVM(unsigned int adr, void *p1, char len)
{
  int page;
  char disp;
  char i;
  char *p=(char *)p1;

  //���������� ������
  page=(adr>>7)+0x600;
  disp=(char)adr;
  //�������� ������ �������
  WaitFlash;
  FlashOn;
  SPDR=0x53;
  WaitSPI;
  SPDR=page>>8;
  WaitSPI;
  SPDR=page;
  WaitSPI;
  SPDR=0;
  WaitSPI;
  FlashOff;
  WaitFlash;
  //�������� ������ ������
  FlashOn;
  SPDR=FlashCMDbuf1read;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=disp;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  PulseSCK;
  for (i=0;i<len;i++)
  {
    SPDR=0xff;
    WaitSPI;
    *p++=SPDR;
  }
  FlashOff;
}

//������ ����������� ����� �������
unsigned int FlashSectorCRC(unsigned int sector, unsigned int len)
{
  unsigned int crc=0;
  //������ ������ �� 45??021 � ����� 1
  WaitFlash;
  FlashOn;
  SPDR=0x53;
  WaitSPI;
  SPDR=sector>>7;
  WaitSPI;
  SPDR=sector<<1;
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
  //��������� �������
  do
  {
    //������ ��������� ����
    SPDR=0xFF;
    WaitSPI;
    crc+=SPDR;
  }
  while (--len); //����, ���� �� ��� �����
  FlashOff;
  return(crc);
}

#define I2Cdelay 50
#define SCLhi PORTB|=2
#define SCLlo PORTB&=~2;
#define SDAhi {PORTB|=4;DDRB&=~4;}
#define SDAlo {PORTB&=~4;DDRB|=4;}


void SendStartI2C(void)
{
  SCLhi;
  SDAhi;
  delay(I2Cdelay);
  SDAlo;
  delay(I2Cdelay);
  SCLlo;
  delay(I2Cdelay);
}

void SendStopI2C(void)
{
  delay(I2Cdelay);
  SDAlo;
  delay(I2Cdelay);
  SCLhi;
  delay(I2Cdelay);
  SDAhi;
  delay(I2Cdelay);
  DDRB|=4;
}

char SendByteI2C(char b)
{
  char i=8;
  do
  {
    if (b&128) SDAhi else SDAlo;
    delay(I2Cdelay);
    SCLhi;
    delay(I2Cdelay);
    SCLlo;
    delay(I2Cdelay);
    b<<=1;
  }
  while (--i!=0);
  SDAhi;
  delay(I2Cdelay);
  SCLhi;
  delay(I2Cdelay);
  i=PINB&4;
  SCLlo;
  delay(I2Cdelay);
  return(i);
}

void WriteI2C(char slave,char adr,char byte)
{
  SPCR=0x00;
  I2Cerror=1;
  SendStartI2C();
  if (!SendByteI2C(slave))
  {
    if (!SendByteI2C(adr))
    {
      if (!SendByteI2C(byte))
      {
        I2Cerror=0;
      }
    }
  }
  SendStopI2C();
  SPCR=0x5C;
}

char ReadI2C(char slave, char adr)
{
  char i=8;
  char b=0;
  I2Cerror=1;
  SPCR=0x00;
  SendStartI2C();
  if (!SendByteI2C(slave))
  {
    if (!SendByteI2C(adr))
    {
      PORTB|=2;
      SendStartI2C();
      SDAhi;
      if (!SendByteI2C(slave+1))
      {
        do
        {
          b<<=1;
          delay(I2Cdelay);
          SCLhi;
          delay(I2Cdelay);
          if (PINB&4) b++;
          SCLlo;
          delay(I2Cdelay);
        }
        while (--i!=0);
        delay(I2Cdelay);
        SCLhi;
        delay(I2Cdelay);
        SCLlo;
        delay(I2Cdelay);
        I2Cerror=0;
      }
    }
  }
  SendStopI2C();
  SPCR=0x5C;
  return(b);
}
