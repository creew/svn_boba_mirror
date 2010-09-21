#include "..\..\include\Lib_Clara.h"
#include "..\..\include\Types.h"
#include "..\..\include\dll.h"
//#include "classes.h"
#include "..\..\include\dir.h"
#include "vkp.h"

#include "include.tmp"

#define DLL_PRIVATE_AREA  // ������������ ������!! ��� ���������� ����� DLL

#include "export\LibraryDLL.h"

void elf_exit(void)
{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}

// -----------------  Dll_System area  -----------------
// ������ ����� DLL. �� ����� �� ���������.

volatile int usage_count;

#if defined(DB3200) || defined(DB3210) || defined(DB3350)
#define USE_FONTS
IFont * pFont;
FONT_DESC font_desc;
int font_count;
#endif


// -------------------  Public area  -------------------
// �������� � ������ ��������� �������.
// ������ ������� �������� Private area.
// ��� ������������� ����� ������� �������������� � h-�����.
// ����� �� ���� ������� ��� ���������� � ����������� DLL, ����� - DLLFILENAME_FunctionName.
//

#ifdef A2
#define USE_dll_GC_PutChar_0
void dll_GC_PutChar_0( GC* gc, int x, int y, int width, int height, wchar_t imageID )
{
  IImageManager * pImageManager=0;
  IUIImage * pUIImage=0;
  IUnknown * pGC =0;
  
  TUIRectangle rect;
  rect.Point.X=x;
  rect.Point.Y=y;
  rect.Size.Width=width;
  rect.Size.Height=height;
  
  CoCreateInstance(&CID_CImageManager, &IID_IImageManager, &pImageManager);
  pImageManager->CreateFromIcon(imageID,&pUIImage);

  DisplayGC_AddRef(gc,&pGC);
  
  pImageManager->Draw(pUIImage,pGC,rect);
  
  if (pImageManager) pImageManager->Release();
  if (pUIImage) pUIImage->Release();
  if (pGC) pGC->Release();
}
#endif

#ifdef A2
#define USE_dll_AB_READSTRING_0
int dll_AB_READSTRING_0( AB_STR_ITEM* ab, int rec_num, int field_ID )
{
  int res = 1;
  char * buf = new char[0x420];
  buf[0xE]=0;
  AB_GET_STRUCT * x = (AB_GET_STRUCT*)rec_num;
  AB_GET_STRUCT * y = (AB_GET_STRUCT*)(buf+0x4);
  y->unk_02=x->unk_02;
  y->rec_num=x->rec_num;
  buf[0x416]=field_ID;
  switch(field_ID)
  {
  default:
    buf[0x12]=2;
    break;
  case 0xC:
  case 0xD:
  case 0xE:
  case 0xF:
  case 0x10:
    buf[0x12]=6;
    break;
  }
  if (!REQUEST_HPHONEBOOK_READSTRING(SYNC,buf)&&buf[0]!=0) AB_STR_ITEM_Copy(ab,(AB_STR_ITEM*)(buf+0x10));
  else res=0;
  delete(buf);
  return(res);
}
#endif

#if defined(DB3200) || defined(DB3210) || defined(DB3350)
#define USE_dll_SetFont_0
int dll_SetFont_0( int font_size )
{
  IFontManager * pFontManager=0;
  IFontFactory * pFontFactory=0;
  TUIFontData pFontData;
  memset(&pFontData,0,sizeof(TUIFontData));
  
  if (pFont)
  {
    pFont->Release();
    pFont=0;
  }

  CoCreateInstance(&CID_CFontManager,&IID_IFontManager,&pFontManager);
  pFontManager->GetFontFactory(&pFontFactory);
  
  int font_size_without_style = font_size&0xFF;
  
  pFontFactory->GetDefaultFontSettings(UIFontSizeLarge,&pFontData);
  pFontData.size=(float)font_size_without_style;
  pFontData.TUIEmphasisStyle=font_size>>8;
  pFontFactory->CreateDefaultFont(&pFontData,&pFont);
  
  if (pFontManager) pFontManager->Release();
  if (pFontFactory) pFontFactory->Release();
  
  return(1);
}
#endif

