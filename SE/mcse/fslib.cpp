#include "inc\mc.h"
#include "inc\mui.h"


//File Name
wchar_t* GetFileExt(wchar_t* fname)
{
  wchar_t *s1, *s2;
  s1=wstrrchr(fname,'.');
  if (s1)
  {
    int i=0;
    s2=++s1;       
    while(*s2++) i++;
    if (i>MAX_EXT) s1=0;
  }
  return (s1);
}

wchar_t* GetFileName(wchar_t* fname)
{
  int len = wstrlen(fname);
  int ii;
  for(ii = len-1; ii >= 0; ii--) {
    if (fname[ii] == '\\' || fname[ii] == '/') break;
  }
  if (ii>=0) return fname+ii+1;
  return fname;
}


wchar_t* GetFileDir(wchar_t* fname, wchar_t* buf)
{
  int c;
  wchar_t *s=fname;
  int len=0;
  int i=0;
  while((c=*s++))
  {
    if ((c=='\\' || c=='/') && *s!=0)  len=i;
    i++;
  }
  if (buf)
  {
    if (len)
    {
      wstrncpy(buf,fname,len);      
    }
    buf[len]=0;    
  }
  return (buf);
}

int fexists(wchar_t* fname)
{
  W_FSTAT fs;
  return (w_fstat(fname,&fs)!=-1);
}


int rmtree(wchar_t* path, int ip)
{
  FN_LIST fnlist;
  fn_zero(&fnlist);
  fn_fill(&fnlist, path);
  fn_rev(&fnlist);
  
  int res = 1;
  FN_ITM *itm = fnlist.items;
  while(itm && !progr_stop)
  {
    switch (itm->ftype)
    {
    case TYPE_COMMON_DIR:
      res &= (!w_remove(itm->full));
      break;
    case TYPE_COMMON_FILE:
      res &= (!w_remove(itm->full));
      break;
    case TYPE_ZIP_DIR:
    case TYPE_ZIP_FILE:
    default:
      break;
    }
    itm=(FN_ITM *)itm->next;
    if (ip) incprogr(1);
  }
  fn_free(&fnlist);
  return res;
}
int fsrm(wchar_t* path, int ip)
{
  int res;
  if (isdir(path))
    res= rmtree(path, ip);
  else
  {
    res=!w_remove(path);
    if (ip) incprogr(1);
  }
  return res;
}

#define BUF_SIZE 0x4000
int fcopy(wchar_t* src, wchar_t* dst)
{
  int fi=-1, fo=-1;
  char* buff=0;
  int cb, left;
  int res = 0;
  //int attr=0;
  
 // if (CONFIG_CONFIRM_REPLACE && fexists(dst))
 // {
 //   if (MsgBoxYesNoWithParam(ind_pmt_exists, 0) != IDYES)
 //     return 1; // ��� ������, ���� �� ����� ��������������
 // }
  fi = w_fopen(src, WA_Read, 0x1FF, 0);
  if (fi >=0) 
  {
    fo = w_fopen(dst, WA_Read+WA_Write+WA_Create+WA_Truncate, 0x1FF, 0);
    if (fo >=0) 
    {
      left = w_lseek(fi, 0, WSEEK_END);
      w_lseek(fi, 0, WSEEK_SET);
      if (left)
      {
        buff = new char[BUF_SIZE];
        if (!buff) goto L_EXIT;
        progrsp_max = left;
        incprogrsp(0);
      }
      while (left) 
      {
        cb = left < BUF_SIZE ? left : BUF_SIZE;
        left -= cb;
        incprogrsp(cb);
        if (w_fread(fi, buff, cb) != cb) goto L_EXIT;
        if (w_fwrite(fo, buff, cb) != cb) goto L_EXIT;
      }
      endprogrsp();
      res = 1;
      WriteLog("fcopy. OK");
    }
  }
L_EXIT:
  if (buff) delete(buff);
  if (fo != -1) w_fclose(fo);
  if (fi !=- 1) w_fclose(fi);
  return res;
}

