// PortContext.cpp: implementation of the CPortContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tcpcom32.h"
#include "PortContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPortContext::CPortContext()
{
	isDisabled = FALSE;
	szPortName[0] = 0;
	
	lpDcb = NULL;
	lpPort = NULL;

	lpSocket = NULL;
	lpszRequest = NULL;
	lpszResponse = NULL;

	htiPort = NULL;
	htiSerial = NULL;
	htiSocket = NULL;
	htiStats = NULL;

	hEvent = NULL;
	hThread = NULL;
	dwThreadId = 0;
	
	dwPortInBytes = 0;
	dwPortOutBytes = 0;
	dwSockInBytes = 0;
	dwSockOutBytes = 0;
	
	for (int i = 0; i < 4; i ++) {
		lpSpare[i] = NULL;
	}
}

CPortContext::~CPortContext()
{

}

void CPortContext::Cleanup()
{
	if (lpDcb) {
		delete lpDcb;
		lpDcb = NULL;
	}
	if (lpPort) {
		delete lpPort;
		lpPort = NULL;
	}
	if (lpSocket) {
		delete lpSocket;
		lpSocket = NULL;
	}
	if (lpszRequest) {
		delete lpszRequest;
		lpszRequest = NULL;
	}
	if (lpszResponse) {
		delete lpszResponse;
		lpszResponse = NULL;
	}

	if (hEvent) {
		CloseHandle(hEvent);
		hEvent = NULL;
	}
	if (hThread) {
		CloseHandle(hThread);
		hThread = NULL;
	}
	dwThreadId = 0;
	
	dwPortInBytes = 0;
	dwPortOutBytes = 0;
	dwSockInBytes = 0;
	dwSockOutBytes = 0;

	for (int i = 0; i < 4; i ++) {
		if (lpSpare[i]) {
			delete (lpSpare[i]);
			lpSpare[i] = NULL;
		}
	}

}