#if defined(DB3200) || defined(DB3210) || defined(DB3350)
#define USE_dll_DrawString_0
void dll_DrawString_0( STRID strid, TUITextAlignment align, int x1, int y1, int x2, int y2, int unk, int unk1, int pen_color, int brush_color )
{
  TUIRectangle rect;
  
  int lineWidth = x2-x1;
  
  ITextRenderingManager * pTextRenderingManager=0;
  ITextRenderingFactory * pTextRenderingFactory=0;
  IRichTextLayout * pRichTextLayout=0;
  IRichText * pTextObject=0;
  IUnknown * pGC =0;
  
  CoCreateInstance(&CID_CTextRenderingManager,&IID_ITextRenderingManager,&pTextRenderingManager);
  pTextRenderingManager->GetTextRenderingFactory(&pTextRenderingFactory);
  pTextRenderingFactory->CreateRichText(&pTextObject);
  pTextRenderingFactory->CreateRichTextLayout(pTextObject,0,0,&pRichTextLayout);
  
  if (!pFont) dll_SetFont_0(20);
  TextObject_SetText(pTextObject,strid);
  TextObject_SetFont(pTextObject,pFont,0x8000000A,0x7FFFFFF5);
  pTextObject->SetTextColor(pen_color,0x8000000A,0x7FFFFFF5);
  
  pTextObject->SetAlignment(align,0x8000000A,0x7FFFFFF5);
  
  pRichTextLayout->Compose(lineWidth);
  
  rect.Point.X=x1;
  rect.Point.Y=y1;
  rect.Size.Width=x2-x1;
  rect.Size.Height=y2-y1;
  
  DisplayGC_AddRef(get_DisplayGC(),&pGC);
  
  pRichTextLayout->Display(pGC,x1,y1,&rect);
  
  if (pTextRenderingManager) pTextRenderingManager->Release();
  if (pTextRenderingFactory) pTextRenderingFactory->Release();
  if (pRichTextLayout) pRichTextLayout->Release();
  if (pTextObject) pTextObject->Release();
  if (pGC) pGC->Release();
}
#endif

#ifdef A2
#define USE_dll_GetImageWidth_0
int dll_GetImageWidth_0( wchar_t imageID )
{
  IUIImage * pUIImage=0;
  IImageManager * pImageManager=0;
  long image_width=0;
  long image_height=0;
  
  CoCreateInstance(&CID_CImageManager, &IID_IImageManager, &pImageManager);
  if (pImageManager->CreateFromIcon(imageID,&pUIImage)>=0)
      pUIImage->GetDimensions(&image_width,0,&image_height,0);
  
  if (pImageManager) pImageManager->Release();
  if (pUIImage) pUIImage->Release();
  return(image_width);
}
#endif

#ifdef A2
#define USE_dll_GetImageHeight_0
int dll_GetImageHeight_0( wchar_t imageID )
{
  IUIImage * pUIImage=0;
  IImageManager * pImageManager=0;
  long image_width=0;
  long image_height=0;
  
  if (imageID<100)
  {
#if defined(DB3200) || defined(DB3210) || defined(DB3350)
    pFont->GetFontHeight(&image_height);
#elif defined(DB3150)
    image_height=GetImageHeight_int( imageID );
#endif
  }
  else
  {
    CoCreateInstance(&CID_CImageManager, &IID_IImageManager, &pImageManager);
    if (pImageManager->CreateFromIcon(imageID,&pUIImage)>=0)
      pUIImage->GetDimensions(&image_width,0,&image_height,0);
  }
  
  if (pImageManager) pImageManager->Release();
  if (pUIImage) pUIImage->Release();
  return(image_height);
}
#endif

#ifdef OrangeLED_ID
#define USE_dll_OrangeLED_Control_0
void dll_OrangeLED_Control_0( int _free_val, int or_LED_ID, int level, int fade_time )
{
  Illumination_LedID_SetLevel_Gradually(OrangeLED_ID,level,fade_time);
}
#endif

