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

/*#pragma pack(1)
typedef struct
{
  unsigned long l;
}ULONG_BA;
#pragma pack()*/


void FreeSmiles(void);
void InitSmiles(void);
void ProcessNextSmile(void);

S_SMILES *FindSmileById(int n);
S_SMILES *FindSmileByUni(int wchar);






