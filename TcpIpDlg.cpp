// TcpIpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tcpcom32.h"
#include "TcpIpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTcpIpDlg dialog


CTcpIpDlg::CTcpIpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTcpIpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTcpIpDlg)
	m_request = _T("");
	m_response = _T("");
	//}}AFX_DATA_INIT
}


void CTcpIpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTcpIpDlg)
	DDX_Text(pDX, IDC_EDIT_REQUEST, m_request);
	DDX_Text(pDX, IDC_EDIT_RESPONSE, m_response);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTcpIpDlg, CDialog)
	//{{AFX_MSG_MAP(CTcpIpDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTcpIpDlg message handlers

BOOL CTcpIpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTcpIpDlg::SetRequest(LPSTR lpszRequest)
{
	m_request = CString(lpszRequest);
}

void CTcpIpDlg::SetResponse(LPSTR lpszResponse)
{
	m_response = CString(lpszResponse);
}
