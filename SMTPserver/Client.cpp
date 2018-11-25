// Client.cpp : 实现文件
//

#include "stdafx.h"
#include "SMTPserver.h"
#include "Client.h"
#include <cstring>
#include<string>
#include<fstream>
#include<vector>

//#define USES_CONVERSION /
//#define T2A /
using namespace std;
// Client

Client::Client()
{
	alldata.Format(L"");
	start=0;
	data_begin = 0;
	data_end = 0;
	line_no = 0;
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
	Receive(buffer,sizeof(buffer),0);
	CString str(buffer);//将buffer转化成str
	//判断邮件是否结束
	if(start==1&&str.Right(5)!="\r\n.\r\n")
	{
		alldata+=str;
	}
	else if(start==1&&str.Right(5)=="\r\n.\r\n"){
		alldata+=str;
		str=alldata;
		char buff[100]="250 data\r\n";
		Send(buff,strlen(buff));//服务器响应250，表示请求的命令成功完成
		//dlg->dlg_log+="Server finished accepting\r\n";
		
		dlg->dlg_message+=str;
		split_str(str);
		int i=0;
		//判断base64出现的语句位置
		for(i = 16;i<line_no;i++)
		{
			CString s1=splitted_str[i].Right(6);
			if(splitted_str[i].Right(6) == "base64")
			{
				CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口	
				break;
			}
		
		}
		CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
		CString input = _T("");
		//将正文部分的base64编码全部加载进input内
		int j = 2;
		for(j = 2;j+i<line_no;j++)
		{
			CString cstring_temp = splitted_str[i+j];
			cstring_temp.Remove('\r');
			cstring_temp.Remove('\n');
			if(cstring_temp.GetLength()==0)
			{
				break;
			}
			input += cstring_temp;
		}
		input.TrimRight('=');
		//获得解码后的结果
		CString output = base64_decode(input);
		dlg->dlg_content += output;
		
		for(j=0;j<line_no;j++)
		{
			input = L"";
			CString filename = splitted_str[j];
			filename.Remove('\r');
			filename.Remove('\n');
			filename.TrimLeft();
			if(filename.Left(8) == "filename")
			{

				filename = filename.Right(filename.GetLength() - 9);//获取附件文件名
				filename .Remove('"');//将"去掉
				dlg->dlg_annex += (filename + L";");
				for(int i = 2;j+i<line_no;i++)
				{
					CString cstring_temp = splitted_str[i+j];
					cstring_temp.Remove('\r');
					cstring_temp.Remove('\n');
					cstring_temp.TrimRight('=');
					if(cstring_temp.GetLength()==0)
					{
						break;
					}
					input += cstring_temp;
				}
				//获得解码后的结果
				//将图片解码后写入本地
				decodeFileContent.clear();
				DecodePicture(input,decodeFileContent);
				std::fstream fout(filename, std::ios_base::out | std::ios_base::binary);;
				fout.write(static_cast<const char*>(&decodeFileContent[0]), decodeFileContent.size());
				fout.close();
				//showImage(filename);
				filenames.push_back(filename);
			}
		}
		
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//接收到HELO或EHLO的情况
	if(str.Left(4) == "EHLO"||str.Left(4) == "HELO")//在本例中是EHLO
	{
		dlg->dlg_log += "Client:";
		dlg->dlg_log += str;
		dlg->dlg_log += "\n";
		dlg->UpdateData(false);//将收到的内容显示在窗口里
		char* s1="250 Received HELO\r\n";
		Send(s1,strlen(s1));
		dlg->dlg_log += "Server:Received HELO\r\n";
		
		dlg->UpdateData(false);
		AsyncSelect(FD_READ);
	}
	//接收到MAIL FROM的情况
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
	//接收到RCPT TO的情况
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
	//接收到DATA的情况
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
		start=1;
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
		showImage();
		AsyncSelect(FD_READ);
	}
	CAsyncSocket::OnReceive(nErrorCode);
}


void Client::OnSend(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	char *s="220 ready\r\n";
	Send(s,strlen(s));
	AsyncSelect(FD_READ);
	CAsyncSocket::OnSend(nErrorCode);
}

CString Client::base64_decode(CString input)
{

	static char encode[64]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N',
                        'O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b',
                        'c','d','e','f','g','h','i','j','k','l','m','n','o','p',
                        'q','r','s','t','u','v','w','x','y','z','0','1','2','3',
                        '4','5','6','7','8','9','+','/'};
	int input_len = input.GetLength();
	int * int_input = new int [input_len]();
	//将input的编码依照encode表转化成int数组类型。
	for(int i = 0;i<input_len;i++)
	{
		int_input[i] = getCode(input[i]);
	}
	int * bin_input = new int[6*input_len]();
	//bin_input将int_input存储的内容转化成二进制
	CString test;
	for(int i=0 ;i<input_len;i++)
	{
		int c = int_input[i];
		//test += c;
		for(int j = 0; j < 6; j++)
		{
			bin_input[i*6+5-j] = int(c)%2;
			test += char(bin_input[i*6+j]+'0');
			
			c>>=1;
		}
	}
	
	//确定output长度
	int output_len;
	if((6 * input_len) %8 ==0)
	{
		output_len = 6*input_len/8;
	}
	else
	{
		output_len = 6*input_len/8+1;
	}
	int *bin_output = new int[8*output_len]();
	//bin_output盛放bin_input补0后的值
	for(int i=0;i<input_len*6;i++)
	{
		bin_output[i] = bin_input[i];
	}
	//
	int i = 0;
	//使用unsigned char*从而支持中文
	unsigned char * output= new unsigned char [output_len+1]();
	for(i=0;i<output_len;i++)
	{
		int temp_output = 0;
		int mul = 1;
		for(int j = 0;j<8;j++)
		{
			temp_output += bin_output[8*i+7-j]*mul;
			mul *= 2;
		}
		char c = (char) temp_output;
		output [i]= c;
	}
	output[i] = 0;
	//释放空间
	delete []int_input;
	int_input = NULL;
	delete []bin_input ;
	bin_input = NULL;
	delete []bin_output;
	bin_output = NULL;
	CString return_output = CString(output);
	delete []output;
	output = NULL;
	return return_output;

}


