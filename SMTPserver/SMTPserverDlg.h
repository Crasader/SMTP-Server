#include"Client.h"
#include"Server.h"
// SMTPserverDlg.h : ͷ�ļ�
//

#pragma once


// CSMTPserverDlg �Ի���
class CSMTPserverDlg : public CDialogEx
{
// ����
public:
	CSMTPserverDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SMTPSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString dlg_log;
	CString dlg_message;
	CString dlg_content;
	CString dlg_annex;
	Server server;
};
