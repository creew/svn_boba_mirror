#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_
#define RUSSIAN

#ifdef RUSSIAN

#define LG_CLTITLE "��������..."
#define LG_CLT9INP "���� T9: "

#define LG_CLERROR "������"

#define LG_CLLOOPBACK "Loopback"

//===============================

#define LG_MENU    "����"
#define LG_OPTIONS "�����"
#define LG_CLOSE   "�������"
#define LG_CLEAR   "<C"
#define LG_DOIT    "���������"
#define LG_PASTESM "��������"
//-------------
#define LG_SELECT  "�����"
#define LG_BACK    "�����"
//--------------
#define LG_ADD      "��������"
#define LG_ADDCNT   "�������� �������"
#define LG_ENTERUIN "������� UIN:"
//--------------
#define LG_ADDREN  "�����./���"
#define LG_SETNICK "Set nickname of %u as:" //������-�� ��� ������ ������ ��������
#define LG_SMLDESC "Smile: %u %s"           //�� ������� ������������ ��������������
#define LG_ADDSMIL "�������� �������"

//===============================

#define LG_ADDCONT "�������� �������..."
#define LG_AUTHREQ   "����������� ����..."
#define LG_AUTHGRANT "�� ������������"

//================================
#define LG_MNUSTATUS  "������"
#define LG_MNUADDCONT "�������� �������"
#define LG_MNUEDCFG   "���������"
#define LG_MNUABOUT   "�� �����"
//--------------
#define LG_MNUQUOTE    "����������"
#define LG_MNUADDSML   "�������� �����"
#define LG_MNUSHINFO   "�������� ����"
#define LG_MNUADDREN   "�����./���"
#define LG_MNUSAUTHREQ "������ �������."
#define LG_MNUSAUTHGRT "������������"
#define LG_MNUOPENLOG  "������� �������"
#define LG_MNUCLEARCHT "�������� ���"

//================================
#define LG_CHGSTATUS "������� ������"

#define LG_STONLINE "� ����"
#define LG_STAWAY   "������"
#define LG_STNA     "����������"
#define LG_STDND    "�� ����������"
#define LG_STOCCUP  "�����"
#define LG_STFFC    "����� �������"
#define LG_STINVIS  "���������"

//================================

#define LG_MSGILLEGMSGCON "�������� ��������� ENIP_SOCK_CONNECTED!"
#define LG_MSGILLEGMSGREA "�������� ��������� ENIP_DATA_READ"
#define LG_MSGCANTCONN    "���������� ������������!"
#define LG_MSGCANTCRSC    "���������� ������� �����, ���������� GPRS!"
#define LG_MSGHOSTNFND    "������ �� ������!"
#define LG_MSGVIBRADIS    "����� ���������!"
#define LG_MSGVIBRAENA    "����� ��������!"
#define LG_MSGSNDDIS      "����� ���������!"
#define LG_MSGSNDENA      "����� ��������!"
#define LG_MSGNOUINPASS   "������� UIN/������!"
#define LG_MSGSMILNOTFND  "������ �� �������!"

//=================================

#define LG_GRSTATESTRING "State: %d, RXstate: %d\n%t"
#define LG_GRSKEYEXIT    "Exit"
#define LG_GRSKEYCLIST   "CList"

#define LG_GRTRYLOGIN    "Try to login..."
#define LG_GRWAITFORGPRS "Waiting for GPRS up..."
#define LG_GRSENDDNR     "Send DNR..."
#define LG_GRDNRERROR    "DNR ERROR %d!"
#define LG_GRDNROK       "DNR Ok, connecting..."
#define LG_GRBADPACKET   "Bad packet"
#define LG_GRLOGINMSG    "%s"
#define LG_GRSTATUSCHNG  "SC%d: %04X"
#define LG_GRERROR       "ERR: %s"
#define LG_GRRECVMSG     "MSG%d: %s"
#define LG_GRGPRSUP      "GPRS up, wait 10 sec..."

