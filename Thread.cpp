// Thread.cpp: thread main procs.
// Last Revision: y2008 m01 d29
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ServerSocket.h"
#include "SettingsDlg.h"
#include "PortContext.h"
#include "tcpcom32Dlg.h"
#include "base64.h"
#include "NTLM.h"
#include "HTTP.h"
#include "Thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSettingsDlg theSettingsDlg;


BOOL StartThread(LPTHREAD_START_ROUTINE lpRoutine, LPVOID lpParam, LPHANDLE lpEvent, LPHANDLE lpThread, LPDWORD lpThreadId)
{
	// Clear output values
	if (lpEvent) {
		*lpEvent = NULL;
	}
	if (lpThread) {
		*lpThread = NULL;
	}
	if (lpThreadId) {
		*lpThreadId = 0;
	}

	// Create termination event
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == hEvent) {
		MessageBox(NULL, "Error creating termination event", "ERROR", MB_OK);
		return FALSE;
	}
	ResetEvent(hEvent);

	// Create thread
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, lpRoutine, lpParam, 0, &dwThreadId);
	if (NULL == hThread) {
		CloseHandle(hEvent);
		MessageBox(NULL, "Error starting thread", "ERROR", MB_OK);
		return FALSE;
	}

	// Set output values
	if (lpEvent) {
		*lpEvent = hEvent;
	}
	if (lpThread) {
		*lpThread = hThread;
	}
	if (lpThreadId) {
		*lpThreadId = dwThreadId;
	}

	return TRUE;
}

DWORD WINAPI StopThreadProc(LPVOID lpParam)
{
	HANDLE *lpArgs = (HANDLE *)lpParam;
	HANDLE hEvent = lpArgs[0];
	HANDLE hThread = lpArgs[1];
	delete lpArgs;

	if (NULL != hEvent) {
		if (0 == SetEvent(hEvent)) {
			MessageBox(NULL, "Error setting termination event", "ERROR", MB_OK);
			return -1;
		}
	}

	if (NULL != hThread) {
		DWORD dwExitCode = STILL_ACTIVE;
		while (GetExitCodeThread(hThread, & dwExitCode)) {
			if (STILL_ACTIVE != dwExitCode) {
				break;
			}
			Sleep(100);
		} 
	}

	return 0;
}

void ShutdownMainThread(CTcpcom32Dlg* lpTcpcom32Dlg)
{
	// Stop main thread
	if (lpTcpcom32Dlg->m_hMainEvent && lpTcpcom32Dlg->m_hMainThread) {
		LPHANDLE lpArgs = new HANDLE[2];
		lpArgs[0] = lpTcpcom32Dlg->m_hMainEvent;
		lpArgs[1] = lpTcpcom32Dlg->m_hMainThread;
		StopThreadProc(lpArgs);
		lpTcpcom32Dlg->m_hMainEvent = NULL;
		lpTcpcom32Dlg->m_hMainThread = NULL;
	}
}

DWORD WINAPI ClientThreadProc(LPVOID lpParam)
{
	// Decode lpParam
	CTcpcom32Dlg *lpTcpcom32Dlg = (CTcpcom32Dlg *)lpParam;
	CTreeCtrl& tree = lpTcpcom32Dlg->m_treeview;

	// Debug
	ASSERT(theSettingsDlg.IsClientMode());

	// Minimize on start
	if (lpTcpcom32Dlg->m_dwCmdShow == SW_HIDE) {
		lpTcpcom32Dlg->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}

	// Status line & trayicon
	lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Running", MAKELONG(CMD_SET_STATUS,0));
	lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)0, MAKELONG(CMD_TRAYICON,0));

	// Main loop
	HTREEITEM htiPort = NULL;
	HANDLE hThread = NULL;
	char buf[16];
	char *lpszPortName = NULL;
	int curbmp = BMP_PORT_OPENING1;
	int nxtbmp = BMP_PORT_OPENING2;
	for (BOOL loop = TRUE; loop; ) {

		// Check termination
		if (WAIT_OBJECT_0 == WaitForSingleObject(lpTcpcom32Dlg->m_hMainEvent, 666)) {
			loop = FALSE;
			break;
		}

		// Wait connection setup
		if (htiPort && hThread && lpszPortName) {
			CPortContext *lpContext = (CPortContext *)tree.GetItemData(htiPort);
			DWORD dwExitCode = STILL_ACTIVE;
			if (GetExitCodeThread(hThread, & dwExitCode)) {
				if (STILL_ACTIVE == dwExitCode && !lpContext) {
					// Animate treeview icon
					lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(htiPort), MAKELONG(CMD_SET_IMAGE,curbmp));
					if (curbmp < nxtbmp) {
						curbmp = nxtbmp;
						nxtbmp = (BMP_PORT_OPENING2 == nxtbmp ? BMP_PORT_OPENING3 : BMP_PORT_OPENING2);
					} else {
						curbmp = nxtbmp;
						nxtbmp = (BMP_PORT_OPENING2 == nxtbmp ? BMP_PORT_OPENING1 : BMP_PORT_OPENING2);
					}
					continue;
				}
			}
			// Reset item image
			if (!lpContext) {
				if (theSettingsDlg.IsPortDisabled(lpszPortName)) {
					lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_DISABLED));
				} else {
					lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_CLOSED));
				}
			}
			htiPort = NULL;
			hThread = NULL;
			lpszPortName = NULL;
		}

		// Open connections
		HTREEITEM hti = tree.GetChildItem(tree.GetRootItem()); 
		for (; hti; hti = tree.GetNextSiblingItem(hti)) {
			CPortContext *lpContext = (CPortContext *)tree.GetItemData(hti);
			strcpy(buf, tree.GetItemText(hti));
			if (!lpContext && !theSettingsDlg.IsPortDisabled(buf)) {
				// Spawn port thread
				LPVOID *lpArgs = new LPVOID[6];
				lpArgs[0] = NULL;
				lpArgs[1] = lpTcpcom32Dlg;
				lpArgs[2] = hti;
				StartThread(PortThreadProc, lpArgs, (LPHANDLE)(lpArgs+3), (LPHANDLE)(lpArgs+4), (LPDWORD)(lpArgs+5));
				htiPort = hti;
				hThread = (HANDLE)(lpArgs[4]);
				lpszPortName = buf;
				break;
			}
		}
	}

	return 0;
}

