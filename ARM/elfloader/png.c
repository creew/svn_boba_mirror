#include "..\inc\swilib.h"
#define number 8



void* xmalloc(int x,int n)
{
  return malloc(n);
}

void xmfree(int x,void* ptr)
{
  mfree(ptr);
}

typedef struct
{
  int file_handler;
  unsigned int errno;
}READ_FILE;


__arm void read_data_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
  READ_FILE*read;
  read=png_get_io_ptr(png_ptr);
  fread(read->file_handler, data, length, &read->errno);
}

__arm IMGHDR* create_imghdr(const char* fname)
{
  READ_FILE read;
  char buf[number];
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

  if ((read.file_handler=fopen(fname, A_ReadOnly+A_BIN, P_READ, &read.errno))==-1) return 0;
  pp=malloc(sizeof(struct PP));
  pp->row=NULL;
  pp->img=NULL;
  pp->img_h=NULL;
  
  if (fread(read.file_handler, &buf, number, &read.errno)!=number) goto L_CLOSE_FILE;
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
    fclose(read.file_handler, &read.errno);
    return NULL;
  }
  
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    goto L_CLOSE_FILE;
  }
  
  png_set_read_fn(png_ptr, &read, read_data_fn);
  
  png_set_sig_bytes(png_ptr, number);
  
  png_read_info(png_ptr, info_ptr);
  
  png_uint_32 width, height;
  int bit_depth, color_type;
  
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);
  
  if (bit_depth == 16) png_set_strip_16(png_ptr);
  
  if (bit_depth < 8) png_set_packing(png_ptr);
  
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png_ptr);
  
  if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);
  
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png_ptr);
  
  png_set_filler(png_ptr,0xFF,PNG_FILLER_AFTER);
  
  
  png_read_update_info(png_ptr, info_ptr);
  
  rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  
  pp->row=malloc(rowbytes);
  pp->img=malloc(width*height);
  
  for (unsigned int y = 0; y<height; y++)
  {
    png_read_row(png_ptr, (png_bytep)pp->row, NULL);
    for (unsigned int x = 0; x<width; x++)
    {
      if (!pp->row[x*4+3])
        pp->img[x+y*width]=0xC0;
      else
      {
	char c=(pp->row[x*4+0] & 0xE0);
	c|=((pp->row[x*4+1]>>3)&0x1C);
	c|=((pp->row[x*4+2]>>6)&0x3);
        pp->img[x+y*width]=c; //(row[x*4+0] & 0xE0)|((row[x*4+1]>>3)&0x1C)|((row[x*4+2]>>6)&0x3);
      }
    }
  }
  pp->img_h=img_hc=malloc(sizeof(IMGHDR));
  pp->img_h->w=width;
  pp->img_h->h=height;
  pp->img_h->bpnum=5;
  //  img_h->zero=0;
  pp->img_h->bitmap=pp->img;
  
  
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
  mfree(pp->row);
  mfree(pp);
  fclose(read.file_handler, &read.errno);
  return (img_hc);
}


#ifdef NEWSGOLD
#define DEFAULT_FOLDER "4:\\ZBin\\img\\"
#else
#define DEFAULT_FOLDER "0:\\zbin\\img\\"
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
    strcat(fname,".png");
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




