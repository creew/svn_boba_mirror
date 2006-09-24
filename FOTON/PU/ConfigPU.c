//--------------------------------------
// ������������ ��
//--------------------------------------

//������������ ���� � �������
void ConfigPUtimedate(void)
{
  char h,m,s,dd,mm,yy; //����, ������, �������, ����, �����, ���
  char c;
  char pos=0;
  unsigned long t;
  _CLI();
  t=TimeDate; //����� ������� �����
  _SEI();
  do
  {
    memset(CON,' ',80);
    PrintTimeDate(CON,t); //���������� �����
    sprintf_P(CON+40,Message_ConfigPU[0]);
    //�� ������ ������ �������� ������� � ����������, ���������� ����, ������, �������, ����, �����, ���
    h=((CON[0]-'0')*10)+CON[1]-'0';
    m=((CON[3]-'0')*10)+CON[4]-'0';
    s=((CON[6]-'0')*10)+CON[7]-'0';
    dd=((CON[9]-'0')*10)+CON[10]-'0';
    mm=((CON[12]-'0')*10)+CON[13]-'0';
    yy=((CON[15]-'0')*10)+CON[16]-'0';
    CursorPos=pos; //������ ������
    c=KBD(0); //���� �������
    //�������� �������������
    if (Edit_dc(c,pos,0,&h)) c=kRT;
    if (Edit_dc(c,pos,3,&m)) c=kRT;
    if (Edit_dc(c,pos,6,&s)) c=kRT;
    if (Edit_dc(c,pos,9,&dd)) c=kRT;
    if (Edit_dc(c,pos,12,&mm)) c=kRT;
    if (Edit_dc(c,pos,15,&yy)) c=kRT;
    switch(c)
    {
      //�������� ���������� ������
    case kLT: if (pos==3||pos==6||pos==9||pos==12||pos==15) pos--;
    if (pos==0)
    {
      c=kESC;
      break;
    }
    if (pos>0) pos--;
    break;
    case kRT: if (pos==1||pos==4||pos==7||pos==10||pos==13) pos++;
    if (pos<16) pos++;
    break;
    //�������� ���������� ���������
    case kUP:
      Select_dc(pos,0,1,&h);
      Select_dc(pos,3,1,&m);
      Select_dc(pos,6,1,&s);
      Select_dc(pos,9,1,&dd);
      Select_dc(pos,12,1,&mm);
      Select_dc(pos,15,1,&yy);
      break;
    case kDW:
      Select_dc(pos,0,-1,&h);
      Select_dc(pos,3,-1,&m);
      Select_dc(pos,6,-1,&s);
      Select_dc(pos,9,-1,&dd);
      Select_dc(pos,12,-1,&mm);
      Select_dc(pos,15,-1,&yy);
      break;
    }
    //����������� ����, ������, �������, ����, �����, ��� � ���������� ������
    t=TimeDate2Long(h,m,s,dd,mm,yy);
  }
  while (c!=kESC); //����, ���� �� �����
  _CLI();
  TimeDate=t; //���������� ����� �������� �������
  _SEI();
}

//------------------------
//������������ � ��
//------------------------
#include "pupc.c"

//��������� �������
void ConfigPassword(void)
{
  static __flash char mess1[]="��� ������� �%01d: %04X%04X";
  // static flash char mess2[]="��� ������� �2: XXXXXXXX";
  char c;
  int pass[4];

  char pos=16;

  pass[0]=PWDlevel2[0];
  pass[1]=PWDlevel2[1];
  pass[2]=PWDlevel3[0];
  pass[3]=PWDlevel3[1];
  do
  {
    ClrScr();
    sprintf_P(CON,mess1,2,pass[0],pass[1]); //������
    if (PWDmode==3)
    {
      sprintf_P(CON+40,mess1,3,pass[2],pass[3]); //������
    }
    CursorPos=pos; //��������� �������
    c=KBD(0); //�������� �������
    if ((c>=32 && c<128)||(c>=192)) //���������� �������
    {
      if (Edit_hi(c,pos,16,&pass[0])) c=kRT; //�������� �������������
      if (Edit_hi(c,pos,20,&pass[1])) c=kRT;
      if (Edit_hi(c,pos,16+40,&pass[2])) c=kRT; //�������� �������������
      if (Edit_hi(c,pos,20+40,&pass[3])) c=kRT;
    }
    switch(c)
    {
    case kLT: //������� �����
      switch (pos)
      {
      case 16: c=kESC; break; //����� ������
      case 16+40: pos=23; break;
      default: pos--; //������ �����
      }
      break;
    case kRT: //������� ������
      switch (pos)
      {
      case 23+40: break; //������ ������
      case 23: if (PWDmode==3) pos=16+40; break;
      default: pos++; //������ ������
      }
      break;
    case kUP: //����������
      Select_hi(pos,16,1,&pass[0]); //�������� �����������
      Select_hi(pos,20,1,&pass[1]);
      Select_hi(pos,16+40,1,&pass[2]); //�������� �����������
      Select_hi(pos,20+40,1,&pass[3]);
      break;
    case kDW: //����������
      Select_hi(pos,16,-1,&pass[0]); //�������� ���������
      Select_hi(pos,20,-1,&pass[1]);
      Select_hi(pos,16+40,-1,&pass[2]); //�������� ���������
      Select_hi(pos,20+40,-1,&pass[3]);
      break;
    }
  }
  while (c!=kESC); //���� �� �����
  PWDlevel2[0]=pass[0];
  PWDlevel2[1]=pass[1];
  PWDlevel3[0]=pass[2];
  PWDlevel3[1]=pass[3];
}

