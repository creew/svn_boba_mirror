#include "..\inc\cfg_items.h"

// ���� ������������� ������ ����
__root const CFG_HDR cfghdr0={CFG_CHECKBOX,"Clear cache on settings update",0,2};
__root const int Req_Clear_Cache=0;

// ���� ������ ����������� ���
__root const CFG_HDR cfghdr1={CFG_CHECKBOX,"Log unknown cells",0,2};
__root const int LOG_UNKNOWN_CELLS=0;

//���������� X, Y ������ �������
__root const CFG_HDR cfghdrioykh={CFG_COORDINATES,"Text position",0,0};
__root const unsigned int TXT_X=50;
__root const unsigned int TXT_Y=50;

__root const CFG_HDR cfghdr4346={CFG_CBOX,"Text align",0,2};
__root const int CENTER_TEXT = 0;
__root const CFG_CBOX_ITEM cfgcbox1435[2]={"Normal","Centered"};

// ����� ������
__root const CFG_HDR cfghdr8o7yo0 = {CFG_UINT, "Font", 0, 11};
__root const unsigned int TXT_FONT = 11;

// ���� ������
__root const CFG_HDR cfghdr5={CFG_COLOR,"Font Color",0,0};
__root const char TXT_COLOR[4]={0xFF,0xFF,0,0x64};

// �������� ������
__root const CFG_HDR cfghdrplkjhl = {CFG_UINT, "Txt attr", 0, 128};
__root const unsigned int TXT_ATTR = 0x20;

// ������ ���� ���
__root const CFG_HDR cfghdito89 = {CFG_UINT, "Cell cache size", 2, 10};
__root const unsigned int CACHE_SIZE = 10;

// ���� � ���� ������
__root const CFG_HDR cfghdr0lkjhlre={CFG_STR_UTF8,"MiniGPS base dir",3,127};
__root const char MGPS_DB[128]="0:\\Misc\\gps";

// ���� � ���-�����
__root const CFG_HDR cfghdr456re={CFG_STR_UTF8,"Log file path",3,127};
__root const char LOGFILE[128]="0:\\Misc\\gps\\gpslog.txt";

//EOL,EOF
