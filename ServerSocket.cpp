//////////////////////////////////////////////////////////////////
// ServerSocket.cpp: implementation of the CServerSocket class.
//////////////////////////////////////////////////////////////////

#include "ServerSocket.h"

//////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////

// ctor
CServerSocket::CServerSocket()
{

}

// dtor
CServerSocket::~CServerSocket()
{

}


//////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////

// Open server side socket 
// Return:	0	on success
//			-1	on error
int CServerSocket::Open(int p_nPort)
{
	if ((m_nHandle = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return -1;

	if (FillAddress(NULL, p_nPort, & m_oAddress) == -1)
		return -1;

	if (bind(m_nHandle, (struct sockaddr *) & m_oAddress, sizeof (struct sockaddr_in)) == -1)
		return (-1);

	if (listen(m_nHandle, 5) == -1)
		return -1;

	return 0;
}

// Open server side socket 
// Return:	0	on success
//			-1	on error
int CServerSocket::Open(char * p_pszAddress, int p_nPort)
{
	if ((m_nHandle = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		return -1;
	}

	if (FillAddress(p_pszAddress, p_nPort, & m_oAddress) == -1) {
		return -1;
	}

	if (bind(m_nHandle, (struct sockaddr *) & m_oAddress, sizeof (struct sockaddr_in)) == -1) {
		return -1;
	}

	if (listen(m_nHandle, 5) == -1) {
		return -1;
	}

	return 0;
}

// Accept a connection
// Return:	0	on success
//			-1	on error
int CServerSocket::Accept(CClientSocket * p_poSocket)
{
	struct sockaddr_in oAddress;
	int nLength = sizeof (struct sockaddr_in);

	int nHandle = accept(m_nHandle, (struct sockaddr *) & oAddress, & nLength);
	if (nHandle == -1) {
		return -1;
	}
	p_poSocket->m_nHandle = nHandle;
	p_poSocket->m_oAddress = oAddress;

	return 0;
}

