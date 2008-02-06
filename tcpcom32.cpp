// tcpcom32.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "tcpcom32.h"
#include "tcpcom32Dlg.h"
#include "SettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTcpcom32App

BEGIN_MESSAGE_MAP(CTcpcom32App, CWinApp)
	//{{AFX_MSG_MAP(CTcpcom32App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTcpcom32App construction

CTcpcom32App::CTcpcom32App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTcpcom32App object

CTcpcom32App theApp;
CSettingsDlg theSettingsDlg;

/////////////////////////////////////////////////////////////////////////////
// CTcpcom32App initialization

BOOL CTcpcom32App::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// full path to .ini file
	CString iniFilePath(m_pszHelpFilePath);
	iniFilePath.Replace("/", "\\");
	int sepId = iniFilePath.ReverseFind('\\');
	if (-1 == sepId) {
		iniFilePath = CString("tcpcom32.ini");
	} else {
		iniFilePath = iniFilePath.Left(sepId) + CString("\\tcpcom32.ini");
	}
	theSettingsDlg.SetIniFilePath(iniFilePath);

	CTcpcom32Dlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