#ifdef A2
#define USE_dll_MetaData_Desc_Create_0
void* dll_MetaData_Desc_Create_0( wchar_t* path, wchar_t* name )
{
  METADATA_DESC * MetaData_Desc = new METADATA_DESC;
  MetaData_Desc->pMetaData=0;
  MetaData_Desc->artist[0]=0;
  MetaData_Desc->title[0]=0;
  MetaData_Desc->album[0]=0;
  MetaData_Desc->year[0]=0;
  MetaData_Desc->genre[0]=0;
  MetaData_Desc->x6[0]=0;
  MetaData_Desc->x7[0]=0;
  CoCreateInstance(&CID_CMetaData, &IID_IMetaData, &MetaData_Desc->pMetaData);
  MetaData_Desc->pMetaData->SetFile(path,name);
  return(MetaData_Desc);
}
#endif

#ifdef A2
#define USE_dll_MetaData_Desc_Destroy_0
void dll_MetaData_Desc_Destroy_0( void* MetaData_Desc )
{
  if (((METADATA_DESC*)MetaData_Desc)->pMetaData) ((METADATA_DESC*)MetaData_Desc)->pMetaData->Release();
  delete(MetaData_Desc);
}
#endif

#ifdef A2
#define USE_dll_MetaData_Desc_GetTags_0
wchar_t* dll_MetaData_Desc_GetTags_0( void* MetaData_Desc, int tagID )
{
  METADATA_DESC * mdd = (METADATA_DESC*)MetaData_Desc;
  wchar_t * buf;
  switch(tagID)
  {
  case 0:
    buf = mdd->artist;
    break;
  case 1:
    buf = mdd->title;
    break;
  case 2:
    buf = mdd->album;
    break;
  case 3:
    buf = mdd->year;
    break;
  case 4:
    buf = mdd->genre;
    break;
  case 5:
    buf = mdd->x6;
    break;
  case 6:
    buf = mdd->x7;
    break;
  }
  mdd->pMetaData->GetTag(tagID,buf);
  return(buf);
}
#endif

#ifdef A2
#define USE_dll_MetaData_Desc_GetTrackNum_0
int dll_MetaData_Desc_GetTrackNum_0( void* MetaData_Desc, int __NULL )
{
  int track_num;
  ((METADATA_DESC*)MetaData_Desc)->pMetaData->GetTrackNum(__NULL,&track_num);
  return(track_num);
}
#endif

#ifdef A2
#define USE_dll_MetaData_Desc_GetCoverInfo_0
int dll_MetaData_Desc_GetCoverInfo_0( void* MetaData_Desc, char* cover_type, int* size, int* cover_offset )
{
  COVER_INFO_DESC cover_info;
  ((METADATA_DESC*)MetaData_Desc)->pMetaData->GetCoverInfo(&cover_info);
  *cover_type = cover_info.cover_type;
  *size = cover_info.size;
  *cover_offset = cover_info.cover_offset;
  return(1);
}
#endif

#ifdef A2
#define USE_dll_RedLED_On_0
void dll_RedLED_On_0( int __NULL )
{
  Illumination_LedID_SetLevel(5,100);
}
#endif

#ifdef A2
#define USE_dll_RedLED_Off_0
void dll_RedLED_Off_0( int __NULL )
{
  Illumination_LedID_Off(5);
}
#endif

#ifdef A2
#define USE_dll_Display_SetBrightness_0
void dll_Display_SetBrightness_0( int display, int brightness )
{
  Display_SetBrightnessLevel( brightness );
}
#endif

#if defined(DB3200) || defined(DB3210) || defined(DB3350)
#define USE_dll_Disp_GetStrIdWidth_0
int dll_Disp_GetStrIdWidth_0( STRID strid, int len )
{
  ITextRenderingManager * pTextRenderingManager=0;
  ITextRenderingFactory * pTextRenderingFactory=0;
  IRichTextLayout * pRichTextLayout=0;
  IRichText * pTextObject=0;
  
  CoCreateInstance(&CID_CTextRenderingManager,&IID_ITextRenderingManager,&pTextRenderingManager);
  pTextRenderingManager->GetTextRenderingFactory(&pTextRenderingFactory);
  pTextRenderingFactory->CreateRichText(&pTextObject);
  pTextRenderingFactory->CreateRichTextLayout(pTextObject,0,0,&pRichTextLayout);
  
  int width=RichTextLayout_GetTextWidth(strid,pRichTextLayout,pFont);
  
  if (pTextRenderingManager) pTextRenderingManager->Release();
  if (pTextRenderingFactory) pTextRenderingFactory->Release();
  if (pRichTextLayout) pRichTextLayout->Release();
  if (pTextObject) pTextObject->Release();
  
  return(width);
}
#endif

