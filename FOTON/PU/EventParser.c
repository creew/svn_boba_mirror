//-----------------------------
// ����������� �������
// ��������� ������ �������� �������� ������� �������� ��
//-----------------------------

struct TEventDesc
{
  unsigned long mask;
  unsigned long val;
  char eventmode;
  char printtype;
  STR message;
};
struct TPPKPEEPROM_Tab
{
  char adr;
  char mask;
  STR mess0;
  STR mess1;
};

struct TEventSupr
{
  unsigned long ev;
  char evtype;
  char text[24];
  char bitn;
  char bitv;
  char dummy;
};

static __flash struct TPPKPEEPROM_Tab PPKPEEPROM_Tab[]=
{
  {0x0,0x0,(STR)"",(STR)""},
  //012345678901234567890123  012345678901234567890123
  {0x21,0x02,(STR)"�������� �����"         ,(STR)"����������� �����"       },
  {0x21,0x01,(STR)"����������� �����"      ,(STR)"������� �����"           },
  {0x22,0x10,(STR)"���� �����/�����"       ,(STR)"���� � ����"             },
  {0x22,0x20,(STR)"�������������� ���."    ,(STR)"�������������� ����."    },
  {0x23,0x01,(STR)"�������������� ����."   ,(STR)"�������������� ���."     },
  {0x24,0x10,(STR)"��������� ����� ����."  ,(STR)"��������� ����� ���."    },
  {0x21,0x08,(STR)"���������� �����-�"     ,(STR)"���������� �����-�"      },

  {0x0,0x0,(STR)"",(STR)""}

};

