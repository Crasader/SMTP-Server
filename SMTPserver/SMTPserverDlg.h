#include"Client.h"
#include"Server.h"
// SMTPserverDlg.h : 头文件
//

#pragma once


// CSMTPserverDlg 对话框
class CSMTPserverDlg : public CDialogEx
{
// 构造
public:
	CSMTPserverDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SMTPSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