const wchar_t badchars[] = {'?', '*', '"', ':', '<', '>', '/', '\\', '|', '\n', '\r'};

int _IsBadChar(wchar_t wch)
{
  for(int cc = 0; cc < sizeof(badchars)/sizeof(wchar_t); cc++)
    if (badchars[cc] == wch) return 1;
  return 0;  
}

int TestFileName(wchar_t* wsname)
{
  wchar_t wchar;
  while((wchar=*wsname++))
  {
    if (_IsBadChar(wchar)) return 0;
  }
  return 1;
}

void CorFileName(wchar_t* wsname)
{
  wchar_t wchar;
  int i=0;
  while((wchar=*wsname++))
  {
    if (!_IsBadChar(wchar)) 
      wsname[i++]=wchar;
  }
  wsname[i]=0;
}



int EnumFilesInDir(wchar_t* dname, ENUM_FILES_PROC enumproc, unsigned int param, int recursive, int enumDirs)
{
  unsigned int ccFiles   = 0;
  unsigned int ccSubDirs = 0;
  wchar_t *path=new wchar_t[MAX_PATH];
  void *handle=w_diropen(dname);
  wchar_t *next;
  if (handle && path)
  {
    w_chdir(dname);
    while((next=w_dirread(handle)))
    {
      W_FSTAT fs;
      w_fstat(next,&fs);
      if (fs.attr & FA_DIRECTORY)
      {
        if (enumDirs)
        {
          ccSubDirs++;
          snwprintf(path, MAX_PATH-1,_ls_ls, dname, next);
          int tmp = 0;
          if (recursive) tmp = EnumFiles(path, enumproc, param);
          ccSubDirs += tmp >> 16;
          ccFiles += tmp & 0xffff;
          if (enumproc)
            if (enumproc(dname, next, &fs, param)==0)
              break;
        }
      }
      else
      {
        ccFiles++;
        if (enumproc)
          if (enumproc(dname, next, &fs, param)==0)
            break;
      }
    }
    w_dirclose(handle);
  }
  delete path;
  if (ccSubDirs > 0xffff) ccSubDirs = 0xffff;
  if (ccFiles > 0xffff)   ccFiles = 0xffff;
  return (ccSubDirs << 16 | ccFiles);
}


int EnumFiles(wchar_t* dname, ENUM_FILES_PROC enumproc, unsigned int param)
{
  // ���������� ��������� � �� ������������ ����
  return EnumFilesInDir( dname, enumproc, param, 1, 1);
}


int GetFilesCnt(wchar_t* path)
{
  int res=1;
  if (isdir(path))
  {
    int sub = EnumFiles(path, NULL, 0);
    res += (sub>>16) + (sub & 0xffff);
  }
  return res;
}

int mktree(wchar_t* path)
{
  if (isdir(path)) return 1;
  int len = wstrlen(path);
  int c;
  wchar_t *buf=new wchar_t[len+1];
  for(int ii=0;ii<len;ii++)
  {
    c = path[ii];
    if (c=='/')
    {
      buf[ii]=0;
      w_mkdir(buf, 0x1FF);
    }
    buf[ii]=c;
  }
  delete buf;
  return !(w_mkdir(path, 0x1FF));
}

