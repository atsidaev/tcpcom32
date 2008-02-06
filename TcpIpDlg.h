// TcpIpDlg.h : header file
//

#if !defined(AFX_TCPIPDLG_H__F64D4465_2DFA_42CB_AB88_3FCBC0DDB43A__INCLUDED_)
#define AFX_TCPIPDLG_H__F64D4465_2DFA_42CB_AB88_3FCBC0DDB43A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTcpIpDlg dialog

class CTcpIpDlg : public CDialog
{
// Construction
public:
	CTcpIpDlg(CWnd* pParent = NULL);   // standard constructor

	void SetRequest(LPSTR lpszRequest);
	void SetResponse(LPSTR lpszResponse);

// Dialog Data
	//{{AFX_DATA(CTcpIpDlg)
	enum { IDD = IDD_DIALOG_TCPIP };
	CString	m_request;
	CString	m_response;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTcpIpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTcpIpDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TCPIPDLG_H__F64D4465_2DFA_42CB_AB88_3FCBC0DDB43A__INCLUDED_)
