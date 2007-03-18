#include "..\inc\swilib.h"
#include <csetjmp>

#define number 8

#ifdef NEWSGOLD
#define PNG_16_SUPPORT
#endif

void* xmalloc(int x,int n)
{
  return malloc(n);
}

void xmfree(int x,void* ptr)
{
  mfree(ptr);
}

__arm void read_data_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
  unsigned int err;
  int* f;
  f=png_get_io_ptr(png_ptr);
  fread(*f, data, length, &err);
}

__arm IMGHDR* create_imghdr(const char* fname)
{
  int f;
  char buf[number];
  unsigned int err;
  struct PP
  {
    char *row;
    char *img;
    IMGHDR * img_h;
  } *pp;
  IMGHDR * img_hc;
  png_structp png_ptr=NULL;
  png_infop info_ptr=NULL;
  png_uint_32 rowbytes;
  
  if ((f=fopen(fname, A_ReadOnly+A_BIN, P_READ, &err))==-1) return 0;
  pp=malloc(sizeof(struct PP));
  pp->row=NULL;
  pp->img=NULL;
  pp->img_h=NULL;
  
  if (fread(f, &buf, number, &err)!=number) goto L_CLOSE_FILE;
  if  (!png_check_sig((png_bytep)buf,number)) goto  L_CLOSE_FILE;
  
  png_ptr = png_create_read_struct_2("1.2.5", (png_voidp)0, 0, 0, (png_voidp)0,(png_malloc_ptr)xmalloc,(png_free_ptr)xmfree);
  if (!png_ptr) goto L_CLOSE_FILE;
  
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
  L_CLOSE_FILE:
    mfree(pp->row);
    mfree(pp->img);
    mfree(pp->img_h);
    mfree(pp);
    fclose(f, &err);
    return NULL;
  }
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    goto L_CLOSE_FILE;
  }
  
  png_set_read_fn(png_ptr, &f, read_data_fn);
  
  png_set_sig_bytes(png_ptr, number);
  
  png_read_info(png_ptr, info_ptr);
  
  png_uint_32 width, height;
  int bit_depth, color_type;
  
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);
  
  if (bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
    
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png_ptr); 
  
  if (bit_depth == 16) png_set_strip_16(png_ptr);
  
  if (bit_depth < 8) png_set_packing(png_ptr);
  
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png_ptr);
  
  if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);
    
  if (color_type != PNG_COLOR_TYPE_GRAY)
    png_set_filler(png_ptr,0xFF,PNG_FILLER_AFTER);
  
  if (color_type == PNG_COLOR_TYPE_GRAY)  png_set_invert_mono(png_ptr);
  
  png_read_update_info(png_ptr, info_ptr);
  
  rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  
  pp->row=malloc(rowbytes);
  pp->img_h=img_hc=malloc(sizeof(IMGHDR));
  if (color_type != PNG_COLOR_TYPE_GRAY)
#ifdef PNG_16_SUPPORT
  {
    unsigned short *iimg=(unsigned short *)(pp->img=malloc(width*height*2));
    for (unsigned int y = 0; y<height; y++)
    {
      png_read_row(png_ptr, (png_bytep)pp->row, NULL);
      for (unsigned int x = 0; x<width; x++)
      {
	if (!pp->row[x*4+3])
	  *iimg++=0xE000;
	else
	{
	  unsigned int c=((pp->row[x*4+0]<<8)&0xF800);
	  c|=((pp->row[x*4+1]<<3)&0x7E0);
	  c|=((pp->row[x*4+2]>>3)&0x1F);
	  *iimg++=c;
	}
      }
    }
    pp->img_h->bpnum=8;
  }
#else
  {
    unsigned char *iimg=(unsigned char *)(pp->img=malloc(width*height));
    for (unsigned int y = 0; y<height; y++)
    {
      png_read_row(png_ptr, (png_bytep)pp->row, NULL);
      for (unsigned int x = 0; x<width; x++)
      {
	if (!pp->row[x*4+3])
	  *iimg++=0xC0;
	else
	{
	  char c=(pp->row[x*4+0] & 0xE0);
	  c|=((pp->row[x*4+1]>>3)&0x1C);
	  c|=((pp->row[x*4+2]>>6)&0x3);
	  *iimg++=c;
	}
      }
    }
    pp->img_h->bpnum=5;
  }
