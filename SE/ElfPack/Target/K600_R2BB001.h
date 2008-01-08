#ifndef K600_R2BB001
#error K600_R2BB001 configuration needed for this header!
#else
#ifndef _K600_R2BB001_
#define _K600_R2BB001_

#define pKBD 0x28025358
#define EXT_TABLE 0x20F1E088

#define RUN_CHECK 0x204E8209


#define STR_START 0xAA7
#define STR_APPLICATION 0x0319
#define STR_Saved_on_Memory_Stick 0x0EAF
#define STR_Saved_in_phone_memory 0x0EAE
#define ELF_SMALL_ICON 0xE816

//#define ROOT_APP 0x2811918C
//#define PID_MMI 0x100EE
#define HELPER_SIG 0x6FF0
#define RAM_BASE (0x2824466C)

#define PATH_AUDIO_INT 0x20EDE5B8
#define PATH_AUDIO_EXT 0x21161366
#define PATH_IMAGE_INT 0x20EDE660
#define PATH_IMAGE_EXT 0x21161444
#define PATH_VIDEO_INT 0x20EDE724
#define PATH_VIDEO_EXT 0x21161530
#define PATH_THEME_INT 0x20EDE704
#define PATH_THEME_EXT 0x211614F2
#define PATH_OTHER_INT 0x20EDE6E4
#define PATH_OTHER_EXT 0x211614B4

#define PATH_ELF_ROOT_INT (L"/tpa/user/other/ZBin")
#define PATH_ELF_ROOT_EXT (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_INT (L"/tpa/user/other/ZBin")
#define PATH_ELF_EXT (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_DAEMONS_INT (L"/tpa/user/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT (L"/card/MSSEMC/Media files/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT (L"/tpa/user/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT (L"/card/MSSEMC/Media files/other/ZBin/Config")
#define PATH_INI (L"/tpa/user/other/ini")
#define PATH_DEFAULT (L"/tpa/user/other")

#define ELF_RUN_PAGE_EV_0x10 0x204E4F75
#define ELF_RUN_PAGE_EV_0x0F 0x204E4F75
#define ELF_RUN_PAGE_EV_0x11 0x204E4F7D
#define ELF_RUN_PAGE_EV_0x06 0x204E4FD5

#define DB_CMD_SETSMALLICON 0x35
#define DB_DB_EXT_C1 0x258

#endif
#endif