//������ ���� ������������
void PrintSetup(void)
{
  static __flash char LPT_AllConfig1[]="�����������\377 ��-� ����� %d �� ";
  static __flash char LPT_AllConfig2[]="��-� ����� %d\n";
  static __flash char LPT_AllConfig3[]="���������� �������������� ��������� �����\377��� ���   ���� 1-4 �    �\n";
  //                          "xx         xxxxxxxxxxxxxxxxxxxxxxxx x         xxxxx xx   xxxx xxxx xxxx\n";
  static __flash char LPT_AllConfig4[]="%02d         %.24s %c         %.5s %02d %02d %02d %04X %02X\n";
  static __flash char LPT_AllConfig5[]="����� ����������� � ���� %d, �������� %d\n";
  static __flash char LPT_AllConfig6[]="����� ����������� � ������� %d, �������� %d\n";
  char ppkp,sensor;
  char st[5];
  struct TSensorConfig sc;
  char ppkp_tots,ppkp_ons;
  int sys_tots,sys_ons;
  unsigned long td;

  sys_tots=sys_ons=0; //��������� ��������� ����������� �� �������
  //�������� �����
  // PrinterOn;
  PrintSeparator();
  printf_P(LPT_AllConfig1,PU_ID);
  _CLI();
  td=TimeDate;
  _SEI();
  LPT_PrintTimeDate(td);
  PRINT_CRLF;
  PrintSeparator();
  //���� �� ���� ����
  for (ppkp=0;ppkp<64;ppkp++)
  {
    if (PPKP_AREA[ppkp]&0x80) //���� ���� �������
    {
      ppkp_tots=ppkp_ons=0; //��������� ��������� ����������� �� ����
      printf_P(LPT_AllConfig2,ppkp); //�������� ����� �� ����
      printf_P(LPT_AllConfig3);
      //���� �� ���� �����������
      for (sensor=0;sensor<64;sensor++)
      {
        ReadSensorConfig(ppkp,sensor,&sc); //������ ������������ �����������
        if (sc.Type&0x1f) //���� ��� ���������� �� "���"
        {
          //�������� ������������
          memcpy_P(st,NameSensorType[sc.Type&127],5);
          printf_P(LPT_AllConfig4,
                   sensor,
                   sc.GeoPos,
                   sc.Type>127?'+':'-',
                   st,
                   sc.Zone1,
                   sc.Zone2,
                   sc.Zone3,
                   sc.Zone4,
                   sc.Led,
                   sc.TCode
                     );
          ppkp_tots++;sys_tots++; //����������� ������� ����� �� ������� � ����
          if (sc.Type&0x80) //���� ���������� �������
          {ppkp_ons++;sys_ons++;} //����������� ������� ���������� �� ������� � ����
        }
      }
      //�������� ������ �� ����
      printf_P(LPT_AllConfig5,ppkp_tots,ppkp_ons);
      PrintSeparator();
    }
  }
  //�������� ������ �� �������
  printf_P(LPT_AllConfig6,sys_tots,sys_ons);
  // PrinterOff;
}

