// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tcpcom32.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog


CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_lgpl = _T("");
	//}}AFX_DATA_INIT
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_EDIT_LGPL, m_lgpl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_lgpl = CString(
		"TCPCOM32 Serial Port to TCP/IP Connection Mapper." "\r\n"
		"Copyright (C) 2008 mepping.com" "\r\n"
		"Copyright (C) 2015 Alexander Tsidaev (Windows 7 compatibility)" "\r\n"
		"\r\n"
		"This program is free software; you can redistribute it and/or" "\r\n"
		"modify it under the terms of the GNU General Public License" "\r\n"
		"as published by the Free Software Foundation; either version 2" "\r\n"
		"of the License, or (at your option) any later version." "\r\n"
		"\r\n"
		"This program is distributed in the hope that it will be useful," "\r\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of" "\r\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." "\r\n"
		"See the GNU General Public License for more details." "\r\n"
		"\r\n"
		"You should have received a copy of the GNU General Public License" "\r\n"
		"along with this program; if not, write to the Free Software Foundation, Inc.," "\r\n"
		"51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA." "\r\n"
		"\r\n");

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
