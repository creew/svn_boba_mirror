#include "include.h"
#include "http.h"
#include "log.h"

/*******************************************************************************
 HTTP_Request
*******************************************************************************/

HTTP_Request::HTTP_Request()
{
  Path = NULL;
  Accept = NULL;
  Accept_Charset = NULL;
  Accept_Encoding = NULL;
  Accept_Language = NULL;
  Authorization = NULL;
  Expect = NULL;
  From = NULL;
  Host = NULL;
  If_Match = NULL;
  If_Modified_Since = NULL;
  If_None_Match = NULL;
  If_Range = NULL;
  If_Unmodified_Since = NULL;
  Max_Forwards = NULL;
  Proxy_Authorization = NULL;
  Range = NULL;
  Referer = NULL;
  TE = NULL;
  User_Agent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)";
}

HTTP_Request::~HTTP_Request()
{
  if (Path) delete Path;
  if (Accept) delete Accept;
  if (Accept_Charset) delete Accept_Charset;
  if (Accept_Encoding) delete Accept_Encoding;
  if (Accept_Language) delete Accept_Language;
  if (Authorization) delete Authorization;
  if (Expect) delete Expect;
  if (From) delete From;
  if (Host) delete Host;
  if (If_Match) delete If_Match;
  if (If_Modified_Since) delete If_Modified_Since;
  if (If_None_Match) delete If_None_Match;
  if (If_Range) delete If_Range;
  if (If_Unmodified_Since) delete If_Unmodified_Since;
  if (Max_Forwards) delete Max_Forwards;
  if (Proxy_Authorization) delete Proxy_Authorization;
  if (Range) delete Range;
  if (Referer) delete Referer;
  if (TE) delete TE;
  //if (User_Agent) delete User_Agent;
}

/*******************************************************************************
 HTTP_Response
*******************************************************************************/
/*
  ������ HTTP ���������
  ���������� ����� ����� ��������� ��� 0, ���� ��������� �� ������
                                                                  */
int HTTP_Response::Parse(char * buf, int maxlen)
{
  _safe_delete(headers); // ����� ��� ������ ���������, ��� ����� ��� ���������� ��� ���
  headers = new HTTP_Response_Headers(log); // ������� ����� ��������� ����������

  int l_len = 0;    // ����� ������� ������
  char l_buf[512];  // ����� ������� ������
  int n_CR = 0;     // ����� CR � ����� ������
  int n_LF = 0;     // ����� LF � ����� ������
  int hlen = 0;     // ����� ����� ���������
  
  /* ������ ������ ������� */
  while (!(buf[l_len]=='\n' || buf[l_len]=='\r') && (l_len<maxlen)) // ������� ����� ������
    l_len++;
  strncpy(l_buf, buf, l_len); // �������� ������ � �����
  l_buf[l_len] = 0;
  if (sscanf(l_buf, "HTTP/%d.%d %d", &http_ver_major, &http_ver_minor, &resp_code)<3) // ��������� �� ������ ������� ������ HTTP � ��� ������
    return 0;
  if (strlen(l_buf)>13) // ���� ���� ��������� �������
  {
    _safe_delete(resp_msg); // ������� ������
    resp_msg = new char[strlen(l_buf)-13+1]; // ������� �����
    strcpy(resp_msg, l_buf+13); // �������� �� ������
  }
  while(buf[l_len]=='\n' || buf[l_len]=='\r') // ���������� CR � LF
  {
    if (buf[l_len]=='\r') n_CR++; // ������� ��� CR � ����� ������
    if (buf[l_len]=='\n') n_LF++; // ������� ��� LF � ����� ������
    l_len++;
  }
  buf += l_len; // ��������� � ��������� ������
  maxlen -= l_len;
  hlen += l_len;
  
  /* ��� ����������� ������ */
  do
  {
    l_len = 0;
    n_CR = 0;
    n_LF = 0;
    
    while (!(buf[l_len]=='\r' || buf[l_len]=='\n') && (l_len<maxlen)) // ������� ����� ������
      l_len++;
    
    strncpy(l_buf, buf, l_len); // �������� ������ � �����
    l_buf[l_len] = 0;
    
    while(buf[l_len]=='\n' || buf[l_len]=='\r') // ���������� CR � LF
    {
      if (buf[l_len]=='\r') n_CR++; // ������� ��� CR � ����� ������
      if (buf[l_len]=='\n') n_LF++; // ������� ��� LF � ����� ������
      l_len++;
    }
    
    if (!headers->Add(l_buf)) return 0;
    
    buf += l_len; // ��������� � ��������� ������
    maxlen -= l_len;
    hlen += l_len;
    if (n_CR == 2 || n_LF == 2) break; // ����� ���������. � �������� ��������� �� \r\n\r\n, �� ������ � ���������� ;)
  } while (maxlen);
  return hlen;
}

HTTP_Response::HTTP_Response(Log * _log)
{
  log = _log;
  headers = NULL;
  resp_msg = NULL;
}

HTTP_Response::~HTTP_Response()
{
  _safe_delete(resp_msg);
  _safe_delete(headers);
}
