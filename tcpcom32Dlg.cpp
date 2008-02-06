// tcpcom32Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "tcpcom32.h"
#include "tcpcom32Dlg.h"
#include "AsyncPort.h"
#include "PortContext.h"
#include "SettingsDlg.h"
#include "AboutDlg.h"
#include "TcpIpDlg.h"
#include "Thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CSettingsDlg theSettingsDlg;


/////////////////////////////////////////////////////////////////////////////
// CTcpcom32Dlg dialog

CTcpcom32Dlg::CTcpcom32Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTcpcom32Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTcpcom32Dlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_PORT);
	m_hMainEvent = NULL;
	m_hMainThread = NULL;
	m_dwMainThreadId = 0;
	m_isInitialized = FALSE;
	m_dwCmdShow = SW_HIDE;
}

void CTcpcom32Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTcpcom32Dlg)
	DDX_Control(pDX, IDC_TREEVIEW, m_treeview);
	DDX_Control(pDX, IDC_EDIT_STATUSLINE, m_statusline);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTcpcom32Dlg, CDialog)
	//{{AFX_MSG_MAP(CTcpcom32Dlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_THREAD_NOTIFY, OnThreadNotification)
	ON_MESSAGE(WM_ICON_NOTIFY, OnTrayNotification)
	ON_MESSAGE(WM_SYSCOMMAND, OnSysCommand)
	ON_COMMAND(ID_SYSTEM_RESTART, OnSystemRestart)
	ON_COMMAND(ID_SYSTEM_SETTINGS, OnSystemSettings)
	ON_COMMAND(ID_PORT_CLOSE, OnPortClose)
	ON_COMMAND(ID_PORT_SETTINGS, OnPortSettings)
	ON_COMMAND(ID_PORT_CONNECTION, OnPortConnection)
	ON_COMMAND(ID_PORT_ENABLE, OnPortEnable)
	ON_COMMAND(ID_PORT_DISABLE, OnPortDisable)
	ON_NOTIFY(NM_RCLICK, IDC_TREEVIEW, OnRclickTreeview)
	ON_NOTIFY(NM_DBLCLK, IDC_TREEVIEW, OnDblclkTreeview)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREEVIEW, OnSelchangedTreeview)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_SYSTEM_EXIT, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTcpcom32Dlg message handlers

BOOL CTcpcom32Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	// Initialize Windows Sockets
	WSADATA data;
	WSAStartup(MAKEWORD(1,1), & data);

	// Add tray icon
	memset(&m_nid, 0, sizeof(NOTIFYICONDATA));
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hWnd = m_hWnd;
	m_nid.uID = IDR_MENU_TCPCOM32;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = WM_ICON_NOTIFY;
	m_nid.hIcon = m_hIcon;
	strcpy(m_nid.szTip, _T( "TcpCOM32" ));
	if(!Shell_NotifyIcon(NIM_ADD, &m_nid)) {
		exit(-1);
	}

	// Build image list
	CImageList* pImgLst = new CImageList();
	pImgLst->Create(16, 16, ILC_COLOR8, 6, 0);
 // BMP_ROOT
	CBitmap* pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_MONITOR);
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_PORT_CLOSED
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_CONN_ERROR); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_PORT_CONNECTED
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_CONN_OPEN); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_PORT_ACTIVE_LOCAL
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_CONN_OPEN_LOCAL); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_PORT_ACTIVE_REMOTE
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_CONN_OPEN_REMOTE); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_PORT_ACTIVE_BOTH
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_CONN_OPEN_BOTH); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_PORT_DISABLED
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_CONN_FORBIDDEN); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_TCP_INFO
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_NETCONN); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_RS232_INFO
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_PORT); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_STATS
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_STATS); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_PORT_OPENING1
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_CONN_OPENING1); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_PORT_OPENING2
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_CONN_OPENING2); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_PORT_OPENING3
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_CONN_OPENING3); 
	pImgLst->Add(pbm, RGB(0, 0, 0));
