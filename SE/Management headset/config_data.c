#include "..\\include\Lib_Clara.h"
#include "..\\include\cfg_items.h"
#include "config_data.h"

#pragma diag_suppress=Pe177

__root const CFG_HDR cfghdr_button={CFG_KEYCODE,"������ ����������",0,0};
__root const int KeyControl=0;
__root const int KeyControlPressMode=3;

__root const CFG_HDR cfghdr_on={CFG_CBOX,"���������",0,3};
__root const int mediaOn=1;
__root const CFG_CBOX_ITEM cfgcbox_on[3]={"�������� �������","������� �������","������� �������"};

__root const CFG_HDR cfghdr_radio_0={CFG_LEVEL,"�����",1,0};
  __root const CFG_HDR cfghdr_radio_on={CFG_CBOX,"����������",0,3};
  __root const int radioOff=2;
  __root const CFG_CBOX_ITEM cfgcbox_radio_on[3]={"�������� �������","������� �������","������� �������"};

  __root const CFG_HDR cfghdr_radio_next={CFG_CBOX,"��������� �������",0,3};
  __root const int radioNext=0;
  __root const CFG_CBOX_ITEM cfgcbox_radio_next[3]={"�������� �������","������� �������","������� �������"};

  __root const CFG_HDR cfghdr_radio_prev={CFG_CBOX,"���������� �������",0,3};
  __root const int radioPrev=1;
  __root const CFG_CBOX_ITEM cfgcbox_radio_prev[3]={"�������� �������","������� �������","������� �������"};

  __root const CFG_HDR cfghdr_radio_x={CFG_LEVEL,"",0,0};

__root const CFG_HDR cfghdr_player_0={CFG_LEVEL,"�����",1,0};
  __root const CFG_HDR cfghdr_player_on={CFG_CBOX,"����������",0,3};
  __root const int playerOff=2;
  __root const CFG_CBOX_ITEM cfgcbox_player_on[3]={"�������� �������","������� �������","������� �������"};

  __root const CFG_HDR cfghdr_player_next={CFG_CBOX,"��������� ����",0,3};
  __root const int playerNext=0;
  __root const CFG_CBOX_ITEM cfgcbox_player_next[3]={"�������� �������","������� �������","������� �������"};

  __root const CFG_HDR cfghdr_player_prev={CFG_CBOX,"���������� ����",0,3};
  __root const int playerPrev=1;
  __root const CFG_CBOX_ITEM cfgcbox_player_prev[3]={"�������� �������","������� �������","������� �������"};

__root const CFG_HDR cfghdr_player_x={CFG_LEVEL,"",0,0};

__root const CFG_HDR cfghdr7={CFG_UINT,"����� �� ������� ������� (����)",10,10000};
__root const unsigned int detectTime=1000;