//��������� ������ ������
void ConfigTextPrint(void)
{
  //                            012345678901234567890123
  static __flash char mess0[]="�������/��������� ����� <�����>-��������";
  static __flash char mess1[]="������ ������� �����    <�����>-��������";
  static __flash char mess2[]="����./������� �����     <�����>-��������";
  static __flash char mess3[]="������ ������� �����    <�����>-��������";
  char c;
  char pos=0;
  do
  {
    ClrScr();
    //������ ������������ ��
    c=PU_STAT;
    //�������� ������� ������������
    sprintf_P(CON,c&1?mess1:mess0);
    sprintf_P(CON+40,c&2?mess3:mess2);
    //������������� ������
    CursorPos=pos;
    c=KBD(0); //���� �������
    switch(c)
    {
      //�������� ���������� ������
    case kLT:
      if (pos==0)
      {
        c=kESC;
      }
      if (pos==40) pos=0;
      break;
    case kRT:
      if (pos==0) pos=40;
      break;
      //�������� ���������� ���������
    case kUP:
      switch (pos)
      {
      case 0:
        PU_STAT|=1;
        break;
      case 40:
        PU_STAT|=2;
        break;
      }
      break;
    case kDW:
      switch (pos)
      {
      case 0:
        PU_STAT&=0xFE;
        break;
      case 40:
        PU_STAT&=0xFD;
        break;
      }
      break;
    }
  }
  while (c!=kESC); //����, ���� �� �����
}

//���� ������ �� ������� �����������
void SearchSensors(void)
{
  void SearchOnSensors(char ppkp, char *tot_sens, char *on_sens); //�������� ��� ��������
  static __flash char mess[]="��-�: %02d <�����>-�����       ������: %02d";
  //                             012345678901234567890123
  static char ppkp=0;
  struct TSensorConfig sc;
  char sensor;
  char pos=6;
  char c,tot_sens,on_sens,i;
  do
  {
    ClrScr();
    SearchOnSensors(ppkp,&tot_sens,&on_sens); //������� ���������� ����������� � ������� ������ (�� ������������)
    sprintf_P(CON,mess,ppkp,tot_sens); //�������� ���
    CursorPos=pos; //������� ������
    c=KBD(0); //���� �������
    if ((c>=32 && c<128)||(c>=192)) //���������� �������, �������� ������������� ������
    {
      if (Edit_dc(c,pos,6,&ppkp)) c=kRT;
    }
    switch(c)
    {
      //�������� ���������� ������
    case kLT:
      switch (pos)
      {
      case 6: c=kESC; break;
      case 9: pos=7; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 9: break;
      case 7: pos=9; break;
      default: pos++;
      }
      break;
      //������� ��������
    case kUP:
      Select_dc(pos,6,1,&ppkp); //������� ����
      if (pos==9) //���� ������� � ������� 9 ("�����")
      {
        Event2FIFO(0xFF,0xFF,20,0); //��������� "����� ������ �����������"
        sensor=0;
        do
        {
          ReadSensorConfig(ppkp,sensor,&sc); //������ ������� ������������
          //�������� "��: ???"
          c=sc.Type;
          sprintf_P(CON+40,(STR)"%02d ��: ",sensor);
          sprintf_P(CON+47,NameSensorType[c&0x1F]);
          i=2;
          do
          {
            //������ ������ ��������� ����������
            do
            {
              //���������� ������
              _CLI();
              //NeedNoError=0;
              SysPPKP_rxsize=0;
              SysPPKP_TXB[0]=ppkp;
              SysPPKP_TXB[1]=sensor;
              SysPPKP_TXB[2]=3;
              SysPPKP_txsize=3;
              _SEI();
              //�������� ���������� ������
              //���� ������ �������, ������������ � �����������
              while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
            }
            while (SysPPKP_rxsize==2); //����, ���� �� ����������� ������
            if (SysPPKP_rxsize==255) break;
            if (SysPPKP_RXB[1]&0x1F) break;
          }
          while(--i);
          //�������� "��: ???"
          sprintf_P(CON+53,(STR)"�����: ");
          sprintf_P(CON+60,NameSensorType[SysPPKP_RXB[1]&0x1F]);
          if (SysPPKP_rxsize!=255) //���� ���������� ����� � ����
          {
            if ((!(sc.Type&0x1F))&&(SysPPKP_RXB[1]&0x1F)) tot_sens++;
            if ((sc.Type&0x1F)&&(!(SysPPKP_RXB[1]&0x1F))) tot_sens--;
            sprintf_P(CON,mess,ppkp,tot_sens); //�������� ���
            sc.Type=SysPPKP_RXB[1]&0x1F; //��������� ��� ����������
            if (sc.Type) sc.Type|=0x80; //���� ��� �� "���", �� �������� ���
            //���������� ������ "���������/���������� ��������� ����������"
            _CLI();
            SysPPKP_rxsize=0;
            SysPPKP_TXB[0]=ppkp;
            SysPPKP_TXB[1]=0x48;
            SysPPKP_TXB[2]=sensor+0xC0;
            SysPPKP_TXB[3]=(sc.Type&0x80?0x20:0x00)|(sc.Type&0x1f); //�������� ��� ���������
            SysPPKP_txsize=4;
            _SEI();
            //�������� ���������� ������
            //���� ������ �������, ������������ � �����������
            while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
            WriteSensorConfig(ppkp,sensor,&sc); //���������� ������������ ����������
            //���� ��� � �������� ���/���� ���������� ���������
            if (c!=sc.Type) Event2FIFO(ppkp,sensor|0xc0,sc.Type>>7,sc.Type); //��������� "���������� ���/����"
          }
          UpdateConsole();
          sensor+=1; //��������� ����������
        }
        //����, ���� �� ��� ���������� ��� �� ������ ������� ��� �� ������ �������
        while (sensor<64 && KEY==0 && Timer30sec!=0);
        if (sensor>=64) //���� ������ ��� ���������� �������� "�������� ����� ����"
        {
          //���������� ������
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp;
          SysPPKP_TXB[1]=0x4F;
          SysPPKP_txsize=2;
          _SEI();
          //�������� ���������� ������
          //���� ������ �������, ������������ � �����������
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        }
        Event2FIFO(0xFF,0xFF,21,0); //��������� "����� ������ �����������"
        ClrScr();
      }
      break;
      //�������� ���������� ���������
    case kDW:
      Select_dc(pos,6,-1,&ppkp);
      break;
    }
  }
  while (c!=kESC); //����, ���� �� �����
}