#endif
  else
  {
    int rowc_w=width&7?(width>>3)+1:width>>3;
    int size=height*rowc_w;
    unsigned char *iimg=(unsigned char *)(pp->img=malloc(size));
    zeromem(iimg,size);
    for (unsigned int y = 0; y<height; y++)
    {
      png_read_row(png_ptr, (png_bytep)pp->row, NULL);
      for (unsigned int x = 0; x<width; x++)
      {
        if (pp->row[x])
          iimg[x>>3]|=(0x80>>(x&7));
      }
      iimg+=rowc_w;
    }
    pp->img_h->bpnum=1;
  } 
  pp->img_h->w=width;
  pp->img_h->h=height;
  //pp->img_h->zero=0;
  pp->img_h->bitmap=pp->img;
  
  
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
  mfree(pp->row);
  mfree(pp);
  fclose(f, &err);
  return (img_hc);
}


#ifdef NEWSGOLD
#define DEFAULT_FOLDER "4:\\ZBin\\img\\"
#else
#define DEFAULT_FOLDER "0:\\ZBin\\img\\"
#endif
#define CACHE_PNG 50

typedef struct
{
  void *next;
  char *pngname;
  IMGHDR * img;
}PNGLIST;

volatile __no_init PNGLIST * pltop;

#pragma inline
int tolower(int C)
{
  if ((C>='A' && C<='Z')) C-='A'-'a';
  return(C);
}

char* strcpy_tolow(char *s1,const char *s2)
{
  while(*s2)
  {
    *s1++=tolower(*s2++);   
  }
  *s1=0;
  return s1;
}

__arm unsigned int div10(unsigned int v, unsigned int divisor, char *s)
{
  unsigned int c='0';
  while(v>=divisor)
  {
    v-=divisor;
    c++;
  }
  *s=c;
  return v;
}

__arm void print10(char *s, unsigned int v)
{
  if (v>=10)
  {
    if (v>=100)
    {
      if (v>=1000)
      {
	if (v>=10000)
	{
	  v=div10(v,10000,s++);
	}
	v=div10(v,1000,s++);
      }
      v=div10(v,100,s++);
    }
    v=div10(v,10,s++);
  }
  *s++=v+'0';
  *s++='.';
  *s++='p';
  *s++='n';
  *s++='g';
  *s='\0';
}


__arm IMGHDR* PatchGetPIT(unsigned int pic)
{
  IMGHDR * img;
  unsigned int i;
  char fname[256];
  
  PNGLIST *pl;
  PNGLIST *pl_prev;
  PNGLIST *cur;
  
  if ((pic>>28)==0xA)
  {
    strcpy_tolow(fname,(char*)pic);
  }
  else
  {
    char*next=strcpy_tolow(fname,DEFAULT_FOLDER);
    print10(next,pic);
  }
  LockSched();
  pl=(PNGLIST *)(&pltop);
  pl_prev=NULL;  
  while((pl=pl->next))
  {
    if (!strcmp(pl->pngname,fname))
    {
      //������, ��������� � ������ � �������
      if (pl_prev)
      {
	//������ ���� �� � ����� ������
	pl_prev->next=pl->next; //������� �� ��������� �����
	pl->next=(PNGLIST *)pltop; //��������� - ���� ������
	pltop=pl; //� ������ � ������ - ��������
      }
      UnlockSched();
      return(pl->img);
    }
    pl_prev=pl; //������� ������������ - ������ ����������
  }
  UnlockSched();
  //������ �� �����, ������ ������� ��������
  img=create_imghdr(fname);
  //if (!img) return (0); //������ ���������
  cur=malloc(sizeof(PNGLIST)); //������� ������� ������
  cur->pngname=malloc(strlen(fname)+1);
  strcpy(cur->pngname,fname);
  cur->img=img;
  i=0; //��� ���������� ��������� � ������
  LockSched();
  cur->next=(PNGLIST *)pltop; //��������� - ���� ������
  pltop=cur; //������ � ������ - ����� �������
  //������ ��������� �����
  pl=(PNGLIST *)(&pltop);
  do
  {
    pl_prev=pl;
    pl=pl->next;
    if (!pl)
    {
      //����������� �������� ������
      UnlockSched();
      return(cur->img);
    }
    i++;
  }
  while(i<=CACHE_PNG); //���� ���������� ��������� ������ �����������
  pl_prev->next=NULL; //������� ������
  UnlockSched();
  //��������� ����� ������� � ����������� ����������������
  do
  {
    //������� �������
    if (pl->img)
    { 
      mfree(pl->img->bitmap);
      mfree(pl->img);
    }
    mfree(pl->pngname);
    pl_prev=pl;
    pl=pl->next;
    mfree(pl_prev);
  }
  while(pl); //���� ���� ��������, ����������� ��
  return(cur->img);
}