DWORD WINAPI AcceptThreadProc(LPVOID lpParam)
{
	// Decode lpParam
	CTcpcom32Dlg *lpTcpcom32Dlg = (CTcpcom32Dlg *)lpParam;

	// Debug
	ASSERT(theSettingsDlg.IsServerMode());

	// Initialize Windows Sockets
	WSADATA data;
	WSAStartup(MAKEWORD(1,1), & data);

	// Minimize on start
	if (lpTcpcom32Dlg->m_dwCmdShow == SW_HIDE) {
		lpTcpcom32Dlg->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}

	// Status line && trayicon
	lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Running", MAKELONG(CMD_SET_STATUS,0));
	lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)0, MAKELONG(CMD_TRAYICON,0));

	// Open server socket
	CServerSocket serverSocket;
	char buf[256];
	if (-1 == serverSocket.Open(theSettingsDlg.GetLocalIpAddress(buf, 128), 
		atoi(theSettingsDlg.GetLocalTcpPort(buf+128, 128)))) {
		MessageBox(NULL, "Error opening server socket", "ERROR", MB_OK);
		return -1;
	}
	serverSocket.SetNonBlocking();

	// Main loop
	CClientSocket* lpSocket = new CClientSocket();
	for (BOOL loop = TRUE; loop; ) {

		// Check termination
		if (WAIT_OBJECT_0 == WaitForSingleObject(lpTcpcom32Dlg->m_hMainEvent, 0)) {
			loop = FALSE;
			break;
		}

		// Wait incoming connections
		if (-1 == serverSocket.WaitForPendingData(1, 0)) {
			continue;
		}

		// Accept connection
		if (0 == serverSocket.Accept(lpSocket)) {

			// Spawn thread
				LPVOID *lpArgs = new LPVOID[6];
				lpArgs[0] = lpSocket;
				lpArgs[1] = lpTcpcom32Dlg;
				lpArgs[2] = NULL;
				StartThread(PortThreadProc, lpArgs, (LPHANDLE)(lpArgs+3), (LPHANDLE)(lpArgs+4), (LPDWORD)(lpArgs+5));
			
			// Yet another socket
			lpSocket = new CClientSocket();
		}
	}

	// Close server socket
	serverSocket.Close();

	return 0;
}

