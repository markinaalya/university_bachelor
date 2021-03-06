// CPP.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "CPP.h"
#include <vector>
#include <fstream>
#include <thread>
#include <string>
#include "..//DLL_Markina/DLL_Markina.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef void(*PFCO)(int &, char* &);

extern "C" _declspec(dllimport) void __stdcall Receive(int &id, char* &msg);

// Единственный объект приложения

CWinApp theApp;

int ThreadAdr = -2; //Адрес
char* msg = new char[256]; //сообщение

HANDLE mMutex = CreateMutex(NULL, FALSE, NULL); // синхронизация
HANDLE hLocalConf = CreateEvent(NULL, FALSE, FALSE, NULL); //локальный флажок

int count_thread = 1;


using namespace std;

vector <HANDLE> vEventMsg; //события прихода сообщений для переключения между потоками
vector <HANDLE> vEventClose; //события прихода сообщений для переключения между потоками

void FileOut(string name) 
{
	ofstream f(name + ".txt");
	f << msg;
	cout << "File  " << name << ".txt " << "  is create" << endl;
	f.close();
	
}

DWORD WINAPI NewThread(LPVOID lpParam)
{
	HANDLE EventTread[2]; //два события для каждого потока. Закрытие и приход сообщения
	int id = (int)lpParam;

	EventTread[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	EventTread[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

	vEventClose.push_back(EventTread[0]); //связь закрытия сообщения с конкретным потоком
	vEventMsg.push_back(EventTread[1]); //связь прихода сообщения с конкретным потоком

	cout <<"Start thread "<< id << endl;
	while (true)
	{
		DWORD dwResult = WaitForMultipleObjects(2, EventTread, FALSE, INFINITE) - WAIT_OBJECT_0;

		switch (dwResult)
		{
		case 0:
		{	
			vEventClose.pop_back();
			vEventMsg.pop_back();
			cout << "Close thread " << id << endl;
			SetEvent(hLocalConf);
			return 0;
		}
		case 1:
		{
			ResetEvent(EventTread[1]);
			WaitForSingleObject(mMutex, INFINITE);
			FileOut(to_string(id));
			cout << "Message was print. Thread " << id << endl;
			ReleaseMutex(mMutex);
			SetEvent(hLocalConf);

			break;
		}
		}
	}

	return 0;
}

void start()
{
	HANDLE hEvents[4];
	hEvents[0] = CreateEvent(NULL, FALSE, FALSE, "Start_event");
	hEvents[1] = CreateEvent(NULL, FALSE, FALSE, "Stop_event");
	hEvents[2] = CreateEvent(NULL, FALSE, FALSE, "Quit_event");
	hEvents[3] = CreateEvent(NULL, FALSE, FALSE, "Send_event");

	HANDLE hEventConfirm = CreateEvent(NULL, FALSE, FALSE, "Confirm_event"); //флажок

	while (1)
	{	//индекс объекта в массиве, который сейчас в сигнальном состоянии
		int index_event = WaitForMultipleObjects(4, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0;
		switch (index_event)
		{ //Start
		case 0: {
			CloseHandle(CreateThread(NULL, FALSE, NewThread, (LPVOID)count_thread++, 0, NULL)); //связываем поток с событием
			SetEvent(hEventConfirm); //установка в сигнальное состояние
			break;
		}

				//Stop
		case 1: {
			SetEvent(vEventClose.back());
			WaitForSingleObject(hLocalConf, INFINITE);
			count_thread--;
			SetEvent(hEventConfirm);
			break;
		}
				//Message
		case 3: 

		{	
			Receive(ThreadAdr, msg);
			cout << "Receive message:  " << msg <<"  for thread   "<< ThreadAdr + 1 << endl;

			if (ThreadAdr == -2) {
				cout << "All Thread" << endl;
				for (auto it : vEventMsg) {
						SetEvent(it);
						WaitForSingleObject(hLocalConf, INFINITE);
				}

			} else if (ThreadAdr == -1) {
				cout << "Main Thread: " << msg << endl;
			}
			else if (ThreadAdr >= 0){
				SetEvent(vEventMsg[ThreadAdr]);
			}

			SetEvent(hEventConfirm);
			break;

			
		}

				//Quit
		default: {

			SetEvent(hEventConfirm);
			exit(0);
			break;
		}
		}
	}

	for (auto e : hEvents)
		CloseHandle(e);
	CloseHandle(hEventConfirm);
}



int main()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// инициализировать MFC, а также печать и сообщения об ошибках про сбое
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: вставьте сюда код для приложения.
			wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
			nRetCode = 1;
		}
		else
		{
			start();
			// TODO: вставьте сюда код для приложения.
		}
	}
	else
	{
		// TODO: измените код ошибки в соответствии с потребностями
		wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
		nRetCode = 1;
	}

	return nRetCode;
}