// BMP_PORT_WARNING
	pbm = new CBitmap();
	pbm->LoadBitmap(IDB_BITMAP_CONN_WARNING); 
	pImgLst->Add(pbm, RGB(0, 0, 0));

	// Set treeview image list
	m_treeview.SetImageList(pImgLst, TVSIL_NORMAL);

	// Initialize
	InitializeThreadProc((LPVOID)this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTcpcom32Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTcpcom32Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTcpcom32Dlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	// Resize controls
	if (m_isInitialized) {
		RECT statusRect, treeRect;
		m_treeview.GetWindowRect(&treeRect);
		m_statusline.GetWindowRect(&statusRect);
		LONG statusHeight = statusRect.bottom - statusRect.top;
		m_treeview.SetWindowPos(&wndTopMost, 0, 0, cx, cy - statusHeight, SWP_NOZORDER );	
		m_statusline.SetWindowPos(&wndTopMost, 0, cy - statusHeight, cx, statusHeight, SWP_NOZORDER );	
	}
}

void CTcpcom32Dlg::OnDestroy() 
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here

	OnOK();
}

LRESULT CTcpcom32Dlg::OnSysCommand(WPARAM wParam, LPARAM lParam) 
{
	if (SC_MINIMIZE == wParam) {
		ShowWindow(m_dwCmdShow = SW_HIDE);
	} else {
		CDialog::OnSysCommand(wParam, lParam);
	}
	return FALSE;
}