DWORD WINAPI PortThreadProc(LPVOID lpParam)
{
#define SHOW_PORT_ACTIVITY	

#define BUFLEN				8192

#define SPARE_BUF			0
#define SPARE_LPPARAM		1
#define SPARE_EXTRA			2

#define MEMSZ_URL			256
#define MEMSZ_IP			128
#define MEMSZ_PORT			16
#define MEMSZ_USERNAME		64
#define MEMSZ_PASSWORD		64
#define MEMSZ_WORKGROUP		64
#define MEMSZ_WORKSTATION	64
#define MEMSZ_AUTH			1024
#define MEMSZ_AUTHSCHEME	32
#define MEMSZ_CONNECTION	32

	// Decode lpParam
	LPVOID *lpArgs = (LPVOID *)lpParam;
	CClientSocket *lpSocket = (CClientSocket *)lpArgs[0];
	CTcpcom32Dlg *lpTcpcom32Dlg = (CTcpcom32Dlg *)lpArgs[1];
	HTREEITEM htiPort = (HTREEITEM)(lpArgs[2]);
	HANDLE hEvent = (HANDLE)(lpArgs[3]);
	HANDLE hThread = (HANDLE)(lpArgs[4]);
	DWORD dwThreadId = (DWORD)(lpArgs[5]);

	// Debug
	ASSERT(lpSocket ? theSettingsDlg.IsServerMode() : theSettingsDlg.IsClientMode());

	// Allocate buffer
	char *buf = new char[BUFLEN+BUFLEN];
	char *mem = buf + BUFLEN;

	// Create context
	CPortContext ctx;
	ctx.lpSocket = lpSocket;
	ctx.hEvent = hEvent;
	ctx.hThread = hThread;
	ctx.htiPort = htiPort;
	ctx.dwThreadId = dwThreadId;
	ctx.lpSpare[SPARE_LPPARAM] = lpParam;
	ctx.lpSpare[SPARE_BUF] = buf;

	// Startup Windows Sockets Library
	WSADATA data;
	WSAStartup(MAKEWORD(1,1), & data);

	// Open connection in not yet opened
	if (!lpSocket) {
		lpSocket = ctx.lpSocket = new CClientSocket();

		// Use HTTP proxy
		if (theSettingsDlg.IsProxyEnabled()) {

			// Connect to Proxy Address
			LPSTR lpszIp = mem;
			LPSTR lpszPort = lpszIp + MEMSZ_IP;
			if (-1 == lpSocket->Connect(
				theSettingsDlg.GetProxyIpAddress(lpszIp, MEMSZ_IP), 
				atoi(theSettingsDlg.GetProxyTcpPort(lpszPort, MEMSZ_PORT)))) 
			{
				ShutdownMainThread(lpTcpcom32Dlg);
				lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Proxy Unreachable", MAKELONG(CMD_SET_STATUS,0));
				lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_WARNING));
				ctx.Cleanup();
				return -1;
			}

			// FIONBIO
			lpSocket->SetNonBlocking();

			// Send CONNECT request to proxy
			LPSTR lpszUrl = mem;
			lpszIp = lpszUrl + MEMSZ_URL;
			lpszPort = lpszIp + MEMSZ_IP;
			wsprintf(lpszUrl, "%s:%s", 
				theSettingsDlg.GetRemoteIpAddress(lpszIp, MEMSZ_IP),
				theSettingsDlg.GetRemoteTcpPort(lpszPort, MEMSZ_PORT));
			LPSTR lpszRequest = FormatRequest("CONNECT", lpszUrl, lpszUrl, NULL, buf, BUFLEN);
			lpSocket->Send(lpszRequest, strlen(lpszRequest));

			// Read response from proxy
			DWORD dwHeaderLength = 0;
			DWORD dwExtraLength = 0;
			switch (ReadHttpHeader(lpSocket, buf, BUFLEN, &dwHeaderLength, &dwExtraLength, 10000)) {
			case READHTTP_SUCCESS:
				break;
			case READHTTP_CONNCLOSED:
			case READHTTP_BUFFTOOSMALL:
			case READHTTP_TIMEDOUT:
			default:
				lpSocket->Close();
				ctx.Cleanup();
				return -1;
			}

			// Parse proxy response
			DWORD dwErrCode = 0;
			DWORD dwContentLength = 0;
			LPSTR lpmszAuth = mem;
			LPSTR lpszConnection = lpmszAuth + MEMSZ_AUTH;
			if (!ParseResponse(&dwErrCode, lpmszAuth, &dwContentLength, lpszConnection, buf, dwHeaderLength)) {
				lpSocket->Close();
				ctx.Cleanup();
				return -1;
			}
			// Read response body
			BOOL bodyRead = FALSE;
			if (dwContentLength > 0) {
				bodyRead = TRUE;
				dwContentLength -= dwExtraLength;
				while (dwContentLength > 0) {
					DWORD count = lpSocket->Recv(buf + dwHeaderLength + dwExtraLength, dwContentLength, 0, 10000);
					if (count <= 0) {
						if (0 == count) {
							strcpy(lpszConnection, "close");
						}
						break;
					}
					dwExtraLength += count;
					dwContentLength -= count;
				}
			}

			// Re-open connection if necessary
			if (!stricmp(lpszConnection, "close")) {
				// Read remaining data
				while (!bodyRead && dwExtraLength + dwHeaderLength < BUFLEN) {
					DWORD count = lpSocket->Recv(buf + dwHeaderLength + dwExtraLength, BUFLEN - dwHeaderLength - dwExtraLength, 0, 10000);
					if (count <= 0) {
						break;
					}
					dwExtraLength += count;
				}
				lpSocket->Close();
				Sleep(100);

				// Connect to Proxy Address again
				lpszIp = lpmszAuth + MEMSZ_AUTH; // save lpmszAuth
				lpszPort = lpszIp + MEMSZ_IP;
				if (-1 == lpSocket->Connect(
						theSettingsDlg.GetProxyIpAddress(lpszIp, MEMSZ_IP), 
						atoi(theSettingsDlg.GetProxyTcpPort(lpszPort, MEMSZ_PORT)))) 
				{
					ShutdownMainThread(lpTcpcom32Dlg);
					lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Proxy Unreachable", MAKELONG(CMD_SET_STATUS,0));
					lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_WARNING));
					ctx.Cleanup();
					return -1;
				}
			}

			// 407 Proxy Authentication Required
			if (407 == dwErrCode) {

				// Authorization settings
				if (!theSettingsDlg.IsAuthEnabled()) {
					ShutdownMainThread(lpTcpcom32Dlg);
					lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Proxy Authentication Required", MAKELONG(CMD_SET_STATUS,0));
					lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_WARNING));
					ctx.Cleanup();
					return -1;
				}
				// Get Auth Scheme
				LPSTR lpszScheme = lpmszAuth + MEMSZ_AUTH; // save lpmszAuth
				theSettingsDlg.GetAuthScheme(lpszScheme, MEMSZ_AUTHSCHEME);


				// Parse Auth Multi String
				BOOL useBasic = FALSE;
				BOOL useNTLM = FALSE;
				BOOL useNTLMv2 = FALSE;
				for (LPSTR scheme = lpmszAuth; *scheme; scheme += 1 + strlen(scheme)) {
					if (strstr(scheme, "Basic") && 
						!stricmp(lpszScheme, "Basic")) {
						useBasic = TRUE;
					}
					if (strstr(scheme, "NTLM")) {
						if (!stricmp(lpszScheme, "NTLM")) {
							useNTLM = TRUE;
						} else if (!stricmp(lpszScheme, "NTLMv2")) {
							useNTLMv2 = TRUE;
						}
					}
				}

				// Basic authentication
				if (useBasic) {

					// Send base64 encoded username:password
					LPSTR lpszUserName = mem;
					LPSTR lpszPassword = lpszUserName + MEMSZ_USERNAME;
					wsprintf(lpmszAuth, "%s:%s", 
						theSettingsDlg.GetUserName(lpszUserName, MEMSZ_USERNAME),
						theSettingsDlg.GetPassword(lpszPassword, MEMSZ_PASSWORD));
					if (!b64encode((LPBYTE)lpmszAuth, strlen(lpmszAuth), buf)) {
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}

					// Send again CONNECT request 
					lpszUrl = mem;
					lpszIp = lpszUrl + MEMSZ_URL;
					lpszPort = lpszIp + MEMSZ_IP;
					lpmszAuth = lpszPort + MEMSZ_PORT;
					wsprintf(lpszUrl, "%s:%s", 
						theSettingsDlg.GetRemoteIpAddress(lpszIp, MEMSZ_IP),
						theSettingsDlg.GetRemoteTcpPort(lpszPort, MEMSZ_PORT));
					wsprintf(lpmszAuth, "Proxy-Authorization: Basic %s\r\n", buf);
					lpszRequest = FormatRequest("CONNECT", lpszUrl, lpszUrl, lpmszAuth, buf, BUFLEN);
					lpSocket->Send(lpszRequest, strlen(lpszRequest));

					// Read response from proxy
					dwHeaderLength = 0;
					switch (ReadHttpHeader(lpSocket, buf, BUFLEN, &dwHeaderLength, NULL, 10000)) {
					case READHTTP_SUCCESS:
						break;
					case READHTTP_CONNCLOSED:
					case READHTTP_BUFFTOOSMALL:
					case READHTTP_TIMEDOUT:
					default:
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}

					// Parse proxy response
					dwErrCode = 0;
					dwContentLength = 0;
					if (!ParseResponse(&dwErrCode, NULL, &dwContentLength, NULL, buf, dwHeaderLength)) {
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}
					// Read response body
					bodyRead = FALSE;
					if (dwContentLength > 0) {
						bodyRead = TRUE;
						dwContentLength -= dwExtraLength;
						while (dwContentLength > 0) {
							DWORD count = lpSocket->Recv(buf + dwHeaderLength + dwExtraLength, dwContentLength, 0, 10000);
							if (count <= 0) {
								break;
							}
							dwExtraLength += count;
							dwContentLength -= count;
						}
					}

					// Only 200 OK allowed
					if (200 != dwErrCode) {
						// Read remaining data
						while (!bodyRead && dwExtraLength + dwHeaderLength < BUFLEN) {
							DWORD count = lpSocket->Recv(buf + dwHeaderLength + dwExtraLength, BUFLEN - dwHeaderLength - dwExtraLength, 0, 10000);
							if (count <= 0) {
								break;
							}
							dwExtraLength += count;
						}
						ShutdownMainThread(lpTcpcom32Dlg);
						lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Proxy Auth Fail", MAKELONG(CMD_SET_STATUS,0));
						lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_WARNING));
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}

				// NTLM authentication
				} else if (useNTLM || useNTLMv2) {

					// Format base64 encoded NTLM Type1 message
					DWORD dwMessageLength = 0;
					LPSTR lpszWorkgroup = mem;
					LPSTR lpszWorkstation = lpszWorkgroup + MEMSZ_WORKGROUP;
					FormatType1Message(useNTLMv2,
						theSettingsDlg.GetWorkgroup(lpszWorkgroup, MEMSZ_WORKGROUP), 
						theSettingsDlg.GetWorkstation(lpszWorkstation, MEMSZ_WORKSTATION), 
						buf, BUFLEN, &dwMessageLength);
					LPSTR lpszType1Message = mem;
					if (!b64encode((LPBYTE)buf, dwMessageLength, lpszType1Message)) {
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}

					// Send again CONNECT request 
					lpszUrl = lpszType1Message + 2 + strlen(lpszType1Message); // save lpszType1Message
					lpszIp = lpszUrl + MEMSZ_URL;
					lpszPort = lpszIp + MEMSZ_IP;
					lpmszAuth = lpszPort + MEMSZ_PORT;
					wsprintf(lpszUrl, "%s:%s", 
						theSettingsDlg.GetRemoteIpAddress(lpszIp, MEMSZ_IP),
						theSettingsDlg.GetRemoteTcpPort(lpszPort, MEMSZ_PORT));
					wsprintf(lpmszAuth, "Proxy-Authorization: NTLM %s\r\n", lpszType1Message);
					lpszRequest = FormatRequest("CONNECT", lpszUrl, lpszUrl, lpmszAuth, buf, BUFLEN);
					lpSocket->Send(lpszRequest, strlen(lpszRequest));

					// Read response from proxy
					dwHeaderLength = 0;
					dwExtraLength = 0;
					switch (ReadHttpHeader(lpSocket, buf, BUFLEN, &dwHeaderLength, &dwExtraLength, 10000)) {
					case READHTTP_SUCCESS:
						break;
					case READHTTP_CONNCLOSED:
					case READHTTP_BUFFTOOSMALL:
					case READHTTP_TIMEDOUT:
					default:
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}

					// Parse proxy response
					lpmszAuth = mem;
					lpszConnection = lpmszAuth + MEMSZ_AUTH;
					dwErrCode = 0;
					dwContentLength = 0;
					if (!ParseResponse(&dwErrCode, lpmszAuth, &dwContentLength, lpszConnection, buf, dwHeaderLength)) {
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}
					// Read response body
					bodyRead = FALSE;
					if (dwContentLength > 0) {
						bodyRead = TRUE;
						dwContentLength -= dwExtraLength;
						while (dwContentLength > 0) {
							DWORD count = lpSocket->Recv(buf + dwHeaderLength + dwExtraLength, dwContentLength, 0, 10000);
							if (count <= 0) {
								if (0 == count) {
									strcpy(lpszConnection, "close");
								}
								break;
							}
							dwExtraLength += count;
							dwContentLength -= count;
						}
					}
					// Re-open connection if necessary
					if (!stricmp(lpszConnection, "close")) {
						// Read remaining data
						while (!bodyRead && dwExtraLength + dwHeaderLength < BUFLEN) {
							DWORD count = lpSocket->Recv(buf + dwHeaderLength + dwExtraLength, BUFLEN - dwHeaderLength - dwExtraLength, 0, 10000);
							if (count <= 0) {
								break;
							}
							dwExtraLength += count;
						}
						lpSocket->Close();
						Sleep(100);
						
						// Connect to Proxy Address again
						lpszIp = lpmszAuth + MEMSZ_AUTH; // save lpmszAuth
						lpszPort = lpszIp + MEMSZ_IP;
						if (-1 == lpSocket->Connect(
								theSettingsDlg.GetProxyIpAddress(lpszIp, MEMSZ_IP), 
								atoi(theSettingsDlg.GetProxyTcpPort(lpszPort, MEMSZ_PORT)))) 
						{
							ShutdownMainThread(lpTcpcom32Dlg);
							lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Proxy Unreachable", MAKELONG(CMD_SET_STATUS,0));
							lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_WARNING));
							ctx.Cleanup();
							return -1;
						}
					}

					// Decode Type2Message
					DWORD dwType2Length = 0;
					if (lpmszAuth = strstr(lpmszAuth, "NTLM ")) {
						lpmszAuth += 5;
					} else {
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}
					if (!b64decode(lpmszAuth, (LPBYTE)buf, &dwType2Length)) {
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}
					// Save challenge
					BYTE lpChallenge[8];
					memcpy(lpChallenge, buf+24, 8);

					// Format base64 encoded NTLM Type3 message
					dwMessageLength = 0;
					lpszWorkgroup = mem;
					lpszWorkstation = lpszWorkgroup + MEMSZ_WORKGROUP;
					LPSTR lpszUserName = lpszWorkstation + MEMSZ_WORKSTATION;
					LPSTR lpszPassword = lpszUserName + MEMSZ_USERNAME;
					FormatType3Message(useNTLMv2,
						theSettingsDlg.GetWorkgroup(lpszWorkgroup, MEMSZ_WORKGROUP), 
						theSettingsDlg.GetWorkstation(lpszWorkstation, MEMSZ_WORKSTATION), 
						theSettingsDlg.GetUserName(lpszUserName, MEMSZ_USERNAME),
						theSettingsDlg.GetPassword(lpszPassword, MEMSZ_PASSWORD), 
						lpChallenge,
						buf, BUFLEN, &dwMessageLength);
					LPSTR lpszType3Message = mem;
					if (!b64encode((LPBYTE)buf, dwMessageLength, lpszType3Message)) {
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}

					// Send again CONNECT request
					lpszUrl = lpszType3Message + 2 + strlen(lpszType3Message); // save lpszType3Message
					lpszIp = lpszUrl + MEMSZ_URL;
					lpszPort = lpszIp + MEMSZ_IP;
					lpmszAuth = lpszPort + MEMSZ_PORT;
					wsprintf(lpszUrl, "%s:%s", 
						theSettingsDlg.GetRemoteIpAddress(lpszIp, MEMSZ_IP),
						theSettingsDlg.GetRemoteTcpPort(lpszPort, MEMSZ_PORT));
					wsprintf(lpmszAuth, "Proxy-Authorization: NTLM %s\r\n", lpszType3Message);
					lpszRequest = FormatRequest("CONNECT", lpszUrl, lpszUrl, lpmszAuth, buf, BUFLEN);
					lpSocket->Send(lpszRequest, strlen(lpszRequest));

					// Read response from proxy
					dwHeaderLength = 0;
					dwExtraLength = 0;
					memset(buf, 0, BUFLEN);
					switch (ReadHttpHeader(lpSocket, buf, BUFLEN, &dwHeaderLength, &dwExtraLength, 10000)) {
					case READHTTP_SUCCESS:
						break;
					case READHTTP_CONNCLOSED:
					case READHTTP_BUFFTOOSMALL:
					case READHTTP_TIMEDOUT:
					default:
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}

					// Parse proxy response
					dwErrCode = 0;
					dwContentLength = 0;
					if (!ParseResponse(&dwErrCode, NULL, &dwContentLength, NULL, buf, dwHeaderLength)) {
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}
					// Read response body
					bodyRead = FALSE;
					if (dwContentLength > 0) {
						bodyRead = TRUE;
						dwContentLength -= dwExtraLength;
						while (dwContentLength > 0) {
							DWORD count = lpSocket->Recv(buf + dwHeaderLength + dwExtraLength, dwContentLength, 0, 10000);
							if (count <= 0) {
								break;
							}
							dwExtraLength += count;
							dwContentLength -= count;
						}
					}
					if (200 != dwErrCode) {
						// Read remaining data
						while (!bodyRead && dwExtraLength + dwHeaderLength < BUFLEN) {
							DWORD count = lpSocket->Recv(buf + dwHeaderLength + dwExtraLength, BUFLEN - dwHeaderLength - dwExtraLength, 0, 10000);
							if (count <= 0) {
								break;
							}
							dwExtraLength += count;
						}
						ShutdownMainThread(lpTcpcom32Dlg);
						lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Unknown Auth Scheme", MAKELONG(CMD_SET_STATUS,0));
						lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_WARNING));
						lpSocket->Close();
						ctx.Cleanup();
						return -1;
					}

				// Unsupported authentication method
				} else {
					ShutdownMainThread(lpTcpcom32Dlg);
					lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Unknown Auth Scheme", MAKELONG(CMD_SET_STATUS,0));
					lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_WARNING));
					lpSocket->Close();
					ctx.Cleanup();
					return -1;
				}

			// Else only 200 OK allowed
			} else if (200 != dwErrCode) {
				ShutdownMainThread(lpTcpcom32Dlg);
				lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"CONNECT Error", MAKELONG(CMD_SET_STATUS,0));
				lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_WARNING));
				lpSocket->Close();
				ctx.Cleanup();
				return -1;
			}

		// No proxy
		} else {

			// Connect to Remote Address
			LPSTR lpszIp = mem;
			LPSTR lpszPort = lpszIp + MEMSZ_IP;
			if (-1 == lpSocket->Connect(
					theSettingsDlg.GetRemoteIpAddress(lpszIp, MEMSZ_IP), 
					atoi(theSettingsDlg.GetRemoteTcpPort(lpszPort, MEMSZ_PORT)))) {
				ShutdownMainThread(lpTcpcom32Dlg);
				lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Remote Address Unreachable", MAKELONG(CMD_SET_STATUS,0));
				lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_WARNING));
				ctx.Cleanup();
				return -1;
			}

			// FIONBIO
			lpSocket->SetNonBlocking();
		}
	} else {
		// FIONBIO
		lpSocket->SetNonBlocking();
	}

	// Wait CONNECT request from peer
	DWORD dwMessageLength = 0;
	DWORD dwExtraLength = 0;
	switch (ReadHttpHeader(lpSocket, buf, BUFLEN, &dwMessageLength, &dwExtraLength, 10000)) {
	case READHTTP_SUCCESS:
		break;
	case READHTTP_CONNCLOSED:
	case READHTTP_BUFFTOOSMALL:
	case READHTTP_TIMEDOUT:
	default:
		lpSocket->Close();
		ctx.Cleanup();
		return -1;
	}

	// Save un-parsed data to handle
	LPBYTE lpExtraBytes = NULL;
	if (dwExtraLength > 0) {
		ctx.lpSpare[SPARE_EXTRA] = lpExtraBytes = new BYTE[dwExtraLength];
		memcpy(lpExtraBytes, buf + dwMessageLength, dwExtraLength);
	}
	buf[dwMessageLength] = 0; // close request string

	// Save HTTP request
	LPSTR lpszRequest = ctx.lpszRequest = new char[1 + dwMessageLength];
	strcpy(lpszRequest, buf);

	// Parse request
	char szPortName[12];
	DCB *lpDcb = ctx.lpDcb = new DCB;
	memset(lpDcb, 0, sizeof(DCB));
	if (!ParseRequest(buf, szPortName, lpDcb)) {
		LPSTR lpszResponse = FormatResponse(400, "Bad Request", buf, BUFLEN);
		lpSocket->Send(lpszResponse, strlen(lpszResponse));
		lpSocket->Close();
		ctx.Cleanup();
		return -1;
	}
	strcpy(ctx.szPortName, szPortName);

	// Check port disabled
	if (theSettingsDlg.IsPortDisabled(szPortName)) {
		lpSocket->Close();
		ctx.Cleanup();
		return -1;
	}

	// Load default settings
	if (0 == lpDcb->BaudRate) {
		if (!theSettingsDlg.GetPortConfig(szPortName, lpDcb)) {
			if (!CAsyncPort::IsInstalled(szPortName)) {
				LPSTR lpszResponse = FormatResponse(404, "Not Found", buf, BUFLEN);
				lpSocket->Send(lpszResponse, strlen(lpszResponse));
				lpSocket->Close();
				ctx.Cleanup();
				return -1;
			}
		}
	}

	// Open asynch port
	CAsyncPort *lpPort = new CAsyncPort();
	ctx.lpPort = lpPort;
	if (theSettingsDlg.IsLogEnabled()) {
		lpPort->Debug(theSettingsDlg.GetLogDir(buf, 256));
	}
	if (-1 == lpPort->OpenConnection(szPortName, *lpDcb)) {
		lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), 
			MAKELONG(CMD_SET_IMAGE,BMP_PORT_CLOSED));
		LPSTR lpszResponse = FormatResponse(500, "Internal Server Error", buf, BUFLEN);
		lpSocket->Send(lpszResponse, strlen(lpszResponse));
		lpSocket->Close();
		ctx.Cleanup();
		return -1;
	}
	theSettingsDlg.SetPortConfig(szPortName, lpDcb);

	// 200 OK
	LPSTR lpszResponse = FormatResponse(200, "OK", buf, BUFLEN);
	lpSocket->Send(lpszResponse, strlen(lpszResponse));

	// Save HTTP response
	lpszResponse = lpszResponse = new char[1 + strlen(buf)];
	strcpy(lpszResponse, buf);
	ctx.lpszResponse = lpszResponse;

	// Update treeview & trayicon
	lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(&ctx), MAKELONG(CMD_OPEN_PORT,0));
	lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,BMP_PORT_OPEN));
	lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)0, MAKELONG(CMD_TRAYICON,0));

	// Send un-parsed data
	if (lpExtraBytes && dwExtraLength > 0) {
		if (dwExtraLength != (DWORD)lpPort->WriteBuffer(lpExtraBytes, dwExtraLength)) {
			lpPort->CloseConnection();
			lpSocket->Close();
			ctx.Cleanup();
			return -1;
		}
	}
	
	// Proxy data
	LPBYTE bytes = (LPBYTE)buf;
	int portlen = -1;
	int socklen = -1;
