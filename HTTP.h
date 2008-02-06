// HTTP.h: HTTP common procs
//////////////////////////////////////////////////////////////////////


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef HTTP_H_INCLUDED
#define HTTP_H_INCLUDED

#define READHTTP_SUCCESS      0x00000000
#define READHTTP_CONNCLOSED   0x00000001
#define READHTTP_BUFFTOOSMALL 0x00000002
#define READHTTP_TIMEDOUT     0x00000003

DWORD ReadHttpHeader(class CClientSocket* lpSocket, LPVOID lpBuffer, DWORD dwBufferSize, LPDWORD lpHeaderLength, LPDWORD lpExtraLength, DWORD dwTimeoutMillis);
LPSTR FormatRequest(LPSTR lpszMethod, LPSTR lpszUrl, LPSTR lpszHost, LPSTR lpszHeaders, LPSTR lpszRequest, int nRequestSize);
LPSTR FormatResponse(int nErrCode, LPSTR lpszText, LPSTR lpszResponse, int nResponseSize);
BOOL ParseRequest(LPSTR lpszRequest, LPSTR lpszPortName, DCB *lpDcb);
BOOL ParseResponse(LPDWORD lpErrCode, LPSTR lpmszAuth, LPDWORD lpContentLength, LPSTR lpszConnection, LPSTR lpszResponse, int nResponseSize);

#endif // HTTP_H_INCLUDED

