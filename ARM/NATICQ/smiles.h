typedef struct
{
  void *next;
  unsigned int key; //��� ������� ��������
  unsigned int mask;
  char text[16];
}STXT_SMILES;

typedef struct
{
  void *next;
  unsigned int uni_smile;
  STXT_SMILES *lines; //������ �����
  STXT_SMILES *botlines;
}S_SMILES;

void FreeSmiles(void);
void InitSmiles(void);
void ProcessNextSmile(void);

void FreeXStatusesImg(void);
void InitXStatusesImg(void);
void ProcessNextXStatImg(void);

S_SMILES *FindSmileById(int n);
S_SMILES *FindSmileByUni(int wchar);






