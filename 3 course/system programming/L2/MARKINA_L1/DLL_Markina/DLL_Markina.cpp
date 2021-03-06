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


extern "C" _declspec(dllexport) Header __stdcall GetAdress() {

	HANDLE hFileMap = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(Header), "FMap");
	BYTE* pBuf = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(Header));

	Header h;
	memcpy(&h, pBuf, sizeof(Header));
	return h;
}

extern "C" _declspec(dllexport) void __stdcall Send(int adr, char* msg ) {
	Header h;
	CString s = CString(msg);
	h.Size = s.GetLength() + 1;
	h.Adress = adr;

	HANDLE hFile = CreateFile("Test.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, NULL);
	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(Header) + h.Size, "FMap");
	BYTE* pBuf = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(Header) + h.Size);
	memcpy(pBuf, &h, sizeof(h));
	memcpy(pBuf + sizeof(h), s.GetString(), h.Size);
	
	UnmapViewOfFile(pBuf);
	CloseHandle(hFileMap);
}

extern "C" _declspec(dllexport) void __stdcall Receive(int &id, char* &msg)
{
	Header h;

	HANDLE hFile = CreateFile("Test.dat", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, NULL);
	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(Header), "FMap");
	BYTE* pBuf = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(Header));
	memcpy(&h, pBuf, sizeof(h));
	//UnmapViewOfFile(pBuf);
	CloseHandle(hFileMap);

	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(Header) + h.Size, "FMap");
	pBuf = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(Header) + h.Size);

	//char* s = new char[h.Size];

	memcpy(msg, pBuf + sizeof(h), h.Size);

	id = h.Adress;

	//UnmapViewOfFile(pBuf);
	CloseHandle(hFileMap);

	//delete[]s;
}
