#ifndef W660_R6AD001
#error W660_R6AD001 configuration needed for this header!
#else
#ifndef _W660_R6AD001_
#define _W660_R6AD001_

#define EXT_TABLE 0x45ACEEC4
#define RUN_CHECK 0x44FDC4B9
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x4586D26C
#define PATH_AUDIO_EXT 0x4586D58C
#define PATH_IMAGE_INT 0x4586D310
#define PATH_IMAGE_EXT 0x4586D5F8
#define PATH_VIDEO_INT 0x4586D398
#define PATH_VIDEO_EXT 0x4586D680
#define PATH_THEME_INT 0x4586D378
#define PATH_THEME_EXT 0x4586D668
#define PATH_OTHER_INT 0x4586D334
#define PATH_OTHER_EXT 0x4586D650

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

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44FDB83D
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44FDB839
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44FDB84D
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44FD452D

#define DB_CMD_SETSMALLICON 0x3B
#define DB_CMD_SETTHUMBNAILICON 0x3C
#define DB_DB_EXT_C1 0x2EC

#endif
#endif