#ifdef A2
#define USE_dll_int2strID_0
STRID dll_int2strID_0( int num )
{
  return int2strID_int( num, 0, 0 );
}
#endif

#ifdef A2
#define USE_dll_Str2ID_0
STRID dll_Str2ID_0( const void* wstr,int flag,int len )
{
  return Str2ID_int( wstr, flag, len, 0, 0 );
}
#endif

#ifdef A2
#define USE_dll_TextID2wstr_0
int dll_TextID2wstr_0( STRID strid,wchar_t * dest,int maxlen )
{
  return TextID2wstr_int( strid, dest, maxlen, 0, 0 );
}
#endif

#ifdef A2
#define USE_dll_TextGetLength_0
int dll_TextGetLength_0( STRID strid )
{
  return TextGetLength_int( strid, 0, 0 );
}
#endif

#ifdef A2
#define USE_dll_TextFree_0
void dll_TextFree_0( STRID strid )
{
  TextFree_int( strid, 0, 0 );
}
#endif

#ifdef A2
#define USE_dll_TextCopyId_0
STRID dll_TextCopyId_0( STRID strid )
{
  return TextCopyId_int( strid, 0, 0 );
}
#endif

#ifdef A2
#define USE_dll_MainInput_getVisible_0
int dll_MainInput_getVisible_0( GUI * gui )
{
  return DispObject_GetVisible( GUIObject_GetDispObject(gui) );
}
#endif

#ifdef A2
#define USE_dll_MainInput_isPlus_0
int dll_MainInput_isPlus_0( GUI * gui )
{
  char str[1];
  PNUM2str( str, MainInput_getPNUM( gui ), 1, 1 );
  if (str[0]=='+') return(1);
  return(0);
}
#endif

#ifdef A2
#define USE_dll_MainInput_getCurPos_0
char dll_MainInput_getCurPos_0( GUI * gui )
{
  char * data = (char*)MainInput_data;
  return data[MainInput_CurPos_Offset];
}
#endif

#ifdef A2
#define USE_dll_GUIObject_SetTitleBackgroundImage_0
void dll_GUIObject_SetTitleBackgroundImage_0( GUI * gui,wchar_t imageID )
{
  DispObject_SetTitleBackgroundImage( GUIObject_GetDispObject(gui), imageID );
}
#endif

#ifdef A2
#define USE_dll_GUIObject_SetCursorImage_0
void dll_GUIObject_SetCursorImage_0( GUI * gui,wchar_t imageID )
{
  DispObject_SetCursorImage( GUIObject_GetDispObject(gui), imageID );
}
#endif

#ifdef A2
#define USE_dll_GUIObject_SetBackgroundImage_0
void dll_GUIObject_SetBackgroundImage_0( GUI * gui,wchar_t imageID )
{
  DispObject_SetBackgroundImage( GUIObject_GetDispObject(gui), imageID );
}
#endif

#ifdef A2
#define USE_dll_GUIObject_SetTitleTextColor_0
void dll_GUIObject_SetTitleTextColor_0( GUI * gui,int color )
{
  DispObject_SetTitleTextColor( GUIObject_GetDispObject(gui), color );
}
#endif

#ifdef A2
#define USE_dll_GetIMSI_0
char* dll_GetIMSI_0( void )
{
  static char imsi[9]={0};
  char error;
  if (!imsi[0]) Request_SIM_IMSI_Read(SYNC,get_imsi_const(),imsi,&error);
  return(imsi);
}
#endif

#ifdef A2
#define USE_dll_GetRadioState_0
int dll_GetRadioState_0( char * level, char * flag )
{
  char error_cause;
  return Request_SL_GetRadioState(SYNC,level,flag,&error_cause);
}
#endif

#ifdef A2
#define USE_dll_GetSignalQuality_0

struct GET_SIGNAL_QUALITY_SIGNAL
{
  SIGSELECT signo;
  int null;
  char unk_er;
  char rssi;
  char ber;
  char error_cause;
};

