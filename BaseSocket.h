// BaseSocket.h: interface for the CBaseSocket class.


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/filio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
**/

#include <time.h>
#include <winsock.h>

#ifndef BASESOCKET_H
#define BASESOCKET_H

class CBaseSocket
{
public:
	SOCKET m_nHandle;
	struct sockaddr_in m_oAddress;

	CBaseSocket();
	virtual ~CBaseSocket();
	int Close();
	int WaitForPendingData(time_t p_nSec, long p_nMicro);

	static int IsDottedAddress(char * p_pszAddress);
	int FillAddress(char * p_pszAddress, int p_nPort, struct sockaddr_in * p_poAddress);
	int PendingData();
	int SetBlocking();
	int SetNonBlocking();
	int GetConnectTime();
	char *GetPeerName(char *lpszBuffer, int nBufferSize);
	char *GetSockName(char *lpszBuffer, int nBufferSize);

};

#endif // BASESOCKET_H

