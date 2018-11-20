
#pragma once
#include"SMTPserverDlg.h"
#include"Client.h"
// Server ÃüÁîÄ¿±ê

class Server : public CAsyncSocket
{
public:
	Server();
	virtual ~Server();
	virtual void OnAccept(int nErrorCode);
};


