//---------------------------------------------------------------------------

#pragma hdrstop

//---------------------------------------------------------------------------

/*
			Simple INETD v1.3 for WINDOWS by Alexander S. Pristenski [a.k.a. RekoD]
*/
#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <windows.h>
#include <winsock2.h>
#include <algorithm>
#pragma argsused

#pragma comment (lib,"ws2_32.lib")

#define BUFSIZE 16384
//#define KEEPALIVE 10
#define MAXCONNQUEUE 5

bool telnet = false; // ����������� �� ������ �� telnet ���������

#define BZERO(a) memset(a,0,sizeof(a))

typedef struct _t_param
{
	SOCKET s;
	char   exe[MAX_PATH];
} THREAD_PARAM;

unsigned int __stdcall ServerThread(LPVOID tParam);

int main(int argc, char* argv[])
{
	WSADATA		WSAData;
	SOCKET		listen_socket, server_socket;
	SOCKADDR_IN	server_addr, temp_addr;
	int			accepted_len = sizeof(temp_addr);

	// �������� ����������
	if(argc < 3)
	{
		printf("\nUsage: %s PORT CMDLINE [-FF]\nExample: %s 23 c:\\winnt\\system32\\cmd.exe -FF\nFlag -FF should be used if client will connect using TELNET protocol.", argv[0], argv[0]);
		return -1;
	}
	if((atoi(argv[1]) < 1) || (atoi(argv[1]) > 65535))
	{
		printf("\nIncorrect port number. Value must be in range 1-65535");
		return -1;
	}
	if((argc > 3) && (!strcmp(argv[3], "-FF")))
	{
		telnet = true;
	}

	// 1.1. ������������� WINSOCK
	if(WSAStartup(MAKEWORD(2,0), &WSAData) != 0)
	{
		printf("\nWSAStartup() failed with error: %i", WSAGetLastError());
		return -1;
	}

	// 1.2. �������� "����������" ������
	if((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("\nsocket() failed with error: %i", WSAGetLastError());
		return -1;
	}

    // ��������� �����: REUSE ADDRESS.
    // ���� ������ "�����" ��� ������������� ��������
	// �� ������ ����� ���������� ��������� ��� �� ����.
	bool opt = true;
    if(SOCKET_ERROR == setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)))
    {
		printf("\nsetsockopt(SO_REUSEADDR) failed with error: %i", WSAGetLastError());
		return -1;
    }

	/* 1.3. �������� ������ � ���������� ������ */
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port        = htons(atoi(argv[1]));

	if(bind(listen_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		printf("\nbind() failed with error: %i", WSAGetLastError());
		return -1;
	}

	/* 1.4. ��������� ������������� �� ��������� ������ */
	if(listen(listen_socket, MAXCONNQUEUE) == SOCKET_ERROR)
	{
		printf("\nlisten() failed with error: %i", WSAGetLastError());
		return -1;
	}

	/* 1.5. ���� �������� ���������� �� �������� */
	while((server_socket = accept(listen_socket, (struct sockaddr *) &temp_addr, (int *)&accepted_len)) != INVALID_SOCKET)
	{
		// ����� ���� ���������� � �������� ?
		SOCKADDR peer;
		int      peerlen  = sizeof(peer);

		if(0 == getpeername(server_socket, &peer, &peerlen))
		{
			printf("\nConnect from %s:%i", inet_ntoa(((SOCKADDR_IN *)&peer)->sin_addr), ntohs(((SOCKADDR_IN *)&peer)->sin_port));
		}

		// ������ ������ ��� ������ � ������� � �������� ����������
		// (������������ ������ ����� ����������� ��� ������ �� ������)
		HANDLE hThread = NULL;

		THREAD_PARAM *t_param = NULL;

		t_param = new THREAD_PARAM;

		if(NULL == t_param)
		{
			printf("\n memory allocation error!");
			break;
		}

		t_param->s   = server_socket;
		BZERO(t_param->exe);
		strcpy(t_param->exe, argv[2]);

		// ������ ����� � ������� _beginthreadex()
		if(NULL == (hThread = (HANDLE)_beginthreadex(NULL, 0, ServerThread, t_param, 0, NULL)))
		{
			printf("\nCreateThread() failed with error: %i", GetLastError());
		}
		else
		{
			CloseHandle(hThread); // HANDLE ������ ��� ������ �� �����
		}
	}
	closesocket(listen_socket);
	return 0;
}

// ��������� �������, ������� ��������� �������� �������
// � �������������� ��� STDIN � STDOUT �� �����
unsigned int __stdcall ServerThread(LPVOID tParam)
{
	THREAD_PARAM *param = (THREAD_PARAM *)tParam;
	unsigned long		quit = 0;
   // ������������� ��������� ���������� ��� ��������� ��������
 	  STARTUPINFO			si;
   	PROCESS_INFORMATION pi;
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // �� ���������� ���� ��������� ����������
	  si.hStdOutput = (HANDLE)param->s; // ������ STDOUT ������������ newstdout
	  si.hStdError  = (HANDLE)param->s; // ������ STDERR ������������ newstdout
	  si.hStdInput  = (HANDLE)param->s;  // ������ STDIN  ������������ newstdin
  	// ������ ��������� ���������� (�������� ���������� - ����� ���������� ����������)
  	// STDIN � STDOUT ���������������� �� ����� - ������ ��������� ���������� ����
  	// ������������� ����������� �������� (HTTP, POP, SMTP, etc.)
	  if(0 == CreateProcess(param->exe, NULL, NULL, NULL, true, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	  {
	  	printf("\nCreateProcess() fails with error: %i", GetLastError());
      return -1;
  	}
    while(1)
    {
      Sleep(1000);
  		if(0 == GetExitCodeProcess(pi.hProcess, &quit))
	  	{
	  		printf("\nGetExitCodeProcess() fails with error: %i", GetLastError());
  		}
  		if(STILL_ACTIVE != quit)
      {
         if (quit) printf("\n!!!EXIT %08x!!!",quit);
         break;
      }
    }
    	// �������� ��� ����������
	SOCKADDR peer;
	int      peerlen  = sizeof(peer);

	if(0 == getpeername(param->s, &peer, &peerlen))
	{
		printf("\nDisconnect %s:%i", inet_ntoa(((SOCKADDR_IN *)&peer)->sin_addr), ntohs(((SOCKADDR_IN *)&peer)->sin_port));
	}

	// ������� ����� � ���������� ������ ���������� ���������
	shutdown(param->s, SD_BOTH);
	closesocket(param->s);
	delete param;
	param = NULL;

  return 0;
}
//---------------------------------------------------------------------------