int cptree(wchar_t* src, wchar_t* dst, int ip)
{
  FN_LIST fnlist;
  fn_zero(&fnlist);
  fn_fill(&fnlist, src);
  fn_rev(&fnlist);
  
  
  wchar_t *dstfull=new wchar_t[MAX_PATH];
  int psrc = wstrlen(src)+1;
  int res = 1;
  //int attr=0;
  
  int tmp = progr_act;
  progr_act = ind_dirmking;
  incprogr(-1);
  
  FN_ITM *itm = fnlist.items;
  while(itm && !progr_stop)
  {
    if (itm->ftype == TYPE_COMMON_DIR) // TODO: ZIP_DIR...
    {
      wchar_t* pdst;
      if (itm->full[psrc-1])
      {
        wchar_t* psrcname = itm->full+psrc;
        snwprintf(dstfull,MAX_PATH-1, _ls_ls, dst, psrcname);
        pdst = dstfull;
      }
      else
      {
        pdst = dst;
      }
      res &= mktree(pdst);
      //GetFileAttrib(itm->full, (unsigned char*)&attr, &err);
      //SetFileAttrib(pdst, attr, &err);
    }
    itm=(FN_ITM *)itm->next;
  }
  progr_act = tmp;
  
  itm = fnlist.items;
  while(itm && !progr_stop)
  {
    if (itm->ftype == TYPE_COMMON_FILE) // TODO: ZIP_FILE...
    {
      wchar_t* psrcname = itm->full+psrc;
      snwprintf(dstfull,MAX_PATH-1, _ls_ls, dst, psrcname);
      res &= fcopy(itm->full, dstfull);
    }
    itm=(FN_ITM *)itm->next;
    if (ip) incprogr(1);
  }
  fn_free(&fnlist);
  delete dstfull;
  return res;
}


typedef struct
{
	int number;
	wchar_t cleanName[MAX_PATH];
	wchar_t* cleanExt;
} ParseFileNameStruct;

void parse_name(wchar_t* fname, ParseFileNameStruct* pfns)
{
  pfns->number = 0;
  wstrcpy(pfns->cleanName, fname);
  int fname_len = wstrlen(fname);
  wchar_t* ps = GetFileExt(pfns->cleanName); // ��������� �� ����������
  
  if (ps != 0)
  {
    pfns->cleanExt = ps;
    *(--ps) = '\0';	// ��������� ��� � ����������
  }
  else
  {
    // ���������� ��� �����
    pfns->cleanExt = (wchar_t *)str_empty;
    ps = pfns->cleanName + fname_len; // ��������� �� ����� �����
  }
  if (ps - pfns->cleanName >= 3) // ������ ���� 3 ������� ��� ������� - ������ � �����
  {
    if (*--ps == ')')  // ����������� ������ ����
    {
      int i = 0;
      int p10 = 1;
      while (--ps >= fname && *ps >= '0' && *ps <= '9' && p10 < 100000)
      {
        i += (*ps - '0') * p10;
        p10 *= 10;
      }
      if (ps >= fname && *ps == '(' && i != 0)
      {
        pfns->number = i;
        *ps = '\0'; // �������� ������ ���
      }
    }
  }
}

int find_next_name(ParseFileNameStruct* pfns, wchar_t *buf)
{
  for (pfns->number++; pfns->number < 100; pfns->number++)
  {
    snwprintf(buf, MAX_PATH-1,_ls_i_ls, pfns->cleanName, pfns->number, pfns->cleanExt);
    if (!fexists(buf))
      return 1; 
  }
  return 0;
}

int fscp(wchar_t* src, wchar_t* dst, int ip)
{
  progrsp_start = 1;
  int res;
  int isSame = (wstrcmpi(src, dst) == 0);
  if (isdir(src))
  {
    res = cptree(src, dst, ip);
  }
  else
  {
    if (isSame)
    {
      wchar_t buf[MAX_PATH];
      // ������� ����� ��� � ��������...
      ParseFileNameStruct fns;
      parse_name(src, &fns);
      // ���� ��������� ��������� ���
      if (find_next_name(&fns,buf))
      {
        res = fcopy(src, buf);
      }
      else res = fcopy(src, dst);
    }
    else res = fcopy(src, dst);
    if (ip) incprogr(1);
  }
  progrsp_start = 0;
  return res;
}

int fsmv(wchar_t* src, wchar_t* dst)
{
  int res = 0;
  /*
  if (src[0] != dst[0])
  {
    if (wstrcmpi(src, dst) != 0) // ������ ��� ������������ ������
      res = fmove(src, dst, &err);
    incprogr(GetFilesCnt(dst));
  }
  else*/
    if (fscp(src, dst, 1)) 
      res = fsrm(src, 0);
  return res;
}

