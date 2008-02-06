//////////////////////////////////////////////////////////////////
// BaseSocket.cpp: implementation of the CBaseSocket class.
//////////////////////////////////////////////////////////////////

#include "BaseSocket.h"


//////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////

// ctor
CBaseSocket::CBaseSocket()
{

}

// dtor
CBaseSocket::~CBaseSocket()
{

}


//////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////

// Perform a quick check on the address format
// Return:	1	if address is formally correct
//			0	else
int CBaseSocket::IsDottedAddress(char * p_pszAddress)
{
	int nDots = 0;
	int nCyphers = 0;

	for (; *p_pszAddress; p_pszAddress ++)
	{
		if (*p_pszAddress == '.' && nCyphers > 0)
		{
			nCyphers = 0;
			if (nDots ++ >= 3)
				return 0;
		}
		else 
		{
			if (*p_pszAddress >= '0' && *p_pszAddress <= '9')
			{
				if (nCyphers ++ >= 3)
					return 0;
			}
			else 
				return 0;
		}
	}

	return (nCyphers > 0 && nDots == 3);
}

// Fill in a sockaddr_in structure
// Return:	0	on success
//			-1	on error
int CBaseSocket::FillAddress(char * p_pszAddress, int p_nPort, struct sockaddr_in * p_poAddress)
{
	memset(p_poAddress, 0, sizeof (struct sockaddr_in));

	p_poAddress->sin_family = AF_INET;
	p_poAddress->sin_port = htons(p_nPort);

	if (NULL == p_pszAddress) {
		char szHostName[256];
		if (gethostname(szHostName, sizeof (szHostName)) == -1) {
			return (-1);
		}
		p_pszAddress = szHostName;
	}
	p_poAddress->sin_addr.s_addr = inet_addr(p_pszAddress);
	if (INADDR_NONE == p_poAddress->sin_addr.s_addr) {
		struct hostent * poHostEntity = gethostbyname(p_pszAddress);
		if (NULL == poHostEntity) {
			return (-1);
		}

		memcpy(& p_poAddress->sin_addr, poHostEntity->h_addr_list[0], poHostEntity->h_length);
	}

	return (0);
}

// Pending data amount
// Return:	n	bytes to be read (on success)
//			-1	on error
int CBaseSocket::PendingData()
{
	u_long uBytes;

	if (ioctlsocket(m_nHandle, FIONREAD, & uBytes) == -1) {
		return (-1);
	}

	return (int) uBytes;
}


// Set non blocking flag
// Return:	0	on success
//			-1	on error
int CBaseSocket::SetNonBlocking()
{
	u_long uNonBlock = 1;
	
	if (ioctlsocket(m_nHandle, FIONBIO, & uNonBlock) == -1) {
		return -1;
	}
  
	return 0;
}

// Set non blocking flag
// Return:	0	on success
//			-1	on error
int CBaseSocket::SetBlocking()
{
	u_long uNonBlock = 0;
	
	if (ioctlsocket(m_nHandle, FIONBIO, & uNonBlock) == -1) {
		return -1;
	}
  
	return 0;
}

// Close socket 
// Return:	0	on success
//			-1	on error
int CBaseSocket::Close()
{
	// disable further send/receive
	shutdown(m_nHandle, 2);

	struct linger oLinger;
	memset(& oLinger, 0, sizeof (struct linger));
	oLinger.l_onoff = 1;
	oLinger.l_linger = 0;

	setsockopt(m_nHandle, SOL_SOCKET, SO_LINGER, (char *) & oLinger, sizeof (struct linger));

	if (closesocket(m_nHandle) == -1) {
		return -1;
	}

	return 0;
}

// WaitForPendingData
// Return:	0	on success
//			-1	on timeout
int CBaseSocket::WaitForPendingData(time_t p_nSec, long p_nMicro)
{
	fd_set oReadDescriptors;
	FD_ZERO(& oReadDescriptors);
	FD_SET(m_nHandle, & oReadDescriptors);

	struct timeval oTime;
	oTime.tv_sec = p_nSec;
	oTime.tv_usec = p_nMicro;

	int err = select(0, & oReadDescriptors, NULL, NULL, & oTime);
	if (err > 0) {
		if (FD_ISSET(m_nHandle, & oReadDescriptors)) {
			return 0;
		}
	}

	return -1;
}

// GetConnectTime
// Return:	>=0	connect time
//			-1	on any error
int CBaseSocket::GetConnectTime() 
{
	DWORD dwTime;
	int err = sizeof (DWORD);
	if (0 == getsockopt(m_nHandle, SOL_SOCKET, SO_CONNECT_TIME, (char *) & dwTime, & err)) {
		return (int)dwTime;
	}

	return -1;
}

// GetPeerName
// Return:	pointer to peer name string
char *CBaseSocket::GetPeerName(char *lpszBuffer, int nBufferSize)
{
	*lpszBuffer = 0;

	struct sockaddr_in peername;
	int namelen = sizeof(struct sockaddr_in);
	getpeername(m_nHandle, (struct sockaddr *)&peername, &namelen);

	wsprintf(lpszBuffer, "%s:%d", 
		inet_ntoa(peername.sin_addr), ntohs(peername.sin_port));

	return lpszBuffer;
}


// GetPeerName
// Return:	pointer to sock name string
char *CBaseSocket::GetSockName(char *lpszBuffer, int nBufferSize)
{
	*lpszBuffer = 0;

	struct sockaddr_in sockname;
	int namelen = sizeof(struct sockaddr_in);
	getsockname(m_nHandle, (struct sockaddr *)&sockname, &namelen);

	wsprintf(lpszBuffer, "%s:%d", 
		inet_ntoa(sockname.sin_addr), ntohs(sockname.sin_port));

	return lpszBuffer;
}

