
// SMTPserver.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CSMTPserverApp:
// 有关此类的实现，请参阅 SMTPserver.cpp
//

class CSMTPserverApp : public CWinApp
{
public:
	CSMTPserverApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CSMTPserverApp theApp;