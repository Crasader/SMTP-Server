// Server.cpp : 实现文件
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


// Server 成员函数


//重写Server类的OnAccept函数
void Server::OnAccept(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	//调用OnAccept函数，判断是否可以接受，如果可以，则dlg_log输出相应内容，否则删除。
	CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
	dlg->dlg_log+="Received a connection request\n";
	dlg->UpdateData(false);
	Client *socket=new Client();
	if(Accept(*socket))
	{
		//socket被接受
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
