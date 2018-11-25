// Client.cpp : ʵ���ļ�
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
	if(start==1&&str.Right(5)!="\r\n.\r\n")
	{
		alldata+=str;
	}
	else if(start==1&&str.Right(5)=="\r\n.\r\n"){
		alldata+=str;
		str=alldata;
		char buff[100]="250 data\r\n";
		Send(buff,strlen(buff));//��������Ӧ250����ʾ���������ɹ����
		//dlg->dlg_log+="Server finished accepting\r\n";
		
		dlg->dlg_message+=str;
		split_str(str);
		int i=0;
		//�ж�base64���ֵ����λ��
		for(i = 16;i<line_no;i++)
		{
			CString s1=splitted_str[i].Right(6);
			if(splitted_str[i].Right(6) == "base64")
			{
				CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������	
				break;
			}
		
		}
		CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd(); //��ȡ������
		CString input = _T("");
		//�����Ĳ��ֵ�base64����ȫ�����ؽ�input��
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
		//��ý����Ľ��
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

				filename = filename.Right(filename.GetLength() - 9);//��ȡ�����ļ���
				filename .Remove('"');//��"ȥ��
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
				//��ý����Ľ��
				//��ͼƬ�����д�뱾��
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
	// TODO: �ڴ����ר�ô����/����û���
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
	//��input�ı�������encode��ת����int�������͡�
	for(int i = 0;i<input_len;i++)
	{
		int_input[i] = getCode(input[i]);
	}
	int * bin_input = new int[6*input_len]();
	//bin_input��int_input�洢������ת���ɶ�����
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
	
	//ȷ��output����
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
	//bin_outputʢ��bin_input��0���ֵ
	for(int i=0;i<input_len*6;i++)
	{
		bin_output[i] = bin_input[i];
	}
	//
	int i = 0;
	//ʹ��unsigned char*�Ӷ�֧������
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
	//�ͷſռ�
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


//�����ķָ�������ŵ�splitted_str��
void Client::split_str(CString str)
{
	CSMTPserverDlg* dlg=(CSMTPserverDlg*)AfxGetApp()->GetMainWnd();
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

//���ַ�ת����base64��Ӧ����
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
	//��input�ı�������encode��ת����int�������͡�
	for(int i = 0;i<input_len;i++)
	{
		int_input[i] = getCode(input[i]);
	}
	int * bin_input = new int[6*input_len]();
	//bin_input��int_input�洢������ת���ɶ�����
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
	
	//ȷ��output����
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
	//bin_outputʢ��bin_input��0���ֵ
	for(int i=0;i<input_len*6;i++)
	{
		bin_output[i] = bin_input[i];
	}
	//
	int i = 0;
	//ʹ��unsigned char*�Ӷ�֧������
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
	//�ͷſռ�
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
			// �ҳ���͸��еĽϴ�ֵ��
			int Sourcemax=(img_width>img_height)?img_width:img_height;
			
			int destmax=(width<height)?width:height;
			// ���㽫ͼƬ���ŵ�TheImage��������ı�������
			float scale = (float) ( (float) Sourcemax / (float)destmax );

			// ���ź�ͼƬ�Ŀ�͸�
			int neww = (int)( img_width/scale );
			int newh= (int)( img_height/scale );

			// Ϊ�˽����ź��ͼƬ�������в�λ�������ͼƬ�� rectDraw ���Ͻǵ���������ֵ
			int tlx = (neww > newh)? 0: (int)(width-neww)/2;
			int tly = (neww > newh)? (int)(height-newh)/2: 0;

			// ���� rectDraw,��������ͼƬimage
			CRect rect = dest;
				rect.SetRect(i%3*width, i/3*height, i%3*width+neww, i/3*height+newh);

			image.Draw(dlg->dlg_picture.GetDC()->GetSafeHdc(), rect);

		}
	}
}
