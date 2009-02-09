#ifndef _ZSLIB_H
#define _ZSLIB_H

#define CASESENSITIVITY (0)
#define MAX_PATH_INZIP  (256)
#define WRITEBUFFERSIZE (2048)

// Common functions
void FreeZipInfo(ZIPINF* pzi);
int ReadZipInfo(ZIPINF* pzi);
int OpenZip(ZIPINF* pzi, char* zipFileName);
void CloseZip(ZIPINF* pzi);
int ExtractCurrentFile(ZIPINF* pzi, int ind, wchar_t* extractDir, int usePaths, int ip);
int ExtractFile(ZIPINF* pzi, wchar_t* fname, wchar_t* extractDir, int usePaths);
int ExtractFileByID(ZIPINF* pzi, int id, wchar_t* extractDir, int usePaths, int ip);
int ExtractDir(ZIPINF* pzi, wchar_t* dname, wchar_t* extractDir, int usePaths);

// Tab functions
int IsZipOpened(int tab);
int IsInZip();
int OpenTabZip(int tab, wchar_t* zipFileName);
void CloseTabZip(int tab);
int FillZipFiles(int tab, wchar_t* subdname);

// Buffer functions
void ZipBufferExtractBegin();
int ZipBufferExtract(FN_ITM* pi, wchar_t* extractDir);
void ZipBufferExtractEnd();

int EnumZipFiles(ZIPINF* pzi, char* subdname, ENUM_SEL_PROC enumproc, unsigned int param);
int IsItZipFile(wchar_t* fname);

#endif
