#include"SMTPserverDlg.h"
#include"Client.h"
#pragma once

// Server ����Ŀ��

class Server : public CAsyncSocket
{
public:
	Server();
	virtual ~Server();
	virtual void OnAccept(int nErrorCode);
};


