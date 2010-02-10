#ifdef __cplusplus

#ifndef OLDFUNCTIONS

#define DEPRECATE(t,f,n) extern t f##_FUNCTION_IS_DEPRECATED_USE_##n(); static t f(...){ f##_FUNCTION_IS_DEPRECATED_USE_##n(); return (t)0; }

DEPRECATE(int,isFileExist,fstat)
DEPRECATE(void,AddMSGHook,GUIObject_Softkey_SetAction)
DEPRECATE(void,UI_Event_toSIDwData,UI_Event_toBookIDwData)
DEPRECATE(void,AddCommand,GUIObject_Softkey_SetText)
DEPRECATE(void,SoftKey_SetEnable,GUIObject_SoftKey_SetEnable)
DEPRECATE(void,SoftKey_AddErrorStr,GUIObject_SoftKey_AddErrorStr)
DEPRECATE(void,SoftKey_RemoveItem,GUIObject_SoftKey_RemoveItem)
DEPRECATE(void,SoftKey_SetVisible,GUIObject_SoftKey_SetVisible)
DEPRECATE(void,SoftKey_SuppressDefaultAction,GUIObject_SoftKey_SuppressDefaultAction)
DEPRECATE(int,GetFocusetListObjectItem,ListMenu_GetSelectedItem)
DEPRECATE(void,SetHeaderText,DispObject_SetTitleText)
DEPRECATE(void,Hide,BookObj_Hide)
DEPRECATE(void,Show,BookObj_Show)
DEPRECATE(int,REQUEST_PROFILE_SETACTIVEPROFILE,Profile_SetActive)
DEPRECATE(int,JavaDialog_Init,JavaDialog_Open)
DEPRECATE(int,JavaDialog_Uninit,JavaDialog_Close)
DEPRECATE(void,GUIObject_SetHeaderText,DispObject_SetTitleText)
#else

#define isFileExist fstat
#define AddMSGHook GUIObject_Softkey_SetAction
#define UI_Event_toSIDwData UI_Event_toBookIDwData
#define AddCommand GUIObject_Softkey_SetText
#define SoftKey_SetEnable GUIObject_SoftKey_SetEnable
#define SoftKey_AddErrorStr GUIObject_SoftKey_AddErrorStr
#define SoftKey_RemoveItem GUIObject_SoftKey_RemoveItem
#define SoftKey_SetVisible GUIObject_SoftKey_SetVisible
#define SoftKey_SuppressDefaultAction GUIObject_SoftKey_SuppressDefaultAction
#define GetFocusetListObjectItem ListMenu_GetSelectedItem
#define SetHeaderText DispObject_SetTitleText
#define Hide BookObj_Hide
#define Show BookObj_Show
#define REQUEST_PROFILE_SETACTIVEPROFILE Profile_SetActive
#define JavaDialog_Init JavaDialog_Open
#define JavaDialog_Uninit JavaDialog_Close
#define GUIObject_SetHeaderText DispObject_SetTitleText

#endif

#endif
