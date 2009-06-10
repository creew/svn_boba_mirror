#include "..\inc\cfg_items.h"
#include "..\inc\swilib.h"

//����������
#ifdef ELKA
__root const CFG_HDR cfghdr0 = {CFG_RECT, "XYXY Position", 0, 239};
//��� ����������� ���, �����, ���. ��������, ���� ��������
__root const RECT position = {65,108,131,153};
#else
__root const CFG_HDR cfghdr0 = {CFG_RECT, "XYXY Position", 0, 239};
__root const RECT position = {110,224,239,280};
#endif

//KeyLock
__root const CFG_HDR cfghdr7 = {CFG_CBOX, "Show In", 0, 3};
__root const int cfgShowIn = 2;
__root const CFG_CBOX_ITEM cfgcbox1[3] = {"Lock", "Unlock", "Both"};

//� string
__root const CFG_HDR cfghdr8 = {CFG_CBOX, "Show copyright text at startup", 0, 2};
__root const int cfgStTxt = 1;
__root const CFG_CBOX_ITEM cfgcbox2[2] = {"No", "Yes"};

//Update Time
__root const CFG_HDR cfghdr9 = {CFG_UINT, "Update Time (1/10 sec)", 0, 50};
__root const unsigned int cfgUpTime = 10;

//Sensors...
//CPU
__root const CFG_HDR cfghdr1={CFG_LEVEL,"CPU Graph",1,0};
  __root const CFG_HDR cfghdr5 = {CFG_CBOX, "Type Graph", 0, 3};
  __root const int cfgLoadType = 2;
  __root const CFG_CBOX_ITEM cfgcbox3[3] = {"None", "Line", "Solid"};

  __root const CFG_HDR cfghdr11 = {CFG_UINT, "Update Time Devisor", 1, 500};
  __root const unsigned int cfgCpuDiv = 1;

  __root const CFG_HDR cfghdr4_1 = {CFG_COLOR, "Color (208 MHz)", 0, 0};
  __root const char cfgLoad208[4] = {0xff,0,0,100};

  __root const CFG_HDR cfghdr4_2 = {CFG_COLOR, "Color (104 MHz)", 0, 0};
  __root const char cfgLoad104[4] = {0xd0,0,0,100};

  __root const CFG_HDR cfghdr4_4 = {CFG_COLOR, "Color (52 MHz)", 0, 0};
  __root const char cfgLoad52[4] = {0xa0,0,0,100};
__root const CFG_HDR cfghdr3={CFG_LEVEL,"",0,0};

//RAM
__root const CFG_HDR cfghdr2={CFG_LEVEL,"RAM Graph",1,0};
  __root const CFG_HDR cfghdr6 = {CFG_CBOX, "Type Graph", 0, 3};
  __root const int cfgRamType = 1;
  __root const CFG_CBOX_ITEM cfgcbox4[3] = {"None", "Line", "Solid"};

  __root const CFG_HDR cfghdr10 = {CFG_UINT, "Update Time Devisor", 1, 500};
  __root const unsigned int cfgRamDiv = 10;

  __root const CFG_HDR cfghdr4_3 = {CFG_COLOR, "Color", 0, 0};
  __root const char cfgFreeRAM[4] = {0,0,255,100};
__root const CFG_HDR cfghdr4={CFG_LEVEL,"",0,0};

//Net
__root const CFG_HDR cfghdr12={CFG_LEVEL,"Net Graph",1,0};
  __root const CFG_HDR cfghdr13 = {CFG_CBOX, "Type Graph", 0, 3};
  __root const int cfgNetType = 1;
  __root const CFG_CBOX_ITEM cfgcbox5[3] = {"None", "Line", "Solid"};

  __root const CFG_HDR cfghdr14 = {CFG_UINT, "Update Time Devisor", 1, 500};
  __root const unsigned int cfgNetDiv = 1;

  __root const CFG_HDR cfghdr15 = {CFG_COLOR, "Color", 0, 0};
  __root const char cfgNet[4] = {0xff,0x90,0,100};
__root const CFG_HDR cfghdr16={CFG_LEVEL,"",0,0};
