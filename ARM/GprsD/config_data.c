#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
__root const CFG_HDR cfghdr0={CFG_STR_WIN1251,"APN:",0,19};
__root const char cfgAPN[20]="internet.mts.ru";

__root const CFG_HDR cfghdr1={CFG_STR_WIN1251,"Username:",0,19};
__root const char cfgUSER[20]="mts";

__root const CFG_HDR cfghdr2={CFG_STR_WIN1251,"Password:",0,19};
__root const char cfgPASS[20]="mts";

__root const CFG_HDR cfghdr3={CFG_UINT,"Net timeout:",0,0xFFFFFFFF};
__root const unsigned int cfgTimeout=0x5F5E0FF; // 99999999 ������ =)