union SIGNAL
{
  SIGSELECT signo;
  struct GET_SIGNAL_QUALITY_SIGNAL hsig;
};

int dll_GetSignalQuality_0( char * rssi, char * ber )
{
  static const SIGSELECT sg[]={1,GetSignalQuality_Receive_SignalID};
  char res=0;
  PROCESS pid;
  
  hunt("LNH_ACC_SIDE/SL_Process",NULL,&pid,NULL);
  union SIGNAL * mem = alloc(8,GetSignalQuality_Send_SignalID);
  mem->hsig.null=0;
  send(&mem,pid);
  union SIGNAL * rec_buf = receive(sg);
  if (rec_buf->signo==GetSignalQuality_Receive_SignalID)
  {
    rssi[0]=rec_buf->hsig.rssi;
    ber[0]=rec_buf->hsig.ber;
    res=rec_buf->hsig.unk_er;
  }
  free_buf(&rec_buf);
  return(res);
}
#endif

#if defined(DB3200) || defined(DB3210) || defined(DB3350)
#define USE_dll_get_CellData_0
int dll_get_CellData_0( PLMN_LAC_DESC * plmn_lac, RAT_CI_DESC * rat_ci, char * CSReg )
{
  CONNECTION_INFO_DESC con_info;
  if(Network_INetworkConnection_GetConnectionInfo(&con_info)<0) return(0);
  plmn_lac->MCC[0]=con_info.MCC[0];
  plmn_lac->MCC[1]=con_info.MCC[1];
  plmn_lac->MCC[2]=con_info.MCC[2];
  plmn_lac->MNC[0]=con_info.MNC[0];
  plmn_lac->MNC[1]=con_info.MNC[1];
  plmn_lac->PLMN_x=con_info.PLMN_x;
  plmn_lac->LAC[0]=con_info.LAC[0];
  plmn_lac->LAC[1]=con_info.LAC[1];
  rat_ci->RAT=con_info.RAT;
  rat_ci->CI=con_info.CI;
  CSReg[0]=con_info.CSReg;
  
  return(1);
}
#endif

#ifdef A2
#define USE_dll_GetChipID_0
int dll_GetChipID_0()
{
  return ((GetChipID_int()&0xFF) <<8);
}
#endif

#if defined(DB3200) || defined(DB3210) || defined(DB3350)
#define USE_dll_get_mem_0

typedef struct
{
  PROCESS pid;
  OSADDRESS from;
  void* to;
  OSADDRESS size;
}get_mem_struct;

OSBOOLEAN dll_get_mem_0( PROCESS pid, OSADDRESS from, void* to, OSADDRESS size )
{
  get_mem_struct buf;
  buf.pid=pid;
  buf.from=from;
  buf.to=to;
  buf.size=size;
  
  return(get_mem_int(0,0,&buf));
}
#endif

#ifdef A2
#define USE_dll_ConnectionManager_Connection_GetState_0
int dll_ConnectionManager_Connection_GetState_0()
{
  char buf=0;
  if (ConnectionManager_Connection_GetState_int(&buf))
  {
    if (!buf) return(1);
  }
  return(0);
}
#endif


