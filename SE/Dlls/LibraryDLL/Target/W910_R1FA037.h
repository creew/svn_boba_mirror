#include "..\..\..\\include\Types.h"
#include "..\classes.h"

void (*CoCreateInstance)(char * guid1,char * guid2,void *pInterface)=(void(*)(char * guid1,char * guid2,void *pInterface))0x116D60ED;
void (*DisplayGC_AddRef)(GC*,void *)=(void(*)(GC*,void *))0x11673A69;
int (*Illumination_LedID_SetLevel)(int LED_ID,int level)=(int(*)(int LED_ID,int level))0x115BE4ED;
int (*Illumination_LedID_Off)(int LED_ID)=(int(*)(int LED_ID))0x115BE67D;
int (*REQUEST_HPHONEBOOK_READSTRING)(const int* sync,void * buf)=(int(*)(const int* sync,void * buf))0x10EB163D;
void (*AB_STR_ITEM_Copy)(AB_STR_ITEM * dest,AB_STR_ITEM * source)=(void(*)(AB_STR_ITEM * dest,AB_STR_ITEM * source))0x10F40215;
int (*GetImageHeight_int)(wchar_t ImageID)=(int(*)(wchar_t ImageID))0x1166FB39;
void (*Display_SetBrightnessLevel)(int level)=(void(*)(int level))0x115F4901;
STRID (*int2strID_int)(int num,char * filename,int line_num)=(STRID(*)(int num,char * filename,int line_num))0x116535C5;
STRID (*Str2ID_int)(const void* wstr,int flag,int len,char * filename,int line_num)=(STRID(*)(const void* wstr,int flag,int len,char * filename,int line_num))0x11653785;
int (*TextID2wstr_int)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num)=(int(*)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num))0x11653BF1;
int (*TextGetLength_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x11653AF1;
void (*TextFree_int)(STRID strid,char * filename,int line_num)=(void(*)(STRID strid,char * filename,int line_num))0x11653A1D;
STRID (*TextCopyId_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x11653929;