static __flash struct TEventDesc EventsTab[]=
{
  //                               0123456789
  {0x00FFC0C0,0x00004000,8,0,(STR)"���������."},
  {0x00FFC0C0,0x00014000,2,0,(STR)"�����"     },
  {0x00FFC0C0,0x00024000,2,0,(STR)"�������"   },
  {0x00FFC0C0,0x00034000,8,0,(STR)"1-� �����" },
  {0x00FFC0C0,0x00044000,2,0,(STR)"K� ������" },
  {0x00FFC0C0,0x00054000,2,0,(STR)"����������"},
  {0x00FFC0C0,0x00064000,2,0,(STR)"������"    },
  {0x00FFC0C0,0x00074000,2,0,(STR)"��� �����" },
  {0x00FFC0C0,0x00084000,2,0,(STR)"��� � ����"},
  {0x00FFC0C0,0x00094000,2,0,(STR)"������.���"},
  {0x00FFC0C0,0x000A4000,2,0,(STR)"0-� ������"},
  {0x00FFC0C0,0x000B4000,2,0,(STR)"��. ������"},
  {0x00FFC0C0,0x000C4000,2,0,(STR)"�����.����"},
  {0x00FFC0C0,0x000D4000,2,0,(STR)"���. ����" },

  {0x00FFC0C0,0x00100000,2,0,(STR)"��. EEPROM"},
  {0x00FFC0C0,0x00200000,2,0,(STR)"��. ������"},
  {0x00FFC0C0,0x00210000,8,0,(STR)"��. ������"},

  {0xFCF3C0C0,0x0C000000,8,1,(STR)"����� ���."},
  {0xFCF3C0C0,0x0C010000,2,1,(STR)"�����"     },
#ifdef SECURITY
  {0xFCF3C0C0,0x0C020000,1,1,(STR)"�������"   },
#else
  {0xFCF3C0C0,0x0C020000,1,1,(STR)"�����"     },
#endif

  {0xFCF3C0C0,0x0C030000,2,1,(STR)"�� ������" },


  //--- ���-� ---------------        0123456789
  {0xFFFFC0C0,0x14000000,8,0,(STR)"����. �� 1"},
  {0xFFFFC0C0,0x14010000,2,0,(STR)"����� �� 1"},
  {0xFFFFC0C0,0x14020000,8+16,0,(STR)"����. ���."},
  {0xFFFFC0C0,0x14030000,2,0,(STR)"�� �� 1"},

  {0xFFFFC0C0,0x140C0000,8,0,(STR)"����.�����"},
  {0xFFFFC0C0,0x140D0000,2,0,(STR)"���. �����"},
  {0xFFFFC0C0,0x140E0000,8+16,0,(STR)"����.�����"},
  {0xFFFFC0C0,0x140F0000,2,0,(STR)"�� �����"},

  {0xFFFFC0C0,0x14080000,8,0,(STR)"����. �� 3"},
  {0xFFFFC0C0,0x14090000,2,0,(STR)"����� �� 3"},
  {0xFFFFC0C0,0x140A0000,1+32,0,(STR)"����. ����"},
  {0xFFFFC0C0,0x140B0000,2,0,(STR)"�� �� 3"},

  {0xFFFFC0C0,0x14040000,8,0,(STR)"����. �� 4"},
  {0xFFFFC0C0,0x14050000,2,0,(STR)"����� �� 4"},
  {0xFFFFC0C0,0x14060000,8+64,0,(STR)"���� ���"}, //���� � ����
  {0xFFFFC0C0,0x14070000,2,0,(STR)"�� �� 4"},
  //------------------------------------------------------------------------------
  {0xFFFFC0C0,0x11000000,8,0,(STR)"����� ���."},
  {0xFFFFC0C0,0x11010000,8,0,(STR)"������"    },
  {0xFFFFC0C0,0x11020000,2,0,(STR)"����� ��"  },
  {0xFFFFC0C0,0x11030000,8,0,(STR)"�������.��"},

  {0x00FFC0C0,0x00000000,8,0,(STR)"�����"     },
  {0x00FFC0C0,0x00010000,4,0,(STR)"�������."  },
#ifdef SECURITY
  {0x00FFC0C0,0x00020000,1,0,(STR)"�������"   },
#else
  {0x00FFC0C0,0x00020000,1,0,(STR)"�����"     },
#endif
  {0x00FFC0C0,0x00030000,8,0,(STR)"������"    },
  {0xFEFFC0C0,0x02040000,2,0,(STR)"����� ��"  },
  {0xFEFFC0C0,0x04040000,2,0,(STR)"����� ��"  },
  {0xFEFFC0C0,0x04080000,2,0,(STR)"���������" },
  {0xFFFFC0C0,0x06040000,2,0,(STR)"����� ��"  },
  {0xFFFFC0C0,0x07040000,2,0,(STR)"����� ��"  },
  {0xFFFFC0C0,0x01040000,2,0,(STR)"�� ������" },

  {0x00FFC0C0,0x00FCC000,8,10,(STR)"������-�%02d"},         //FC, Sensor=C0+pnt, Event2 - val & mask
  {0x00FFC0C0,0x00FDC000,8,2,(STR)"��-� %02d ��������"},         //FD, Event2 - ���������� EEPROM
  {0x00FFC0C0,0x00FEC000,8,2,(STR)"��-� %02d �������"},          //FE, Event2 - ���������� EEPROM
  {0x00FFC0C0,0x00FFC000,2,11,(STR)"��� ������ � ��-� %02d/RS%d"},     //FF

  {0x00FFC0E0,0x00FDC040,8,2,(STR)"��� %02d ��������"},         //FD, Event2 - ���������� EEPROM
  {0x00FFC0E0,0x00FEC040,8,2,(STR)"��� %02d �������"},          //FE, Event2 - ���������� EEPROM
  {0x00FFC0E0,0x00FFC040,2,11,(STR)"��� ������ � ��� %02d/RS%d"},     //FF

  {0x0000FFE0,0x00000040,8,12,(STR)"��� %02d �������� �����"    },
  {0x0000FFE0,0x00000140,2,12,(STR)"��� %02d ����� �������� ����"    },
  {0x0000FFE0,0x00000240,2,12,(STR)"��� %02d ����� ��������� ����"    },
  {0x0000FFE0,0x00000340,8,12,(STR)"��� %02d ����� ���"    },
  {0x0000FFE0,0x00000440,2,12,(STR)"��� %02d ����� ����� ������� 1"    },
  {0x0000FFE0,0x00000540,2,12,(STR)"��� %02d ����� ����� ������� 2"    },

  {0x00FFC0E0,0x00FDC060,8,2,(STR)"��� %02d ��������"},         //FD, Event2 - ���������� EEPROM
  {0x00FFC0E0,0x00FEC060,8,2,(STR)"��� %02d �������"},          //FE, Event2 - ���������� EEPROM
  {0x00FFC0E0,0x00FFC060,2,11,(STR)"��� ������ � ��� %02d/RS%d"},     //FF

  //{0x00FFC0E0,0x00104060,8,13,(STR)"��� %02d �������������� ���� %02d"},
  {0x00FFC0E0,0x00114060,2,13,(STR)"��� %02d ����� ���� %02d"     },
  //  {0x00FFC0E0,0x00124060,4,13,(STR)"��� %02d �������� ���� %02d"  },
  {0x00FFC0E0,0x00134060,2,13,(STR)"��� %02d �� ���� %02d"},
  //{0x00FFC0E0,0x00144060,2,13,(STR)"��� %02d ������. 1 ���� %02d" },
  //{0x00FFC0E0,0x00154060,2,13,(STR)"��� %02d ������. 2 ���� %02d" },
  //{0x00FFC0E0,0x00164060,2,13,(STR)"��� %02d ������. 3 ���� %02d" },
  //{0x00FFC0E0,0x00174060,2,13,(STR)"��� %02d ������. 4 ���� %02d" },

  {0x00FFFFE0,0x00126160,8,13,(STR)"��� %02d ������� ��1"},
  {0x00FFFFE0,0x00126260,8,13,(STR)"��� %02d ������� ��2"},
  {0x00FFFFE0,0x00126360,8,13,(STR)"��� %02d ������� ��3"},
  {0x00FFFFE0,0x00126460,8,13,(STR)"��� %02d ������� ��4"},
  {0x00FFC0E0,0x00124060,8,13,(STR)"��� %02d ��������� ���� ����� %02d"},
  {0x00FFC0E0,0x00144060,8,13,(STR)"��� %02d ��������� ��� %02d"},
  {0x00FFC0E0,0x00154060,8,13,(STR)"��� %02d ���������� ��� %02d"},

  {0x00FFFFE0,0x00016560,2,13,(STR)"��� %02d ��� ������� ��1"},
  {0x00FFFFE0,0x00016660,2,13,(STR)"��� %02d ��� ������� ��2"},
  {0x00FFFFE0,0x00016760,2,13,(STR)"��� %02d ��� ������� ��3"},
  {0x00FFFFE0,0x00016860,2,13,(STR)"��� %02d ��� ������� ��4"},

  {0x00FFFFE0,0x00016960,2,13,(STR)"��� %02d ���������� ���� ��1"},
  {0x00FFFFE0,0x00016A60,2,13,(STR)"��� %02d ���������� ���� ��2"},
  {0x00FFFFE0,0x00016B60,2,13,(STR)"��� %02d ���������� ���� ��3"},
  {0x00FFFFE0,0x00016C60,2,13,(STR)"��� %02d ���������� ���� ��4"},

  {0x00FFFFE0,0x00016D60,2,13,(STR)"��� %02d ������ ���� ��1"},
  {0x00FFFFE0,0x00016E60,2,13,(STR)"��� %02d ������ ���� ��2"},
  {0x00FFFFE0,0x00016F60,2,13,(STR)"��� %02d ������ ���� ��3"},
  {0x00FFFFE0,0x00017060,2,13,(STR)"��� %02d ������ ���� ��4"},

  {0x00FFC0E0,0x00014060,2,13,(STR)"��� %02d ����� ���� ����� %02d"},

  {0x00FFFFE0,0x00000060,8,13,(STR)"��� %02d �������� �����"},
  {0x00FFFFE0,0x00010060,2,13,(STR)"��� %02d ����� �������� ����"},
  {0x00FFFFE0,0x00020060,2,13,(STR)"��� %02d ����� ��������� ����"},
  {0x00FFFFE0,0x000C0060,2,13,(STR)"��� %02d ��������� ������"},
  {0x00FFFFE0,0x000D0060,2,13,(STR)"��� %02d ������ �������"},
  {0x00FFFFE0,0x00100060,2,13,(STR)"��� %02d ����� EEPROM"},

  {0x00FFC0C0,0x0000C000,8,0,(STR)"��������"},                   //0, Event2 - ���������� ���/���� � ���
  {0x00FFC0C0,0x0001C000,8,0,(STR)"�������"},                    //1, Event2 - ���������� ���/���� � ���
  {0x00FEC080,0x0002C000,8,4,(STR)"���.������"},                //2..3

  {0x00FF00C0,0x00000080,8,8,(STR)"��������������"},             //������� ��� ���
  {0x00FF00C0,0x00010080,2,8,(STR)"����� ������"},
  {0x00FF00C0,0x00020080,8,8,(STR)"���� ����������� %03d"},
  {0x00FF00C0,0x00030080,8,8,(STR)"���. ���� ����. %03d"},
  {0x00FF00C0,0x00040080,8,8,(STR)"����. %03d � ����. ������"},
  {0x00FF00C0,0x00050080,8,8,(STR)"��������� �� ��������"},

  {0x00FFFFFF,0x0000FFFF,8,5,(STR)"��������� �������"},              // 0
  {0x00FFFFFF,0x0001FFFF,2,5,(STR)"EventFIFO overflow"},             // 1
  {0x00FFFFFF,0x0002FFFF,8,5,(STR)"����� ������"},                   // 2
  {0x00FFFFFF,0x0003FFFF,8,5,(STR)"Flash ������"},                   // 3
  {0x00FFFFFF,0x0004FFFF,8,5,(STR)"�����"},                          // 4
  {0x00FFFFFF,0x0005FFFF,2,5,(STR)"����� ���������"},                // 5
  {0x00FFFFFF,0x0006FFFF,2,5,(STR)"��� ������ � ���1"},              // 6
  {0x00FFFFFF,0x0007FFFF,2,5,(STR)"��� ������ � ���2"},              // 7
  {0x00FFFFFF,0x0008FFFF,2,5,(STR)"��� ������ � ��"},                // 8
  {0x00FFFFFF,0x0009FFFF,8,6,(STR)"�����-��� �������� ����"},        // 9
  {0x00FFFFFF,0x000AFFFF,8,6,(STR)"����� ���"},                   // 10
  {0x00FFFFFF,0x000BFFFF,8,6,(STR)"����� ������ ���"},            // 11
  {0x00FFFFFF,0x000CFFFF,2,6,(STR)"����� ���"},                      // 12
  {0x00FFFFFF,0x000DFFFF,2,6,(STR)"����� �������� ����"},            // 13
  {0x00FFFFFF,0x000EFFFF,2,6,(STR)"����� ���-��� ����"},             // 14
  {0x00FFFFFF,0x000FFFFF,2,6,(STR)"����� FU1"},                      // 15
  {0x00FFFFFF,0x0010FFFF,2,6,(STR)"��� ������ � ���"},               // 16
  {0x00FFFFFF,0x0011FFFF,8,5,(STR)"������ ���. �������/����"},       // 17
  {0x00FFFFFF,0x0012FFFF,8,5,(STR)"����� ���. �������/����"},        // 18
  {0x00FFFFFF,0x0013FFFF,2,5,(STR)"������� � ������ OffLine"},       // 19
  {0x00FFFFFF,0x0014FFFF,8,5,(STR)"����� ������ �����������"},       // 20
  {0x00FFFFFF,0x0015FFFF,8,5,(STR)"����� ������ �����������"},       // 21
  {0x00FFFFFF,0x0016FFFF,2,5,(STR)"�������� ������� ��-�"},          // 22
  {0x00FFFFFF,0x0017FFFF,2,5,(STR)"��� ������ � ��"},                // 23
  {0x00FFFFFF,0x0018FFFF,2,5,(STR)"��� ������ � ���"},               // 24
  {0x00FFFFFF,0x0019FFFF,2,5,(STR)"��� ������ � ���"},               // 25
  {0x00FFFFFF,0x001AFFFF,2,5,(STR)"��� ������ � ���"},               // 26
  {0x00FFFFFF,0x001BFFFF,2,5,(STR)"����� CANTX0"},                   // 27
  {0x00FFFFFF,0x001CFFFF,2,5,(STR)"����� CANTX1"},                   // 28
  {0x00FFFFFF,0x001DFFFF,2,9,(STR)"������ CANRX0 %02X"},             // 29
  {0x00FFFFFF,0x001EFFFF,2,9,(STR)"������ CANRX1 %02X"},             // 30
  {0x00FFFFFF,0x001FFFFF,8,9,(STR)"������������ � ���� %02X"},       // 31
  {0x00FFFFFF,0x0020FFFF,8,5,(STR)"������� <���� ����.>"},           // 32
  {0x00FFFFFF,0x0021FFFF,2,9,(STR)"VM%02d �����!"},	 	     // 33
  {0x00FFFFFF,0x0022FFFF,2,9,(STR)"��� ������ � ����. �� RS%01d"},          // 34
  {0x00FFFFFF,0x0023FFFF,2,5,(STR)"��������� �������������"},          // 35
  {0x00FFFFFF,0x0024FFFF,2,5,(STR)"������ Flash ����������"},          // 36
  {0x00FFFFFF,0x0025FFFF,8,9,(STR)"�����. ���. ���� %03d"},          // 37
  {0x00FFFFFF,0x0026FFFF,8,9,(STR)"���� %03d ���������"},          // 38 event2 - ����� ����
  {0x00FFFFFF,0x0027FFFF,8,9,(STR)"���� %03d ��������"},          // 39 event2 - ����� ����

  {0x00000000,0x00000000,2,255,(STR)"�����. ������� %02X%02X%02X"},   //
};


