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
	dlg->dlg_log+="Received a connection request\n";
	dlg->UpdateData(false);
	Client *socket=new Client();
	if(Accept(*socket))
	{
		//socket������
		CString return_ready_info = L"220 Simple Mail Server Ready for Mail";
		socket->Send(return_ready_info,return_ready_info.GetLength());
		dlg->dlg_log+="Connecting is being established\n";
		dlg->UpdateData(false);
	}
	else
	{
		delete socket;
	}

	CAsyncSocket::OnAccept(nErrorCode);
}
