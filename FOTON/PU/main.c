//---------------------------------------------
// ���������� ����������� ������� ��-�1
// Copyright (C) 2001-2005 by Rst7
//---------------------------------------------

volatile char DBG1;

//���������� ��-� ��� ��-�1
#include "devdef.h"

#define __8_BIT_ASCII_WANTED

#define _NOP __no_operation

// ����������� ����������� ������ ��������
#include "stdio.h"

#define puts !!!ERROR!!!
#define putc !!!ERROR!!!

// �������������� �������� ������ � EEPROM
#undef _EEGET
#undef _EEPUT
//void WriteEEPROM(unsigned int adr, unsigned char val);
//char ReadEEPROM(unsigned int adr);
//#define _EEPUT(ADR,VAL)  { WriteEEPROM((unsigned int)(ADR),(unsigned char)(VAL)); }
//#define _EEGET(VAR, ADR) { (VAR)=ReadEEPROM((unsigned int)(ADR)); }

#include "pgmspace.h"
typedef char __flash * STR;

#include "iom128.h"
#include "inavr.h"
#include "setjmp.h"

#define _CLI __disable_interrupt
#define _SEI __enable_interrupt


#define USR UCSR0A
#define UCR UCSR0B
#define UDR UDR0
#define UBRR UBRR0L
#define WaitTX while(PINE&2)

#include "ctype.h"
#include "can.h"

#undef tiny
#define tiny

//-----------------------------
// ��������� ��� ������ � 45??021
//-----------------------------

// ���������� �������� CS
#define FlashOn PORTB&=0xFE
// ������ �������� CS
#define FlashOff PORTB|=0x01
// �������� ���������� ������ �� SPI
#define WaitSPI while(!(SPSR&0x80))
// �������� ��������� �������� � 45??021
#define WaitFlash while(!(ReadFlashStatus()&0x80))

// ������� �� ����� ������ (��� ��������)
#define PRINT_CRLF {putchar(13);putchar(10);}

//-----------------------------
// �������� ������ � EEPROM ����������
//-----------------------------
// ����� ��
__eeprom __no_init char PU_ID @0x06;
// ������ ���1, 0x80 - �������
__eeprom __no_init char MRV1_STAT @0x01;
// ������ ���2, 0x80 - �������
__eeprom __no_init char MRV2_STAT @0x02;
// ������ ��, 0x80 - �������
__eeprom __no_init char TK_STAT @0x03;

// ������������ ��
// ���0=0 �������/��������� �����
// ���0=1 ������ ������� �����
// ���1=0 ����������/������� �����
// ���1=1 ������ ������� �����
// ���2=0 ���� �����
// ���2=1 ���� ����� 120�
// ���3=0 ���� ����� �� ������
// ���3=1 ���� ����� �� ���� ����.
// ���4=0 ���� �������
// ���4=1 ���� ��������
// ���5=0 ������������� RS0/RS1
// ���5=1 ����������� RS0/RS1
#define INDEPENDED_RS (COPY_PU_STAT&0x20)
// ���6=0 ��� ����. ��
// ���6=1 Ec�� ����. ��
#define DOUBLE_PU (COPY_PU_STAT&0x40)
// ���7=0 �� ��������� - ������
#define IS_DOUBLER_DEFAULT (COPY_PU_STAT&0x80)

__eeprom __no_init char PU_STAT @0x05;

__eeprom __no_init char PU_STAT2 @0x04;

#define IS_ALLWAYS_RESET (PU_STAT2&1)
#define IS_NOLOG_ACK (PU_STAT2&2)
#define IS_TAMPER_DISABLED (PU_STAT2&4)

//��������� ������������
//0 - ���� ������ "���� ����."
//1 - ���� ������ "������� �����"
//2 - ���� ������ "������� ������"
//3 - ���� ������ "������� �����
//4 - ���� ������ "������� ����
//5 - ���� ������ "�����"
//6 - ���� PC ���������� "1"
//7 - ���� PC ���������� "2"
//8 - ���� PC ���������� "3"
//9 - ���� PC ���������� "4"
//10 - ���� PC ���������� "5"
//11 - ���� PC ���������� "6"
//12 - ���� PC ���������� "7"
//13 - ���� PC ���������� "8"
//14 - ���� PC ���������� "9"
//15 - ���� PC ���������� "0"
//16 - ���� RS0/RS1
//17 - ���� RS2
//18 - ���� ��������

__eeprom __no_init char TEST_STAT @0x07;
volatile char TSTS; //����� TEST_STAT
volatile char TSTDBG; //���������� ������ ������������
__flash STR testm1[]=
{
  //012345678901234567890123
  (STR)"����� 0 �� PC ����������", //6
  (STR)"����� 1 �� PC ����������", //7
  (STR)"����� 2 �� PC ����������", //8
  (STR)"����� 3 �� PC ����������", //9
  (STR)"����� 4 �� PC ����������", //10
  (STR)"����� 5 �� PC ����������", //11
  (STR)"����� 6 �� PC ����������", //12
  (STR)"����� 7 �� PC ����������", //13
  (STR)"����� 8 �� PC ����������", //14
  (STR)"����� 9 �� PC ����������", //15
  (STR)"�� ���������� RS0/RS1! ������: %03d", //16
  (STR)"�� ���������� RS2!", //17
  (STR)"�� ���������� �������!"  //18
};

__flash char lpt_tab[]=
{
  0x00,0x2B,0x2B,0xFF,0x2B,0xFF,0xFF,0xFF,0x2F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x02,0x80,0x82,0x08,0x0A,0x88,0x8A,0x20,0x22,0xA0,0xA2,0x28,0x2A,0xA8,0xAA,
  0x2B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x01,0x03,0x81,0x83,0x09,0x0B,0x89,0x8B,0x21,0x23,0xA1,0xA3,0x29,0x2B,0xA9,0xAB
};

// ����� ������ ������� � �����, 0 - ����� �����������, 65535 - �������� �����
__eeprom __no_init unsigned int TRIALTIME @0x0C;

// ������ �� ��-�, 0x80 - �������
__eeprom __no_init char PPKP_AREA[64] @0x10;

// ������ �� �����
// ������� ��� ���
__eeprom __no_init char ZONETO_AREA[256] @0x050;
// ���������� ����������� �������� ��� ���
__eeprom __no_init char ZONEMF_AREA[256] @0x150;
// ����� ������������, ��������� ������ ZoneJob
__eeprom __no_init unsigned int ZONEPOINT_AREA[1024] @0x250;
// ������ ������� ������ �������
__eeprom __no_init unsigned int PWDlevel2[2] @0xA50;
// ������ �������� ������ �������
__eeprom __no_init unsigned int PWDlevel3[2] @0xA58;
// ������� ������������� ����� ������� ��� ��
__eeprom __no_init char TKtable[256] @0xA60;

// ������ ���, 0x80 - �������
__eeprom __no_init char VPK_STAT @0xB60;
// ������ ���, 0x80 - �������
__eeprom __no_init char VST_STAT @0xB61;
// ������ ���, 0x80 - �������
__eeprom __no_init char DVP_STAT @0xB62;
// ������ ���, 0x80 - �������
__eeprom __no_init char VTK_STAT @0xB63;

// ������ �� ������� �������������� ����������� (���), 0x80 - ��������
__eeprom __no_init char BRA_AREA [32] @0xB70;
// ������ �� ���, 0x80 - �������
__eeprom __no_init char UPS_AREA [32] @0xB90;
// �������� �� �����
__eeprom __no_init char ZONEDELAY_AREA[1024] @0xC00;

//---------------------------------------------------------------
// �������� ��������
//---------------------------------------------------------------

//��������� ������������ ����������, ��������� ������ �������� �������� ��������� ������� ��
struct TSensorConfig
{
  char GeoPos[24];
  char Zone1;
  char Type;
  char Zone2;
  char TCode;
  char Zone4;
  char Zone3;
  int Led;
};

// ��������� ������� � �������, ��������� ������ �������� �������� ��������� ������� ��
struct TLogEvent
{
  char PPKP;
  char Sensor;
  char Event;
  char Event2;
  unsigned long Second;
};

// ����� ��� ������� ��������, ������ �������� �������� ��������� ������� ��
struct TDVPpacket
{
  char Packet_Type;

  struct TLogEvent ev;       //8

  struct TSensorConfig sc;   //32

  char TKEvent;
  char LED;
  char LEDflash;
  char SndMode;
  char FreeSpace[4];

} DVPpacket;

//---------------------------------------------------------------
// ��������� ���������
//---------------------------------------------------------------

//������� ������������� ������� ����
static __flash char Win2LCDTable[]=
{
  'A',0xa0,'B',0xa1,0xe0,'E',0xa3,0xa4,
  0xa5,0xa6,'K',0xa7,'M','H','O',0xa8,
  'P','C','T',0xa9,0xaa,'X',0xe1,0xab,
  0xac,0xe2,0xad,0xae,'b',0xaf,0xb0,0xb1,
  'a',0xb2,0xb3,0xb4,0xe3,'e',0xb6,0xb7,
  0xb8,0xb9,0xba,0xbb,0xbc,0xbd,'o',0xbe,
  'p','c',0xbf,'y',0xe4,'x',0xe5,0xc0,
  0xc1,0xe6,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7
};

__flash char Message_Trial[]=
"��������������� �����";
__flash char Message_NeedMoney[]=
"���������� � ����������";

