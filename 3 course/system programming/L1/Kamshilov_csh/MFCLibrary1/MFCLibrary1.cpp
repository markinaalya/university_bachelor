// MFCLibrary1.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "stdafx.h"
#include "MFCLibrary1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CString str;
int cc = 0;
using namespace std;


struct Header {
	int size;
	int addres;
	int p;
};

HANDLE hFile;
HANDLE hMap;
LPBYTE pByte;

extern "C" _declspec(dllexport) int __stdcall Start(int n)
{
	Header header;
	header.p = n;
	cc = 0;
	if (WaitNamedPipe("\\\\.\\pipe\\MyPipe", 10000))
	{
		HANDLE hPipe = CreateFile("\\\\.\\pipe\\MyPipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		DWORD dwRead, dwWrite;
		WriteFile(hPipe, LPCVOID(&(header.p)), sizeof(header.p), &dwWrite, NULL);
		ReadFile(hPipe, LPVOID(&cc), sizeof(cc), &dwRead, NULL);
		CloseHandle(hPipe);
		cc++;
	}

	return cc;
}

extern "C" _declspec(dllexport) int __stdcall Stop(int n)
{
	cc = 0;
	Header header;
	header.p = n;
	if (WaitNamedPipe("\\\\.\\pipe\\MyPipe", 10000))
	{
		HANDLE hPipe = CreateFile("\\\\.\\pipe\\MyPipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		DWORD dwRead, dwWrite;
		WriteFile(hPipe, LPCVOID(&(header.p)), sizeof(header.p), &dwWrite, NULL);
		ReadFile(hPipe, LPVOID(&cc), sizeof(cc), &dwRead, NULL);
		CloseHandle(hPipe);
		cc++;
	}
	return cc;
}

extern "C" _declspec(dllexport) void __stdcall Quit(int n)
{
	Header header;
	header.p = n;
	if (WaitNamedPipe("\\\\.\\pipe\\MyPipe", 10000))
	{
		HANDLE hPipe = CreateFile("\\\\.\\pipe\\MyPipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		DWORD dwRead, dwWrite;
		WriteFile(hPipe, LPCVOID(&(header.p)), sizeof(header.p), &dwWrite, NULL);
		ReadFile(hPipe, LPVOID(&cc), sizeof(cc), &dwRead, NULL);
		CloseHandle(hPipe);
	}
}

extern "C" _declspec(dllexport) int __stdcall Send(int nn, int n, int s, char *message)
{
	bool k = false;
	Header header;
	header.p = nn;
	header.addres = n;
	header.size = s + 1;
	if (WaitNamedPipe("\\\\.\\pipe\\MyPipe", 10000))
	{
		k = true;
		HANDLE hPipe = CreateFile("\\\\.\\pipe\\MyPipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		str = CString(message);
		str = new char[header.size];
		DWORD dwRead, dwWrite;
		WriteFile(hPipe, LPCVOID(&(header.p)), sizeof(header.p), &dwWrite, NULL);
		WriteFile(hPipe, LPCVOID(&(header.addres)), sizeof(header.addres), &dwWrite, NULL);
		WriteFile(hPipe, LPCVOID(&(header.size)), sizeof(header.size), &dwWrite, NULL);
		WriteFile(hPipe, LPCVOID(message), header.size, &dwWrite, NULL);
		CloseHandle(hPipe);
	}
	return k;
}


//
//TODO: если эта библиотека DLL динамически связана с библиотеками DLL MFC,
//		все функции, экспортированные из данной DLL-библиотеки, которые выполняют вызовы к
//		MFC, должны содержать макрос AFX_MANAGE_STATE в
//		самое начало функции.
//
//		Например:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// тело нормальной функции
//		}
//
//		Важно, чтобы данный макрос был представлен в каждой
//		функции до вызова MFC.  Это означает, что
//		должен стоять в качестве первого оператора в
//		функции и предшествовать даже любым объявлениям переменных объекта,
//		поскольку их конструкторы могут выполнять вызовы к MFC
//		DLL.
//
//		В Технических указаниях MFC 33 и 58 содержатся более
//		подробные сведения.
//

// CMFCLibrary1App

BEGIN_MESSAGE_MAP(CMFCLibrary1App, CWinApp)
END_MESSAGE_MAP()


// Создание CMFCLibrary1App

CMFCLibrary1App::CMFCLibrary1App()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}


// Единственный объект CMFCLibrary1App

CMFCLibrary1App theApp;


// Инициализация CMFCLibrary1App

BOOL CMFCLibrary1App::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
