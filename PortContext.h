// PortContext.h: interface for the CPortContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AsyncPort.h"
#include "ClientSocket.h"

#if !defined(AFX_PORTCONTEXT_H__BBF3973F_3CD3_4230_8B6A_14C08517EAA7__INCLUDED_)
#define AFX_PORTCONTEXT_H__BBF3973F_3CD3_4230_8B6A_14C08517EAA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct CPortContext  
{
public:
	CPortContext();
	virtual ~CPortContext();

	void Cleanup();

public:

	// COM Port
	BOOL isDisabled;
	CHAR szPortName[12];
	DCB *lpDcb;
	CAsyncPort *lpPort;

	// TCP Connection
	CClientSocket* lpSocket;
	LPSTR lpszRequest;
	LPSTR lpszResponse;

	// Port Thread
	HANDLE hEvent;
	HANDLE hThread;
	DWORD dwThreadId;

	// Dialog
	HTREEITEM htiPort;
	HTREEITEM htiSerial;
	HTREEITEM htiSocket;
	HTREEITEM htiStats;

	// Statistics
	DWORD dwPortInBytes;
	DWORD dwPortOutBytes;
	DWORD dwSockInBytes;
	DWORD dwSockOutBytes;

	// Spare pointers to cleanup
	LPVOID lpSpare[4];

};

#endif // !defined(AFX_PORTCONTEXT_H__BBF3973F_3CD3_4230_8B6A_14C08517EAA7__INCLUDED_)