const LIBRARY_DLL_FUNCTIONINFO functions[]=
{
    #ifdef USE_dll_GC_PutChar_0
    0x140, (void*) dll_GC_PutChar_0,
    #endif
    
    #ifdef USE_dll_AB_READSTRING_0
    0x163, (void*) dll_AB_READSTRING_0,
    #endif
    
    #ifdef USE_dll_DrawString_0
    0x1ED, (void*) dll_DrawString_0,
    #endif
    
    #ifdef USE_dll_SetFont_0
    0x1F1, (void*) dll_SetFont_0,
    #endif
    
    #ifdef USE_dll_GetImageWidth_0
    0x293, (void*) dll_GetImageWidth_0,
    #endif
    
    #ifdef USE_dll_GetImageHeight_0
    0x294, (void*) dll_GetImageHeight_0,
    #endif
    
    #ifdef USE_dll_OrangeLED_Control_0
    0x2DC, (void*) dll_OrangeLED_Control_0,
    #endif
    
    #ifdef USE_dll_MetaData_Desc_Create_0
    0x2EF, (void*) dll_MetaData_Desc_Create_0,
    #endif
    
    #ifdef USE_dll_MetaData_Desc_Destroy_0
    0x2F0, (void*) dll_MetaData_Desc_Destroy_0,
    #endif
    
    #ifdef USE_dll_MetaData_Desc_GetTags_0
    0x2F1, (void*) dll_MetaData_Desc_GetTags_0,
    #endif
    
    #ifdef USE_dll_MetaData_Desc_GetTrackNum_0
    0x2F2, (void*) dll_MetaData_Desc_GetTrackNum_0,
    #endif
    
    #ifdef USE_dll_MetaData_Desc_GetCoverInfo_0
    0x301, (void*) dll_MetaData_Desc_GetCoverInfo_0,
    #endif
    
    #ifdef USE_dll_RedLED_On_0
    0x307, (void*) dll_RedLED_On_0,
    #endif
    
    #ifdef USE_dll_RedLED_Off_0
    0x308, (void*) dll_RedLED_Off_0,
    #endif
    
    #ifdef USE_dll_Display_SetBrightness_0
    0x31C, (void*) dll_Display_SetBrightness_0,
    #endif
    
    #ifdef USE_dll_Disp_GetStrIdWidth_0
    0x300, (void*) dll_Disp_GetStrIdWidth_0,
    #endif
    
    #ifdef USE_dll_int2strID_0
    0x15C, (void*) dll_int2strID_0,
    #endif
    
    #ifdef USE_dll_Str2ID_0
    0x15D, (void*) dll_Str2ID_0,
    #endif
    
    #ifdef USE_dll_TextID2wstr_0
    0x15F, (void*) dll_TextID2wstr_0,
    #endif
    
    #ifdef USE_dll_TextGetLength_0
    0x160, (void*) dll_TextGetLength_0,
    #endif
    
    #ifdef USE_dll_TextFree_0
    0x161, (void*) dll_TextFree_0,
    #endif
    
    #ifdef USE_dll_TextCopyId_0
    0x242, (void*) dll_TextCopyId_0,
    #endif
    
    #ifdef USE_dll_MainInput_getVisible_0
    0x1F6, (void*) dll_MainInput_getVisible_0,
    #endif
    
    #ifdef USE_dll_MainInput_isPlus_0
    0x1F9, (void*) dll_MainInput_isPlus_0,
    #endif
    
    #ifdef USE_dll_MainInput_getCurPos_0
    0x2EA, (void*) dll_MainInput_getCurPos_0,
    #endif
    
    #ifdef USE_dll_GUIObject_SetTitleBackgroundImage_0
    0x313, (void*) dll_GUIObject_SetTitleBackgroundImage_0,
    #endif
    
    #ifdef USE_dll_GUIObject_SetCursorImage_0
    0x315, (void*) dll_GUIObject_SetCursorImage_0,
    #endif
    
    #ifdef USE_dll_GUIObject_SetBackgroundImage_0
    0x316, (void*) dll_GUIObject_SetBackgroundImage_0,
    #endif
    
    #ifdef USE_dll_GUIObject_SetTitleTextColor_0
    0x3A4, (void*) dll_GUIObject_SetTitleTextColor_0,
    #endif
    
    #ifdef USE_dll_GetIMSI_0
    0x2E8, (void*) dll_GetIMSI_0,
    #endif
    
    #ifdef USE_dll_GetRadioState_0
    0x327, (void*) dll_GetRadioState_0,
    #endif
    
    #ifdef USE_dll_GetSignalQuality_0
    0x332, (void*) dll_GetSignalQuality_0,
    #endif
    
    #ifdef USE_dll_get_CellData_0
    0x324, (void*) dll_get_CellData_0,
    #endif
    
    #ifdef USE_dll_GetChipID_0
    0x24E, (void*) dll_GetChipID_0,
    #endif
    
    #ifdef USE_dll_get_mem_0
    0x18C, (void*) dll_get_mem_0,
    #endif
    
    #ifdef USE_dll_ConnectionManager_Connection_GetState_0
    0x331, (void*) dll_ConnectionManager_Connection_GetState_0,
    #endif
    
    #ifdef USE_FONTS
    0x2BB, (void*) &font_desc,
    0x2BC, (void*) &font_count,
    #endif
    
	//�����
    0, NULL
};

