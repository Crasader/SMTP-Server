
// SMTPserver.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSMTPserverApp:
// �йش����ʵ�֣������ SMTPserver.cpp
//

class CSMTPserverApp : public CWinApp
{
public:
	CSMTPserverApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSMTPserverApp theApp;