//������ ������� 10+1+24+1+24+1=61

void PrintEventType(struct TLogEvent *le, char *s, char pflag)
{
  void FlashReadVM(unsigned int adr, void *c, char len);

  static __flash char scoord[]="�����: %02d/%02d ";
  static __flash char scoord1[]="�����: %02d/%02d/%01d ";
  static __flash char SensorChAdrmess[]="����������: %02d/%02d->%02d";
  static __flash char PPKPChAdrmess[]="��-�: %02d->%02d";
  static __flash char UPSChAdrmess[]="���: %02d->%02d";
  static __flash char BRAChAdrmess[]="���: %02d->%02d";
  static __flash char UPSnamepu[]="���-��";
  static __flash char UPSnameppkp[]="���-��-�%02d";
  static __flash char PUSname[]="��� %02d";

  void ReadSensorConfig(char ppkp, char sensor, struct TSensorConfig *s);
  char event2;
  struct TSensorConfig sc;
  struct TEventDesc __flash *tp=EventsTab;
  struct TPPKPEEPROM_Tab __flash *tpt;
  unsigned int fp;
  char i;
  struct TEventSupr es;

  if (pflag)
  {
    memset(s,' ',60);
  }
  s[60]=0;

  event2=le->Event2;

  TemporaryBITN=0;

  fp=0;
  do
  {
    FlashReadVM(fp,&es,32);
    fp+=32;
    if (es.ev==*(unsigned long *)le)
    {
      TemporaryZONE0=0;
      TemporaryZONE1=0;
      TemporaryZONE2=0;
      TemporaryZONE3=0;
      TemporaryEVENT=es.evtype;
      TKevent=0;
      TemporaryBITN=es.bitn;
      TemporaryBITV=es.bitv;
      memcpy(s,es.text,24);
      return;
    }
  }
  while(es.ev!=0xFFFFFFFF);

  if (le->PPKP<64)        // Sensor/ppkp event
  {
    ReadSensorConfig(le->PPKP,le->Sensor&0x3f,&sc);
    TemporaryZONE0=sc.Zone1;
    TemporaryZONE1=sc.Zone2;
    TemporaryZONE2=sc.Zone3;
    TemporaryZONE3=sc.Zone4;
    //  TemporaryTYPE=sc.Type;
    le->Event2=sc.Type&0x1F;
  }

  i=0xFF;
  do
  {
    i++;
    if (((tp->mask)&(*(unsigned long*)le))==tp->val) break;
    tp++;
  }
  while (tp->mask!=0);

  TemporaryEVENT=tp->eventmode;
  if (TemporaryEVENT&1)
  {
    TemporaryBITN=sc.Led;
    TemporaryBITV=1;
  }
  TKevent=TKtable[i];
  if ((sc.TCode) && (TemporaryEVENT&1)) TKevent=sc.TCode;

#define EventMP ((PGM_P)tp->message)
#define EventPEText(VAL) ((PGM_P)tpt->VAL)

  if (pflag) switch (tp->printtype)
  {
  case 0:
    if (IsLineSpace(sc.GeoPos))
    {
      sprintf_P(s+11,scoord,le->PPKP,le->Sensor&63);
      memcpy_P(s+24,NameSensorType[(sc.Type)&0x1f],5);
    }
    else
    {
      memcpy(s+11,sc.GeoPos,24);
      sprintf_P(s+36,scoord,le->PPKP,le->Sensor&63);
      memcpy_P(s+49,NameSensorType[(sc.Type)&0x1f],5);
    }
    s[sprintf_P(s,EventMP)]=' ';
    break;
  case 1:
    ReadSensorConfig(le->PPKP,(le->Sensor&0x3f)+(le->Event>>2),&sc);
    if (IsLineSpace(sc.GeoPos))
    {
      sprintf_P(s+11,scoord1,le->PPKP,le->Sensor&63,(le->Event>>2)+1);
      memcpy_P(s+26,NameSensorType[(sc.Type)&0x1f],5);
    }
    else
    {
      memcpy(s+11,sc.GeoPos,24);
      sprintf_P(s+36,scoord1,le->PPKP,le->Sensor&63,(le->Event>>2)+1);
      memcpy_P(s+51,NameSensorType[(sc.Type)&0x1f],5);
    }
    TemporaryZONE0=sc.Zone1;
    TemporaryZONE1=sc.Zone2;
    TemporaryZONE2=sc.Zone3;
    TemporaryZONE3=sc.Zone4;
    s[sprintf_P(s,EventMP)]=' ';
    break;
  case 2:
    i=le->PPKP;
    if (i>63) i&=0x1F;
    sprintf_P(s,EventMP,i);
    break;
  case 4:
    if (le->Event&1)
    {
      switch(le->PPKP&0x60)
      {
      case 0x40:
        sprintf_P(s+11,UPSChAdrmess,le->PPKP&0x1F,event2);
        break;
      case 0x60:
        sprintf_P(s+11,BRAChAdrmess,le->PPKP&0x1F,event2);
        break;
      default:
        sprintf_P(s+11,PPKPChAdrmess,le->PPKP,event2);
        break;
      }
    }
    else
      sprintf_P(s+11,SensorChAdrmess,le->PPKP,le->Sensor&63,event2);
    s[sprintf_P(s,EventMP)]=' ';
    break;
  case 5:
    sprintf_P(s,EventMP);
    break;
  case 6:
    if (event2)
      s[sprintf_P(s,UPSnameppkp,event2&63)]=' ';
    else
      s[sprintf_P(s,UPSnamepu)]=' ';
    sprintf_P(s+11,EventMP);
    break;
  case 8:
    sprintf_P(s+11,EventMP,event2);
    s[sprintf_P(s,PUSname,le->PPKP&0x3F)]=' ';
    break;
  case 9:
    sprintf_P(s,EventMP,event2);
    break;
  case 10:
    s[sprintf_P(s,EventMP,le->PPKP&0x3F)]=' ';
    tpt=&PPKPEEPROM_Tab[le->Sensor&0x3F];
    sprintf_P(s+11,event2?EventPEText(mess1):EventPEText(mess0));
    break;
  case 11:
    i=le->PPKP;
    if (i>63) i&=0x1F;
    sprintf_P(s,EventMP,i,event2&1);
    break;
  case 12:
    sprintf_P(s,EventMP,le->PPKP&0x1F);
    break;
  case 13:
    sprintf_P(s,EventMP,le->PPKP&0x1F,le->Sensor&0x3F);
    break;
  default:
    sprintf_P(s,EventMP,le->PPKP,le->Sensor,le->Event);
    break;
  }
  le->Event2=event2;
}