//������� ���. ���������
void AddDevice(void)
{
  static __flash char messon[]="���";
  static __flash char messoff[]="����";
  char c;
  char pos=5;
  do
  {
    //�������� ��������� ���������
    ClrScr();
    sprintf_P(CON+0 ,(STR)"���1:");
    sprintf_P(CON+5 ,MRV1_STAT&0x80?messon:messoff);
    sprintf_P(CON+10 ,(STR)"���2:");
    sprintf_P(CON+15 ,MRV2_STAT&0x80?messon:messoff);
    sprintf_P(CON+20 ,(STR)"��:");
    sprintf_P(CON+25 ,TK_STAT&0x80?messon:messoff);
    sprintf_P(CON+30 ,(STR)"���:");
    sprintf_P(CON+35 ,DVP_STAT&0x80?messon:messoff);
    sprintf_P(CON+40 ,(STR)"���:");
    sprintf_P(CON+45 ,VST_STAT&0x80?messon:messoff);
    sprintf_P(CON+50 ,(STR)"���:");
    sprintf_P(CON+55 ,VTK_STAT&0x80?messon:messoff);
    sprintf_P(CON+60 ,(STR)"��:");
    sprintf_P(CON+65 ,VPK_STAT&0x80?messon:messoff);
    sprintf_P(CON+70 ,(STR)"���:");
    sprintf_P(CON+75 ,messoff);
    CursorPos=pos; //������� ������
    c=KBD(0); //������� �������
    switch(c)
    {
      //�������� ����������� ������ �����
    case kLT:
      switch(pos)
      {
      case 5:  c=kESC; break;
      default: pos-=10; break;
      }
      break;
      //�������� ����������� ������ ������
    case kRT:
      switch(pos)
      {
      case 75:  break;
      default: pos+=10; break;
      }
      break;
      //�������� ���������� ������
    case kUP:
      switch(pos)
      {
      case 5:
        MRV1_STAT|=0x80;
        break;
      case 15:
        MRV2_STAT|=0x80;
        break;
      case 25:
        TK_STAT|=0x80;
        break;
      case 35:
        DVP_STAT|=0x80;
        break;
      case 45:
        VST_STAT|=0x80;
        break;
      case 55:
        VTK_STAT|=0x80;
        break;
      case 65:
        VPK_STAT|=0x80;
        break;
      case 75:
        //PU_STAT|=0x80;
        break;
      };
      break;
    case kDW:
      switch(pos)
      {
      case 5:
        MRV1_STAT&=0x7F;
        break;
      case 15:
        MRV2_STAT&=0x7F;
        break;
      case 25:
        TK_STAT&=0x7F;
        break;
      case 35:
        DVP_STAT&=0x7F;
        break;
      case 45:
        VST_STAT&=0x7F;
        break;
      case 55:
        VTK_STAT&=0x7F;
        break;
      case 65:
        VPK_STAT&=0x7F;
        break;
      case 75:
        //PU_STAT&=0x7F;
        break;
      };
      break;
    }
  }
  while (c!=kESC); //����, ���� �� �����
}

