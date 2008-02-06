// SettingsDlg.h : header file
//

#if !defined(AFX_SETTINGSDLG_H__C79CE2DE_F30B_4A13_8A7F_CE281C345DA3__INCLUDED_)
#define AFX_SETTINGSDLG_H__C79CE2DE_F30B_4A13_8A7F_CE281C345DA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
// Construction
public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	
	void SetIniFilePath(CString iniFilePath);

	BOOL IsServerMode();
	void SetServerMode(BOOL isServerMode);

	LPSTR GetLocalTcpPort(LPSTR lpszBuffer, int nBufferSize);
	void SetLocalTcpPort(LPSTR lpszBuffer);
	
	LPSTR GetLocalIpAddress(LPSTR lpszBuffer, int nBufferSize);
	void SetLocalIpAddress(LPSTR lpszBuffer);
	
	BOOL IsClientMode();
	void SetClientMode(BOOL isClientMode);

	LPSTR GetRemoteTcpPort(LPSTR lpszBuffer, int nBufferSize);
	void SetRemoteTcpPort(LPSTR lpszBuffer);
	
	LPSTR GetRemoteIpAddress(LPSTR lpszBuffer, int nBufferSize);
	void SetRemoteIpAddress(LPSTR lpszBuffer);

	BOOL IsProxyEnabled();
	void SetProxyEnabled(BOOL isProxyEnabled);

	LPSTR GetProxyTcpPort(LPSTR lpszBuffer, int nBufferSize);
	void SetProxyTcpPort(LPSTR lpszBuffer);
	
	LPSTR GetProxyIpAddress(LPSTR lpszBuffer, int nBufferSize);
	void SetProxyIpAddress(LPSTR lpszBuffer);

	BOOL IsAuthEnabled();
	void SetAuthEnabled(BOOL isAuthEnabled);

	LPSTR GetUserName(LPSTR lpszBuffer, int nBufferSize);
	void SetUserName(LPSTR lpszBuffer);

	LPSTR GetPassword(LPSTR lpszBuffer, int nBufferSize);
	void SetPassword(LPSTR lpszBuffer);

	LPSTR GetWorkgroup(LPSTR lpszBuffer, int nBufferSize);
	void SetWorkgroup(LPSTR lpszBuffer);

	LPSTR GetWorkstation(LPSTR lpszBuffer, int nBufferSize);
	void SetWorkstation(LPSTR lpszBuffer);

	LPSTR GetAuthScheme(LPSTR lpszBuffer, int nBufferSize);
	void SetAuthScheme(LPSTR lpszBuffer);
	
	BOOL IsLogEnabled();
	void SetLogEnabled(BOOL isLogEnabled);

	LPSTR GetLogDir(LPSTR lpszBuffer, int nBufferSize);
	void SetLogDir(LPSTR lpszLogDir);

	BOOL IsPortDisabled(LPSTR lpszPortName);
	void SetPortDisabled(LPSTR lpszPortName, BOOL isDisabled);

	BOOL GetPortConfig(LPSTR lpszPortName, DCB *lpDcb);
	BOOL SetPortConfig(LPSTR lpszPortName, DCB *lpDcb);

// Dialog Data
	//{{AFX_DATA(CSettingsDlg)
	enum { IDD = IDD_DIALOG_SETTINGS };
	CButton	m_radioNTLMv2;
	CButton	m_radioNTLM;
	CButton	m_radioBasic;
	CButton	m_checkAuth;
	CEdit	m_editWorkstation;
	CEdit	m_editWorkgroup;
	CButton	m_checkServerMode;
	CButton	m_checkProxy;
	CButton	m_checkLog;
	CButton	m_checkClientMode;
	CEdit	m_editLocalPort;
	CEdit	m_editLocalIp;
	CEdit	m_editRemotePort;
	CEdit	m_editRemoteIp;
	CEdit	m_editProxyPort;
	CEdit	m_editProxyIp;
	CEdit	m_editUserName;
	CEdit	m_editPassword;
	CEdit	m_editLogDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	CString m_iniFilePath;

	void UpdateControls(BOOL bSaveAndValidate);
	void EnableControls();

	BOOL ReadConfigString(LPSTR lpszSection, LPSTR lpszName, LPSTR lpszBuffer, DWORD dwBufferSize);
	BOOL WriteConfigString(LPSTR lpszSection, LPSTR lpszName, LPSTR lpszBuffer, BOOL bCreate = TRUE);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettingsDlg)
	afx_msg void OnCheckLogEnabled();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckServerMode();
	afx_msg void OnCheckClientMode();
	afx_msg void OnCheckProxy();
	afx_msg void OnCheckAuth();
	afx_msg void OnRadioBasic();
	afx_msg void OnRadioNtlm();
	afx_msg void OnRadioNtlmv2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSDLG_H__C79CE2DE_F30B_4A13_8A7F_CE281C345DA3__INCLUDED_)
