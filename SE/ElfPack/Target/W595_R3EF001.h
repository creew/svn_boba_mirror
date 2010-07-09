#ifndef W595_R3EF001
#error W595_R3EF001 configuration needed for this header!
#else
#ifndef _W595_R3EF001_
#define _W595_R3EF001_

#define EXT_TABLE 0x11E531F4
#define Library_Start 0x127CA000
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x11DA5978
#define PATH_AUDIO_EXT 0x11DA2BF8
#define PATH_IMAGE_INT 0x11DA59F8
#define PATH_IMAGE_EXT 0x11DA2C64
#define PATH_VIDEO_INT 0x11DA5CB8
#define PATH_VIDEO_EXT 0x11DA2CDC
#define PATH_THEME_INT 0x11DA5C24
#define PATH_THEME_EXT 0x11DA2CC4
#define PATH_OTHER_INT 0x11EDF60C
#define PATH_OTHER_EXT 0x11DA2CAC

#define PATH_ELF_ROOT_INT     (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/card/other/ZBin")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/card/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/other/ZBin/Config")
#define PATH_INI              (L"/usb/other/ini")
#define PATH_DLL              (L"/usb/other/ZBin/DLL")
#define PATH_DEFAULT          (L"/usb/other")

#define PATH_USER_INT         (L"/usb")
#define PATH_USER_EXT         (L"/card")

#define DB_CMD_SETSMALLICON 0x3A
#define DB_CMD_SETTHUMBNAILICON 0x3B

#define PAGE_ENTER_EVENT 0xF
#define PAGE_EXIT_EVENT 0x10
#define ACCEPT_EVENT 2
#define PREVIOUS_EVENT 3
#define CANCEL_EVENT 4

#endif
#endif