// ��������� ��������� ����
__flash STR Message_MainMenu[]=
{
  (STR)"�����-�",
  (STR)"�������� ������ �������",
  (STR)"���������� �������",
  (STR)"������������ ��",
  (STR)"������������ ��-�",
  (STR)"������������ �����������",
  (STR)"������������ ���",
  (STR)"������������ ��",
  (STR)"������������ ���/���",
  (STR)"������������ ���",
  (STR)"��������� �������",
  (STR)"������� ��������"
};

__flash STR Message_ViewLog[]=
{
#ifdef SECURITY
  (STR)"�������",
#else
(STR)"������",
#endif
(STR)"������",
(STR)"��������������",
(STR)"���",
(STR)"������� ����������"
};

__flash STR Message_Boot[]=
{
  (STR)"������� ����������",
  (STR)"�������� ����������",
  (STR)"�������� ���������",
  (STR)"������ �������� ��"
};

__flash STR Message_ConfigPU[]=
{
  (STR)"��������� ���� � �������",
  (STR)"������������ � ��",
  (STR)"��������� �������",
  (STR)"������ ���� ������������",
  (STR)"������� ���. ���������",
  (STR)"��������� ������ ������",
  (STR)"���� ������ �� ���. ���.",
  (STR)"��������� ������ ��",
  (STR)"���� �����",
#ifdef SECURITY
(STR)"���� ������� 120�",
#else
(STR)"���� ����� 120�",
#endif

(STR)"���� ����� �� ������",
(STR)"���� ����� �� ���� ����.",

(STR)"���� �������",
(STR)"���� ��������",

(STR)"������������ RS0/RS1",
(STR)"����������� RS0/RS1",

(STR)"��������� ��",
(STR)"������������ ��",

(STR)"�������� �� �� ���������",
(STR)"����������� �� �� ���������",

(STR)"����� ����� ���� ����.",
(STR)"����� ������",

(STR)"���� ���� - ����. �������",
(STR)"���� ���� - ������ ����",

(STR)"������ �������",
(STR)"������ ��������"
};

__flash STR Message_Debug[]=
{
  (STR)"������ EEPROM ���������",
  (STR)"������ ������� ���-��",
  (STR)"������ ��������� ���-��",
  (STR)"������ ����� ������",
  (STR)"������� Flash-������",
  (STR)"������� ������ �������",
  (STR)"������� ���",
  (STR)"����������� �����-�",
};

__flash STR Message_EditAdr[]=
{
  (STR)"��������� ������ ��-�",
  (STR)"��������� ������ ���-��",
  (STR)"���������� ����� ���-���",
  (STR)"����� ����� ���-���",
  (STR)"��������� ������ ���",
  (STR)"��������� ������ ���/���",
};

// ���� ����������� 0-31
__flash char NameSensorType[][6]=
{
  "���  ","���  ","���  ","���� ",
  "���  ","���  ","���  ","���  ",
  "-----","-----","-----","-----",
  "��-1 ","��   ","-----","-----",
  "���  ","���  ","-----","-----",
  "���  ","-----","-----","-----", //20,21,22,23
  "-----","-----","-----","-----",
  "-----","-----","-----","-----"
};

// ���������� ���� �� ������ ������
__flash int DMonth[]={0,31,59,90,120,151,181,212,243,273,304,334,365};

//---------------------------------------------------------------
// Define bios variables
//---------------------------------------------------------------

tiny static char NeedDecTrialTime=1; //���� ������������� ���������� ���������� ������� �����, ��������������� ��� � ���
tiny static char DoorsStatus=0; // ������ ������� �����, 1 - �������

//tiny static char PU_IDmem; // ����� ������ ��
tiny unsigned int PrinterOnLine=1000; // ��������� ��������

//char DPU_txcnt;

struct
{
  char CursorPos; //������� �������
  unsigned int cursector; //���. ����������� ������
  unsigned int curseccrc; //���. �����. ����� �������
  unsigned long date; //����� � ���� ��� �������������
  char CON[81]; // ������� (81 �� �� sprintf'a)
};
volatile char NeedSendScr;

// ������ ��� ����������� ������ � ����������

// �������� �� �������
#define KEY_DELAY -5
// �������� �� ������� �������
#define KEY_REPDEL 30
// �������� �� ����������
#define KEY_REPPER 24
// ���� ������
#define kREDRAW	0x1F
#define kSECRET 0x1E
#define kACK  0x1A
#define kLT   8
#define kRT   10
#define kUP   11
#define kDW   12
#define kESC   27
#define kINS  14
#define kDEL  15
#define kPGUP 4
#define kPGDW 3
#define kHOME 2
#define kEND  1

volatile tiny char LED=0; // ������ �����������, �� �����, 1 - �������
volatile tiny char LEDflash=0; // ������ ������� �����������, �� �����, 1 - ������
tiny char LEDflashcnt=1; // ������� ��� �������

volatile tiny char SndMode=0; // ��� �����, 0 - ��������, 1 - �����, 2 - �����, 3 - ��������������, 4 - �������� ������
volatile tiny char COPY_PU_STAT=0;

// ���� ����������� � �����
#ifdef PUA1
#define LEDfire   0x80
#define LEDmfire  0x40
#define LEDoff    0x20
#define LEDfail   0x04
#define LEDsndoff 0x10
#define LEDsysf   0x08
#else
#define LEDfire   0x01
#define LEDmfire  0x20
#define LEDoff    0x08
#define LEDfail   0x04
#define LEDsndoff 0x02
#define LEDsysf   0x10
#endif

char IsPreAtt;
char IsOffline;

volatile tiny char KEY=0x00; //��� ������� �������, 0 - ��� �������
tiny char OldKey=0x00; //������ ��� �������
tiny signed char KeyCounter=KEY_DELAY; // ������� ��� �������� ����������
tiny unsigned char ATKbitcount=11; // ������� �������� ����� �� ������� ����������
static char ATKbuf[8]; // ����� �������� ������ �� ������� ����������
static char ATKwp=0; // ��������� ������ � ����� �������� ������ �� ������� ����������
static char ATKrp=0; // ��������� ������ �� ������ �������� ������ �� ������� ����������
tiny static char ATKstat=0; // ������ ������ Shift, Alt, Ctrl, CapsLock

static unsigned int check_flash_sector_number=0xFFFF;

// ����� � �������� ����� 01-01-2000 00:00, ����������� � SRAM �� ����������� ������
#define TimeDate (*(unsigned long*)(0xFFC))

tiny unsigned long Timer30sec=0xFFFFFFFF; //������ 30�
tiny unsigned long Fire120sec=0xFFFFFFFF; //������ 120�
tiny static unsigned int TotalFires; // ����� ���������� �������

tiny static char PWDmode=0; //����� ������ ������, 0 - �� ������, 2 - ������� 2, 3 - ������� 3

char EventFIFO[64]; // ���� �������, FIFO, ������� ��� �������, �� 4 �����, ����� 16 �������
char EventFIFOwp=0; // ��������� ����� ������� ��� ������
char EventFIFOrp=0; // ��������� ����� ������� ��� ������


// ������ ����� ���������� �������, ��������� ��. �������� �������� ��������� ������� ��
tiny static char TemporaryEVENT; // ��� �������� ��������������� �������
tiny static char TemporaryZONE0; // ���� �������� �������
tiny static char TemporaryZONE1;
tiny static char TemporaryZONE2;
tiny static char TemporaryZONE3;

static char IsSuprDZ; //������� ������� ��� � ����. ������

//tiny static char TemporaryTYPE; // ��� ���������� �������� �������

//==============================================================================================================
//static char SaveTIMSK; //����� �������� TIMSK

volatile char ISMASTER=0; //<>0 ���� �� - ������

struct
{
  struct TLogEvent DPU_MESS; //����� ��� �������� � ����. ��
  char DPU_SEQ; //����� ������, ���. ��� ��������� �������������
};

volatile char DPUmess_ready; //������ ������� ��� ����. ��
static int LogFileRP_DPU; //��������� �� ������� ��� ����. ��

volatile static char WarmBoot_cmd; //������� ��� ������ 4E ��� 4F

volatile char RebootMode=0; //0 - ����. �����, FF - ����, ����� ������� ������

// ����� �������� ���������� ������,
// ������ ���� - ����� ��-�, ���� �� �� 255 - ������ �������� ���������� ������
char SysPPKP_TXB[8];
volatile char SysPPKP_txsize; //���������� ���� � ������ ���������� ������
// ����� ������ ���������� ������
char SysPPKP_RXB[8];
// ���������� �������� ���� ���������� ������,
// ���� 0 - ����� ��� �� ������
// 255 - ��� ������ � ��-� � ��������� ������
volatile char SysPPKP_rxsize=255;

//volatile char NeedNoError=0; // ���� 1 - �� �������� �� ���������� ������ � ��-� � ��������� ������

