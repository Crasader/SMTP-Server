// Client.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SMTPserver.h"
#include "Client.h"
#include <cstring>
#include<string>

#define USES_CONVERSION /
#define T2A /
using namespace std;
// Client

Client::Client()
{
	data_begin = 0;
	data_end = 0;
	line_no = 0;
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
	//�ж��ʼ��Ƿ����
	if(str.Right(5)=="\r\n.\r\n")
	{
		char buff[100]="250 data\r\n";
		Send(buff,strlen(buff));//��������Ӧ250����ʾ���������ɹ����
		//dlg->dlg_log+="Server finished accepting\r\n";
		
		dlg->dlg_message+=str;
		split_str(str);
		int i=0;
		for(i = 16;i<line_no;i++)
		{
			//CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
			//dlg->dlg_content += splitted_str[i]+="test \r\n";
			CString s1=splitted_str[i].Right(6);
			if(splitted_str[i].Right(6) == "base64")
			{
				CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
				//code = splitted_str[i+2];
				
				break;
			}
		
		}
		CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
		CString code= splitted_str[i+2];
		dlg->dlg_content += code;
		//char * input = code.GetBuffer();
		//std::string input;
		//input=CT2A((LPCTSTR)code);
		//char* input = code.GetBuffer(code.GetLength());
		//char* sz = code.GetAt(0);
		//LPCTSTR input = code;
		//char *input=(LPSTR)(LPCTSTR)code;
		char input[256];//����򵥲����Ͳ���̬������
		wchar_t *lp = (LPTSTR)(LPCTSTR)code; 
		int len = WideCharToMultiByte(CP_ACP,0,lp,wcslen(lp),NULL,0,NULL,NULL);
		WideCharToMultiByte(CP_ACP,0,lp,wcslen(lp),input,len,NULL,NULL);
		input[len] = '\0'; 
		CString s1(input);
		s1.Remove('\r');
		s1.Remove('\n');
      //char *input = T2A(code);
		//char*input = "NjY2DQoNCg0KDQpsaXlpbmhhbzA0MTNAcXEuY29tDQo=";
		CString output = base64_decode(s1);
		dlg->dlg_content += output;
		//dlg->dlg_content += decoded_data.c_str();
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//���յ�HELO��EHLO�����
	if(str.Left(4) == "EHLO"||str.Left(4) == "HELO")//�ڱ�������EHLO
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += "\n";
		dlg->UpdateData(false);//���յ���������ʾ�ڴ�����
		char* s1="250 Received HELO\r\n";
		Send(s1,strlen(s1));
		dlg->dlg_log += "Server:Received HELO\r\n";
		
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//���յ�MAIL FROM�����
	if(str.Left(10) == "MAIL FROM:")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += "\n";
		dlg->UpdateData(false);
		char* s1="250 Received MAIL FROM\r\n";
		Send(s1,strlen(s1));
		dlg->dlg_log += "Server:Received MAIL FROM\r\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//���յ�RCPT TO�����
	if(str.Left(8) == "RCPT TO:")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += "\n";
		dlg->UpdateData(false);
		char * s2 = "250 Received RECP TO\r\n";
		Send(s2,strlen(s2));
		dlg->dlg_log += "Server:Received RECP TO\r\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//���յ�DATA�����
	if(str.Left(4) == "DATA")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += "\n";
		dlg->UpdateData(false);
		data_begin = true;
		char * s3 = "354 Received DATA\r\n";
		Send(s3,strlen(s3));
		dlg->dlg_log += "Server:Received DATA\r\n";
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	if(str.Left(4) == "QUIT")
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += "\n";
		dlg->UpdateData(false);
		char *s4 = "221 Received QUIT\r\n";
		Send(s4,strlen(s4));
		dlg->dlg_log += "Server:Received QUIT\r\n";
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

bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}
CString Client::base64_decode(CString find)
{
	
		int bytelength=0;
	unsigned char * base64 = (unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	int findlength = find.GetLength();
	unsigned char *temp = new unsigned char[findlength];
	unsigned char * result;
	int assign = 0;
	for (int i = 0; i < findlength; i++)
	{
		temp[i] = find[i];
	}
	while (findlength > 0 && temp[findlength - 1] == '=')
	{
		assign++;
		temp[findlength - 1] = 0;
		findlength--;
	}
	for (int i = 0; i < findlength; i++)
	{
		unsigned char *p = (unsigned char *)strchr((const char *)base64, (int)temp[i]);  //(int)src[i]֮����ַ�����p��ʼ
		if (!p)
			break;
		temp[i] = p - (unsigned char *)base64;  //ӳ�䵽6bit�ַ�
	}
	result = (unsigned char *)malloc(findlength * 3 / 4 + 1);
	memset(result, 0, findlength * 3 / 4 + 1);
	int j = 0;
	for (int i = 0; i < findlength; i += 4, j += 3)
	{
		result[j] = (temp[i] << 2) + ((temp[i + 1] & 0x30) >> 4);  //��һ��6bit��ǰ��λ+�ڶ���6bit��ǰ��λ
		result[j + 1] = ((temp[i + 1] & 0x0F) << 4) + ((temp[i + 2] & 0x3C) >> 2);  //�ڶ���6bit�ĺ���λ+������6bit��ǰ��λ
		result[j + 2] = ((temp[i + 2] & 0x03) << 6) + temp[i + 3];  //������6bit�ĺ���λ+���ĸ�6bit
		bytelength += 3;  //����һ���ֽ���Ŀ��3
	}
	bytelength -= assign;
	result[bytelength + 1] = '\0';
	return CString(result);
}




void Client::split_str(CString str)
{
	CString szGet;	
	CString szText = str;
	bool result = true;	//�ж��Ƿ�ָ���
	line_no = 0;
	//�����İ��зָ�洢��splitted_str��
	while(result)
	{
		result = AfxExtractSubString(szGet, szText, line_no, '\n');	//��/nΪ��ָ��ַ���
		result = AfxExtractSubString(szGet, szText, line_no, '\r');	//��/rȥ����
		splitted_str[line_no] = szGet;
		line_no++;
		CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
	}
}
