#ifndef W710_R1JC002
#error W710_R1JC002 configuration needed for this header!
#else
#ifndef _W710_R1JC002_
#define _W710_R1JC002_

#define EXT_TABLE 0x4597994C
#define RUN_CHECK 0x44EA1D25
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x459B81DC
#define PATH_AUDIO_EXT 0x456EE2E8
#define PATH_IMAGE_INT 0x456EE048
#define PATH_IMAGE_EXT 0x456EE354
#define PATH_VIDEO_INT 0x456EE0F4
#define PATH_VIDEO_EXT 0x456EE3F8
#define PATH_THEME_INT 0x456EE0D4
#define PATH_THEME_EXT 0x456EE3E0
#define PATH_OTHER_INT 0x456EE06C
#define PATH_OTHER_EXT 0x456EE3AC

#define PATH_ELF_ROOT_INT     (L"/tpa/user/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/card/other/ZBin")
#define PATH_ELF_INT          (L"/tpa/user/other/ZBin")
#define PATH_ELF_EXT          (L"/card/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/tpa/user/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/tpa/user/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/other/ZBin/Config")
#define PATH_INI              (L"/tpa/user/other/ini")
#define PATH_DEFAULT          (L"/tpa/user/other")

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44EA54AD
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44EA54A9
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44EA54BD
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44E9E1FD

#define DB_CMD_SETSMALLICON 0x3A
#define DB_CMD_SETTHUMBNAILICON 0x3B
#define DB_DB_EXT_C1 0x2D8

#endif
#endif