#else

#define LG_CLTITLE "Contacts..."
#define LG_CLT9INP "T9 Key: "

#define LG_CLERROR "error"

#define LG_CLLOOPBACK "Loopback"

//===============================

#define LG_MENU    "Menu"
#define LG_OPTIONS "Options"
#define LG_CLOSE   "Close"
#define LG_CLEAR   "< C"
#define LG_DOIT    "Do it!"
#define LG_PASTESM "Paste it!"
//--------------
#define LG_ADD      "Add!"
#define LG_ADDCNT   "Add Contact"
#define LG_ENTERUIN "Please enter uin:"
//--------------
#define LG_SELECT  "Select"
#define LG_BACK    "Back"
//--------------
#define LG_ADDREN  "Add/Rename"
#define LG_SETNICK "Set nickname of %u as"
#define LG_SMLDESC "Smile: %u %s"
#define LG_ADDSMIL "Add Smiles"

//===============================

#define LG_ADDCONT "Add contact..."
#define LG_AUTHREQ   "Please authorize me..."
#define LG_AUTHGRANT "You are authorized!"

//================================

#define LG_MNUSTATUS  "Status"
#define LG_MNUADDCONT "Add Contact"
#define LG_MNUEDCFG   "Edit config"
#define LG_MNUABOUT   "About"

//-------------
#define LG_MNUQUOTE    "Quote"
#define LG_MNUADDSML   "Add smile"
#define LG_MNUSHINFO   "Get short info"
#define LG_MNUADDREN   "Add/rename"
#define LG_MNUSAUTHREQ "Send Auth Req"
#define LG_MNUSAUTHGRT "Send Auth Grant"
#define LG_MNUOPENLOG  "Open logfile"
#define LG_MNUCLEARCHT "Clear log"

//================================
#define LG_CHGSTATUS "Change Status"

#define LG_STONLINE "Online"
#define LG_STAWAY   "Away"
#define LG_STNA     "Not Available"
#define LG_STDND    "Do Not Disturb"
#define LG_STOCCUP  "Occupied"
#define LG_STFFC    "Free For Chat"
#define LG_STINVIS  "Invisible"

//================================

#define LG_MSGILLEGMSGCON "Illegal message ENIP_SOCK_CONNECTED!"
#define LG_MSGILLEGMSGREA "Illegal message ENIP_DATA_READ"
#define LG_MSGCANTCONN    "Can't connect!"
#define LG_MSGCANTCRSC    "Can't create socket, GPRS restarted!"
#define LG_MSGHOSTNFND    "Host not found!"
#define LG_MSGVIBRADIS    "Vibr� disabled!"
#define LG_MSGVIBRAENA    "Vibra enabled!"
#define LG_MSGSNDDIS      "Sounds disabled!"
#define LG_MSGSNDENA      "Sounds enabled!"
#define LG_MSGNOUINPASS   "Please setup UIN/PASS!"
#define LG_MSGSMILNOTFND  "Can't find smiles!"

//=================================

#define LG_GRSTATESTRING "State: %d, RXstate: %d\n%t"
#define LG_GRSKEYEXIT    "Exit"
#define LG_GRSKEYCLIST   "CList"

#define LG_GRTRYLOGIN    "Try to login..."
#define LG_GRWAITFORGPRS "Waiting for GPRS up..."
#define LG_GRSENDDNR     "Send DNR..."
#define LG_GRDNRERROR    "DNR ERROR %d!"
#define LG_GRDNROK       "DNR Ok, connecting..."
#define LG_GRBADPACKET   "Bad packet"
#define LG_GRLOGINMSG    "%s"
#define LG_GRSTATUSCHNG  "SC%d: %04X"
#define LG_GRERROR       "ERR: %s"
#define LG_GRRECVMSG     "MSG%d: %s"
#define LG_GRGPRSUP      "GPRS up, wait 10 sec..."

#endif

#endif

