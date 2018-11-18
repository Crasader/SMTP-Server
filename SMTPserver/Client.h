#pragma once
#include"SMTPserverDlg.h"
// Client ÃüÁîÄ¿±ê

class Client : public CAsyncSocket
{
public:
	Client();
	virtual ~Client();
	virtual void OnReceive(int nErrorCode);
	char buffer[10000000];//»º´æ
};