// -------------------  Private area  ------------------
// ����� ����, ��� ������ ������������ ���������� ���������� � Public �������,
// �� ��������� ���������� � DLLFILENAME_private.h ����������.
// ��� ���������� ����� ����� � �������� DLL ( func2 , func3 , main )
// -----------------------------------------------------

void tryvkp(int dirplace, Vkp& vkp)
{
    wchar_t* patchesdir = GetDir(dirplace | DIR_ELFS_CONFIG);
    FILELISTITEM * fli;
    FILELISTITEM* mem = (FILELISTITEM* ) malloc(sizeof(FILELISTITEM));
    DIR_HANDLE * handle = AllocDirHandle( patchesdir );
    if(handle)
    {
        while(fli=(FILELISTITEM*)GetFname(handle,mem))
        {
            wchar_t* ext = getFileExtention(fli->fname);
            if(ext && !wstrcmpi(ext,L"vkp"))
            {
                FSTAT st;
                if(!fstat(patchesdir,fli->fname,&st))
                {
                    int f;
                    
                    if ((f=_fopen(patchesdir,fli->fname,0x001,0x180,0))>=0)
                    {
                        char* vkpf = (char*)malloc(st.fsize);
                        fread(f,vkpf,st.fsize);
                        fclose(f);
                        
                        int errorline = vkp.dovkp(vkpf, st.fsize);
                        //������ � ��� �� �������?
                        
                        mfree( vkpf );
                    }
                }
            }
        }
        DestroyDirHandle( handle);
    }
    mfree( mem );
}

LIBRARY_DLL_FUNCTIONINFO* dovkp()
{
    Vkp vkp;
    
    tryvkp(MEM_INTERNAL, vkp);
    tryvkp(MEM_EXTERNAL, vkp);
    
    int size = vkp.gettable( NULL );
    
    LIBRARY_DLL_FUNCTIONINFO* ptr = new LIBRARY_DLL_FUNCTIONINFO[size + MAXELEMS(functions)];
    
    vkp.gettable( (void**)ptr );
    memcpy( &ptr[size], functions, sizeof(functions) );
    
    return ptr;
}

int main ( int Action , LIBRARY_DLL_DATA * data )
{
  LIBRARY_DLL_DATA * p;
  switch (Action)
  {
  case DLL_INIT:
    // ������������� Dll_System area. ���������� ������ ���� ���, ��� �������� � ������.
    // ���� ������� �� ����, �� �������� ������������ � ����� DLL_ERROR_INIT
    // data == NULL
    debug_printf("\nlibrary.dll: dll init\n");
    usage_count=0;
#ifdef USE_FONTS
    pFont=0;
    font_count=1;
    font_desc.id=20;
    wstrcpy(font_desc.name,L"E_20R");
#endif
    
    debug_printf("\nlibrary.dll: dll init done\n");
    return(0);

  case DLL_LOAD:
    debug_printf("\nlibrary.dll: dll load\n");
    // ������������� Private / Public area. ���������� ������ ���, ��� ������ LoadDLL.
    // data == NULL

    usage_count++;                      // ���������� �������� ������������� ���� DLL

    if (!(p = new LIBRARY_DLL_DATA)) return(0);       // �������� this � ������������� ������� � �������

    memset(p,0,sizeof(LIBRARY_DLL_DATA));

    //��� ����� ifdef ��������
    p->functions = dovkp();
    //p->functions = ::functions;
    
    // Private area
    
    debug_printf("\nlibrary.dll: dll load done\n");
    return((int)p);

  case DLL_UNLOAD:
    debug_printf("\nlibrary.dll: dll unload\n");
    // ������������ Private / Public area. ���������� ������ ���, ��� ������ UnLoadDLL.
    // data == this

    //  ����������� ��� ������������� data->...

    delete[] data->functions;

    //  ����������� ������ data
    mfree(data);
    usage_count--;                      // ���������� �������� ������������� ���� DLL

    // ���� ������ ����� �� ����������...
    if(!usage_count)
    {
        //
        //  ����������� ��� ������������� Dll_System area
        //
      SUBPROC(elf_exit);
    }
    return(usage_count);

  }
  return(0);
}
