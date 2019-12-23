// MFCLib_Lab2.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define DllImport extern "C" _declspec(dllexport) void __stdcall  
#define DllImportCPP extern "C" _declspec(dllexport)  

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct header
{
	int adr;
	int size;
};



void FileOut(int id, CString str)
{
	ofstream fout(to_string(id) + ".txt", ios_base::out | ios_base::app);
	fout << str << endl;
	fout.close();
}

DllImportCPP void FCOUT(CString str, int id, int num)
{
	
	if ((num == -1) && (id == 1))
	{
		cout << "AllThreads" << endl;
		cout << str << endl;
	}

	if (num == -1)
	{
		FileOut(id, str);
	}

	if ((num == 0) && (id == 1))
	{
		cout << "MainThread" << endl;
		cout << str << endl;
	}
	if ((num > 0) && (id == num))
	{
		FileOut(id, str);
	}
}

DllImportCPP CString Receive(int &num)
{
	header hd;
	HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(hd), "FMap");
	LPBYTE buf = (LPBYTE)MapViewOfFile(hMap, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, sizeof(hd));
	memcpy(&hd, buf, sizeof(hd));
	UnmapViewOfFile(buf);
	CloseHandle(hMap);

	hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(hd) + hd.size, "FMap");
	buf = (LPBYTE)MapViewOfFile(hMap, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, sizeof(hd) + hd.size);
	CString str((char*)buf + sizeof(header));
	UnmapViewOfFile(buf);
	num = hd.adr;
	CloseHandle(hMap);
	return str;
}

DllImport Send(int adr, int size, char* message)
{
	header hd;
	CString st = CString(message);
	hd.adr = adr;
	hd.size = size + 1;
	HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(hd) + hd.size, "FMap");
	LPBYTE buf = (LPBYTE)MapViewOfFile(hMap, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, sizeof(hd) + hd.size);

	memcpy(buf, &hd, sizeof(hd));
	memcpy(buf + sizeof(hd), st.GetString(), hd.size);

	UnmapViewOfFile(buf);
	CloseHandle(hMap);
	
}