#ifdef SHOW_PORT_ACTIVITY
	int lastbmp = BMP_PORT_OPEN;
	DWORD dwPortTicks = GetTickCount();
	DWORD dwSockTicks = GetTickCount();
#endif // SHOW_PORT_ACTIVITY
	while (TRUE) {

		// Check thread termination
		if (WAIT_OBJECT_0 == WaitForSingleObject(hEvent, 0)) {
			break;
		}

#ifdef SHOW_PORT_ACTIVITY
		DWORD dwTicks = GetTickCount();
		if (dwTicks - dwPortTicks < 1000) {
			if (dwTicks - dwSockTicks < 1000) {
				if (BMP_PORT_OPEN_BOTH != lastbmp) {
					lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,(lastbmp=BMP_PORT_OPEN_BOTH)));
				}
			} else {
				if (BMP_PORT_OPEN_LOCAL != lastbmp) {
					lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,(lastbmp=BMP_PORT_OPEN_LOCAL)));
				}
			}
		} else if (dwTicks - dwSockTicks < 1000) {
			if (BMP_PORT_OPEN_REMOTE != lastbmp) {
				lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,(lastbmp=BMP_PORT_OPEN_REMOTE)));
			}
		} else {
			if (BMP_PORT_OPEN != lastbmp) {
				lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), MAKELONG(CMD_SET_IMAGE,(lastbmp=BMP_PORT_OPEN)));
			}
		}