// ������� ��� ������� CRC
__flash static char CRC8tab[256]=
{
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

// ������ �� ��������� RS485, 0-63 ��-�, 64-95 - ���, 96-127 - ���
// ��� 0-2 - ������ �� RS0, 7 - ������ �����
// ��� 3-5 - ������ �� RS1, 7 - ������ �����
// ��� 6 - ����� ������������� ACK
// ��� 7 - �������/��������

volatile static char ABONS[128];

static int LogFileWP; // ��������� �� ���� ������� � 45??021 ��� ������

volatile tiny static char CommunicateWithPC=0; // ���� ������������� �������� � ����� ������ � �� (���������������� � ��)

volatile static char DBLE_RS0; //������� ������ �� ������ ������ � ����. �� �� RS0
volatile static char DBLE_RS1; //������� ������ �� ������ ������ � ����. �� �� RS1
volatile static char DBLS; //������� ������ �� �������� � ����� ���. �������, �����
//������� ������������� �������� � ����. ������

//-----------------------------------------
tiny static int LogFileRP_CAN;
tiny static char PUID;
// ������ CAN
// bit 0 - TX0BUSY
// bit 1 - TX1BUSY
// bit 2 - RX0ERR
// bit 3 - RX1ERR
tiny static char FlagsCAN;

tiny static char CANtimeout; //������� ��� ��������/������ �/�� ���
static char EPUS[64];

//-----------------------------------------
tiny static char NeedMoney=0; // ���� ��������� ����� ������, 1 - ���������� � ����������
//-----------------------------------------
//-----------------------------------------
// ���������� ��� ������ � ������
//-----------------------------------------
// ��� ������ ���� �� 2 �����, ������ - ������, ������ - ���������� ��������
static char ZoneTF[512];
static char ZoneDelays[1024];
static unsigned int CurrentZoneCheck;
static tiny char mrv1relay; // ����� ����� ���� ��� ���1
static tiny char mrv2relay; // ����� ����� ���� ��� ���2

static tiny char TADEVoff; // ����� ���������� ������ � �������� (��� ��-�)

static tiny char I2Cerror; // ���� ������ ������ �� I2C
static tiny char TKevent; // ��� ������� ��

//==================================================
// ���������� ��� ������ � �����������
//==================================================
tiny static char TemporaryBITN;
tiny static char TemporaryBITV;
static char BitFlags[32]; //���� ��������� 0-255
static char MacroTimers[32]; //������� (16 ��), ������ + ����� ����
static unsigned int VMPC[16]; //��������� � ���������
static unsigned int VMBASE[16]; //��������� � ���������


char IsLineSpace(char *p);
void FullPrintEvent(int LFRP, unsigned int n);

//===================================================================
// ��������� ��������� �������, ��������� ��. �������� �������� ��������� ������� ��
//===================================================================
#include "EventParser.c"

//===================================================================

// �������� ��� ��������
void WarmBoot(char cmd);
__monitor void Event2FIFO(char p,char s,char e,char e2);
void DoConfigPUpk(void);
void SendCursorPos(char pos);
void PUSconnect(char mode, char pus, char section);

//#pragma warnings=off
#pragma optimize=no_inline
void delay(unsigned int delay) // �������� � delay �����������
{
  // delay-=5;
  while(--delay);
}
//#pragma warnings=on

void Max63(char *v) // �������� �������� � ��������� 0..63 � ��� ���������
{
  if (*v>63) *v=63;
}

char IsLineSpace(char *p) // ���� ������ ������� �� ��������, ������� 1
{
  char c=24;
  do
  {
    if (*p++!=' ') return(0);
    if (!(--c)) return(1); //��� 24
  }
  while (*p);
  return(1);
}

//������ ������ � ����� ABONS
void ConstructABONS()
{
  //char c;
  char i=0;
  do
  {
    ABONS[i]=PPKP_AREA[i]&0x80;
    i++;
  }
  while (i<64);
  do
  {
    ABONS[i]=UPS_AREA[i&0x1F]&0x80;
    i++;
  }
  while(i<96);
  do
  {
    ABONS[i]=BRA_AREA[i&0x1F]&0x80;
    i++;
  }
  while(i<128);
}

//---------------------------------
// BIOS ����������, �������, ���
//---------------------------------
#include "KBDLCDLPT.c"
//=================================

// �������������� ������� � ���� � ���������� ������ ����� 01-01-2000 00:00
unsigned long TimeDate2Long(char hrs, char min,char sec,char day,char mon,char yrs)
{
  unsigned long iday;
  iday=365*yrs+DMonth[mon-1]+(day - 1);
  iday=iday+(yrs>>2);
  if (mon>2||(yrs&3)>0)
    iday++;
  return(sec+60*(min+60*(hrs+24* iday)));
}

// ������ ���� � ������� �� ���������� ������ � 01-01-2000 00:00
void PrintTimeDate(char *p, unsigned long x)
{
  char sec,min,hrs,mon,yrs;
  unsigned int day,iday,day4,yrs4;

  sec=x%60;
  min=(x/60)%60;
  hrs=(x/3600)%24;
  iday=x/86400;
  yrs4=x/((4*365+1)*86400);
  day4=iday%(4*365+1);
  iday=(day4==(31+28));
  if (day4>=(31+28)) day4--;
  yrs=(yrs4<<2)+day4/365;
  day=day4%365;
  mon=0;
  while (DMonth[++mon]<=day);
  day-=DMonth[mon-1];
  if (iday) day++;
  day++;
  if (yrs>99) yrs=0;
  sprintf_P(p,(STR)"%02d:%02d:%02d %02d-%02d-%02d",hrs,min,sec,day,mon,yrs);
}

void LPT_PrintTimeDate(unsigned long x)
{
  char sec,min,hrs,mon,yrs;
  unsigned int day,iday,day4,yrs4;

  sec=x%60;
  min=(x/60)%60;
  hrs=(x/3600)%24;
  iday=x/86400;
  yrs4=x/((4*365+1)*86400);
  day4=iday%(4*365+1);
  iday=(day4==(31+28));
  if (day4>=(31+28)) day4--;
  yrs=(yrs4<<2)+day4/365;
  day=day4%365;
  mon=0;
  while (DMonth[++mon]<=day);
  day-=DMonth[mon-1];
  if (iday) day++;
  day++;
  if (yrs>99) yrs=0;
  printf_P((STR)"%02d:%02d:%02d %02d-%02d-%02d ",hrs,min,sec,day,mon,yrs);
}

//---------------------------------------------
// ������������� ������������
//---------------------------------------------

#include "SysInit.c"

//---------------------------------------------------------
// ������ � 45??021
//---------------------------------------------------------
#include "Flash.c"

//�������� CRC ����� ��������
#include "PGMCRC.c"

//---------------------------------------------------------
// �������� ������� +24�
//---------------------------------------------------------
char CheckPowerDeath(void)
{
  unsigned long td;

#ifdef PUA1
  if (PINF&2) return(0); //���� +24� ����, ������ ������������
#else
  if (PIND&0x04) return(0);
#endif
  // ���������� � �������� � ����� ����������������
  __disable_interrupt();
  __watchdog_reset();
  WDTCR=0x18;
  WDTCR=0x00;
  __watchdog_reset();
  MCUCR=0x38;
  UCSR0B=0;
  SPCR=0;
  ADCSR=0;
  ACSR=1<<ACD;
  TCCR2=0;
  TCCR1B=0;
  EIMSK=0;
  TIMSK=1;
#ifdef PUA1
  PORTB=0;
  DDRE=0;
  PORTE=0x00;
  PORTA=0x00;
  PORTC=0x00;
  PORTD=0x00;
#else
  PORTB=0;
  DDRE=0;
  PORTE=0x00;
  PORTA=0x00;
  PORTC=0x00;
  DDRD=~4;
  PORTD=0x00;
#endif
  //FlushLogFile(); // �������� � 45??021 ������������ ������
  PORTB=0;
#ifdef PUA1
#else
  DDRB=~0x20;
#endif

  while(ASSR&7); // ��������� ���������� �� ����� 1 ��� �� 8 ������
  TCCR0=0x07;
  while(ASSR&7);
  TIFR=0xFF;
  td=TimeDate;
  __enable_interrupt();
  __sleep(); // �������� �� 8 ������
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  td+=8; // ����������� �����
  __insert_opcode(0xF3FF);
  __enable_interrupt();
  do
  {
    __sleep(); // �������� �� 8 ������
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    td+=8; // ����������� �����
    __insert_opcode(0xF3FF);
    __enable_interrupt();
  }
#ifdef PUA1
  while (!(PINF&4)); // ����, ���� �� ��������� �������
#else
  while (!(PIND&0x04));
#endif
  TimeDate=td;
  __disable_interrupt();
  /* __watchdog_reset();
  WDTCR=0x08;
  WDTCR=0x18;
  WDTCR=0x0A; //��������� ���������� ������
  __watchdog_reset();
  for(;;);*/
  asm("JMP 0\n");
  return(0);
}

//----------------------------------------------------
// ��������� �������
//----------------------------------------------------

// �������� ������� � ����, ��� ������������ ���������� ��������������� �������
__monitor void Event2FIFO(char p,char s,char e,char e2)
{
  char* p1;
  if (NeedMoney) return; // ���� ���������� ����� ������, �� ����������� �� �������
  p1=&EventFIFO[EventFIFOwp];
  // ������ � ����
  *p1++=p;
  *p1++=s;
  *p1++=e;
  *p1++=e2;
  EventFIFOwp=(EventFIFOwp+4)&0x3f; // ��������� ���������
  if (EventFIFOwp==EventFIFOrp) // ���� ������������, ���������� ��������������� �������
  {
    p1=&EventFIFO[EventFIFOwp];
    *p1++=0xFF;
    *p1++=0xFF;
    *p1++=0x01;
    *p1=0x00;
    EventFIFOwp=(EventFIFOwp+4)&0x3f;
  }
}

char LEDCONV(char i)
{
  char c;
  c=0;
  if (i&0x01) c|=0x80;
  if (i&0x20) c|=0x40;
  if (i&0x08) c|=0x20;
  if (i&0x04) c|=0x04;
  if (i&0x02) c|=0x10;
  if (i&0x10) c|=0x08;
  return(c);
}

void CheckDoors(void)
{
  char ds=PIND&0x40;
  if (IS_TAMPER_DISABLED) return;
  if (DoorsStatus^ds)
  {
    DoorsStatus=ds;
    if (DoorsStatus)
    {
      Event2FIFO(0xFF,0xFF,22,0);
    }
  }
}

void CheckADevices(void)
{
  char c;
  char v;
  static char sec;
  static char oldsec;
  static char pus;

  pus++;
  c=pus>>2;
  if (!(pus&3))
  {
    if (EPUS[c]==0x80) PUSconnect(0,c,0);
  }
  if (oldsec!=(char)TimeDate)
  {
    oldsec=(char)TimeDate;
    sec=(sec+1)&7;
    switch (sec)
    {
    case 0:
      //mrv1
      if (MRV1_STAT&0x80)
      {
        for (c=0;c<16;c++)
        {
          v=ReadI2C(0x92,1);
          if (v!=mrv1relay) I2Cerror=1;
          if (!I2Cerror) break;
        }
        if (I2Cerror&&(!(TADEVoff&1)))
        {
          Event2FIFO(0xFF,0xFF,6,0);
          TADEVoff|=1;
        }
      }
      break;
    case 1:
      //mrv2
      if (MRV2_STAT&0x80)
      {
        for (c=0;c<16;c++)
        {
          v=ReadI2C(0x94,1);
          if (v!=mrv2relay) I2Cerror=1;
          if (!I2Cerror) break;
        }
        if (I2Cerror&&(!(TADEVoff&2)))
        {
          Event2FIFO(0xFF,0xFF,7,0);
          TADEVoff|=2;
        }
      }
      break;
    case 2:
      //tk
      break;
    case 3:
      //vst
      break;
    case 4:
      break;
    case 5:
      //vtk
      break;
    }
  }
}

void SendToTK(char data)
{
  char i;
  if (!(TK_STAT&0x80)) return;
  if (data)
  {
    for (i=0;i<16;i++)
    {
      WriteI2C(0xF2,1,data);
      if (!I2Cerror) break;
    }
    if (I2Cerror)
    {
      Event2FIFO(0xFF,0xFF,8,0);
    }
  }
}

// ��������� ����
void ZoneJob(char TemporaryZONE,char is32)
{
  char c;

  is32&=32;

  if ((ZONEMF_AREA[TemporaryZONE]&0x80)==0) return; //���� ���������!
  if (is32) goto L1; //�������������� ��������� ���� ���
  // ������������� ����� ������ ��� ����
  ZoneTF[TemporaryZONE<<1]=ZONETO_AREA[TemporaryZONE];
  // ����������� ���������� �������� � ����, ���� ���� �� �������������
  if (ZoneTF[(TemporaryZONE<<1)+1]==0xFF) return; //� ���� ����������� ����������
  ZoneTF[(TemporaryZONE<<1)+1]++;
  // ���������� � ����������� �����������
  c=(ZONEMF_AREA[TemporaryZONE]&3)+1;
  if (ZoneTF[(TemporaryZONE<<1)+1]>=c)
  {
    unsigned int z;
  L1:
    z=TemporaryZONE<<2;
    if (TemporaryZONE) Event2FIFO(0xFF,0xFF,31,TemporaryZONE);
    c=4;
    do
    {
      if (ZoneDelays[z]==0xFE) ZoneDelays[z]=ZONEDELAY_AREA[z];
      z++;
    }
    while(--c);
  }
}

// ���������� ���������� ����
void ZoneCheck(void)
{
  char i;
  unsigned int zp;
  char s0,s1,s2;
  unsigned int z;

  z=CurrentZoneCheck&1023;

  do
  {
    //  i=z;
    if (ZoneDelays[z]==0xFF) goto L1;
    z++;
    //  i++;
  }
  while(z&0x3F);
  CurrentZoneCheck=z;
  return;
L1:
  CurrentZoneCheck=z+1;
  // �������� ����� ��������
  zp=ZONEPOINT_AREA[z];
  if (zp==0xffff)
  {
    ZoneDelays[z]=0xFE;
    return;
  }
  switch (zp&0xC000)
  {
    // ��-�/����������/����
  case 0x0000:
  case 0x4000:
    if (!ISMASTER) break;
    if (zp&0x1f8)
    {
      // ����������<>0, �������� ���� �� ���
      while (SysPPKP_rxsize==0); // ������� ��������� ���������� ������
      __disable_interrupt();
      s0=SysPPKP_rxsize; // ��������� �������� ������
      s1=SysPPKP_RXB[0];
      s2=SysPPKP_RXB[1];
      // ���������� ������ ��� �������� �������� ��������� ����
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=zp>>9;
      SysPPKP_TXB[1]=0x8;
      SysPPKP_TXB[2]=(zp&0x1f8)>>3;
      SysPPKP_TXB[3]=1<<(zp&7);
      SysPPKP_txsize=4;
      __enable_interrupt();
      while (SysPPKP_rxsize==0); // ������� ��������� ���������� ������
      SysPPKP_rxsize=s0; // ��������������� ������ ������
      SysPPKP_RXB[0]=s1;
      SysPPKP_RXB[1]=s2;
    }
    else
    {
      // ����������=0, �������� ���� �� ��-�
      while (SysPPKP_rxsize==0); // ������� ��������� ���������� ������
      __disable_interrupt();
      s0=SysPPKP_rxsize; // ��������� �������� ������
      s1=SysPPKP_RXB[0];
      s2=SysPPKP_RXB[1];
      // ���������� ������ ��� �������� �������� ��������� ���� � ��-�
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=zp>>9;
      SysPPKP_TXB[1]=0x48;
      SysPPKP_TXB[2]=0;
      SysPPKP_TXB[3]=1<<(zp&7);
      SysPPKP_txsize=4;
      __enable_interrupt();
      while (SysPPKP_rxsize==0); // ������� ��������� ���������� ������
      SysPPKP_rxsize=s0; // ��������������� ������ ������
      SysPPKP_RXB[0]=s1;
      SysPPKP_RXB[1]=s2;
    }
    ZoneDelays[z]=0xFE;
    break;
  case 0x8000:
    if (!ISMASTER) break;
    // ���� � ���
    if (z>=4) Event2FIFO(((zp>>8)&0x3F)+0x80,0,2,(char)zp);
    PUSconnect(1,(zp>>8)&0x3F,(char)zp);
    ZoneDelays[z]=0xFE;
    break;
  case 0xC000:
    if (zp&0x2000)
    {
      if (!ISMASTER) break;
      //���� � ���/���
      //111aaaaaxxxxxxxx
      while (SysPPKP_rxsize==0); // ������� ��������� ���������� ������
      __disable_interrupt();
      s0=SysPPKP_rxsize; // ��������� �������� ������
      s1=SysPPKP_RXB[0];
      s2=SysPPKP_RXB[1];
      // ���������� ������ ��� �������� �������� ��������� ���� � ���
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=(zp>>8)&0x7F;
      SysPPKP_TXB[1]=0x48;
      SysPPKP_TXB[2]=0;
      SysPPKP_TXB[3]=(char)zp; //����� ����
      SysPPKP_txsize=4;
      __enable_interrupt();
      while (SysPPKP_rxsize==0); // ������� ��������� ���������� ������
      SysPPKP_rxsize=s0; // ��������������� ������ ������
      SysPPKP_RXB[0]=s1;
      SysPPKP_RXB[1]=s2;
      ZoneDelays[z]=0xFE;
      break;
    }
    // point - mrv
    if (!(zp&8))
    {
      mrv1relay|=1<<(zp&7);
      if (MRV1_STAT&0x80)
      {
        for (i=0;i<16;i++)
        {
          WriteI2C(0x92,1,mrv1relay);
          if (!I2Cerror) break;
        }
        if (I2Cerror&&(!(TADEVoff&1)))
        {
          Event2FIFO(0xFF,0xFF,6,0);
          TADEVoff|=1;
        }
      }
    }
    else
    {
      mrv2relay|=1<<(zp&7);
      if (MRV2_STAT&0x80)
      {
        for (i=0;i<16;i++)
        {
          WriteI2C(0x94,1,mrv2relay);
          if (!I2Cerror) break;
        }
        if (I2Cerror&&(!(TADEVoff&2)))
        {
          Event2FIFO(0xFF,0xFF,7,0);
          TADEVoff|=2;
        }
      }
    }
    ZoneDelays[z]=0xFE;
    break;
  }
}

void TransmitToDPU(void)
{
  if (DPUmess_ready) return; //�� �������� ����. �������
  if (LogFileRP_DPU!=LogFileWP)
  {
    ReadEvent(LogFileRP_DPU,&DPU_MESS); // ������ ��� �� 45??021
    LogFileRP_DPU=(LogFileRP_DPU+1)&0xFFF;
    DPUmess_ready=1;
  }
}

//--------------------------------------------------------------
// ���������� ������� �� �����,
// ������ � ����� 45??021, ��������� ���������
// ������ �� �������, �������� � ���, ��� � �.�.
// ������ ��������� ���������� ���������, � �.�. � ��� �������� ������� ������,
// ��� ������������� ���� ����� ��������� ������� ������
//--------------------------------------------------------------
char FIFO2EventLog(void)
{
  struct TLogEvent ev; // ������� �������
  union
  {
    char s[61]; // ����� ��� ��������� �����
    struct TSensorConfig sc;
  };
  char c;
  void Event2CAN(void);
  void DoVirtualMachines(void);

  CheckPowerDeath(); // ��������� ������� �������
  CheckADevices();
  if (ISMASTER)
  {
    Event2CAN();
    TransmitToDPU();
  }
  else
  {
    //������, ���� ������ ����� slave
    WorkSecond(); // ���� ���� ���� ������� 1�, �������� ����������
    LogFileRP_CAN=LogFileWP; //��� ������ �� ���� ���������� ������� ��� ������������
    LogFileRP_DPU=LogFileWP; //�� �������
  }
  ZoneCheck();
  DoVirtualMachines();

  COPY_PU_STAT=PU_STAT;
  if (COPY_PU_STAT&0x10) SetLEDflash(LEDsndoff); //���� ���� ��������

#ifdef PUA1
#else
  CheckDoors();
#endif
  _CLI();
  c=COPY_PU_STAT;
  if ((Fire120sec<=TimeDate)&&(c&4)) // ���� �������� ������ 120���, �������� ����
  {
#ifdef PUA1
    PORTB|=0x40;
#else
    PORTD|=0x10;
#endif
    Fire120sec=0xFFFFFFFF;
  }
  _SEI();
L_SKIPEVENT:
  if (EventFIFOwp==EventFIFOrp) // ���� ������� � ����� ���, �������
  {
    return(0);
  }
  else
  {
    memcpy(&ev,&EventFIFO[EventFIFOrp],4); // �������� ������� �� ����� � �����
    _CLI();
    ev.Second=TimeDate; // ���������� ����� �������
    _SEI();
    EventFIFOrp=(EventFIFOrp+4)&0x3f; // ���������� ��������� ������
    //���������� ������� �� ������ ����� �������, ���� �� �������� � ������������
    if ((ev.PPKP&0xE0)==0x40) //������� �� ���
    {
      if (ev.Sensor==4) //����� ����� 1
      {
        if (!(UPS_AREA[ev.PPKP&0x1F]&1)) goto L_SKIPEVENT;
      }
      if (ev.Sensor==5) //����� ����� 2
      {
        if (!(UPS_AREA[ev.PPKP&0x1F]&2)) goto L_SKIPEVENT;
      }
    }
    WriteEvent(&ev); // ���������� ������� � 45??021, ��������� ������ �������� �������������
    if (!ISMASTER)
    {
      if ((ev.Event==0xFD)||(ev.Event==0xFE))
      {
	//��������/��������� ��-�/���/��� � ����. �������
	if ((ev.Sensor&0xC0)==0xC0)
	{
	  if ((ev.PPKP&0xC0)==0x00) PPKP_AREA[ev.PPKP&0x3F]=ev.Event2;
	  if ((ev.PPKP&0xC0)==0x40) UPS_AREA[ev.PPKP&0x1F]=ev.Event2;
	  if ((ev.PPKP&0xC0)==0x60) BRA_AREA[ev.PPKP&0x1F]=ev.Event2;
	}
      }
      //�������� ���/���� ����������
      //Event2FIFO(ppkp,sensor|0xc0,ts.Type>>7,ts.Type); //��������� "���������� �������/��������"
      if (ev.PPKP<0x40)
      {
	if ((ev.Sensor&0xC0)==0xC0)
	{
	  if (ev.Event<2)
	  {
	    ReadSensorConfig(ev.PPKP,ev.Sensor&0x3F,&sc);
	    sc.Type=ev.Event2&0x9F;
	    WriteSensorConfig(ev.PPKP,ev.Sensor&0x3F,&sc);
	  }
	}
      }
    }
    if (
        (ev.PPKP==255)&&
          (ev.Sensor==255))
    {
      if ((ev.Event>=34)&&(ev.Event<=36)) SetLEDflash(LEDsysf); //��������� ������
      if (ev.Event==4) //������� - �����
      {
        void DoBoot(char cmd); //��������� ����� �������
        DoBoot(ev.Event2);
        check_flash_sector_number=0;
        KEY=0x81; //����� �� ��������� �������
      }
      if (!ISMASTER)
      {
	if (ev.Event==0x26)
	{
	  //���������� ���� � ����. �������
	  ZONEMF_AREA[ev.Event2]&=~0x80; //��������� ����
	}
	if (ev.Event==0x27)
	{
	  //��������� ���� � ����. �������
	  ZONEMF_AREA[ev.Event2]|=0x80; //�������� ����
	}
      }
      if (ev.Event==37)
      {
        //�������������� ����������
        char ppkp=0;
        char z=ev.Event2;
        ZoneTF[(z<<1)+1]=0; //�������������
        if (ISMASTER)
        {
          ClrScr();
          sprintf_P(CON,(STR)"�������������� ���������� � ���� %03d",z);
          UpdateConsole();
          do
          {
            if (ABONS[ppkp]!=0) //���� ��-� � ������������ � � ��� ���� �����
            {
              char sensor=0;
              do
              {
                ReadSensorConfig(ppkp,sensor,&sc); //������ ������������
                if ((sc.Type==(0x80+0x14))&&(sc.Zone1==z)) //���� ��� ���������� ��� � �� �������
                {
                  char s0,s1,s2;
                  while (SysPPKP_rxsize==0); // ������� ��������� ���������� ������
                  __disable_interrupt();
                  s0=SysPPKP_rxsize; // ��������� �������� ������
                  s1=SysPPKP_RXB[0];
                  s2=SysPPKP_RXB[1];
                  // ���������� ������ ��� �������� �������� ���������� ����������
                  SysPPKP_rxsize=0;
                  SysPPKP_TXB[0]=ppkp;
                  SysPPKP_TXB[1]=sensor;
                  SysPPKP_TXB[2]=0;
                  SysPPKP_txsize=3;
                  _SEI();
                  __enable_interrupt();
                  while (SysPPKP_rxsize==0); // ������� ��������� ���������� ������
                  SysPPKP_rxsize=s0; // ��������������� ������ ������
                  SysPPKP_RXB[0]=s1;
                  SysPPKP_RXB[1]=s2;
                }
                sensor++;
              }
              while(sensor<64);
            }
            ppkp++;
          }
          while(ppkp<64);
        }
        KEY=kREDRAW;
        IsSuprDZ=0;
      }
    }
    PrintEventType(&ev,s,1); // ��������� �������
    c=TemporaryBITN;
    //  _CLI();
    if (TemporaryBITV)
      BitFlags[c>>3]|=(1<<(c&7));
    else
      BitFlags[c>>3]&=~(1<<(c&7));
    //  _SEI();
    // ��������� ����� �� ������� � ��������
    LPT_PrintTimeDate(ev.Second);
    putstr(s);
    PRINT_CRLF;
    SendToTK(TKevent);
    if (TemporaryEVENT&16)
    {
      //���������� ���������� � ����
      char ppkp=0;
      ZoneTF[(TemporaryZONE0<<1)+1]=0xFF; //��������� ����
      if (ISMASTER)
      {
        ClrScr();
        sprintf_P(CON,(STR)"���������� ���������� � ���� %03d",TemporaryZONE0);
        UpdateConsole();
        do
        {
          if (ABONS[ppkp]!=0) //���� ��-� � ������������ � � ��� ���� �����
          {
            char sensor=0;
            do
            {
              ReadSensorConfig(ppkp,sensor,&sc); //������ ������������
              if ((sc.Type==(0x80+0x14))&&(sc.Zone1==TemporaryZONE0)) //���� ��� ���������� ��� � �� �������
              {
                char s0,s1,s2;
                while (SysPPKP_rxsize==0); // ������� ��������� ���������� ������
                __disable_interrupt();
                s0=SysPPKP_rxsize; // ��������� �������� ������
                s1=SysPPKP_RXB[0];
                s2=SysPPKP_RXB[1];
                // ���������� ������ ��� �������� �������� ��������� ����������
                SysPPKP_rxsize=0;
                SysPPKP_TXB[0]=ppkp;
                SysPPKP_TXB[1]=sensor;
                SysPPKP_TXB[2]=1;
                SysPPKP_txsize=3;
                _SEI();
                __enable_interrupt();
                while (SysPPKP_rxsize==0); // ������� ��������� ���������� ������
                SysPPKP_rxsize=s0; // ��������������� ������ ������
                SysPPKP_RXB[0]=s1;
                SysPPKP_RXB[1]=s2;
              }
              sensor++;
            }
            while(sensor<64);
          }
          ppkp++;
        }
        while(ppkp<64);
        KEY=kREDRAW;
      }
    }
    if (TemporaryEVENT&64)
    {
      //������������ ����
      unsigned int z;
      z=TemporaryZONE0<<2;
      if (ZoneDelays[z]==0xFE) ZoneDelays[z]=ZONEDELAY_AREA[z]; //������ ���� � ���� - ���
    }
    if (TemporaryEVENT&1) // ���� ������� ������� - �����
    {
      TotalFires++; // ����������� ����� ���������� �������
      _CLI();
#ifdef PUA1
      PORTB|=0x10; // �������� ���� "�����"
#else
      PORTD|=0x20;
#endif
      if (Fire120sec==0xFFFFFFFF) Fire120sec=TimeDate+120; //������������� ������ 120���
      _SEI();
      if (LED&LEDfire) // ���� ��� ����� "�����"
      {
        SetLEDon(LEDmfire); // �������� "����� �������"
        SndMode=1; // ���� - �����
      }
      else // ������ �����
      {
        SetLEDon(LEDfire); // �������� "�����"
        SndMode=1; // ���� - �����
      }
      ZoneJob(TemporaryZONE0,TemporaryEVENT); //������������ ���� �� ������� ����������
      ZoneJob(TemporaryZONE1,TemporaryEVENT);
      ZoneJob(TemporaryZONE2,TemporaryEVENT);
      ZoneJob(TemporaryZONE3,TemporaryEVENT);
    }
    if (TemporaryEVENT&2) // ���� ������� - �����
    {
      if (!(PU_STAT&4))
      {
#ifdef PUA1
        PORTB&=~0x40;
#else
        PORTD&=~0x10;
#endif
      }
      SetLEDflash(LEDfail); // �������� ������� "�����"
      if (SndMode!=1) SndMode=2; //���� ���� �� �����, �� ���� - �����
    }
    if (TemporaryEVENT&4) // ���� ������� - ��������������
    {
      IsPreAtt=1;
      if (SndMode!=1) SndMode=3; // ���� ���� �� �����, �� ���� - ��������������
    }
    if (TemporaryEVENT&8) // ���� ������� ��������������
    {
      if (SndMode==0) SndMode=4; // ���� ���� ��������, �� ���� - ��������� ������
    }
    return(TemporaryEVENT&7); // ���� ���� ������, ������, �������. - ���������� ������
  }
}

#include "RS.c"


//----------------------------------------------------
// �������� ������� �� ������ ���� ����
// ���������� ��� ������� ������ ���� ���� ��� ��� ��������� ������� � �.�.
//----------------------------------------------------
void ShowLog(void)
{
  char c;
  unsigned int i;
  char s[61]; // ������ �����
  struct TLogEvent ev; // ������� ���������

  unsigned int FirstEv; //��������� �� ������ ������������ �������
  unsigned int LastEv; //��������� �� ��������� ������������ �������
  unsigned int EvCnt1; //����� ������� � ������ ������
  unsigned int EvCnt2; //����� ������� �� ������ ������
  char mask=0; //����� ����������� - 0 - ���, 1 - ������, 2 - ������, 4 - �������.
  //������� ��� - ������ �����������
#ifdef PUA1
  PORTA|=0x01; // �������� ���������
#else
  PORTB|=0x40;
#endif
  KEY=0; // ��� ������� �� �������

L_CHECK_PRI:
  mask=0x7F; //���
  if (IsPreAtt) mask=4; //���� ��������������
  if (LEDflash&LEDfail) mask=2; //���� ���� ������
  if (LED&LEDfire) mask=1; //���� ���� ����� ������������ ������
L_CHECK_PRI1:
  i=SearchLastNormal();
  EvCnt1=1;
  EvCnt2=0;
  c=1; //������ �� �������
  for(;;)
  {
    ReadEvent(i,&ev); //������ �������
    if (ev.Second==0xFFFFFFFF) break; //��� �����
    PrintEventType(&ev,s,0); // ��������� ������� ��� ��������� �����
    if (TemporaryEVENT&mask)
    {
      //����� ���������� �������
      EvCnt2++; //������� ���������� �������
      LastEv=i; //����. �������
      if (c)
      {
        FirstEv=i; //������� ������ �������
        c=0; //������ �� ������ ������ �������
      }
    }
    i++;
    i&=0xFFF;
  }
  //��������� �� ������� �������� ������� � ������
  if (c&&mask)
  {
    //���� ������ � �� ����� �� ������ �������
    WarmBoot(0x4F); //����� �������
    Event2FIFO(0xff,0xff,5,0); // ������� "����� ���������"
    goto L_CHECK_PRI;
  }
  __disable_interrupt();
  Timer30sec=TimeDate+30;
  __enable_interrupt();
  do
  {
    ClrScr();
    if ((mask&0x80)||(FirstEv==LastEv))
    {
      //�������������� ������ ������ �������
      ReadEvent(FirstEv,&ev);
      PrintEventType(&ev,s,1);
      sprintf_P(CON,(STR)"%04d ",EvCnt1);
      memcpy(CON+5,s,35);
      PrintTimeDate(CON+40, ev.Second);
      memcpy(CON+59,s+36,21);
    }
    else
    {
      //�������� 2 �������
      ReadEvent(FirstEv,&ev);
      PrintEventType(&ev,s,1);
      sprintf_P(CON,(STR)"%04d ",EvCnt1);
      memcpy(CON+5,s,35);
      ReadEvent(LastEv,&ev);
      PrintEventType(&ev,s,1);
      sprintf_P(CON+40,(STR)"%04d ",EvCnt2);
      memcpy(CON+45,s,35);
    }
    UpdateConsole();
    if (FIFO2EventLog())
    {
#ifdef PUA1
      PORTA|=0x01; // �������� ���������
#else
      PORTB|=0x40;
#endif
      goto L_CHECK_PRI; // ���� ����� ������ ������� (������������)
    }
    __disable_interrupt();
    if (Timer30sec<TimeDate) //������ 30���
    {
      __enable_interrupt();
      if (!SndMode)
      {
#ifdef PUA1
        PORTA&=~0x01; //��������� ���������
#else
        PORTB&=~0x40;
#endif
      }
      goto L_CHECK_PRI;
    }
    __enable_interrupt();
    c=KEY; // ��������� ������
    if (c==0x81) goto L_EXIT;
    if (c)
    {
      KEY=0;
      __disable_interrupt();
      Timer30sec=TimeDate+25;
      __enable_interrupt();
#ifdef PUA1
      PORTA|=0x01; // �������� ���������
#else
      PORTB|=0x40;
#endif
    } //���� ������ �� ����������������� ������ 30 ���
    if (SndMode) // ���� ���� ����
    {
      if (c==kACK) // � ������ ���� ����.
      {
        Event2FIFO(0xFF,0xFF,32,0);
        if (!(COPY_PU_STAT&0x10)) SetLEDon(LEDsndoff); //�������� ��������� ���� ����
        SndMode=0; //��������� ����
        if (COPY_PU_STAT&0x08)
        {
#ifdef PUA1
          PORTB&=~0x10; // ��������� ���� "�����"
#else
          PORTD&=~0x20;
#endif
        }
        if (COPY_PU_STAT&4)
        {
#ifdef PUA1
	  PORTB&=~0x40; //��������� ���� "����� 120���"
#else
	  PORTD&=~0x10;
#endif
        }
        Fire120sec=0xFFFFFFFF; //���������� ������ 120���
      }
      else //��� ������� ��� ������ �� ���� ����.
      {
        c=0;
        if ((SndMode!=4)&&(!(COPY_PU_STAT&0x10))) SetLEDoff(LEDsndoff); //���� ���� - �� ��������� ������, �� ��������� ��������� ���� ����.
      }
    }
    else //���� ��������
    {
      switch(c)
      {
      case kRT:
        //������������ ������ ������
        mask&=0x7F;
        switch(mask)
        {
        case 0x7F:
          if (LED&LEDfire) {mask=1; goto L_CHECK_PRI1;}
        case 1:
          if (LEDflash&LEDfail) {mask=2; goto L_CHECK_PRI1;}
        case 2:
          if (IsPreAtt) {mask=4; goto L_CHECK_PRI1;}
        case 4:
          mask=0x7F;
          goto L_CHECK_PRI1;
        }
        break;
      case kLT: //������������ ������ ������
        mask^=0x80;
        break;
      case kUP:
        LastEv=FirstEv; //������ ������� -> ������ �������
        EvCnt2=EvCnt1;
        if (EvCnt1==1) break; //������ ������ �������
        //���� ���������� �������
        EvCnt1--;
        do
        {
          FirstEv--;
          FirstEv&=0xFFF;
          ReadEvent(FirstEv,&ev); //������ ����. �������
          if (ev.Second==0xFFFFFFFF) goto L_CHECK_PRI; //������
          PrintEventType(&ev,s,0); // ��������� ������� ��� ��������� �����
        }
        while(!(TemporaryEVENT&mask)); //���� �� �������
        break;
      case kDW:
        FirstEv=LastEv; //������ ������� -> ������ �������
        EvCnt1=EvCnt2;
        //���� ��. �������
        EvCnt2++;
        do
        {
          LastEv++;
          LastEv&=0xFFF;
          ReadEvent(LastEv,&ev); //������ ��. �������
          if (ev.Second==0xFFFFFFFF)
          {
            //��� ������ �������
            LastEv=FirstEv;
            EvCnt2=EvCnt1;
            break;
          }
          PrintEventType(&ev,s,0); // ��������� ������� ��� ��������� �����
        }
        while(!(TemporaryEVENT&mask)); //���� �� �������
        break;
      }
    }
  }
  while (c!=kESC); //���� ���� �� ������ ������ �����
  WarmBoot(0x4E); //���� ����� - �� ������ �������
L_EXIT:
  KEY=kREDRAW;
}

//--------------------------------------
// ���� ���� ����������� ����������, �� �������� "����������"
//--------------------------------------
void IncOfflineCounter(void)
{
  SetLEDon(LEDoff);
  IsOffline=1;
}

//-------------------------------------------------------------------
// �������� ������� �� �������
// �� ����� - ����, ���� �� �������� ���� � ����� (��� ��������� ����)
// ��������� �������� ��������� FIFO2EventLog
//-------------------------------------------------------------------
char KBD(char mode)
{
  char c,total,on;
  unsigned long td;
  unsigned int tt;
  static char CurrentTestPPKP;
  static char ViewSuprDZ; //������� ��� ��� ��������� ����. �����
  static char ViewSuprDZ_flag; //���� ��� ��������� �����
  unsigned int fs;
  void TestLPT(void);
  void SearchOnSensors(char ppkp, char *tot_sens, char *on_sens); //�������� ��� ��������
  if (!mode) UpdateConsole();
  do
  {
    fs=check_flash_sector_number;
    if (fs<512)
    {
      if ((char)FlashSectorCRC(fs,257)) //� ���������� ����������� ������
      {
        //�� ������� CRC, ���������� ������
        fs|=512;
        Event2FIFO(255,255,36,0);
      }
      else
      {
        fs=(fs+1)&0x1FF;
      }
    }
    check_flash_sector_number=fs;
    //���������/���������� ���������� ����������
    if (!CurrentTestPPKP)
    {
      if (IsOffline==0) SetLEDoff(LEDoff);
      IsOffline=0; //������ �����
    }
    if (ZONEMF_AREA[CurrentTestPPKP]&0x80)
    {}
    else
    {
      //���� ���������
      unsigned int z=CurrentTestPPKP;
      z<<=2;
      if (ZONEPOINT_AREA[z++]!=0xFFFF) IncOfflineCounter();
      if (ZONEPOINT_AREA[z++]!=0xFFFF) IncOfflineCounter();
      if (ZONEPOINT_AREA[z++]!=0xFFFF) IncOfflineCounter();
      if (ZONEPOINT_AREA[z]!=0xFFFF) IncOfflineCounter();
    }
    if (CurrentTestPPKP<64) //���� �� �������������� ��� ��-�
    {
      SearchOnSensors(CurrentTestPPKP,&total,&on); //������� ������� ����� ����������� ��� �������� ��-� � ������� ���.
      if (PPKP_AREA[CurrentTestPPKP]&0x80) //���� ��-� �������
      {
        if (total!=on) IncOfflineCounter(); //���� ��������� ���-�� �� ��������� � ���-��� ���, �������� "�����"
      }
      else //��-� ��������
      {
        if (total>0) IncOfflineCounter(); //� ���������� ����, �������� "�����"
      }
    }
    if ((CurrentTestPPKP&0xE0)==64)
    {
      //��������� ���
      if ((UPS_AREA[CurrentTestPPKP&0x1F]&0xC0)==0x40) IncOfflineCounter();
    }
    if ((CurrentTestPPKP&0xE0)==96)
    {
      //��������� ���/���
      if ((BRA_AREA[CurrentTestPPKP&0x1F]&0xC0)==0x40) IncOfflineCounter();
    }
    CurrentTestPPKP++; //��������� � ���������� ��������
    tt=TRIALTIME; //��������� ���������� ������� �����
    switch(tt)
    {
    case 0xFFFF: break; //�������� �����
    case 0x0000: NeedMoney=1; break; //����� �������
    default: if (NeedDecTrialTime) // ���� ������ ���
    {
      NeedDecTrialTime=0;
      tt--; //��������� �����
      TRIALTIME=tt; //� ���������� ���
    }
    }
    _CLI();
    td=TimeDate;
    _SEI();
    if ((td>Timer30sec)) //���� ������ 30 ���
#ifdef PUA1
      PORTA&=~0x01; //��������� ���������
    else PORTA|=0x01; //����� �������� ���������
#else
    PORTB&=~0x40;
    else PORTB|=0x40;
#endif
    if (mode) // ���� ����������, �������� ���� � �����
    {
      char z=ViewSuprDZ;
      PrintTimeDate(CON+40,td);
      if (!z) z=1;
      if (ZoneTF[(z<<1)+1]==0xFF)
      {
        //� ���� ����������� ����������
        if (!IsSuprDZ)
        {
          IsSuprDZ=1;
          KEY=kREDRAW;
        }
        sprintf_P(CON+58,(STR)"����%03d",z); //�������� ���������
        if ((char)TimeDate!=ViewSuprDZ_flag) goto LNDZ;
      }
      else
      {
      LNDZ:
        do
        {
          z++;
          if (ZoneTF[(z<<1)+1]==0xFF) break; //������� ����
        }
        while(z&0x3F);
        ViewSuprDZ_flag=(char)TimeDate;
      }
      ViewSuprDZ=z;
      UpdateConsole();
    }
    TestLPT();
    if (MCUCSR&(1<<WDRF))
    {
      Event2FIFO(255,255,35,0); //����� �� WDT
      MCUCSR=0;
    }
    PGMCRC256();
    c=KEY;
    if (
        FIFO2EventLog() ||   // ���� ������ �������
          ((c==kACK)&&(!IS_NOLOG_ACK)) || // ��� ������ ���� ����.
            (td>Timer30sec && (LED&LEDfire || LEDflash&LEDfail) ) // ��� ������ 30 ��� � ���� ������ ��� ������
              )
    {
      c=KEY=0; //��� �������
      ShowLog(); // ������������� �������
    }
  }
  while(c==0); //���� ��� ������� �� �������
#ifdef PUA1
  PORTA|=0x01; //�������� ���������
#else
  PORTB|=0x40;
#endif
  Timer30sec=td+25; //������������� ������ 30 ���
  KEY=0; //������� �����
  if ((c==kESC)&&IS_ALLWAYS_RESET)
  {
    WarmBoot(0x4E);
    c=kREDRAW;
  }
  SendCmdLCD(0x0C); //��������� ������
  CursorPos=80;
  return(c); //���������� ��� ������� �������
}

//--------------------------------------
// ���������
//--------------------------------------
#include "vm.c"

//--------------------------------------
// ��������� �������������� ������
//--------------------------------------
#include "EditSelect.c"

//----------------------------------------------------------
// �������������� ������������ �����������
//----------------------------------------------------------
#include "SensorConfig.c"

//--------------------------------------
// ������������ ��
//--------------------------------------
#include "ConfigPU.c"

//--------------------------------------
// ���������� �������
//--------------------------------------
#include "Boot.c"

//--------------------------------------------------
// �������
//--------------------------------------------------
#include "Debug.c"

//--------------------------------------------------
// �������� ����
//--------------------------------------------------

// ���������� ��������� � ������ � ������ ��� �������������
void ShowC(void)
{                             //012345678901234567890123
  static __flash char ecode[]="��� ������������� %04X%02X";

  int tt;
  char pos=18;
  char crc=0;
  char c;

  tt=TRIALTIME; //������� �������
  do
  {
    ClrScr();
    //���������� ���� ��� ������ ��������� �� 4 ���
    if ((char)TimeDate&4)
    {
      sprintf_P(CON,(STR)"��-� ������ 4.0   ���� \"��������\"");
      sprintf_P(CON+40,(STR)"�������, ������� +380(572)528026");
    }
    else
    {
      sprintf_P(CON,(STR)"����/����� ������: " __DATE__ " " __TIME__);
    }
    UpdateConsole();
    if (FIFO2EventLog()) {Timer30sec=0; break;} //���� ���� ������� - �����
  } while (KEY==0); //���� �� ������ ������
  if ((KBD(0)==kSECRET) && (tt!=0xFFFF)) //���� ������ ������� ������ � �� ������ ������
  {
    ClrScr(); //�������� �����
    tt=0;
    do
    {
      sprintf_P(CON,ecode,tt,crc); //��������� � ���� �������������
      CursorPos=pos; //������������� ������
      c=KBD(0); //������� ��� �������
      if ((c>=32 && c<128)||(c>=192)) //���� ���������� �������
      {
        if (Edit_hi(c,pos,18,&tt)) {c=kRT;} //�������� �������������
        if (Edit_hb(c,pos,22,&crc)) {c=kRT;}
      }
      switch(c) //������������ ������
      {
      case kLT: //������ "<<"
        switch (pos)
        {
        case 18: c=kESC; break; //������ ��� ����� �����
        default: pos--; //����������� ������� �����
        }
        break;
      case kRT:
        switch (pos)
        {
        case 23: break; //������ ��� ������ �����
        default: pos++; //����������� ������
        }
        break;
      case kUP:
        Select_hi(pos,18,1,&tt); //�������� ������������� (����������)
        Select_hb(pos,22,1,&crc);
        break;
      case kDW:
        Select_hi(pos,18,-1,&tt); //�������� ������������� (����������)
        Select_hb(pos,22,-1,&crc);
        break;
      }
    }
    while (c!=kESC); // ���� ���� �� �����
    //������� ���
    c=CRC8tab[(char)tt];
    c=CRC8tab[((char)(tt>>8))^c];
    if ((!CRC8tab[crc^c])&&tt) // ���� ��� �������
    {
      NeedMoney=0; // ������� �����
      TRIALTIME=tt; //���������� ���-�� ��������� �����
    }
  }
}

//----------------------
// �������� ������ �������
//----------------------
#include "ViewLog.c"

//--------------------------
// ������������ ��-�
//--------------------------
#include "ConfigPPKP.c"


//---------------------------
// ������������ ���
//---------------------------
#include "Zone.c"
#include "ConfigTK.c"

//-------------------------------------
// ��������� �������
//-------------------------------------
#include "EditAdr.c"


//-------------------------------------------------------
#include "MainCAN.c"

//-------------------------------------------------------
#include "ConfigBRA.c"
#include "ConfigUPS.c"

//-------------------------------------------------------
// ��������� ���������� ������������
//-------------------------------------------------------
void TestKBD(void)
{
  static __flash STR testm[]=
  {
    (STR)"����� <���� ����.>",
    (STR)"����� <�����>",
    (STR)"����� <������>",
    (STR)"����� <�����>",
    (STR)"����� <����>",
    (STR)"����� <�����>"
  };

  char tm;
  char c;
  tm=TEST_STAT;
  SetLEDflash(LEDfire|LEDmfire|LEDoff|LEDfail|LEDsndoff|LEDsysf);
  for(;;)
  {
    if (tm>5) return;
    ClrScr();
    sprintf_P(CON,testm[tm]);
    UpdateConsole();
    KEY=0;
    while((c=KEY)==0);
    switch(tm)
    {
    case 0: if (c!=kACK) tm=0xFF; break;
    case 1: if (c!=kLT) tm=0xFF; break;
    case 2: if (c!=kRT) tm=0xFF; break;
    case 3: if (c!=kUP) tm=0xFF; break;
    case 4: if (c!=kDW) tm=0xFF; break;
    case 5: if (c==kESC) {TEST_STAT=6; return; }
    default: tm=0xFF;
    }
    tm++;
  }
}

void TestLPT(void)
{
  static unsigned int LPTstep=128;
  //static char LPTbyte;
  char c;
  char s;
  char stb;
  if (TSTS!=18) return;

  s=(char)LPTstep;
  stb=LPTstep>>8;

#ifdef PUA1
  PORTC=~s; //������� ������
  if (stb) PORTA&=0x7F; else PORTA|=0x80;
  delay(100);
  s=(PINA&0x40)?0:1;
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
  PORTC=~c;
  if (stb) PORTB&=0xEF; else PORTB|=0x10;
  delay(100);
  s=(PINB&0x20)?0:1;
#endif
  c=lpt_tab[LPTstep>>3]&(0x80>>(LPTstep&7))?1:0;
  //if (s) LPTbyte=(LPTbyte<<1)+1; else LPTbyte<<=1;

  memset(CON,' ',40);
  sprintf_P(CON,(PGM_P)"���� �������� %03X=%d/%d",LPTstep,s,c);
  UpdateConsole();
  //if ((LPTstep&7)==7)
  //{
  // _EEPUT((BRVU_AREA+(LPTstep>>3)),LPTbyte);
  //}
  if (c^s)
  {
    LPTstep=128;
    return;
  }
  LPTstep++;
  if (LPTstep==256) LPTstep=256+128;
  if (LPTstep==512)
  {
    TSTS=19;
    TEST_STAT=19;
    KEY=1;
  }
}

//�������������� ���������� � �������� ����
void AutoSupr(void)
{
  char z;
  char c=0;
  do
  {
    z=1;
    //���� ����
    do
    {
      if (ZoneTF[(z<<1)+1]==0xFF) break; //������� ����
    LNEXT:
      z++;
    }
    while(z);
    if (!z)
    {
      if (c)
      {
        c=0;
        continue;
      }
      else
        return;
    }
    ClrScr();
    sprintf_P(CON,(STR)"���� %03d � ������������� ������",z);
    sprintf_P(CON+40,(STR)"<������>-������������  ������-���������");
    c=KBD(0);
    switch(c)
    {
    case kLT: c=kESC; break;
    case kRT:
      //��������������� ����������
      Event2FIFO(255,255,37,z); //�������� ��������� � Event2FIFO
      return;
      //      break;
    default:
      goto LNEXT;
    }
  }
  while(c!=kESC);
}

//-------------------------------------------------------
// �������� ������
//-------------------------------------------------------
void main(void)
{
  unsigned long td;
  char c,i=0;
  unsigned int z;
  InitSystem(); //�������������� ����������
  InitLCD(); //�������������� ���
  memcpy_P(CON,
           "�������� ������ ��������                ",40);
  memcpy_P(CON+40,MCUCSR&(1<<WDRF)?
           "��������� �������������!!!              ":
             "                                        ",40);
  if (MCUCSR&(1<<WDRF))
  {
    PORTB|=0x80; //�������� ����...
#ifdef PUA1
    PORTD=LEDsysf;
#else
    PORTA=LEDsysf;
#endif
  }
  else
  {
#ifdef PUA1
    PORTD=0;
#else
    PORTA=0;
#endif
  }
  UpdateConsole();
  PGMCRC();
  ClrScr();
  PUID=PU_ID;
  COPY_PU_STAT=PU_STAT;
  SysPPKP_txsize=0; //����� ����� ���������� ������
  InitCAN(PUID);
  z=0;
  do
  {
    ZoneDelays[z]=0xFE;
    z++;
  }
  while (z!=1024);
  SearchTopEventLog(); //���� ��������� ������� � ������
  LogFileRP_CAN=LogFileWP;
  LogFileRP_DPU=LogFileWP;
  ConstructABONS(); //������ ������ �� ��-�
  __watchdog_reset();
  WDTCR=0x08;
  WDTCR=0x18;
  WDTCR=0x0B; //��������� ���������� ������
#ifdef PUA1
  if (!(PINA&0x40)) PrinterOnLine=0; //��������� �������������� ��������
#else
  if (!(PINB&0x20)) PrinterOnLine=0;
#endif
  InitRS();
  _SEI();
  printf_P(Message_MainMenu[i]); //�������� ���������
  _CLI(); //��������� ��������� ������� ����������
  DDRD&=0xFE;
  PORTD|=0x01;
  EIMSK=0x01;
  td=TimeDate;
  _SEI();
  putchar(' ');
  LPT_PrintTimeDate(td);
  PRINT_CRLF;
  PrintSeparator();
L_TEST1:
#ifdef PUA1
  PORTA|=0x01; //�������� ���������
#else
  PORTB|=0x40;
#endif
  TestKBD();
  TSTS=TEST_STAT;
  //WarmBoot(0x4F);
  Event2FIFO(255,255,0,0);
  Event2FIFO(255,255,4,0x4F);
  PrinterOnLine=1000; //����� �� ������� "������� � ������ OffLine"
  for(;;)
  {
    ClrScr();
    // ���� ����������� �����, �������� ���������, ����� ����� ����
    if (NeedMoney) sprintf_P(CON,Message_NeedMoney); else
    {
      if (i)
      {
        sprintf_P(CON,Message_MainMenu[i]);
        sprintf_P(CON+66,(STR)"<������>-�����");
      }
      else
      {
        if (TSTS<6) goto L_TEST1;
        if (TSTS>18)
        {
          if (IsSuprDZ&&ISMASTER) sprintf_P(CON+28,(STR)"<������>-���");
          sprintf_P(CON,Message_MainMenu[i]);
          sprintf_P(CON+69,(STR)"<����>-����");
          if (DOUBLE_PU)
          {
            if (ISMASTER)
            {
              sprintf_P(CON+8,(STR)"��������");
            }
            else
            {
              sprintf_P(CON+8,(STR)"�����������");
            }
          }
        }
        else
        {
          c=TSTDBG;
          sprintf_P(CON,testm1[TSTS-6],c);
        }
      }
    }
    c=KBD(1); //���� �������
    if (
        (TSTS>5)&&(TSTS<16) //���� ����� ������� �� PC ����.
          &&((TSTS-6+'0')==c) //� ������ ��������� �������
            )
    {
      TSTS++; //��������� ��������
      TEST_STAT=TSTS;
    }
    if (c>=0x11 && c<=0x19) //���� F1..F8
    {
      i=c-0x10; //������������� ������ �����
      c=kRT; //� ������ � ����
    }
    if (NeedMoney) i=0; //���� ����������� �����, �� �� ������ � ����
    if ((!ISMASTER)&&(i>2)) i=0;
    switch(c)
    {
    case kLT:
      i=0;
      break;
    case kRT: //������ ������� ����
      switch(i) //���� � ����� ����
      {
      case 0: if (IsSuprDZ&&ISMASTER) AutoSupr(); else ShowC(); break;
      case 1: ViewLog(); break;
      case 2: Boot(); break;
      case 3: ConfigPU(); break;
      case 4: ConfigPPKP(); break;
      case 5: ConfigSensor(); break;
      case 6: ConfigZone(); break;
      case 7: break;
      case 8: ConfigBRA(); break;
      case 9: ConfigUPS(); break;
      case 10: EditAdresses(); break;
      case 11: Debug(); break;
      } ClrScr(); break;
      //����� ������ ����
    case kUP: if (i>0) i--; if (i==7) i=6; break;
    case kDW: if (i<11) i++; if (i==7) i=8; break;
    }
    if ((!ISMASTER)&&(i>2)) i=0;
  }
}

