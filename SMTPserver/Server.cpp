// Server.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SMTPserver.h"
#include "Server.h"

// Server

Server::Server()
{
}

Server::~Server()
{
}


// Server ��Ա����


//��дServer���OnAccept����
void Server::OnAccept(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	//����OnAccept�������ж��Ƿ���Խ��ܣ�������ԣ���dlg_log�����Ӧ���ݣ�����ɾ����
	CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
	dlg->dlg_log+="Received a connection request\r\n";
	//dlg->UpdateData(false);
	Client *socket=new Client();
	if(Accept(*socket))
	{
		//socket������
		 //MessageBox(L"aaa");
		dlg->dlg_log+="Connecting is being established\r\n";
		//dlg->UpdateData(false);

		socket->AsyncSelect(FD_WRITE);
	}
	else
	{
		delete socket;
	}
	CAsyncSocket::OnAccept(nErrorCode);
}
