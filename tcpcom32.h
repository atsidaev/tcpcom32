// tcpcom32.h : main header file for the TCPCOM32 application
//

#if !defined(AFX_TCPCOM32_H__8CB1AEE7_1183_489F_9AF2_7F20C5566EAE__INCLUDED_)
#define AFX_TCPCOM32_H__8CB1AEE7_1183_489F_9AF2_7F20C5566EAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTcpcom32App:
// See tcpcom32.cpp for the implementation of this class
//

class CTcpcom32App : public CWinApp
{
public:
	CTcpcom32App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTcpcom32App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTcpcom32App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TCPCOM32_H__8CB1AEE7_1183_489F_9AF2_7F20C5566EAE__INCLUDED_)
