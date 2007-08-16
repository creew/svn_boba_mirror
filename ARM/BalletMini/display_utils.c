#include "../inc/swilib.h"
#include "view.h"
#include "display_utils.h"
#include "siemens_unicode.h"

unsigned int SearchNextDisplayLine(VIEWDATA *vd, LINECACHE *p, unsigned int *max_h)
{
  int left=ScreenW();
  int c;
  int h;
  unsigned int pos=p->pos;
  while(pos<vd->rawtext_size)
  {
    c=vd->rawtext[pos++];
    if ((c&0xFF00)==0xE100)
    {
      h=GetImgHeight(GetPicNByUnicodeSymbol(c));
    }
    else
    {
      h=GetFontYSIZE(p->bold?FONT_SMALL_BOLD:FONT_SMALL);
    }
    if (max_h)
    {
      if (h>*max_h) *max_h=h;
    }
    switch(c)
    {
    case 0x0A:
    case 0x0D:
      return(pos);
    case UTF16_DIS_INVERT:
      p->ref=0;
      continue;
    case UTF16_ENA_INVERT:
      p->ref=1;
      continue;
    case UTF16_DIS_UNDERLINE:
      p->underline=0;
      continue;
    case UTF16_ENA_UNDERLINE:
      p->underline=1;
      continue;
    case UTF16_FONT_SMALL:
      p->bold=0;
      continue;
    case UTF16_FONT_SMALL_BOLD:
      p->bold=1;
      continue;
    case UTF16_INK_RGBA:
      if (pos>(vd->rawtext_size-2)) goto LERR;
      p->ink1=vd->rawtext[pos++];
      p->ink2=vd->rawtext[pos++];
      continue;
    case UTF16_PAPER_RGBA:
      if (pos>(vd->rawtext_size-2)) goto LERR;
      p->paper1=vd->rawtext[pos++];
      p->paper2=vd->rawtext[pos++];
      continue;
    case UTF16_INK_INDEX:
    case UTF16_PAPER_INDEX:
      pos++;
      if (pos>=vd->rawtext_size) goto LERR;
      continue;
    }
    left-=GetSymbolWidth(c,p->bold?FONT_SMALL_BOLD:FONT_SMALL);
    if (left<0)
    {
      return pos-1;
    }
  }
LERR:
  return(vd->rawtext_size);
}

#define LINESCACHECHUNK 256

static void AddLineToCache(VIEWDATA *vd, LINECACHE *p)
{
  if ((vd->lines_cache_size%LINESCACHECHUNK)==0)
  {
    //����� �� ����� �����, ������������ ��� �����
    vd->lines_cache=realloc(vd->lines_cache,(vd->lines_cache_size+LINESCACHECHUNK)*sizeof(LINECACHE));
  }
  memcpy(vd->lines_cache+(vd->lines_cache_size++),p,sizeof(LINECACHE));
}

int LineDown(VIEWDATA *vd)
{
  LINECACHE lc;
  unsigned int h;
  unsigned int pos=vd->view_pos;
  if (pos>=vd->rawtext_size) return 0;
  if (vd->view_line>=vd->lines_cache_size)
  {
    lc.ink1=0x0000;
    lc.ink2=0x0064;
    lc.paper1=0xFFFF;
    lc.paper2=0xFF64;
    lc.pixheight=0;
    lc.bold=0;
    lc.underline=0;
    lc.ref=0;
    lc.pos=pos;
    AddLineToCache(vd,&lc);
  }
  else
  {
    memcpy(&lc,vd->lines_cache+vd->view_line,sizeof(lc));
  }
  h=0;
  pos=SearchNextDisplayLine(vd,&lc,&h);
  (vd->lines_cache+vd->view_line)->pixheight=h;
  
  if (pos>=vd->rawtext_size) return 0;
  vd->view_pos=pos;
  lc.pos=pos;
  vd->view_line++;
  if (vd->view_line>=vd->lines_cache_size)
  {
    AddLineToCache(vd,&lc);
  }
  return 1;
}

int LineUp(VIEWDATA *vd)
{
  int vl=vd->view_line;
  if (vl)
  {
    vl--;
    vd->view_pos=vd->lines_cache[vl].pos;
    vd->view_line=vl;
    return 1;
  }
  else
    return 0;
}

