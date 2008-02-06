// ServerSocket.h: interface for the CServerSocket class.

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseSocket.h"
#include "ClientSocket.h"

#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

class CServerSocket : public CBaseSocket
{
public:
	CServerSocket();
	virtual ~CServerSocket();

	int Open(int p_nPort);
	int Open(char * p_pszAddress, int p_nPort);
	int Accept(CClientSocket * p_poSocket);
};

#endif // SERVERSOCKET_H