void ConfigPUID(void)
{                            //012345678901234567890123
  static __flash char mess[]="������� ����� �� - %02d";
  char id;
  char pos=19;
  char c;
  id=PU_ID;
  do
  {
    id&=31;
    ClrScr();
    sprintf_P(CON,mess,id);
    CursorPos=pos; //���������� ������
    c=KBD(0); //���� �������
    if ((c>=32 && c<128)||(c>=192)) //���� ���������� �������
    {
      if (Edit_dc(c,pos,19,&id)) { c=kRT;} //�������� �������������
    }
    switch(c)
    {
      //�������� ���������� ������ �����
    case kLT:
      switch (pos)
      {
      case 19: c=kESC; break;
      default: pos--;
      }
      break;
      // �������� ���������� ������ ������
    case kRT:
      switch (pos)
      {
      case 20: break;
      default: pos++;
      }
      break;
    case kUP:
      Select_dc(pos,19,1,&id); //�������� ����������
      break;
    case kDW:
      Select_dc(pos,19,-1,&id);//�������� ����������
      break;
    }
  } while (c!=kESC); //����, ���� �� �����
  PU_ID=id;
  PUID=id;
}

//������� ������������ ��
void ConfigPU(void)
{
  char c,i=0;
  TestPWD(40); //��������� ������
  do
  {
    //�������� ����
    ClrScr();
    sprintf_P(CON,Message_MainMenu[3]);
    c=PU_STAT;
    if (i>=8)
    {
      char x;
      x=(i<<1)-8;
      switch(x)
      {
      case 8: if (c&0x04) x++; break;
      case 10: if (c&0x08) x++; break;
      case 12: if (c&0x10) x++; break;
      case 14: if (c&0x20) x++; break;
      case 16: if (c&0x40) x++; break;
      case 18: if (c&0x80) x++; break;
      case 20: if (IS_ALLWAYS_RESET) x++; break;
      case 22: if (IS_NOLOG_ACK) x++; break;
      case 24: if (IS_TAMPER_DISABLED) x++; break;
      }
      sprintf_P(CON+40,Message_ConfigPU[x]);
      sprintf_P(CON+63,(STR)"<������>-��������");
    }
    else
    {
      sprintf_P(CON+40,Message_ConfigPU[i]);
      sprintf_P(CON+66,(STR)"<������>-�����");
    }
    c=KBD(0);
    if (c>=0x11 && c<=0x18) //������������ �������������� �������
    {
      i=c-0x11;
      c=kRT;
    }
    switch(c)
    {
    case kRT:
      //� ����������� �� ������ ���� ��������� ������� ������� �, ���� �������, �������� ���������
      switch(i)
      {
      case 0: if (PWDmode>=2)
      {
        Event2FIFO(0xFF,0xFF,17,0); //��������� "������ ��������� ����/�������"
        ConfigPUtimedate();
        Event2FIFO(0xFF,0xFF,18,0); //��������� "����� ��������� ����/�������"
      }
      break;
      case 1: if (PWDmode==3) ConfigPUpk(); break;
      case 2: if (PWDmode>=2) ConfigPassword(); break;
      case 3: PrintSetup(); break;
      case 4: if (PWDmode==3) AddDevice(); break;
      case 5: ConfigTextPrint(); break;
      case 6: if (PWDmode==3) SearchSensors(); break;
      case 7: if (PWDmode==3) ConfigPUID(); break;
      case 8: if (PWDmode!=3) break;
      PU_STAT^=0x04;
      break;
      case 9: if (PWDmode!=3) break;
      PU_STAT^=0x08;
      break;
      case 10: if (PWDmode!=3) break;
      PU_STAT^=0x10;
      break;
      case 11: if (PWDmode!=3) break;
      PU_STAT^=0x20;
      break;
      case 12: if (PWDmode!=3) break;
      PU_STAT^=0x40;
      break;
      case 13: if (PWDmode!=3) break;
      PU_STAT^=0x80;
      break;
      case 14: if (PWDmode!=3) break;
      PU_STAT2^=0x01;
      break;
      case 15: if (PWDmode!=3) break;
      PU_STAT2^=0x02;
      break;
      case 16: if (PWDmode!=3) break;
      PU_STAT2^=0x04;
      break;
      } ClrScr(); break;
    case kUP: if (i>0) i--; break;
    case kDW: if (i<16) i++; break;
    }
  } while (c!=kLT); //����, ���� �� �����
}

