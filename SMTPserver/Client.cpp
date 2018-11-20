// Client.cpp : ʵ���ļ�
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


// Client ��Ա����


void Client::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	memset(buffer,0,sizeof(buffer));//�������
	CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
	CAsyncSocket::OnReceive(nErrorCode);
	Receive(buffer,sizeof(buffer),0);
	CString str(buffer);//��bufferת����str
	//���յ�HELO�����
	if(str.Left(4) == "EHLO")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += '\n';
		dlg->UpdateData(false);//���յ���������ʾ�ڴ�����
		char* s1="250 Received HELO\n";
		Send(s1,strlen(s1));
		dlg->dlg_log += "Server:Received HELO\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//���յ�MAIL FROM�����
	if(str.Left(10) == "MAIL FROM:")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += '\n';
		dlg->UpdateData(false);
		char* s1="250 Received MAIL FROM\r\n";
		Send(s1,strlen(s1));
		dlg->dlg_log += "Server:Received MAIL FROM\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//���յ�RCPT TO�����
	if(str.Left(8) == "RCPT TO:")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += '\n';
		dlg->UpdateData(false);
		CString return_RCPT_TO_text = L"250 Received RECP TO\n";
		Send(return_RCPT_TO_text,return_RCPT_TO_text.GetLength());
		dlg->dlg_log += "Server:Received RECP TO\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//���յ�DATA�����
	if(str.Left(4) == "DATA")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += '\n';
		dlg->UpdateData(false);
		CString return_DATA_text = L"354 Received DATA\n";
		Send(return_DATA_text,return_DATA_text.GetLength());
		dlg->dlg_log += "Server:Received DATA\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	if(str.Left(4) == "QUIT")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += '\n';
		dlg->UpdateData(false);
		CString return_QUIT_text = L"221 Received QUIT\n";
		Send(return_QUIT_text,return_QUIT_text.GetLength());
		dlg->dlg_log += "Server:Received QUIT\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	CAsyncSocket::OnReceive(nErrorCode);
}


void Client::OnSend(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	char *s="220 ready\r\n";
	Send(s,strlen(s));
	AsyncSelect(FD_READ);
	CAsyncSocket::OnSend(nErrorCode);
}
