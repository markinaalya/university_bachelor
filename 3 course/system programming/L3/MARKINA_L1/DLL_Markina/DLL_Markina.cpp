// DLL_Markina.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "stdafx.h"
#include "DLL_Markina.h"
#include <thread>

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

int count_pros = 0;
HANDLE hRead, hWrite;

void WriteInPipe(int id) {
	DWORD dwWrite;
	WriteFile(hWrite, LPCVOID(&id), sizeof(id), &dwWrite, NULL);
}


extern "C" _declspec(dllexport) void __stdcall Start(int id_event) {

	if (count_pros == 0) {
		SECURITY_ATTRIBUTES sa = { 0 };
		sa.bInheritHandle = TRUE;
		BOOL bRes = CreatePipe(&hRead, &hWrite, &sa, 0);


		STARTUPINFO si = { 0 };
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdInput = hRead;
		si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

		PROCESS_INFORMATION pi;

		CreateProcess(NULL, (LPSTR)"CPP.exe", &sa, NULL, TRUE, NULL, NULL, NULL, &si, &pi);
	}
	else
		WriteInPipe(id_event);
		count_pros++;
}

extern "C" _declspec(dllexport) void __stdcall Stop(int id_event) {
	if (count_pros != 0) {
		WriteInPipe(id_event);
		count_pros--;
	}
}

extern "C" _declspec(dllexport) void __stdcall Quit(int id_event) {

	if (count_pros != 0) {
		WriteInPipe(id_event);
		CloseHandle(hWrite);
		count_pros = 0;
	}
}

extern "C" _declspec(dllexport) void __stdcall Send(int id_event, int adr, char* msg ) {

	if (count_pros != 0) {
		Header h;
		h.Adress = adr;
		h.Size = strlen(msg) + 1;

		DWORD dwWrite;
		WriteInPipe(id_event);
		WriteFile(hWrite, LPCVOID(&h.Adress), sizeof(h.Adress), &dwWrite, NULL); // отправить адрес
		WriteFile(hWrite, LPCVOID(&h.Size), sizeof(h.Size), &dwWrite, NULL); // отправить размер
		WriteFile(hWrite, LPCVOID(msg), sizeof(h.Size), &dwWrite, NULL); // отправить сообщение
	}
}