#endif // SHOW_PORT_ACTIVITY

		// poll asynch port
		if (lpPort->BufferedDataSize() > 0) {
			portlen = lpPort->ReadBuffer(bytes, BUFLEN, TRUE);
			if (portlen > 0) {
				ctx.dwPortInBytes += portlen;
				if (portlen != lpSocket->Send(buf, portlen)) {
					break;
				}
				ctx.dwSockOutBytes += portlen;
#ifdef SHOW_PORT_ACTIVITY
				dwPortTicks = dwTicks;
				lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(&ctx), MAKELONG(CMD_SET_STATS,0));
#endif // SHOW_PORT_ACTIVITY
				continue;
			}
		}
		if (-1 == lpPort->IsConnected()) {
			break;
		}

		// poll socket
		socklen = lpSocket->Recv(buf, BUFLEN, 0, 1000);
		if (0 == socklen) {
			break;
		} 
		if (socklen > 0) {
			ctx.dwSockInBytes += socklen;
			if (socklen != lpPort->WriteBuffer(bytes, socklen)) {
				break;
			}
			ctx.dwPortOutBytes += socklen;
#ifdef SHOW_PORT_ACTIVITY
			dwSockTicks = dwTicks;
			lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(&ctx), MAKELONG(CMD_SET_STATS,0));
