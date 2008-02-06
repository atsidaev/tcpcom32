// ClientSocket.h: interface for the CClientSocket class.


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseSocket.h"

#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

class CClientSocket : public CBaseSocket
{
private:
	struct sockaddr_in m_oServerAddress;

public:
	CClientSocket();
	virtual ~CClientSocket();

	int Connect(char * p_pszAddress, int p_nPort);
	int Send(char * p_pBuffer, int p_nLength, time_t p_nSec = 0);
	int Recv(char * p_pBuffer, int p_nMaxLength);
	int Recv(char * p_pBuffer, int p_nMaxLength, time_t p_nSec, long p_nMicro = 0);

};

#endif // CLIENTSOCKET_H