void CTcpcom32Dlg::OnSystemSettings() 
{
	int nResponse = theSettingsDlg.DoModal();
	if (nResponse == IDOK) {
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK	
	} else if (nResponse == IDCANCEL) {
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
}

void CTcpcom32Dlg::OnSystemRestart() 
{
	// Start initialize thread
	StartThread(InitializeThreadProc, (LPVOID)this, NULL, NULL, NULL);
}

void CTcpcom32Dlg::OnPortEnable() 
{
	HTREEITEM hti = m_treeview.GetSelectedItem();
	if (hti) {
		theSettingsDlg.SetPortDisabled(m_treeview.GetItemText(hti).GetBuffer(0), FALSE);
		m_treeview.SetItemImage(hti, BMP_PORT_CLOSED, BMP_PORT_CLOSED);
	}
	UpdateData(FALSE);
}

void CTcpcom32Dlg::OnPortDisable() 
{
	HTREEITEM hti = m_treeview.GetSelectedItem();
	if (hti) {
		CPortContext *lpContext = (CPortContext *)m_treeview.GetItemData(hti);
		if (lpContext) {
			lpContext->isDisabled = TRUE;
			if (lpContext->lpSocket) {
				LPHANDLE lpArgs = new HANDLE[2];
				lpArgs[0] = lpContext->hEvent;
				lpArgs[1] = lpContext->hThread;
				StartThread(StopThreadProc, lpArgs, NULL, NULL, NULL);
			}
			theSettingsDlg.SetPortDisabled(lpContext->szPortName, TRUE);
		} else {
			theSettingsDlg.SetPortDisabled(m_treeview.GetItemText(hti).GetBuffer(0), TRUE);
		}
		m_treeview.SetItemImage(hti, BMP_PORT_DISABLED, BMP_PORT_DISABLED);
	}
	UpdateData(FALSE);
}

void CTcpcom32Dlg::OnPortClose() 
{
	HTREEITEM hti = m_treeview.GetSelectedItem();
	//HTREEITEM hti = m_treeview.GetNextItem(TVI_ROOT, TVGN_CARET);
	if (hti) {
		CPortContext *lpContext = (CPortContext *)m_treeview.GetItemData(hti);
		if (lpContext) {
			LPHANDLE lpArgs = new HANDLE[2];
			lpArgs[0] = lpContext->hEvent;
			lpArgs[1] = lpContext->hThread;
			StartThread(StopThreadProc, lpArgs, NULL, NULL, NULL);
		}
	}
}

void CTcpcom32Dlg::OnPortSettings() 
{
	HTREEITEM hti = m_treeview.GetSelectedItem();
	if (hti) {
		CPortContext *lpContext = (CPortContext *)m_treeview.GetItemData(hti);
		if (lpContext) {
			char buf[32];
			lpContext->lpPort->ShowConfigDialog(GetSafeHwnd());
			lpContext->lpPort->GetCommState(lpContext->lpDcb);
			theSettingsDlg.SetPortConfig(lpContext->szPortName, lpContext->lpDcb);
			if (hti = m_treeview.GetNextItem(lpContext->htiPort, TVGN_CHILD)) {
				m_treeview.SetItemText(hti, TEXT( CAsyncPort::FormatDcb(lpContext->lpDcb, buf, 32) ));
				UpdateData(FALSE);
			}
		}
	}
}

void CTcpcom32Dlg::OnPortConnection() 
{
	HTREEITEM hti = m_treeview.GetSelectedItem();
	if (hti) {
		CPortContext *lpContext = (CPortContext *)m_treeview.GetItemData(hti);
		if (lpContext) {
			CTcpIpDlg dlg;
			dlg.SetRequest(lpContext->lpszRequest);
			dlg.SetResponse(lpContext->lpszResponse);
			dlg.DoModal();
		}
	}

}

LRESULT CTcpcom32Dlg::OnTrayNotification(WPARAM wParam, LPARAM lParam) 
{
	switch (LOWORD(lParam))
	{
	case WM_RBUTTONDOWN: 
		{
			CMenu *lpMenu = GetMenu()->GetSubMenu(MENU_SYSTEM);
			if (!m_isInitialized) {
				lpMenu->EnableMenuItem(ID_SYSTEM_RESTART, MF_DISABLED|MF_GRAYED);
			}
			SetMenuDefaultItem(lpMenu->m_hMenu, 0, 0);
			CPoint pos;
			GetCursorPos(&pos);
			SetForegroundWindow();  
			TrackPopupMenu(lpMenu->m_hMenu, 0, pos.x, pos.y, 0, GetSafeHwnd(), NULL);
			PostMessage(WM_NULL, 0, 0);
		}
		break;

	case WM_LBUTTONDBLCLK:
		{
			m_dwCmdShow = (SW_HIDE == m_dwCmdShow) ? SW_RESTORE : SW_HIDE; 
			ShowWindow(m_dwCmdShow);
		}
		break;
	}

	return FALSE;
}

LRESULT CTcpcom32Dlg::OnThreadNotification(WPARAM wParam, LPARAM lParam) 
{
	int cmd = LOWORD(lParam);
	int arg = HIWORD(lParam);
	HTREEITEM hti;
	CPortContext *lpContext;
	char buf[64];

	switch (cmd) {
	case CMD_SET_IMAGE:
		hti = (HTREEITEM)wParam;
		m_treeview.SetItemImage(hti, arg, arg);
		UpdateData(FALSE);
		break;

	case CMD_OPEN_PORT:
		lpContext = (CPortContext *)wParam;
		hti = hti = m_treeview.GetChildItem(m_treeview.GetRootItem());
		for (; hti; hti = m_treeview.GetNextSiblingItem(hti)) {
			if (m_treeview.GetItemText(hti) == CString(lpContext->szPortName)) {
				lpContext->htiPort = hti;
				m_treeview.SetItemData(hti, (DWORD)lpContext);
				lpContext->htiSerial = m_treeview.InsertItem(TEXT( CAsyncPort::FormatDcb(lpContext->lpDcb, buf, 64) ), 
					BMP_RS232_INFO, BMP_RS232_INFO, lpContext->htiPort);
				m_treeview.SetItemData(lpContext->htiSerial, (unsigned long)lpContext);
				lpContext->htiSocket = m_treeview.InsertItem(TEXT( lpContext->lpSocket->GetPeerName(buf, 32) ), 
					BMP_TCP_INFO, BMP_TCP_INFO, lpContext->htiPort);
				m_treeview.SetItemData(lpContext->htiSocket, (unsigned long)lpContext);
				lpContext->htiStats = m_treeview.InsertItem(TEXT( "in=0 out=0" ), 
					BMP_STATS, BMP_STATS, lpContext->htiPort);
				m_treeview.SetItemData(lpContext->htiStats, (unsigned long)lpContext);
				//m_treeview.Expand(lpContext->hti, TVE_EXPAND);
				UpdateData(FALSE);
				break;
			}
		}
		break;

	case CMD_CLOSE_PORT:
		lpContext = (CPortContext *)wParam;
		m_treeview.SetItemData(lpContext->htiPort, (DWORD)NULL);
		m_treeview.DeleteItem(lpContext->htiSerial);
		m_treeview.DeleteItem(lpContext->htiSocket);
		m_treeview.DeleteItem(lpContext->htiStats);
		UpdateData(FALSE);
		break;
	
	case CMD_SET_STATS:
		lpContext = (CPortContext *)wParam;
		if (lpContext->htiStats) {
			wsprintf(buf, "in=%d out=%d", lpContext->dwPortInBytes, lpContext->dwPortOutBytes);
			m_treeview.SetItemText(lpContext->htiStats, TEXT( buf ));
			UpdateData(FALSE);
		}
		break;

	case CMD_SET_STATUS:
		m_statusline.SetWindowText(TEXT( (LPSTR)wParam ));
		UpdateData(FALSE);
		break;

	case CMD_REFRESH:
		UpdateData(FALSE);
		break;
	}

	return FALSE;
}


void CTcpcom32Dlg::OnDblclkTreeview(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	HTREEITEM hti = m_treeview.GetSelectedItem();
	if (hti) {
		CPortContext *lpContext = (CPortContext *)m_treeview.GetItemData(hti);
		if (lpContext) {
			if (hti == lpContext->htiSerial) {
				OnPortSettings();
			} else if (hti == lpContext->htiSocket) {
				OnPortConnection();
			}
		}
	}

	*pResult = 0;
}

void CTcpcom32Dlg::OnRclickTreeview(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	HTREEITEM hti = m_treeview.GetNextItem(TVI_ROOT, TVGN_DROPHILITE);
	if (hti) {
		m_treeview.SelectItem(hti);
	} else {
		hti = m_treeview.GetSelectedItem();
	}
	if (hti) {
		CPortContext *lpContext = (CPortContext *)m_treeview.GetItemData(hti);
		if (lpContext) {
			UpdateMenu(NULL, lpContext);
		} else {
			UpdateMenu(m_treeview.GetItemText(hti).GetBuffer(0), NULL);
		}
		CMenu *lpMenu = GetMenu()->GetSubMenu(MENU_PORT);
		SetMenuDefaultItem(lpMenu->m_hMenu, 0, 0);
		CPoint pos;
		GetCursorPos(&pos);
		SetForegroundWindow();  
		TrackPopupMenu(lpMenu->m_hMenu, 0, pos.x, pos.y, 0, GetSafeHwnd(), NULL);
		PostMessage(WM_NULL, 0, 0);
	}

	*pResult = 0;
}

void CTcpcom32Dlg::OnSelchangedTreeview(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	UpdateMenu(NULL, NULL);

	*pResult = 0;
}

void CTcpcom32Dlg::OnOK() 
{
	// TODO: Add extra validation here
	
	Shell_NotifyIcon(NIM_DELETE, &m_nid);

	CDialog::OnOK();
}

void CTcpcom32Dlg::UpdateMenu(LPSTR lpszPortName, CPortContext *lpContext)
{
	if (!lpContext) {
		HTREEITEM hti = m_treeview.GetSelectedItem();
		if (hti) {
			lpContext = (CPortContext *)m_treeview.GetItemData(hti);
		}
	}
	CMenu *lpMenu = GetMenu();
	CMenu *lpMenuPort = lpMenu->GetSubMenu(MENU_PORT);
	if (lpContext) {
		if (lpContext->isDisabled) { // disabled
		//if (theSettingsDlg.IsPortDisabled(CString(lpszPortName))) { // disabled
			lpMenuPort->EnableMenuItem(ID_PORT_CLOSE, MF_DISABLED|MF_GRAYED);
			lpMenuPort->EnableMenuItem(ID_PORT_SETTINGS, MF_DISABLED|MF_GRAYED);
			lpMenuPort->EnableMenuItem(ID_PORT_CONNECTION, MF_DISABLED|MF_GRAYED);
			lpMenuPort->EnableMenuItem(ID_PORT_ENABLE, MF_ENABLED);
			lpMenuPort->EnableMenuItem(ID_PORT_DISABLE, MF_DISABLED|MF_GRAYED);
		} else { // enabled
			if (lpContext->lpSocket) { // open
				lpMenuPort->EnableMenuItem(ID_PORT_CLOSE, MF_ENABLED);
				lpMenuPort->EnableMenuItem(ID_PORT_SETTINGS, MF_ENABLED);
				lpMenuPort->EnableMenuItem(ID_PORT_CONNECTION, MF_ENABLED);
			} else { // closed
				lpMenuPort->EnableMenuItem(ID_PORT_CLOSE, MF_DISABLED|MF_GRAYED);
				lpMenuPort->EnableMenuItem(ID_PORT_SETTINGS, MF_DISABLED|MF_GRAYED);
				lpMenuPort->EnableMenuItem(ID_PORT_CONNECTION, MF_DISABLED|MF_GRAYED);
			}
			lpMenuPort->EnableMenuItem(ID_PORT_ENABLE, MF_DISABLED|MF_GRAYED);
			lpMenuPort->EnableMenuItem(ID_PORT_DISABLE, MF_ENABLED);
		}
	} else if (lpszPortName) {
		if (theSettingsDlg.IsPortDisabled(lpszPortName)) { // disabled
			lpMenuPort->EnableMenuItem(ID_PORT_CLOSE, MF_DISABLED|MF_GRAYED);
			lpMenuPort->EnableMenuItem(ID_PORT_SETTINGS, MF_DISABLED|MF_GRAYED);
			lpMenuPort->EnableMenuItem(ID_PORT_CONNECTION, MF_DISABLED|MF_GRAYED);
			lpMenuPort->EnableMenuItem(ID_PORT_ENABLE, MF_ENABLED);
			lpMenuPort->EnableMenuItem(ID_PORT_DISABLE, MF_DISABLED|MF_GRAYED);
		} else { // enabled & closed
			lpMenuPort->EnableMenuItem(ID_PORT_CLOSE, MF_DISABLED|MF_GRAYED);
			lpMenuPort->EnableMenuItem(ID_PORT_SETTINGS, MF_DISABLED|MF_GRAYED);
			lpMenuPort->EnableMenuItem(ID_PORT_CONNECTION, MF_DISABLED|MF_GRAYED);
			lpMenuPort->EnableMenuItem(ID_PORT_ENABLE, MF_DISABLED|MF_GRAYED);
			lpMenuPort->EnableMenuItem(ID_PORT_DISABLE, MF_ENABLED);
		}
	} else {
		lpMenuPort->EnableMenuItem(ID_PORT_CLOSE, MF_DISABLED|MF_GRAYED);
		lpMenuPort->EnableMenuItem(ID_PORT_SETTINGS, MF_DISABLED|MF_GRAYED);
		lpMenuPort->EnableMenuItem(ID_PORT_CONNECTION, MF_DISABLED|MF_GRAYED);
		lpMenuPort->EnableMenuItem(ID_PORT_ENABLE, MF_DISABLED|MF_GRAYED);
		lpMenuPort->EnableMenuItem(ID_PORT_DISABLE, MF_DISABLED|MF_GRAYED);
	}

}

void CTcpcom32Dlg::OnHelpAbout() 
{
	// TODO: Add your command handler code here

	CAboutDlg dlg;
	dlg.DoModal();

}


