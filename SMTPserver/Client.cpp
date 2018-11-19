// Client.cpp : 实现文件
//

#include "stdafx.h"
#include "SMTPserver.h"
#include "Client.h"
#include <cstring>

// Client

Client::Client()
{
}

Client::~Client()
{
}


// Client 成员函数


void Client::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	memset(buffer,0,sizeof(buffer));//清除缓存
	CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
	CAsyncSocket::OnReceive(nErrorCode);
	CString str(buffer);//将buffer转化成str
	//接收到HELO的情况
	if(str.Left(4) == "HELO"||str.Left (4)=="EHLO")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += '\n';
		dlg->UpdateData(false);//将收到的内容显示在窗口里
		CString return_HELO_text = L"250 OK 127.0.0.1";
		Send(return_HELO_text,return_HELO_text.GetLength());
		dlg->dlg_log += "Server:250 OK 127.0.0.1\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//接收到MAIL FROM的情况
	if(str.Left(10) == "MAIL FROM:")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += '\n';
		dlg->UpdateData(false);
		CString return_MAIL_FROM_text = L"250 Sender OK";
		Send(return_MAIL_FROM_text,return_MAIL_FROM_text.GetLength());
		dlg->dlg_log += "Server:250 Sender OK\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//接收到RCPT TO的情况
	if(str.Left(8) == "RCPT TO:")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += '\n';
		dlg->UpdateData(false);
		CString return_RCPT_TO_text = L"250 Receiver OK";
		Send(return_RCPT_TO_text,return_RCPT_TO_text.GetLength());
		dlg->dlg_log += "Server:250 Receiver OK\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//接收到DATA的情况
	if(str.Left(4) == "DATA")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += '\n';
		dlg->UpdateData(false);
		CString return_DATA_text = L"354 Go ahead.End with<CRLF>.<CRLF>";
		Send(return_DATA_text,return_DATA_text.GetLength());
		dlg->dlg_log += "Server:354 Go ahead.End with<CRLF>.<CRLF>\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	if(str.Left(4) == "QUIT")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += '\n';
		dlg->UpdateData(false);
		CString return_QUIT_text = L"221 Quit,goodbye !\n";
		Send(return_QUIT_text,return_QUIT_text.GetLength());
		dlg->dlg_log += "Server:221 Quit,goodbye !\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	CAsyncSocket::OnReceive(nErrorCode);
}
