// MFCApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MFCApplication.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <windows.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef CString(*PREC)(int &num);
typedef void(*PFCO)(CString str, int id, int num);
int num;
CString str;

//struct hMTHandles
//{
//	HANDLE hEvent;
//	HANDLE hMSG;
//};

HANDLE hMTEvents[2];
HANDLE hConf = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);

HINSTANCE hLib = LoadLibrary("MFCLib_Lab2.dll");
PREC receive = (PREC)GetProcAddress(hLib, "Receive");
PFCO fcout = (PFCO)GetProcAddress(hLib, "FCOUT");

DWORD WINAPI MyThread(LPVOID lpParameter)
{

	
	int id = (int)lpParameter;
	cout << id << endl;
	while (true)
	{
		DWORD dwResult = WaitForMultipleObjects(2, hMTEvents, FALSE, INFINITE) - WAIT_OBJECT_0;
		switch (dwResult)
		{
		case 0:
		{
			//WaitForSingleObject((HANDLE)lpParameter, INFINITE);
			SetEvent(hConf);
			return 0;
		}
		case 1:
		{
			ResetEvent(hMTEvents[1]);
			WaitForSingleObject(hMutex, INFINITE);
			fcout(str, id, num);
			ReleaseMutex(hMutex);
			
			break;
		}
		}
	}


	//int a;
	//cin >> a;
	return 0;
}

// The one and only application object

CWinApp theApp;

using namespace std;




void start()
{
	//vector <hMTHandles> hMTEvs;
	//vector <HANDLE> hThreads;


	HANDLE hEvents[4];
	hEvents[0] = CreateEvent(NULL, FALSE, FALSE, "EventStart");
	hEvents[1] = CreateEvent(NULL, FALSE, FALSE, "EventStop");
	hEvents[2] = CreateEvent(NULL, FALSE, FALSE, "EventQuit");
	hEvents[3] = CreateEvent(NULL, FALSE, FALSE, "EventSend");
	HANDLE hConfirm = CreateEvent(NULL, FALSE, FALSE, "EventConfirm");

	hMTEvents[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	hMTEvents[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
	int a = 1;

	while (1)
	{
		DWORD dwResult = WaitForMultipleObjects(4, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0;
		switch (dwResult)
		{
		case 0:
		{
			cout << "Start" << endl;
			/*hMTHandles mth;
			mth.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			mth.hMSG = CreateEvent(NULL, FALSE, FALSE, NULL);
			hMTEvs.push_back(mth);*/
			HANDLE hThread = CreateThread(NULL, 0, MyThread, (LPVOID)a++, 0, NULL);
			//hThreads.push_back(CreateThread(NULL, 0, MyThread, (LPVOID)&hMTEvs, 0, NULL));
			CloseHandle(hThread);
			//cout << a << endl;
			SetEvent(hConfirm);
			break;
		}
		case 1:
		{
			cout << "Stop" << endl;
			if (a > 1)
			{
				//TerminateThread(hThreads.back(), NULL);
				SetEvent(hMTEvents[0]);
				WaitForSingleObject(hConf, INFINITE);
				a--;

				/*CloseHandle(hThreads.back());
				hThreads.pop_back();*/

				cout << a << endl;

			}
			SetEvent(hConfirm);
			if (a == 1)
			{
				for (int i = 0; i < 4; i++)
				{
					CloseHandle(hEvents[i]);
				}
				CloseHandle(hMTEvents[0]);
				CloseHandle(hMTEvents[1]);
				CloseHandle(hMutex);
				CloseHandle(hConfirm);
				return;
			}


			break;
		}
		case 2:
		{
			cout << "Quit" << endl;
			/*if (hThreads->size() != NULL)
			{
			for (int i = 0; i != hThreads->size(); i++)
			{
			TerminateThread(hThreads->back(), NULL);
			hThreads->erase(hThreads->end() - 1);
			}
			}*/
			for (int i = 0; i < 4; i++)
			{
				CloseHandle(hEvents[i]);
			}
			CloseHandle(hMTEvents[0]);
			CloseHandle(hMTEvents[1]);
			CloseHandle(hMutex);
			CloseHandle(hConfirm);

			return;
		}
		case 3:
		{

			str = receive(num);
			cout << str << "  " << num << endl;
			SetEvent(hMTEvents[1]);
			SetEvent(hConfirm);

			break;
		}
		}

	}
}

int main()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			start();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}
