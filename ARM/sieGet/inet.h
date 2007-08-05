/*
  ������ SieGet Downloader
                          */

//inetapi.h
//��������������� ������� ��� ��������� ������ ������ ������� �� ������ �������

#ifndef _INETAPI_H_
#define _INETAPI_H_

typedef void (*RecvCallbackProc)(int res, void *data, int size);

//��������� ������, ���������� ����� ������� �������.
//����������, ������� �� ����������� ��������.
//����� ������ � ��� ������ ������������ � ���������� dat � size.
void GetDataByReq(char *req, int ip, short port, RecvCallbackProc rproc);

#endif
