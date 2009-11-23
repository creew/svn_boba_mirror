#ifndef _LANG_H_
#define _LANG_H_

void lgpInitLangPack(void);
void lgpFreeLangPack(void);

enum LGP_ID
{
  LGP_CantOpenFile=0,
  LGP_CantLoadAuthCode,
  LGP_HistoryFileFailed,
  LGP_CfgUpdated,
  LGP_RefUnderConstruction,
  LGP_RefEmpty,
  LGP_Error,
  LGP_Stop,
  LGP_WaitDNR,
  LGP_IpConnect,
  LGP_GetHostByName,
  LGP_FaultDNR,
  LGP_OkDNR,
  LGP_OpenSocket,
  LGP_ConnectFault,
  LGP_AnswerDNR,
  LGP_Connected,
  LGP_DataReceived,
  LGP_DataSending,
  LGP_DataSent,
  LGP_RemoteClosed,
  LGP_LocalClosed,
  LGP_InetBusy,
  LGP_EnableGPRS,
  LGP_UnderConstruction,
  LGP_Select,
  LGP_Close,
  LGP_Ok,
  LGP_Cancel,
  LGP_Templates,
  LGP_NameHeader,
  LGP_LinkHeader,
  LGP_TextHeader,
  LGP_SearchEngine,
  LGP_FindString,
  LGP_SavePageString,
  LGP_SelectHeader,
  LGP_EnterHeader,
  LGP_Add,
  LGP_BookmarkHeader,
  LGP_Save,
  LGP_NewBookmark,
  LGP_NewFolder,
  LGP_Absent,
  LGP_OptionsHeader,
  LGP_Edit,
  LGP_Delete,
  LGP_Back,
  LGP_Options,
  LGP_SelectBookmarkHeader,
  LGP_SearchHeader,
  LGP_FindHeader,
  LGP_SavePageHeader,
  LGP_MatchCase,
  LGP_Go,
  LGP_HistoryHeader,
  LGP_AddressHeader,
  LGP_Menu,
  LGP_GoTo,
  LGP_Bookmarks,
  LGP_History,
  LGP_Settings,
  LGP_Exit,
  LGP_Search,
  LGP_Find,
  LGP_SavePage,
  LGP_EnterName,
  LGP_EnterURL,
  LGP_FolderExists,
  LGP_FileExists,
  LGP_FileInvalid,
  LGP_FolderNotEmpty,
  LGP_Yes,
  LGP_No,
  LGP_Root,
  
  LGP_LangCode, // ����� ������ ����� � �����
  
  LGP_DATA_NUM
};

extern char * lgpData[LGP_DATA_NUM];

#endif /* _LANG_H_ */

