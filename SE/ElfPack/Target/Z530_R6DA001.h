#ifndef Z530_R6DA001
#error Z530_R6DA001 configuration needed for this header!
#else
#ifndef _Z530_R6DA001_
#define _Z530_R6DA001_

#define EXT_TABLE 0x444E2C54
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x4462D036
#define PATH_AUDIO_EXT 0x4462B9D4
#define PATH_IMAGE_INT 0x4462D0B6
#define PATH_IMAGE_EXT 0x4462BAF4
#define PATH_VIDEO_INT 0x4462D192
#define PATH_VIDEO_EXT 0x4462BBE0
#define PATH_THEME_INT 0x4462D17C
#define PATH_THEME_EXT 0x4462BBA2
#define PATH_OTHER_INT 0x4462D144
#define PATH_OTHER_EXT 0x4462BB64

#define PATH_ELF_ROOT_INT     (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/MSSEMC/Media files/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/MSSEMC/Media files/other/ZBin/Config")
#define PATH_INI              (L"/usb/other/ini")
#define PATH_DLL              (L"/usb/other/ZBin/DLL")
#define PATH_DEFAULT          (L"/usb/other")

#define PATH_USER_INT         (L"/usb")
#define PATH_USER_EXT         (L"/card/MSSEMC/Media files")

#define DB_CMD_SETSMALLICON 0x36

#define PAGE_ENTER_EVENT 5
#define PAGE_EXIT_EVENT 6
#define ACCEPT_EVENT 0x0F
#define PREVIOUS_EVENT 0x10
#define CANCEL_EVENT 0x11

#endif
#endif
