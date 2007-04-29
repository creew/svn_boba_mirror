//Context:HELPER
char *Rstream_p; //��������� �� ���������� �����
int Rstream_n; //���������� ����

void get_answer(void)
{
  char rb[1024];
  int i=recv(sock,rb,sizeof(rb),0);
  int j;
  int err;
  char *p;
  int c;
  if (i<0)
  {
    end_socket();
    return;
  }
  if (i==0) return;
  if (Is_Compression_Enabled)
  {
    //���������� ZLib ��� �������� ������ � ���������� �����
    d_stream.next_in  = (Byte*)rb;
    d_stream.avail_in = (uInt)i;
    do
    {
      d_stream.next_out = (Byte*)((Rstream_p=realloc(Rstream_p,Rstream_n+i+1))+Rstream_n); //����� ������ ����������� ������
      d_stream.avail_out = (uInt)i;
      err = inflate(&d_stream, /*Z_NO_FLUSH*/Z_SYNC_FLUSH);
      switch (err)
      {
      case Z_NEED_DICT:
	//ret = Z_DATA_ERROR;     /* and fall through */
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
	//(void)inflateEnd(&strm);
	end_socket();
	return;
      }
      Rstream_n+=(i-d_stream.avail_out);
    }
    while(d_stream.avail_out==0);
    
  }
  else
  {
    memcpy((Rstream_p=realloc(Rstream_p,Rstream_n+i+1))+Rstream_n,rb,i);
    Rstream_n+=i;
  }
  //������ ������� ����
  Rstream_p[Rstream_n]=0; //��������� ������ \0 ��� ���������
  p=Rstream_p;
  i=0; //������ �����
  j=0; //������ ������
  while((p=strstr(p,"<stream>"))) {i--; p++;} //������� - ������� ����� stream, ��� ��� ��� �����������
  p=Rstream_p;
  while((c=*p++))
  {
    if (c=='<')
    {
      j++;
      if (*p!='/') i++; else i--;
    }
    if (c=='>')
    {
      j--;
      if ((!i)&&(!j))
      {
	//������� ������, ������ �� ���������
	int bytecount=p-Rstream_p;
	IPC_BUFFER* tmp_buffer=malloc(sizeof(IPC_BUFFER)); // ���� ���������
	memcpy(tmp_buffer->xml_buffer=malloc(bytecount),Rstream_p,tmp_buffer->buf_size=bytecount); // ����� � ���������
	memcpy(p=Rstream_p,Rstream_p+bytecount,(Rstream_n-=bytecount)+1); //����������� � ������ � ������ \0
	GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,tmp_buffer,sock); //���������� ������� ����
      }
    }
  }
}