#endif // SHOW_PORT_ACTIVITY
		}

	}

	// Update treeview & trayicon
	if (ctx.isDisabled = theSettingsDlg.IsPortDisabled(szPortName)) {
		lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), 
			MAKELONG(CMD_SET_IMAGE,BMP_PORT_DISABLED));
	} else {
		lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(ctx.htiPort), 
			MAKELONG(CMD_SET_IMAGE,BMP_PORT_CLOSED));
	}
	lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)(&ctx), MAKELONG(CMD_CLOSE_PORT,0));
	lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)0, MAKELONG(CMD_TRAYICON,0));

	// Close asynch port
	lpPort->CloseConnection();

	// Close socket
	lpSocket->Close();

	// Clenup context
	ctx.Cleanup();

	return 0;

#undef SHOW_PORT_ACTIVITY
#undef BUFLEN
#undef SPARE_BUF
#undef SPARE_LPPARAM
#undef SPARE_EXTRA
#undef MEMSZ_URL
#undef MEMSZ_IP
#undef MEMSZ_PORT
#undef MEMSZ_USERNAME
#undef MEMSZ_PASSWORD
#undef MEMSZ_WORKGROUP
#undef MEMSZ_WORKSTATION
#undef MEMSZ_AUTH
#undef MEMSZ_AUTHSCHEME
#undef MEMSZ_CONNECTION
}


