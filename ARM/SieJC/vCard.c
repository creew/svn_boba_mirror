#include "../inc/swilib.h"
#include "main.h"
#include "clist_util.h"
#include "xml_parser.h"
#include "base64.h"
#include "siejc_ipc.h"
#include "history.h"
#include "lang.h"
#include "string_util.h"
#include "vCard.h"

char *vcard_index[N_VCARD_FIELDS] =
{
  "JABBERID",
  "FN",
  "NICKNAME",
  "BDAY",
  "GENDER",

  "ORG/ORGNAME",
  "ORG/ORGUNIT",
  "TITLE",
  "ROLE",

  "EMAIL/USERID",
  "URL",

  "DESC"
};

char *vcard_names[N_VCARD_FIELDS] =
{
  LG_VCARD_JABBERID,
  LG_VCARD_FN,
  LG_VCARD_NICKNAME,
  LG_VCARD_BDAY,
  LG_VCARD_GENDER,

  LG_VCARD_ORGNAME,
  LG_VCARD_ORGUNIT,
  LG_VCARD_TITLE,
  LG_VCARD_ROLE,

  LG_VCARD_EMAIL,
  LG_VCARD_URL,

  LG_VCARD_DESC
};

// ��������� �������, ��������� ��������� vCard
void Add_vCard_Value(VCARD dest, char *par_name, char *val)
{
  int field_id = 0;
  while (field_id<N_VCARD_FIELDS && strcmp(par_name, vcard_index[field_id]))
    field_id++;
  if (field_id<N_VCARD_FIELDS)
  {
    if (!dest[field_id])
      dest[field_id] = malloc(strlen(val)+1);
    strcpy(dest[field_id], val);
  }
}

char *Get_VCard_Value(VCARD src, char *par_name)
{
  int field_id = 0;
  while (field_id<N_VCARD_FIELDS && strcmp(par_name, vcard_index[field_id]))
    field_id++;
  if (field_id<N_VCARD_FIELDS)
  {
    return src[field_id];
  }
  return NULL;
}

VCARD Create_vCard()
{
  VCARD tmp = malloc(4*N_VCARD_FIELDS);
  zeromem(tmp, 4*N_VCARD_FIELDS);
  return tmp;
}

void Free_vCard(VCARD vcard)
{
  if (!vcard)
    return;
  for (int i=0; i<N_VCARD_FIELDS; i++)
    if (vcard[i])
      mfree(vcard[i]);
  mfree(vcard);
}

extern const char DEFAULT_DISC[128];
extern const char ipc_my_name[32];

void vCard_Photo_Display(char *path)
{
  WSHDR *fp = AllocWS(128);
  str_2ws(fp,path,128);
  ExecuteFile(fp, NULL, NULL);
  FreeWS(fp);
  mfree(path);
}

static IPC_REQ vcard_ipc;

//Context:HELPER
void DecodePhoto(char *path, void *data)
{
  char *buf = malloc(strlen(data));
  int binlen = base64_decode(data, buf);
  char Saved_OK = 0;
  unsigned int ec = 0;
  unlink(path, &ec);
  ec=0;   // ������, ��� ���������� ��������.
  volatile int f = fopen(path, A_ReadWrite +A_Create+ A_Append + A_BIN, P_READ+ P_WRITE, &ec);
  if(!ec)
  {
    fwrite(f, buf, binlen, &ec);
    fclose(f, &ec);
    Saved_OK = 1;
  }
  else
  {
    LockSched();
    MsgBoxError(1,(int)path);
    UnlockSched();
  }

  // Display
  if(Saved_OK)
  {
    //WSHDR *fp = AllocWS(128);
    //str_2ws(fp,full_path,128);
    //ExecuteFile(fp, NULL, NULL);
    //FreeWS(fp);
    char *fp = malloc(128);
    strcpy(fp, path);

    vcard_ipc.name_to=ipc_my_name;
    vcard_ipc.name_from=ipc_my_name;
    vcard_ipc.data=fp;
    GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_AVATAR_DECODE_OK,&vcard_ipc);
  }

  // Cleanup
  mfree(buf);
  mfree(data);
  mfree(path);
}

// ���������� ����������
void SavePhoto(XMLNode *photonode)
{
  // Prepare path
  char ph_path[]="4:\\Zbin\\var\\UserTmpAvatar."; // ����������, � ����
  char extension[]="jpg";
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
  memcpy(buf, binval->value, ln);
  SUBPROC((void*)DecodePhoto, full_path, buf);
}

// ���������� vCard
void Process_vCard(char *from, XMLNode *vCard)
{
  int Use_Resource = 0;
  TRESOURCE *ResEx = CList_IsResourceInList(from);
  if (ResEx->entry_type==T_CONF_NODE) // ���� ����������� - ���������� ������
    Use_Resource = 1;
  CLIST *ClEx = CList_FindContactByJID(from);

  VCARD vcard;
  if (Use_Resource)
    vcard = ResEx->vcard;
  else
    vcard = ClEx->vcard;
  if (!vcard)
    vcard = Create_vCard();

  // ������� ��� ���������� ���� vCard
  XMLNode* vCard_Node = vCard->subnode;
  while(vCard_Node)
  {
    if(vCard_Node->name && vCard_Node->value) // ���� �� �������� Photo, � ����� ����
      Add_vCard_Value(vcard, vCard_Node->name, vCard_Node->value);
    vCard_Node = vCard_Node->next;
  }

  // Save photo :))
  XMLNode *photo = XML_Get_Child_Node_By_Name(vCard,"PHOTO");
  if(photo)
    if(photo->subnode)SavePhoto(photo);

  // ���������� vCard

  if (Use_Resource)
    ResEx->vcard = vcard;
  else
    ClEx->vcard = vcard;

  Show_vCard(from);
}



