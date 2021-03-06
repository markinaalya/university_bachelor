// DLL_Markina.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "stdafx.h"
#include "DLL_Markina.h"
#include <thread>
#include <fstream>

#include <afxsock.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDLLMarkinaApp

BEGIN_MESSAGE_MAP(CDLLMarkinaApp, CWinApp)
END_MESSAGE_MAP()


// Создание CDLLMarkinaApp

CDLLMarkinaApp::CDLLMarkinaApp()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}


// Единственный объект CDLLMarkinaApp

CDLLMarkinaApp theApp;

using namespace std;

ofstream f("logdll.txt", ios::app);

// Инициализация CDLLMarkinaApp

BOOL CDLLMarkinaApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

struct Header
{
	int Adress;
	int Size;
};

int static count_thread = 0;

extern "C" _declspec(dllexport) int __stdcall GetCountThread(int id_event) {
	f << "GetCountThread(int id_event) " << id_event << endl;

	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
	AfxSocketInit();
	CSocket s;
	s.Create();

	f << "Create" << endl;

	if (s.Connect("127.0.0.1", 12345))
	{
		s.Send(LPCVOID(&id_event), sizeof(id_event));
		f << "Отправили событие " << id_event << endl;
		s.Receive(LPVOID(&count_thread), sizeof(count_thread));
		return count_thread;

	} else return -1;
}

extern "C" _declspec(dllexport) int __stdcall Start(int id_event) {
	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
	AfxSocketInit();
	CSocket s;
	s.Create();

	f<<"Пришло событие    " << id_event << endl;
	if (s.Connect("127.0.0.1", 12345))
	{
		f << "Подключились к каналу" << endl;
		s.Send(LPCVOID(&id_event), sizeof(id_event));
		f << "Отправили событие " << id_event << endl;
		s.Receive(LPVOID(&count_thread), sizeof(count_thread));
		f << "Пришло потоков " << count_thread << endl;
	}
	s.Close();

	return count_thread;
}

extern "C" _declspec(dllexport) int __stdcall Stop(int id_event) {

	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
	AfxSocketInit();
	CSocket s;
	s.Create();

	if (s.Connect("127.0.0.1", 12345)) {

		HANDLE hPipe = CreateFile("\\\\.\\pipe\\MyPipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		DWORD dwRead, dwWrite;

		if (count_thread != 0) {
			f << "Отправили событие " << id_event << endl;
			s.Send(LPCVOID(&id_event), sizeof(id_event));
			s.Receive(LPVOID(&count_thread), sizeof(count_thread));
			f << "Пришло потоков " << count_thread << endl;
		}

		CloseHandle(hPipe);
	}
	return count_thread;
}

extern "C" _declspec(dllexport) bool __stdcall Send(int id_event, int adr, char* msg ) {

	AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
	AfxSocketInit();
	CSocket s;
	s.Create();

	if (s.Connect("127.0.0.1", 12345)) {
		
		if (count_thread != 0) {
			Header h;
			h.Adress = adr;
			h.Size = strlen(msg) + 1;

			s.Send(LPCVOID(&id_event), sizeof(id_event));
			s.Send(LPCVOID(&h.Adress), sizeof(h.Adress));
			s.Send(LPCVOID(&h.Size), sizeof(h.Size));
			s.Send(LPCVOID(msg), h.Size);
		}

	}
	return true;
}


