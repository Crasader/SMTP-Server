#pragma once
#include"SMTPserverDlg.h"
// Client ����Ŀ��

class Client : public CAsyncSocket
{
public:
	Client();
	virtual ~Client();
	virtual void OnReceive(int nErrorCode);
	char buffer[10000000];//����
	virtual void OnSend(int nErrorCode);
};


