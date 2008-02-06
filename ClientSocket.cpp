//////////////////////////////////////////////////////////////////
// ClientSocket.cpp: implementation of the CClientSocket class.
//////////////////////////////////////////////////////////////////

#include "ClientSocket.h"

//////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////

// ctor
CClientSocket::CClientSocket()
{

}

// dtor
CClientSocket::~CClientSocket()
{

}


//////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////

// Open socket and establish a connection to a remote socket
// Return:	0	on success
//			-1	on error
int CClientSocket::Connect(char * p_pszAddress, int p_nPort)
{
	if ((m_nHandle = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		return (-1);
	}

	if (FillAddress(p_pszAddress, p_nPort, & m_oServerAddress) == -1) {
		return (-1);
	}

	if (connect(m_nHandle, (struct sockaddr *) & m_oServerAddress, sizeof (struct sockaddr_in)) == -1) {
		return (-1);
	}

	int data = 1;
	if (setsockopt(m_nHandle, SOL_SOCKET, SO_KEEPALIVE, (char *) &data, sizeof(int)) == -1) {
		return (-1);
	}

	return (0);
}

// Send data
// Return:	n	number of bytes sent (on success)
//			-1	on error
int CClientSocket::Send(char * p_pBuffer, int p_nLength, time_t p_nSec)
{
	int nFailure = 1;
	int nBytesSent = 0;
	time_t nStartTime = time(NULL);

	do {
		int nRetCode = send(m_nHandle, p_pBuffer + nBytesSent, p_nLength - nBytesSent, 0);
		if (nRetCode >= 0) {
			nFailure = 0;
			nBytesSent += nRetCode;
		}
	} while ((time(NULL) - nStartTime < p_nSec) && (p_nLength > nBytesSent));

	return (nFailure ? -1 : nBytesSent);
}

// Receive data
// Return:	n	number of bytes received (on success)
//			-1	on error
int CClientSocket::Recv(char * p_pBuffer, int p_nMaxLength, time_t p_nSec, long p_nMicro)
{
	if (!WaitForPendingData(p_nSec, p_nMicro)) {
		return Recv(p_pBuffer, p_nMaxLength);
	} 
    return -1;
}

// Receive data
// Return:	n	number of bytes received (on success)
//			-1	on error
int CClientSocket::Recv(char * p_pBuffer, int p_nMaxLength)
{
	int err = recv(m_nHandle, p_pBuffer, p_nMaxLength, 0);
	if (-1 == err && WSAEWOULDBLOCK != WSAGetLastError()) {
		return 0;
	}
	return err;
}

