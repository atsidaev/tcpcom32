// tcpcom32Dlg.h : header file
//

#if !defined(AFX_TCPCOM32DLG_H__E38E727D_AD97_4CDD_ACD5_C6699147D814__INCLUDED_)
#define AFX_TCPCOM32DLG_H__E38E727D_AD97_4CDD_ACD5_C6699147D814__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define WM_THREAD_NOTIFY (WM_USER+9)
#define WM_ICON_NOTIFY (WM_USER+10)

#define CMD_REFRESH 100
#define CMD_SET_IMAGE 101
#define CMD_SET_STATS 102
#define CMD_OPEN_PORT 103
#define CMD_CLOSE_PORT 104
#define CMD_SET_STATUS 105
#define CMD_TRAYICON 106

#define BMP_ROOT 0
#define BMP_PORT_CLOSED 1
#define BMP_PORT_OPEN 2
#define BMP_PORT_OPEN_LOCAL 3
#define BMP_PORT_OPEN_REMOTE 4
#define BMP_PORT_OPEN_BOTH 5
#define BMP_PORT_DISABLED 6
#define BMP_TCP_INFO 7
#define BMP_RS232_INFO 8
#define BMP_STATS 9
#define BMP_PORT_OPENING1 10
#define BMP_PORT_OPENING2 11
#define BMP_PORT_OPENING3 12
#define BMP_PORT_WARNING 13

#define MENU_SYSTEM 0
#define MENU_PORT 1
#define MENU_HELP 2

/////////////////////////////////////////////////////////////////////////////
// CTcpcom32Dlg dialog

class CTcpcom32Dlg : public CDialog
{
// Construction
public:
	CTcpcom32Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTcpcom32Dlg)
	enum { IDD = IDD_DIALOG_TCPCOM32 };
	CEdit	m_statusline;
	CTreeCtrl	m_treeview;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTcpcom32Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

private:
	NOTIFYICONDATA m_nid;
	
	BOOL m_isInitialized;
	DWORD m_dwCmdShow;

	HANDLE m_hMainEvent;
	HANDLE m_hMainThread;
	DWORD m_dwMainThreadId;

private:
	void UpdateMenu(LPSTR lpszPortName, struct CPortContext *lpContext);

	friend void ShutdownMainThread(CTcpcom32Dlg* lpTcpcom32Dlg);
	friend DWORD WINAPI InitializeThreadProc(LPVOID lpParam);
	friend DWORD WINAPI StopThreadProc(LPVOID lpParam);
	friend DWORD WINAPI AcceptThreadProc(LPVOID lpParam);
	friend DWORD WINAPI ClientThreadProc(LPVOID lpParam);
	friend DWORD WINAPI PortThreadProc(LPVOID lpParam);

// Implementation
protected:
	HICON m_hIcon;
	HICON m_hIconErr;

	// Generated message map functions
	//{{AFX_MSG(CTcpcom32Dlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnThreadNotification(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSystemRestart();
	afx_msg void OnSystemSettings();
	afx_msg void OnPortClose();
	afx_msg void OnPortSettings();
	afx_msg void OnPortConnection();
	afx_msg void OnPortEnable();
	afx_msg void OnPortDisable();
	afx_msg void OnRclickTreeview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTreeview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHelpAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TCPCOM32DLG_H__E38E727D_AD97_4CDD_ACD5_C6699147D814__INCLUDED_)
