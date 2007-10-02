#include "../inc/swilib.h"
#include "clist_util.h"
#include "xml_parser.h"
#include "base64.h"

// ��������� �������, ��������� � ��������� ��������� 
// ���� ������
unsigned int result_len = 0;
char *Add_vCard_Value(char *dest, char *par_name, char *val)
{
  char *param_pair = malloc(1024);
  zeromem(param_pair, 1024);
  snprintf(param_pair, 1023, "%s: %s\n", par_name, val);  // binary-safe
  unsigned int now_dest_len, parpair_len;
  now_dest_len = strlen(dest);
  parpair_len = strlen(param_pair);
  while(now_dest_len+parpair_len>result_len)
  {
    result_len += 1024;
    dest = realloc(dest, result_len);
  }
  strcat(dest,param_pair);
  mfree(param_pair);
  return dest;
}

extern const char DEFAULT_DISC[128];

// ���������� ����������
void SavePhoto(XMLNode *photonode)
{
  // Prepare path
  char ph_path[]="4:\\Zbin\\var\\UserTmpAvatar."; // ����������, � ����
  char extension[]="jpg";
  char Saved_OK = 0;
  ph_path[0] = DEFAULT_DISC[0];
  XMLNode *ph_node = XML_Get_Child_Node_By_Name(photonode, "TYPE");
  if(ph_node)
  {
    char *ph_node_val = ph_node->value;
    if(!strcmp(ph_node_val, "image/jpeg"))
    {
      strcpy(extension,"jpg");
    }
    else if(!strcmp(ph_node_val, "image/png"))
    {
      strcpy(extension,"png");
    }
    else if(!strcmp(ph_node_val, "image/gif"))
    {
      strcpy(extension,"gif");
    }
  }
  char *full_path = malloc(128);
  strcpy(full_path, ph_path);  
  strcat(full_path, extension);
  
  // Decode & write
  XMLNode *binval = XML_Get_Child_Node_By_Name(photonode, "BINVAL");
  int ln = strlen(binval->value);
  char *buf = malloc(ln);
  int binlen = base64_decode(binval->value, buf);
  unsigned int ec = 0;
  unlink(full_path, &ec);
  ec=0;   // ������, ��� ���������� ��������.
  volatile int f = fopen(full_path, A_ReadWrite +A_Create+ A_Append + A_BIN, P_READ+ P_WRITE, &ec);
  if(!ec)
  {
    fwrite(f, buf, binlen, &ec);
    fclose(f, &ec);
    Saved_OK = 1;
  }
  else MsgBoxError(1,(int)full_path);

  // Display
  if(Saved_OK)
  {
    WSHDR *fp = AllocWS(128);
    str_2ws(fp,full_path,128);
    ExecuteFile(fp, NULL, NULL);
    FreeWS(fp);
  }
  
  // Cleanup
  mfree(buf);  
  mfree(full_path);
}

// ���������� vCard
void Process_vCard(char *from, XMLNode *vCard)
{
  result_len = 1024;
  char* result=malloc(result_len);
  strcpy(result, "vCard:\n");  
  // ������� ��� ���������� ���� vCard
  XMLNode* vCard_Node = vCard->subnode;
  while(vCard_Node)
  {
    if(vCard_Node->name && vCard_Node->value) // ���� �� �������� Photo, � ����� ����
      result=Add_vCard_Value(result, vCard_Node->name, vCard_Node->value);
    vCard_Node = vCard_Node->next;
  }    
  //��������� ���������
  CList_AddMessage(from, MSG_SYSTEM, result);
  mfree(result);
  
  // Save photo :))
  XMLNode *photo = XML_Get_Child_Node_By_Name(vCard,"PHOTO");
  if(photo)SavePhoto(photo);
}
//EOL,EOF
