// NTLM.h
//////////////////////////////////////////////////////////////////////


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef NTLM_H_INCLUDED
#define NTLM_H_INCLUDED


LPSTR HexDump(LPVOID lpBuffer, DWORD dwBufferSize, LPSTR lpszHexDump);
BOOL FormatType1Message(BOOL useNTLMv2, LPSTR lpszDomain, LPSTR lpszWorkstation, LPVOID lpBuffer, DWORD dwBufferSize, LPDWORD lpMessageSize);
BOOL FormatType3Message(BOOL useNTLMv2, LPSTR lpszDomain, LPSTR lpszWorkstation, LPSTR lpszUserName, LPSTR lpszPassword, LPBYTE lpChallenge, LPVOID lpBuffer, DWORD dwBufferSize, LPDWORD lpMessageSize);


#endif // NTLM_H_INCLUDED