int Show_vCard2(char *jid)
{
  CLIST *ClEx = CList_FindContactByJID(jid);
  if (!ClEx)
    return 0;
  VCARD vcard = ClEx->vcard;
  if (!vcard) // ���� �� ����� - ���� � ������� (��� �����������)
  {
    TRESOURCE *ResEx = CList_IsResourceInList(jid);
    if (ResEx)
      vcard = ResEx->vcard;
  }
  if (vcard)
  {
    char *result = malloc(1024);
    int result_len = 1024;
    strcpy(result, "VCard:\n");
    char *param_pair = malloc(2048);
    for (int i=0; i<N_VCARD_FIELDS; i++)
      if (vcard[i])
      {
        char *parname = ANSI2UTF8(vcard_names[i], 1024);
        snprintf(param_pair, 2048, "%s: %s\n", parname+1 /* +1 - ��� ����� ���, �� �������� */, vcard[i]);
        mfree(parname);
        unsigned int real_result_len, parpair_len;
        real_result_len = strlen(result);
        parpair_len = strlen(param_pair);
        while(real_result_len+parpair_len>result_len)
        {
          result_len += 1024;
          result = realloc(result, result_len);
        }
        strcat(result,param_pair);
      }
    CList_AddMessage(jid, MSG_SYSTEM, result);
    mfree(param_pair);
    mfree(result);
    return 1;
  }
  return 0;
}

//====================   GUI   ========================

int vcinfo_onkey(GUI *gui, GUI_MSG *msg)
{
  if (msg->keys==0x18)
  {
    return(-1); //do redraw
  }
  return(0); //Do standart keys
}

void vcinfo_ghook(GUI *gui, int cmd)
{
  if (cmd==2)
  {
    //Called after onCreate
  }

  if (cmd==7)
  {

  }

  if(cmd==0xA)
  {
    DisableIDLETMR();   // ��������� ������ ������ �� ��������
  }

  if(cmd==0x03)     // onDestroy
  {
  }
}

extern void inp_locret(void);

SOFTKEY_DESC vcinfo_sk[]=
{
  {0x0018,0x0000,(int)LG_UPDATE},
  {0x0001,0x0000,(int)LG_BACK},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB vcinfo_skt=
{
  vcinfo_sk,0
};

INPUTDIA_DESC vcinfo_desc=
{
  1,
  vcinfo_onkey,
  vcinfo_ghook,
  (void *)inp_locret,
  0,
  &vcinfo_skt,
  {0,0,0,0},
  4,
  100,
  101,
  0,
  0,
  0x40000000// �������� ����-������
};

HEADER_DESC vcinfo_hdr={0,0,0,0,NULL,(int)"vCard",LGP_NULL};

extern const char percent_t[];
extern const char percent_s[];

int Show_vCard(char *jid)
{
#define MAX_VCARD_LINE_LEN 1024
  CLIST *ClEx = CList_FindContactByJID(jid);
  if (!ClEx)
    return 0;
  VCARD vcard = ClEx->vcard;
  if (!vcard) // ���� �� ����� - ���� � ������� (��� �����������)
  {
    TRESOURCE *ResEx = CList_IsResourceInList(jid);
    if (ResEx)
      vcard = ResEx->vcard;
  }
  if (vcard)
  {
    WSHDR* ws_info = AllocWS(MAX_VCARD_LINE_LEN);

    EDITCONTROL ec;
    void *ma=malloc_adr();
    void *eq;
    PrepareEditControl(&ec);
    eq=AllocEQueue(ma,mfree_adr());

    for (int i=0; i<N_VCARD_FIELDS; i++)
    {
      if (vcard[i])
      {
        wsprintf(ws_info, percent_t, vcard_names[i]);
        ConstructEditControl(&ec, ECT_HEADER, ECF_APPEND_EOL, ws_info, MAX_VCARD_LINE_LEN);
        AddEditControlToEditQend(eq,&ec,ma);
        utf8_2ws(ws_info, vcard[i], MAX_VCARD_LINE_LEN);
        ConstructEditControl(&ec, ECT_NORMAL_TEXT, ECF_APPEND_EOL, ws_info, MAX_VCARD_LINE_LEN);
        AddEditControlToEditQend(eq,&ec,ma);
      }
    }

    wsprintf(ws_info, percent_t, LG_UPDATE);
    ConstructEditControl(&ec, ECT_LINK, ECF_APPEND_EOL, ws_info, MAX_VCARD_LINE_LEN);
    AddEditControlToEditQend(eq,&ec,ma);

    patch_header(&vcinfo_hdr);
    patch_input(&vcinfo_desc);
    CreateInputTextDialog(&vcinfo_desc,&vcinfo_hdr,eq,1,0);

    FreeWS(ws_info);
    return 1;
  }
  return 0;
}

//EOL,EOF
