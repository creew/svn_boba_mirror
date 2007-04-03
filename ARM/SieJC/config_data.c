#include "..\inc\cfg_items.h"
//������������
__root const CFG_HDR cfghdr0={CFG_STR_UTF8,"Login",0,31};
__root const char USERNAME[32]="";

__root const CFG_HDR cfghdr1={CFG_STR_PASS,"Password",0,31};
__root const char PASSWORD[32]="";

__root const CFG_HDR cfghdr2={CFG_STR_WIN1251,"Server",0,127};
__root const char JABBER_SERVER[128]="jabber.ru";

__root const CFG_HDR cfghdr3={CFG_STR_WIN1251,"Host",0,127};
__root const char JABBER_HOST[128]="jabber.ru";

__root const CFG_HDR cfghdr3_1={CFG_CBOX,"Host type",0,2};
__root const int IS_IP = 0;
__root const CFG_CBOX_ITEM cfgcbox1[2]={"Host name","IP address"};

__root const CFG_HDR cfghdr4={CFG_UINT,"Port",0,65535};
__root const unsigned int JABBER_PORT=5222;

__root const CFG_HDR cfghdr4346={CFG_CBOX,"History",0,2};
__root const int WRITE_HISTORY = 0;
__root const CFG_CBOX_ITEM cfgcbox1435[2]={"Disabled","Enabled"};

__root const CFG_HDR cfghdr357835={CFG_CBOX,"MUC history",0,2};
__root const int WRITE_MUC_HISTORY = 0;
__root const CFG_CBOX_ITEM cfgcbox36546[2]={"Disabled","Enabled"};

__root const CFG_HDR cfghdr5={CFG_STR_UTF8,"History path",0,127};
__root const char HIST_PATH[128]="4:\\ZBin\\SieJC\\history\\";

__root const CFG_HDR cfghdr6={CFG_STR_UTF8,"Pictures path",0,127};
__root const char PATH_TO_PIC[128]="4:\\ZBin\\SieJC\\img\\";

__root const CFG_HDR cfghdr7={CFG_STR_WIN1251,"Default disc",0,127};
__root const char DEFAULT_DISC[128]="4";

__root const CFG_HDR cfghdr19={CFG_COORDINATES,"Idle icon position",0,0};
__root const unsigned int IDLE_ICON_X=0;
__root const unsigned int IDLE_ICON_Y=50;

__root const CFG_HDR cfghdr8={CFG_CBOX,"Auth type",0,2};
__root const int USE_SASL = 0;
__root const CFG_CBOX_ITEM cfgcbox2[2]={"Simple","SASL"};

__root const CFG_HDR cfghdr9={CFG_CBOX,"ZLib usage",0,2};
__root const int USE_ZLIB = 0;
__root const CFG_CBOX_ITEM cfgcbox3[2]={"No","Yes"};

// EOL, EOF
