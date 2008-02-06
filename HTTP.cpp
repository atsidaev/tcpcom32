// HTTP.cpp: HTTP common procs
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClientSocket.h"
#include "HTTP.h"


//////////////////////////////////////////////////////////////////////
// ReadHttpHeader
//
DWORD ReadHttpHeader(CClientSocket* lpSocket, LPVOID lpBuffer, DWORD dwBufferSize, LPDWORD lpHeaderLength, LPDWORD lpExtraLength, DWORD dwTimeoutMillis)
{
	char *buf = (char *)lpBuffer;
	int maxlen = dwBufferSize;
	int length = 0;
	DWORD eor = 0x00000000;
	int count, parsed;
	DWORD dwTicks = GetTickCount();
	while (0x0D0A0D0A != eor && length < maxlen) {
		count = lpSocket->Recv(buf + length, maxlen - length, 0, 10000);
		parsed = 0;
		if (count > 0) {
			for (parsed = 0; 0x0D0A0D0A != eor && parsed < count; parsed ++, length ++) {
				eor = ((eor & 0x00FFFFFF) << 8) + buf[length];
			}
		} else if (0 == count) {
			return READHTTP_CONNCLOSED;
		} else if (GetTickCount() - dwTicks > dwTimeoutMillis) {
			return READHTTP_TIMEDOUT;
		}
	}
	// CR LF CR LF not found
	if (0x0D0A0D0A != eor) {
		ASSERT(length >= maxlen);
		return READHTTP_BUFFTOOSMALL;
	}

	*lpHeaderLength = length;
	if (lpExtraLength) {
		*lpExtraLength = (count - parsed);
	}
	return READHTTP_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// FormatRequest
//
LPSTR FormatRequest(LPSTR lpszMethod, LPSTR lpszUrl, LPSTR lpszHost, LPSTR lpszHeaders, LPSTR lpszRequest, int nRequestSize)
{
	wsprintf(lpszRequest,
		"%s %s HTTP/1.1\r\n"
		"Host: %s\r\n"
		"User-Agent: TcpCom32/1.0\r\n"
		"Accept: */*\r\n"
		"Accept-Encoding: *\r\n"
		"Proxy-Connection: keep-alive\r\n"
		"%s"
		"\r\n",
		lpszMethod, 
		lpszUrl,
		lpszHost,
		lpszHeaders ? lpszHeaders : ""
	);

	return lpszRequest;
}

//////////////////////////////////////////////////////////////////////
// FormatResponse
//
LPSTR FormatResponse(int nErrCode, LPSTR lpszText, LPSTR lpszResponse, int nResponseSize)
{
	static char *days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	static char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	time_t rawtime;
	time ( &rawtime );
	struct tm *timeinfo = localtime ( &rawtime );	
	
	wsprintf(lpszResponse,
		"HTTP/1.1 %d %s\r\n"
		"Date: %s, %02d %s %d %02d:%02d:%02d GMT\r\n"
		"Server: TcpCom32/1.0\r\n"
		"Cache-Control: no-cache\r\n"
		"Pragma: no-cache\r\n"
//		"Connection: keep-alive\r\n"
		"\r\n",
		nErrCode, 
		lpszText,
		days[timeinfo->tm_wday],
		timeinfo->tm_mday,
		months[timeinfo->tm_mon],
		timeinfo->tm_year + 1900,
		timeinfo->tm_hour,
		timeinfo->tm_min,
		timeinfo->tm_sec
	);

	return lpszResponse;
}


//////////////////////////////////////////////////////////////////////
// ParseResponse
//
BOOL ParseResponse(LPDWORD lpErrCode, LPSTR lpmszAuth, LPDWORD lpContentLength, LPSTR lpszConnection, LPSTR lpszResponse, int nResponseSize)
{
	if (lpmszAuth) {
		*lpmszAuth = 0;
	}
	if (lpContentLength) {
		*lpContentLength = 0;
	}
	if (lpszConnection) {
		*lpszConnection = 0;
	}
	int nErrCode = 0;
	char *cur = lpszResponse;
	char *nxt, *hdr;
	while (nxt = strstr(cur, "\r\n")) {
		*nxt = 0; // close request line (kill '\r')
		if (hdr = strchr(cur, ' ')) {
			*hdr = 0; // close header name
			for (hdr ++; ' ' == *hdr; hdr ++); // skip white spaces
			if (!stricmp(cur, "HTTP/1.0")) {
				if (cur = strchr(hdr, ' ')) {
					*cur = 0;
					nErrCode = atoi(hdr);
				}
			} else if (!stricmp(cur, "HTTP/1.1")) {
				if (cur = strchr(hdr, ' ')) {
					*cur = 0;
					nErrCode = atoi(hdr);
				}
			} else if (!stricmp(cur, "WWW-Authenticate:")) {
				if (lpmszAuth) {
					strcpy(lpmszAuth, hdr);
					lpmszAuth += 1 + strlen(hdr);
					*lpmszAuth = 0;
				}
			} else if (!stricmp(cur, "Proxy-Authenticate:")) {
				if (lpmszAuth) {
					strcpy(lpmszAuth, hdr);
					lpmszAuth += 1 + strlen(hdr);
					*lpmszAuth = 0;
				}
			} else if (!stricmp(cur, "Content-Length:")) {
				if (lpContentLength) {
					*lpContentLength = atoi(hdr);
				}
			} else if (!stricmp(cur, "Connection:")) {
				if (lpszConnection) {
					strcpy(lpszConnection, hdr);
				}
			} else if (!stricmp(cur, "Proxy-Connection:")) {
				if (lpszConnection) {
					strcpy(lpszConnection, hdr);
				}
			}
		}
		cur = nxt + 2;
	}

	if (nErrCode) {
		*lpErrCode = nErrCode;
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// ParseRequest
//
BOOL ParseRequest(LPSTR lpszRequest, LPSTR lpszPortName, DCB *lpDcb)
{
#define PREFIX "Comm-"
#define MASK_METHOD   0x0001
#define MASK_PORTNAME 0x0002
#define MASK_BAUDRATE 0x0004
#define MASK_BYTESIZE 0x0008
#define MASK_PARITY   0x0010
#define MASK_STOPBITS 0x0020
	
	DCB dcb;
	memset(&dcb, 0, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	lpszPortName[0] = 0;

	DWORD dwMask = 0;
	char *cur = lpszRequest;
	char *nxt, *hdr;
	while (nxt = strstr(cur, "\r\n")) {
		*nxt = 0; // close request line
		if (hdr = strchr(cur, ' ')) {
			*hdr = 0; // close header name
			for (hdr ++; ' ' == *hdr; hdr ++); // skip white spaces
			if (!stricmp(cur, "CONNECT")) {
				dwMask |= MASK_METHOD;
			} else if (!stricmp(cur, "User-Agent:")) {
				// nop
			} else if (!strcmp(cur, "Accept:")) {
				// nop
			} else if (!strcmp(cur, "Host:")) {
				// nop
			} else if (!strcmp(cur, PREFIX "PortName:")) {
				dwMask |= MASK_PORTNAME;
				strcpy(lpszPortName, hdr);
			} else if (!strcmp(cur, PREFIX "BaudRate:")) {
				dwMask |= MASK_BAUDRATE;
				dcb.BaudRate = atoi(hdr);
			} else if (!strcmp(cur, PREFIX "ByteSize:")) {
				dwMask |= MASK_BYTESIZE;
				dcb.ByteSize = atoi(hdr);
			} else if (!strcmp(cur, PREFIX "Parity:")) {
				if (!stricmp(hdr, "NOPARITY")) {
					dcb.Parity = NOPARITY;
				} else if (!stricmp(hdr, "MARKPARITY")) {
					dcb.Parity = MARKPARITY;
				} else if (!stricmp(hdr, "EVENPARITY")) {
					dcb.Parity = EVENPARITY;
				} else if (!stricmp(hdr, "ODDPARITY")) {
					dcb.Parity = ODDPARITY;
				} else if (!stricmp(hdr, "SPACEPARITY")) {
					dcb.Parity = SPACEPARITY;
				} else {
					return FALSE;
				}
				dwMask |= MASK_PARITY;
			} else if (!strcmp(cur, PREFIX "StopBits:")) {
				if (!stricmp(hdr, "1")) {
					dcb.StopBits = ONESTOPBIT;
				} else if (!stricmp(hdr, "1.5")) {
					dcb.StopBits = ONE5STOPBITS;
				} else if (!stricmp(hdr, "2")) {
					dcb.StopBits = TWOSTOPBITS;
				} else {
					return FALSE;
				}
				dwMask |= MASK_STOPBITS;
			} else if (!strcmp(cur, PREFIX "fBinary:")) {
				dcb.fBinary = !stricmp(hdr, "TRUE");
			} else if (!strcmp(cur, PREFIX "fParity:")) {
				dcb.fParity = !stricmp(hdr, "TRUE");
			} else if (!strcmp(cur, PREFIX "fOutxCtsFlow:")) {
				dcb.fOutxCtsFlow = !stricmp(hdr, "TRUE");
			} else if (!strcmp(cur, PREFIX "fOutxDsrFlow:")) {
				dcb.fOutxDsrFlow = !stricmp(hdr, "TRUE");
			} else if (!strcmp(cur, PREFIX "fDtrControl:")) {
				if (!stricmp(hdr, "DTR_CONTROL_DISABLE")) {
					dcb.fDtrControl = DTR_CONTROL_DISABLE;
				} else if (!stricmp(hdr, "DTR_CONTROL_ENABLE")) {
					dcb.fDtrControl = DTR_CONTROL_ENABLE;
				} else if (!stricmp(hdr, "DTR_CONTROL_HANDSHAKE")) {
					dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
				} else {
					return FALSE;
				}
			} else if (!strcmp(cur, PREFIX "fDsrSensitivity:")) {
				dcb.fDsrSensitivity = !stricmp(hdr, "TRUE");
			} else if (!strcmp(cur, PREFIX "RtsControl:")) {
				if (!stricmp(hdr, "RTS_CONTROL_DISABLE")) {
					dcb.fRtsControl = RTS_CONTROL_DISABLE;
				} else if (!stricmp(hdr, "RTS_CONTROL_ENABLE")) {
					dcb.fRtsControl = RTS_CONTROL_ENABLE;
				} else if (!stricmp(hdr, "RTS_CONTROL_HANDSHAKE")) {
					dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
				} else if (!stricmp(hdr, "RTS_CONTROL_TOGGLE")) {
					dcb.fRtsControl = RTS_CONTROL_TOGGLE;
				} else {
					return FALSE;
				}
			} else if (!strcmp(cur, PREFIX "fTXContinueOnXoff:")) {
				dcb.fTXContinueOnXoff = !stricmp(hdr, "TRUE");
			} else if (!strcmp(cur, PREFIX "fOutX:")) {
				dcb.fOutX = !stricmp(hdr, "TRUE");
			} else if (!strcmp(cur, PREFIX "fInX:")) {
				dcb.fInX = !stricmp(hdr, "TRUE");
			} else if (!strcmp(cur, PREFIX "fErrorChar:")) {
				dcb.fErrorChar = !stricmp(hdr, "TRUE");
			} else if (!strcmp(cur, PREFIX "fNull:")) {
				dcb.fNull = !stricmp(hdr, "TRUE");
			} else if (!strcmp(cur, PREFIX "fAbortOnError:")) {
				dcb.fAbortOnError = !stricmp(hdr, "TRUE");
			} else if (!strcmp(cur, PREFIX "XonLim:")) {
				dcb.XonLim = atoi(hdr);
			} else if (!strcmp(cur, PREFIX "XoffLim:")) {
				dcb.XoffLim = atoi(hdr);
			} else if (!strcmp(cur, PREFIX "XonChar:")) {
				dcb.XonChar = atoi(hdr);			
			} else if (!strcmp(cur, PREFIX "XoffChar:")) {
				dcb.XoffChar = atoi(hdr);
			} else if (!strcmp(cur, PREFIX "ErrorChar:")) {
				dcb.ErrorChar = atoi(hdr);
			} else if (!strcmp(cur, PREFIX "EofChar:")) {
				dcb.EofChar = atoi(hdr);
			} else if (!strcmp(cur, PREFIX "EvtChar:")) {
				dcb.EvtChar = atoi(hdr);
			}
		}
		cur = nxt + 2;

	}
	
	if ((MASK_BAUDRATE | MASK_BYTESIZE | MASK_PARITY | MASK_STOPBITS |
		MASK_METHOD | MASK_PORTNAME) == dwMask) {
		*lpDcb = dcb;
		return TRUE;
	} else if ((MASK_METHOD | MASK_PORTNAME) == dwMask) {
		return TRUE;
	}
	return FALSE;

#undef PREFIX
#undef MASK_METHOD
#undef MASK_PORTNAME
#undef MASK_BAUDRATE
#undef MASK_BYTESIZE
#undef MASK_PARITY
#undef MASK_STOPBITS
}

