#include "inc\mc.h"
#include "inc\mui.h"
#include "inc\zslib.h"



int isdir(wchar_t *name){
  W_FSTAT fs;
  w_fstat(name,&fs);
  return(fs.attr&FA_DIRECTORY);
}

int isDir(int tab, wchar_t* dname)
{
  if (IsArchiveOpened(tab))
    return 1; // ���� ������������� ��� ����������
  else
    return isdir(dname);
}

void MsgBoxErrorMmi(int err)
{
  char *e=(char *)err;
  MsgBoxError(e,0);
}

int cd(int tab, wchar_t *dname)
{
  int drv;
  if (IsArchiveOpened(tab))
  {
    drv = _CurDrv;
  }
  else
  {
    if (!isDir(tab, dname))
    {
      wchar_t buf[MAX_PATH];
      snwprintf(buf, MAXELEMS(buf)-1, L"%ls\\", dname);
      if (!isdir(buf))
      {
        MsgBoxError(ind_err_badname_t, dname);
        return 0;
      }
    }
    drv=-1;
    for (int i=0; i<MAX_DRV; i++)
    {
      if (!wstrcmpni(Drives[i].path,dname,wstrlen(Drives[i].path))) drv=i;
    }
    if (drv==-1)
    {
      MsgBoxError(ind_err_baddrv_t, dname);
      return 0;
    }
  }
  _cd_tab(tab, drv, dname);
  int res = SetTabDrv(tab, drv);
  if (!res) SetTabIndex(tab, 0, 0);
  return res;
}

void _NewDir(wchar_t *wsname)
{
  wstrncpy(szLastNewDir, wsname, MAXELEMS(szLastNewDir));	// ��������� ��������� ���
  CurFullPath(wsname);
  
  if (w_mkdir(pathbuf, 0x1FF))
    MsgBoxError(muitxt(ind_err_makedir));
  else
  {
    DoRefresh();
    //���� ����� ������� �������
    int ind = GetCurTabFileIndex(wsname);
    SetCurTabIndex(ind, 0);
  }
}



int M_Delit(FILEINF *file, int param)
{
  if (file && pathbuf && wstrlen(file->ws_name))
  {
    CurFullPath(file->ws_name);
    /*if (file->attr & FA_READONLY)
    {
      if (!CONFIG_CONFIRM_DELETERO || MsgBoxYesNoWithParam(ind_pmt_rodel, file->sname) == IDYES)
      {
        SetFileAttrib(pathbuf, 0, &err);
        
        if (!fsrm(pathbuf, 1))
          *(int*)param = 0;;
      }
    }
    else*/
    {
      if (!fsrm(pathbuf, 1))
        *(int*)param = 0;
    }
    return 1;
  }
  return 0;
}

int M_FileCnt(FILEINF *file, int param)
{
  if (file && pathbuf)
  {
    CurFullPath(file->ws_name);
    *(int*)param += GetFilesCnt(pathbuf);
    return 1;
  }
  return 0;
}

void S_Delit(void)
{
  if (_CurIndex < 0) return;
  Busy = 1;
  int res = 1;
  
  initprogr(ind_deling);
  EnumSel(M_FileCnt, (int)&progr_max);
  incprogr(0);
  
  // Store current file index
  int ind = _CurIndex;
  EnumSel(M_Delit, (int)&res);
  
  if (!res)
  {
    MsgBoxError(muitxt(ind_err_delete));
  }
  else if (ind < _CurTab->ccFiles) // ������ ���� ���� ���� ���� ����
  {
    // List is not refreshed yet, so move index to the next file if there are no errors
    SetCurTabIndex(ind + 1, 0);
  }
  DoRefresh();
  
  endprogr();
  Busy = 0;
}

int M_MoveCopy(FILEINF *file, int param)
{
  if (file && wstrlen(file->ws_name))
  {
    int pname = wstrlen(_CurPath) + 1;
    CurFullPath(file->ws_name);
    fn_add(&buffer, param, file->ftype, pname, pathbuf, IsInArchive() ? _CurTab->zipInfo->szZipPath : NULL);
    return 1;
  }
  return 0;
}

//Buffer
FN_LIST buffer;


void S_Paste(void)
{
  {
    int res=1;
    FN_ITM* itm = buffer.items;
    FN_ITM* last_itm = itm;
    while(itm)
    {
      last_itm = itm;
      itm = (FN_ITM *) itm->next;
    }
    
    itm = buffer.items;
    ArchiveBufferExtractBegin();
    while(itm && !progr_stop)
    {
      if (itm->ftype == TYPE_COMMON_DIR || itm->ftype == TYPE_COMMON_FILE)
      {
        wchar_t *sname = itm->full + itm->pname;
        snwprintf(pathbuf, MAXELEMS(pathbuf)-1, _ls_ls, _CurPath, sname);
        if (buffer.type == FNT_MOVE)
        {
          progr_act = ind_moveing;
          res &= fsmv(itm->full, pathbuf);
        }
        else if (buffer.type == FNT_COPY)
        {
          progr_act = ind_copying;
          res &= fscp(itm->full, pathbuf, 1);
        }
      }
      else if (itm->ftype == TYPE_ZIP_FILE || itm->ftype == TYPE_ZIP_DIR)
      {
        // ���� ������������ ������ �����������
        if (buffer.type == FNT_COPY)
          res &= (ArchiveBufferExtract(ZIP_ARCH, itm, _CurPath) == UNZ_OK);
      }
      else if (itm->ftype == TYPE_7Z_FILE || itm->ftype == TYPE_7Z_DIR)
      {
        // ���� ������������ ������ �����������
        if (buffer.type == FNT_COPY)
          res &= (ArchiveBufferExtract(_7Z_ARCH, itm, _CurPath) == UNZ_OK);
      }
      itm=(FN_ITM *)itm->next;
    }
    ArchiveBufferExtractEnd();
    
    if (!res)
      MMIPROC(MsgBoxErrorMmi,  (int)muitxt(ind_err_resnok));

    MMIPROC(UpdateAll);
    
    
    //���� ������ ����
    int ind = GetCurTabFileIndex(GetFileName(last_itm->full));
    SetCurTabIndex(ind, 0);
    fn_free(&buffer);
    
    endprogr();
    Busy = 0;
  }
}

void PasteFindFiles(void)
{
  if (buffer.count)
  {
    Busy = 1;
    if (buffer.type == FNT_MOVE) progr_act = ind_moveing;
    else if (buffer.type == FNT_COPY) progr_act = ind_copying;
    initprogr(progr_act);
    FN_ITM* itm = buffer.items;
    while(itm)
    {
      //	 progr_max+=itm->full[0]!=_CurPath[0] || buffer.type!=FNT_MOVE?GetFilesCnt(itm->full):1;
      if (itm->ftype == TYPE_COMMON_DIR || itm->ftype == TYPE_COMMON_FILE)
        progr_max += GetFilesCnt(itm->full);
      else
        progr_max++; // ��� ���� ���� ��������
      itm = (FN_ITM *) itm->next;
    }
    incprogr(0);
    S_Paste();
  }
  else
    MsgBoxError(muitxt(ind_err_nofiles));
}


