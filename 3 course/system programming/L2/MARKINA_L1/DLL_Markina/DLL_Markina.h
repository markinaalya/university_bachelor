// DLL_Markina.h: основной файл заголовка для библиотеки DLL DLL_Markina
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CDLLMarkinaApp
// Реализацию этого класса см. в файле DLL_Markina.cpp
//

class CDLLMarkinaApp : public CWinApp
{
public:
	CDLLMarkinaApp();

// Переопределение
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
