#pragma once
#include"SMTPserverDlg.h"
// Client ÃüÁîÄ¿±ê
#include<string>
using namespace std;
class Client : public CAsyncSocket
{
public:
	Client();
	virtual ~Client();
	virtual void OnReceive(int nErrorCode);
	char buffer[10000000];//»º´æ
	virtual void OnSend(int nErrorCode);
	int data_begin;
	int data_end;
	CString base64_decode(CString find);
	CString splitted_str[1000];
	void split_str(CString str);
	int line_no;
	int getCode(char c);
};