//将报文分隔开，存放到splitted_str里
void Client::split_str(CString str)
{
	CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd();
	CString szGet;	
	CString szText = str;
	bool result = true;	//判断是否分割完
	line_no = 0;
	//将报文按行分割存储到splitted_str里
	while(result)
	{
		result = AfxExtractSubString(szGet, szText, line_no, '\n');	//以/n为界分割字符串
		result = AfxExtractSubString(szGet, szText, line_no, '\r');	//将/r去除掉
		splitted_str[line_no] = szGet;
		line_no++;
		CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //获取主窗口
	}
}

//将字符转化成base64对应编码
int Client::getCode(char c)
{
	static char encode[64]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N',
                        'O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b',
                        'c','d','e','f','g','h','i','j','k','l','m','n','o','p',
                        'q','r','s','t','u','v','w','x','y','z','0','1','2','3',
                        '4','5','6','7','8','9','+','/'};
	for(int i = 0; i < 64;i++)
	{
		if(encode[i] == c)
		{
			return i;
		}
	}
}



void Client::DecodePicture(CString input, vector<char>& bytes)
{
	static char encode[64]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N',
                        'O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b',
                        'c','d','e','f','g','h','i','j','k','l','m','n','o','p',
                        'q','r','s','t','u','v','w','x','y','z','0','1','2','3',
                        '4','5','6','7','8','9','+','/'};
	int input_len = input.GetLength();
	int * int_input = new int [input_len]();
	//将input的编码依照encode表转化成int数组类型。
	for(int i = 0;i<input_len;i++)
	{
		int_input[i] = getCode(input[i]);
	}
	int * bin_input = new int[6*input_len]();
	//bin_input将int_input存储的内容转化成二进制
	CString test;
	for(int i=0 ;i<input_len;i++)
	{
		int c = int_input[i];
		//test += c;
		for(int j = 0; j < 6; j++)
		{
			bin_input[i*6+5-j] = int(c)%2;
			test += char(bin_input[i*6+j]+'0');
			
			c>>=1;
		}
	}
	
	//确定output长度
	int output_len;
	if((6 * input_len) %8 ==0)
	{
		output_len = 6*input_len/8;
	}
	else
	{
		output_len = 6*input_len/8+1;
	}
	int *bin_output = new int[8*output_len]();
	//bin_output盛放bin_input补0后的值
	for(int i=0;i<input_len*6;i++)
	{
		bin_output[i] = bin_input[i];
	}
	//
	int i = 0;
	//使用unsigned char*从而支持中文
	unsigned char * output= new unsigned char [output_len+1]();
	for(i=0;i<output_len;i++)
	{
		int temp_output = 0;
		int mul = 1;
		for(int j = 0;j<8;j++)
		{
			temp_output += bin_output[8*i+7-j]*mul;
			mul *= 2;
		}
		char c = (char) temp_output;
		output [i]= c;
	}
	output[i] = 0;
	//释放空间
	delete []int_input;
	int_input = NULL;
	delete []bin_input ;
	bin_input = NULL;
	delete []bin_output;
	bin_output = NULL;
	for(int i = 0;i<output_len;i++)
	{
		bytes.push_back(output[i]);
	}
	delete []output;
	output = NULL;
}


void Client::showImage()
{
	CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd();
	SetStretchBltMode(dlg->dlg_picture.GetDC()->GetSafeHdc(), HALFTONE);
	CRect dest;
	dlg->dlg_picture.GetClientRect(&dest);
	int width = dest.Width()/3;
	int height = dest.Height()/3;
	for(int i = 0;i<filenames.size();i++)
	{
		CImage image;
		image.Load(filenames[i]);
		if (!image.IsNull()) 
		{
			

			int img_width=image.GetWidth();
			int img_height=image.GetHeight();
			// 找出宽和高中的较大值者
			int Sourcemax=(img_width>img_height)?img_width:img_height;
			
			int destmax=(width<height)?width:height;
			// 计算将图片缩放到TheImage区域所需的比例因子
			float scale = (float) ( (float) Sourcemax / (float)destmax );

			// 缩放后图片的宽和高
			int neww = (int)( img_width/scale );
			int newh= (int)( img_height/scale );

			// 为了将缩放后的图片存入正中部位，需计算图片在 rectDraw 左上角的期望坐标值
			int tlx = (neww > newh)? 0: (int)(width-neww)/2;
			int tly = (neww > newh)? (int)(height-newh)/2: 0;

			// 设置 rectDraw,用来存入图片image
			CRect rect = dest;
				rect.SetRect(i%3*width, i/3*height, i%3*width+neww, i/3*height+newh);

			image.Draw(dlg->dlg_picture.GetDC()->GetSafeHdc(), rect);

		}
	}
}