int RenderPage(VIEWDATA *vd, int do_draw)
{
  int scr_w=ScreenW()-1;
  int scr_h=ScreenH()-1;
  int sc;
  int dc;
//  VIEWDATA *vd=data->vd;
  WSHDR *ws=vd->ws;
  LINECACHE *lc;
  unsigned int vl;
  int ypos=YDISP;
  unsigned int store_pos=vd->view_pos;
  unsigned int store_line=vl=vd->view_line;
  unsigned int len;
  unsigned int y2;
  
  char def_ink[4];
  char def_paper[4];
  
  int c;
  int ena_ref=0;
  
  int result=1;

  unsigned int _ref=0xFFFFFFFF;
  unsigned int flag=0;
  
  vd->pos_first_ref=0xFFFFFFFF;
  vd->pos_last_ref=0xFFFFFFFF;
  vd->pos_prev_ref=0xFFFFFFFF;
  vd->pos_next_ref=0xFFFFFFFF;
  
  while(ypos<scr_h)
  {
    if (LineDown(vd))
    {
      lc=vd->lines_cache+vl;
      dc=0;
      if ((vl+1)<vd->lines_cache_size)
      {
	len=(lc[1]).pos-(lc[0]).pos;
      }
      else
	len=vd->rawtext_size-lc->pos;
      sc=lc->pos;
      if (ena_ref) ws->wsbody[++dc]=UTF16_ENA_INVERT;
      while(len&&(dc<32000))
      {
	c=vd->rawtext[sc];
	if (c==UTF16_ENA_INVERT)
	{
	  //Found begin of ref
	  _ref=sc;
	  if (vd->pos_first_ref==0xFFFFFFFF)
	  {
	    vd->pos_first_ref=sc;
	    vd->pos_last_ref=sc;
	  }
	  if (vd->pos_cur_ref!=sc)
	  {
	    if (flag==0) vd->pos_prev_ref=sc;
	    if (flag==1)
	    {
	      flag=2;
	    }
	    goto L1;
	  }
	  flag=1;
	  ena_ref=1;
	}
	if (c==UTF16_DIS_INVERT)
	{
//	  if ((scr_h-ypos)>lc->pixheight)
//	  {
//	    vd->pos_botview_ref=prepare_bot_ref;
//	  }
	  if (flag==2)
	  {
	    if ((scr_h-ypos)>lc->pixheight)
	    {
	      vd->pos_next_ref=_ref;
	    }
	    flag=3;
	  }
	  if ((scr_h-ypos)>lc->pixheight)
	  {
	    if (vd->pos_first_ref==0xFFFFFFFF) vd->pos_first_ref=_ref;
	    vd->pos_last_ref=_ref;
	  }
	  if (!ena_ref) goto L1;
	  ena_ref=0;
	}
	ws->wsbody[++dc]=c;
      L1:
	sc++;
	len--;
      }
//      ws->wsbody[++dc]=lc->bold?UTF16_FONT_SMALL_BOLD:UTF16_FONT_SMALL;
//      ws->wsbody[++dc]=lc->underline?UTF16_ENA_UNDERLINE:UTF16_DIS_UNDERLINE;
      ws->wsbody[0]=dc;
      y2=lc->pixheight+ypos-1;
      if (do_draw)
      {
	def_ink[0]=lc->ink1>>8;
	def_ink[1]=lc->ink1;
	def_ink[2]=lc->ink2>>8;
	def_ink[3]=lc->ink2;
	def_paper[0]=lc->paper1>>8;
	def_paper[1]=lc->paper1;
	def_paper[2]=lc->paper2>>8;
	def_paper[3]=lc->paper2;
	DrawRectangle(0,ypos,scr_w,y2,
		      RECT_FILL_WITH_PEN,def_paper,def_paper);
	DrawString(ws,0,ypos,scr_w,lc->pixheight+ypos-1,
		   lc->bold?FONT_SMALL_BOLD:FONT_SMALL,TEXT_NOFORMAT
		     +(lc->underline?TEXT_UNDERLINE:0)
		       ,def_ink,def_paper);
      }
      ypos+=lc->pixheight;
      vl++;
    }
    else
    {
      result=0;
//      if (ena_ref) vd->pos_botview_ref=prepare_bot_ref;
      break;
    }
  }
  vd->view_pos=store_pos;
  vd->view_line=store_line;
  return(result);
}