DWORD WINAPI InitializeThreadProc(LPVOID lpParam)
{
	CTcpcom32Dlg *lpTcpcom32Dlg = (CTcpcom32Dlg *)lpParam;
	char buf[256];
	CTreeCtrl& tree = lpTcpcom32Dlg->m_treeview;

	// Clear initialized flag
	lpTcpcom32Dlg->m_isInitialized = FALSE;

	// Disable initialize
	CMenu *lpMenu = lpTcpcom32Dlg->GetMenu()->GetSubMenu(MENU_SYSTEM);
	lpMenu->EnableMenuItem(ID_SYSTEM_SETTINGS, MF_DISABLED|MF_GRAYED);
	lpMenu->EnableMenuItem(ID_SYSTEM_RESTART, MF_DISABLED|MF_GRAYED);

	// Status line
	lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Initializing...", MAKELONG(CMD_SET_STATUS,0));

	// Stop main thread
	ShutdownMainThread(lpTcpcom32Dlg);

	// Stop threads
	HTREEITEM hti = tree.GetChildItem(tree.GetRootItem()); 
	for (; hti; hti = tree.GetNextSiblingItem(hti)) {
		CPortContext *lpContext = (CPortContext *)tree.GetItemData(hti);
		if (lpContext) {
			LPHANDLE lpArgs = new HANDLE[2];
			lpArgs[0] = lpContext->hEvent;
			lpArgs[1] = lpContext->hThread;
			StopThreadProc(lpArgs);
		}
	}

	// Re-initialize treeview
	tree.DeleteAllItems();

	gethostname(buf, 256);
	HTREEITEM htroot = tree.InsertItem(TEXT( buf ), BMP_ROOT, BMP_ROOT);

	// Create treeview & profiles
	for (int i = 1; i <= 16; i ++) {
		wsprintf(buf, "COM%d", i);
		if (CAsyncPort::IsInstalled(TEXT( buf ))) {
			if (theSettingsDlg.IsPortDisabled(buf)) {
				hti = tree.InsertItem(TEXT( buf ), BMP_PORT_DISABLED, BMP_PORT_DISABLED, htroot);
			} else {
				hti = tree.InsertItem(TEXT( buf ), BMP_PORT_CLOSED, BMP_PORT_CLOSED, htroot);
			}
			tree.SetItemData(hti, 0);
		}
	}
	tree.Expand(htroot, TVE_EXPAND);

	// Start main thread
	if (theSettingsDlg.IsServerMode()) {
		StartThread(AcceptThreadProc, lpTcpcom32Dlg, &(lpTcpcom32Dlg->m_hMainEvent),
			&(lpTcpcom32Dlg->m_hMainThread), &(lpTcpcom32Dlg->m_dwMainThreadId));
	} else {
		StartThread(ClientThreadProc, lpTcpcom32Dlg, &(lpTcpcom32Dlg->m_hMainEvent),
			&(lpTcpcom32Dlg->m_hMainThread), &(lpTcpcom32Dlg->m_dwMainThreadId));
	}

	// Status line
	lpTcpcom32Dlg->SendMessage(WM_THREAD_NOTIFY, (WPARAM)"Initialized", MAKELONG(CMD_SET_STATUS,0));

	// Enable initialize
	lpMenu->EnableMenuItem(ID_SYSTEM_SETTINGS, MF_ENABLED);
	lpMenu->EnableMenuItem(ID_SYSTEM_RESTART, MF_ENABLED);

	// Initialized
	lpTcpcom32Dlg->m_isInitialized = TRUE;

	return 0;
}

