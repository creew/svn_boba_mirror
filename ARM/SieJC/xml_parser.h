#ifndef _XML_PARSER_
  #define _XML_PARSER

typedef struct{
  void *next;
  char *name;
  char *param;
}XMLAttr;


typedef struct{
  void *next;
  void *subnode;
  char *name;
  XMLAttr *attr;
  char *value;
} XMLNode;


#define MS_BEGIN 1						///< ������ ��������
#define MS_BEGINTAG 2					///< ������� ���
#define MS_MIDDLETAG 3					///< �������� ����, �� ��� �� �����
#define MS_TAGNAME 4						///< ������ ��� ����
#define MS_ENDTAGNAME 5					///< ��������� ������� ��� ����
#define MS_ATTRIBNAME 6					///< ������ ��� ���������
#define MS_ENDATTRIBNAME 7				///< ��������� ��� ���������
#define MS_ENDEQUALLY 8					///< ������ ���� = ����� ������ ��������� � ���������
#define MS_ATTRIBVALUE 9					///< ������ �������� ��������� (����� ")
#define MS_ENDTAG 10						///< ����� ���� (��� ������������������ ����� /)
#define MS_TEXT 11						///< �����


#define TS_INDEFINITE 1					///< ��������������, �.�. ��� ������ ������ ������� (���� ������)
#define TS_NORMAL 2						///< ������� (��)�������������. ����� �� ����������
#define TS_CLOSE 3						///< �������������
#define TS_EMPTY 4						///< ��� � �����������
#define TS_DECLARATION 5					///< ����������, ��� ��� XML. ������ ��������� ����������

void* XMLDecode(char *buf, int size);
void SaveTree(XMLNode *tree);
void DestroyTree(XMLNode *tree);

char* XML_Get_Attr_Value(char* req_attr_name, XMLAttr* attr_list);
XMLNode* XML_Get_Child_Node_By_Name(XMLNode* node, char* req_node_name);
#endif
