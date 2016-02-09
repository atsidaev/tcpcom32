// AsyncPort.cpp: implementation of the CAsyncPort class.
// Last Revision: y2003 m09 d12
//////////////////////////////////////////////////////////////////////

#include "AsyncPort.h"

//////////////////////////////////////////////////////////////////////
// Friend Functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CommWatchProc
//
static DWORD WINAPI CommWatchProc(LPVOID lpParam)
{
	return ((DWORD)((CAsyncPort *)lpParam)->WatchCommDev());
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// ctor
//
CAsyncPort::CAsyncPort()
{
	// init instance variables

	memset(m_szCommDevName, 0, sizeof (m_szCommDevName));
	memset(m_szInLogPath, 0, sizeof (m_szCommDevName));
	memset(m_szOutLogPath, 0, sizeof (m_szCommDevName));
	m_hCommDev = INVALID_HANDLE_VALUE;
	memset(& m_commConfig, 0, sizeof (COMMCONFIG));;
	m_commConfig.dwSize = sizeof (COMMCONFIG);
	memset(& m_commProp, 0, sizeof (COMMPROP));
	memset(& m_commTimeOuts, 0, sizeof (COMMTIMEOUTS));
	memset(& m_ovlRead, 0, sizeof (OVERLAPPED));
	memset(& m_ovlWrite, 0, sizeof (OVERLAPPED));
	m_isConnected = FALSE;
	m_hWatchThread = NULL;
	m_dwThreadID = 0;
	memset(m_baInBuf, 0, sizeof (m_baInBuf));
	m_nInBufIndex = 0;
	m_nInBufSize = 0;
	wsprintf(m_szMutexName, "CAsyncPort%08x", GetCurrentThreadId());

	// create I/O event used for overlapped reads / writes

	m_ovlRead.hEvent = CreateEvent (
		NULL,	// no security
		TRUE,	// explicit reset req
		FALSE,	// initial event reset
		NULL	// no name
	);

	if (NULL == m_ovlRead.hEvent) {
		throw "CAsyncPort::CAsyncPort()\n"
			"unable to create read event";
	}

	m_ovlWrite.hEvent = CreateEvent(
		NULL,	// no security
		TRUE,	// explicit reset req
		FALSE,	// initial event reset
		NULL	// no name
	);

	if (NULL == m_ovlWrite.hEvent) {
		CloseHandle(m_ovlRead.hEvent) ;
		throw "CAsyncPort::CAsyncPort()\n"
			"unable to create write event";
	}
}

//////////////////////////////////////////////////////////////////////
// dtor
//
CAsyncPort::~CAsyncPort()
{
	if (m_isConnected) {
		CloseConnection();
	}

	// clean up event objects

	if (m_ovlRead.hEvent != NULL) {
		CloseHandle(m_ovlRead.hEvent);
	}
	if (NULL != m_ovlWrite.hEvent) {
		CloseHandle(m_ovlWrite.hEvent);
	}
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

int CAsyncPort::IsInstalled(const char * pszCommDevName) 
{
	HANDLE hCommDev = CreateFile (
		pszCommDevName,
		GENERIC_READ | GENERIC_WRITE,
		0,								// exclusive access
		NULL,							// no security attrs
		OPEN_EXISTING,
		0,
		NULL
	);

	if (hCommDev == INVALID_HANDLE_VALUE) {
		return 0;
	} else {
		CloseHandle(hCommDev);
		return 1;
	}
}

//////////////////////////////////////////////////////////////////////
// OpenConnection
// return:   0  success
//          -1  error
//
int CAsyncPort::OpenConnection(const char * pszPortName, const char * pszSetupString)
{
	DWORD dwBaudRate;
	BYTE bByteSize;
	BYTE bParity;
	BYTE bStopBits;

	if (! ParseSetupString(pszSetupString, & dwBaudRate, & bByteSize, & bParity, & bStopBits)) {
		return OpenConnection(pszPortName, dwBaudRate, bByteSize, bParity, bStopBits);
	}

	return (-1);
}

//////////////////////////////////////////////////////////////////////
// OpenConnection
// return:   0  success
//          -1  error
//
int CAsyncPort::OpenConnection(const char * pszPortName, DWORD dwBaudRate, BYTE bByteSize, BYTE bParity, BYTE bStopBits)
{
	// setup connection properties
    DCB dcb;
	dcb.DCBlength = sizeof (DCB) ;

	// common settings
	dcb.BaudRate = dwBaudRate;
	dcb.ByteSize = bByteSize;
	dcb.Parity = bParity;
	dcb.StopBits = bStopBits;

	// setup hardware flow control
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;

	// setup software flow control
	dcb.fInX = FALSE;
	dcb.fOutX = FALSE;
	dcb.XonChar = 0x11;
	dcb.XoffChar = 0x13;
	dcb.XonLim = 100;
	dcb.XoffLim = 100;

	// other 
	dcb.fBinary = TRUE;
	dcb.fParity = TRUE;

	return OpenConnection(pszPortName, dcb);
}

//////////////////////////////////////////////////////////////////////
// OpenConnection
// return:   0  success
//          -1  error
//
int CAsyncPort::OpenConnection(const char *pszPortName, DCB dcb)
{
	// already connected
	if (m_isConnected) {
		return -1;
	}

	// store port name
	strcpy(m_szCommDevName, pszPortName);

	// open COMM device
	m_hCommDev = CreateFile (
		m_szCommDevName,
		GENERIC_READ | GENERIC_WRITE,
		0,								// exclusive access
		NULL,							// no security attrs
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL |
		FILE_FLAG_OVERLAPPED,			// overlapped I/O
		NULL
	);
	if (INVALID_HANDLE_VALUE == m_hCommDev) {
		return -1;
	}

	// get any early notifications
	SetCommMask(m_hCommDev, EV_RXCHAR);

	// setup device buffers
	SetupComm(m_hCommDev, IN_QUEUE_SIZE, OUT_QUEUE_SIZE);

	// purge any information in the buffer
	PurgeComm (
		m_hCommDev,
		PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR
	);

	// setup connection
	if (0 == SetupConnection(dcb)) {

		// set connected flag
		m_isConnected = TRUE;
	
		// create a thread to watch for input data
		m_hWatchThread = CreateThread (
			(LPSECURITY_ATTRIBUTES) NULL,
			0,
			(LPTHREAD_START_ROUTINE) CommWatchProc,
			(LPVOID) this,
			0,
			& m_dwThreadID
		);
		if (NULL == m_hWatchThread) {

			// clear connected flag
			m_isConnected = FALSE;
			
			// close port
			CloseHandle(m_hCommDev);
			
			return -1;

		} else {
			// assert DTR
			EscapeCommFunction(m_hCommDev, SETDTR);
		}
	} else {

		// clear connected flag
		m_isConnected = FALSE;
		
		// close port
		CloseHandle(m_hCommDev) ;

		return -1;
	}
	return (0);
}

//////////////////////////////////////////////////////////////////////
// CloseConnection
// return:   0  always
//
int CAsyncPort::CloseConnection()
{
	// not connected
	m_isConnected = FALSE;

	// disable event notification and wait for thread to halt
	SetCommMask(m_hCommDev, 0);

	// block until thread has been halted
	while (m_dwThreadID != 0) {
		Sleep(10);
	}

	// drop DTR
	EscapeCommFunction(m_hCommDev, CLRDTR);

	// purge any outstanding reads/writes and close device handle
	PurgeComm (
		m_hCommDev,
		PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR
	);

	// close device
	CloseHandle(m_hCommDev);

	// reset instance variables
	m_hCommDev = INVALID_HANDLE_VALUE;
	memset(m_szCommDevName, 0, sizeof (m_szCommDevName));
	memset(& m_commConfig, 0, sizeof (COMMCONFIG));;
	m_commConfig.dwSize = sizeof (COMMCONFIG);
	memset(& m_commProp, 0, sizeof (COMMPROP));
	memset(& m_commTimeOuts, 0, sizeof (COMMTIMEOUTS));
	memset(m_baInBuf, 0, sizeof (m_baInBuf));
	m_nInBufIndex = 0;
	m_nInBufSize = 0;

	return (0);
}

//////////////////////////////////////////////////////////////////////
// SetupConnection
// return:   0  success
//          -1  error
//
int CAsyncPort::SetupConnection(DCB dcb)
{
	memcpy(& m_commConfig.dcb, & dcb, sizeof (DCB));

	if (! SetCommState(m_hCommDev, & m_commConfig.dcb)) {
        return -1;
	}

	// set up for overlapped I/O

	m_commTimeOuts.ReadIntervalTimeout = MAXDWORD;
	m_commTimeOuts.ReadTotalTimeoutMultiplier = 0;
	m_commTimeOuts.ReadTotalTimeoutConstant = 0; // one sec
	// CBR_9600 is approximately 1byte/ms. For our purposes, allow
	// double the expected time per character for a fudge factor.
	m_commTimeOuts.WriteTotalTimeoutMultiplier = 2 * CBR_9600 / m_commConfig.dcb.BaudRate;
	m_commTimeOuts.WriteTotalTimeoutConstant = 1000;

	if (! SetCommTimeouts(m_hCommDev, & m_commTimeOuts)) {
        return (-1);
	}

	return (0);
}

//////////////////////////////////////////////////////////////////////
// IsConnected
// return:  0  if not connected
//          1  if connected
//
BOOL CAsyncPort::IsConnected()
{
    return (m_isConnected);
}


//////////////////////////////////////////////////////////////////////
// ParseSetupString
// return:   pointer to input buffer
//
LPSTR CAsyncPort::GetSetupString(LPSTR pszBuffer, int nBufferSize)
{
	return FormatDcb(&(m_commConfig.dcb), pszBuffer, nBufferSize);
}

//////////////////////////////////////////////////////////////////////
// ParseSetupString
// return:   0  success
//          -1  error
//
int CAsyncPort::ParseSetupString(const char * pszSetupString, DWORD * pdwBaudRate, BYTE * pbByteSize, BYTE * pbParity, BYTE * pbStopBits, char nSeparator)
{
	if (strlen(pszSetupString) > 15) {
		return -1;
	}

	char szBuffer[16];
	strcpy(szBuffer, pszSetupString);

	char* pszTmp;
	for (pszTmp = szBuffer; *pszTmp; pszTmp ++) {
		if (*pszTmp == nSeparator) {
			*pszTmp = '\0';
		}
	}

	// baud rate
	pszTmp = szBuffer;
	if (!strcmp(pszTmp, "110")) {
		*pdwBaudRate = CBR_110;
	} else if (!strcmp(pszTmp, "300")) {
		*pdwBaudRate = CBR_300;
	} else if (!strcmp(pszTmp, "600")) {
		*pdwBaudRate = CBR_600;
	} else if (!strcmp(pszTmp, "1200")) {
		*pdwBaudRate = CBR_1200;
	} else if (!strcmp(pszTmp, "2400")) {
		*pdwBaudRate = CBR_2400;
	} else if (!strcmp(pszTmp, "4800")) {
		*pdwBaudRate = CBR_4800;
	} else if (!strcmp(pszTmp, "9600")) {
		*pdwBaudRate = CBR_9600;
	} else if (!strcmp(pszTmp, "14400")) {
		*pdwBaudRate = CBR_14400;
	} else if (!strcmp(pszTmp, "19200")) {
		*pdwBaudRate = CBR_19200;
	} else if (!strcmp(pszTmp, "38400")) {
		*pdwBaudRate = CBR_38400;
	} else if (!strcmp(pszTmp, "56000")) {
		*pdwBaudRate = CBR_56000;
	} else if (!strcmp(pszTmp, "57600")) {
		*pdwBaudRate = CBR_57600;
	} else if (!strcmp(pszTmp, "115200")) {
		*pdwBaudRate = CBR_115200;
	} else if (!strcmp(pszTmp, "128000")) {
		*pdwBaudRate = CBR_128000;
	} else if (!strcmp(pszTmp, "256000")) {
		*pdwBaudRate = CBR_256000;
	}
	else
	{
		return -1;
	}

	// byte size

	pszTmp += 1 + strlen(pszTmp);
	if (!strcmp(pszTmp, "6")) {
		*pbByteSize = 6;
	} else if (!strcmp(pszTmp, "7")) {
		*pbByteSize = 7;
	} else if (!strcmp(pszTmp, "8")) {
		*pbByteSize = 8;
	} else {
		return -1;
	}

	// parity

	pszTmp += 1 + strlen(pszTmp);
	if (!_stricmp(pszTmp, "E")) {
		*pbParity = EVENPARITY;
	} else if (!_stricmp(pszTmp, "M")) {
		*pbParity = MARKPARITY;
	} else if (!_stricmp(pszTmp, "N")) {
		*pbParity = NOPARITY;
	} else if (!_stricmp(pszTmp, "O")) {
		*pbParity = ODDPARITY;
	} else if (!_stricmp(pszTmp, "S")) {
		*pbParity = SPACEPARITY;
	} else {
		return -1;
	}

	// stop bits

	pszTmp += 1 + strlen(pszTmp);
	if (!strcmp(pszTmp, "1")) {
		*pbStopBits = ONESTOPBIT;
	} else if (!strcmp(pszTmp, "1.5")) {
		*pbByteSize = ONE5STOPBITS;
	} else if (!strcmp(pszTmp, "2")) {
		*pbByteSize = TWOSTOPBITS;
	} else {
		return -1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// SetupConnection
// return:   0  success
//          -1  error
//
int CAsyncPort::SetupConnection(DWORD dwBaudRate, BYTE bByteSize, BYTE bParity, BYTE bStopBits)
{
    DCB dcb;

	// setup connection properties

	dcb.DCBlength = sizeof (DCB) ;

	::GetCommState(m_hCommDev, & dcb);

	dcb.BaudRate = dwBaudRate;
	dcb.ByteSize = bByteSize;
	dcb.Parity = bParity;
	dcb.StopBits = bStopBits;

	// setup hardware flow control

	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;

	// setup software flow control

	dcb.fInX = FALSE;
	dcb.fOutX = FALSE;
	dcb.XonChar = 0x11;
	dcb.XoffChar = 0x13;
	dcb.XonLim = 100;
	dcb.XoffLim = 100;

	// other various settings

	dcb.fBinary = TRUE;
	dcb.fParity = TRUE;

	return (SetupConnection(dcb));
}

//////////////////////////////////////////////////////////////////////
// WatchCommDev
// return:   0  success
//          -1  error
//
int CAsyncPort::WatchCommDev()
{
	OVERLAPPED ovl;
	memset(& ovl, 0, sizeof (OVERLAPPED));

	// create I/O event used for overlapped read
	ovl.hEvent = CreateEvent (
		NULL,	// no security
		TRUE,	// explicit reset req
		FALSE,	// initial event reset
		NULL	// no name
	);

	if (ovl.hEvent == NULL) {
		return (-1);
	}

	if (! SetCommMask(m_hCommDev, EV_RXCHAR)){
		return (-1);
	}

	int nLength;
	BYTE baTmpInBuf[IN_QUEUE_SIZE];

	while (m_isConnected)
	{
		DWORD dwEvtMask = 0;

		WaitCommEvent(m_hCommDev, & dwEvtMask, NULL);

		if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR) {
			do {
				if ((nLength = ReadCommBlock((char *) baTmpInBuf, IN_QUEUE_SIZE)) > 0) {
					// sync
                    HANDLE hMutex = CreateMutex(NULL, FALSE, m_szMutexName);
                    if (hMutex == NULL) {
                        break;
                    }
                    if (WaitForSingleObject(hMutex, INFINITE) == WAIT_FAILED) {
                        CloseHandle(hMutex);
                        break;
                    }

					if (*m_szInLogPath) {
						HANDLE hFile = CreateFile (
							m_szInLogPath,
							GENERIC_WRITE,
							0,
							(LPSECURITY_ATTRIBUTES) NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
							NULL
						);
						if (INVALID_HANDLE_VALUE != hFile) {
							DWORD dwBytes;
							SetFilePointer(hFile, 0, NULL, FILE_END);
							WriteFile(hFile, baTmpInBuf, nLength, & dwBytes, NULL);
							CloseHandle(hFile);
						}
					}

					int nBytes =
						nLength + m_nInBufSize > IN_BUFFER_SIZE ?
						IN_BUFFER_SIZE - m_nInBufSize :
						nLength ;

					BYTE * bp = baTmpInBuf;
					int nIndex = (m_nInBufIndex + m_nInBufSize) % IN_BUFFER_SIZE;

					m_nInBufSize += nBytes;

					while (nBytes -- > 0) {
						m_baInBuf[nIndex] = *bp ++;
						nIndex = (nIndex + 1) % IN_BUFFER_SIZE;
					}

					ReleaseMutex(hMutex);
					CloseHandle(hMutex);
				}
			}
			while (nLength > 0) ;
		}
	}

	// get rid of event handle

	CloseHandle(ovl.hEvent);

	// clear information in structure (kind of a "we're done flag")

	m_dwThreadID = 0;
	m_hWatchThread = NULL;

	return(0);
}

//////////////////////////////////////////////////////////////////////
// ReadCommBlock
// return:   n  number of bytes read
//          -1  error
//
int CAsyncPort::ReadCommBlock(char * pszBlock, int nMaxLength)
{
   BOOL fReadStat;
   COMSTAT ComStat;
   DWORD dwErrorFlags;
   DWORD dwLength;
   DWORD dwError;

	// only try to read number of bytes in queue
	ClearCommError(m_hCommDev, & dwErrorFlags, & ComStat);
	dwLength = min((DWORD) nMaxLength, ComStat.cbInQue);

	if (dwLength > 0) {

		fReadStat = ReadFile (
			m_hCommDev,
			pszBlock,
			dwLength,
			& dwLength,
			& m_ovlRead
		);

		if (! fReadStat) {
			if (GetLastError() == ERROR_IO_PENDING) {
				// We have to wait for read to complete.
				// This function will timeout according to the
				// CommTimeOuts.ReadTotalTimeoutConstant variable
				// Every time it times out, check for port errors
				while(! GetOverlappedResult(m_hCommDev, & m_ovlRead, & dwLength, TRUE)) {
					dwError = GetLastError();
					if(dwError == ERROR_IO_INCOMPLETE) {
						// normal result if not finished
						continue;
					} else {
						// an error occurred, try to recover
						ClearCommError(m_hCommDev, &dwErrorFlags, &ComStat ) ;
						break;
					}
				}
			} else {
				// some other error occurred
				ClearCommError(m_hCommDev, & dwErrorFlags, & ComStat);
                return (-1);
			}
		}
	}

	return dwLength;
}

//////////////////////////////////////////////////////////////////////
// WriteCommBlock
// return:   n  number of bytes written
//          -1  error
//
int CAsyncPort::WriteCommBlock(BYTE * pBlock, DWORD dwBytesToWrite)
{
	BOOL fWriteStat ;
	DWORD dwBytesWritten ;
	DWORD dwErrorFlags;
	DWORD dwError;
	DWORD dwBytesSent = 0;
	COMSTAT ComStat;

	fWriteStat = WriteFile (
		m_hCommDev,
		pBlock,
		dwBytesToWrite,
		& dwBytesWritten,
		& m_ovlWrite
	);

	// Note that normally the code will not execute the following
	// because the driver caches write operations. Small I/O requests
	// (up to several thousand bytes) will normally be accepted
	// immediately and WriteFile will return true even though an
	// overlapped operation was specified

	if (! fWriteStat) {

		if (GetLastError() == ERROR_IO_PENDING) {
		
			// We should wait for the completion of the write operation
			// so we know if it worked or not

			// This is only one way to do this. It might be beneficial to
			// place the write operation in a separate thread
			// so that blocking on completion will not negatively
			// affect the responsiveness of the UI

			// If the write takes too long to complete, this
			// function will timeout according to the
			// CommTimeOuts.WriteTotalTimeoutMultiplier variable.
			// This code logs the timeout but does not retry
			// the write.

			while (! GetOverlappedResult(m_hCommDev, & m_ovlWrite, & dwBytesWritten, TRUE)) {
				dwError = GetLastError();
				if (dwError == ERROR_IO_INCOMPLETE) {
					// normal result if not finished
					dwBytesSent += dwBytesWritten;
					continue;
				} else {
					// an error occurred, try to recover
					ClearCommError(m_hCommDev, &dwErrorFlags, & ComStat);
					break;
				}
			}
			dwBytesSent += dwBytesWritten;
		} else {
			// some other error occurred
			ClearCommError(m_hCommDev, & dwErrorFlags, & ComStat);
			return (-1);
		}
	} else {
        dwBytesSent = dwBytesWritten;
    }

	if (*m_szOutLogPath) {
		HANDLE hFile = CreateFile (
			m_szOutLogPath,
			GENERIC_WRITE,
			0,
			(LPSECURITY_ATTRIBUTES) NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
			NULL
		);
		if (INVALID_HANDLE_VALUE != hFile) {
			DWORD dwBytes;
			SetFilePointer(hFile, 0, NULL, FILE_END);
			WriteFile(hFile, pBlock, dwBytesSent, & dwBytes, NULL);
			CloseHandle(hFile);
		}
	}

	return (dwBytesSent);
}

//////////////////////////////////////////////////////////////////////
// ClearInputBuffer
// return:   0  success
//          -1  error
//
int CAsyncPort::ClearInputBuffer()
{
    HANDLE hMutex = CreateMutex(NULL, FALSE, m_szMutexName);
    if (hMutex == NULL) {
        return (-1);
    }
    if (WaitForSingleObject(hMutex, INFINITE) == WAIT_FAILED) {
        CloseHandle(hMutex);
        return (-1);
    }

    m_nInBufIndex = 0;
    m_nInBufSize = 0;

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);

    return (0);
}

//////////////////////////////////////////////////////////////////////
// BufferedDataSize
// return:  number of bytes in input buffer
//
unsigned CAsyncPort::BufferedDataSize()
{
    return m_nInBufSize;
}

//////////////////////////////////////////////////////////////////////
// ReadBuffer
// return:  n>=0	number of bytes read
//          -1		error
//
int CAsyncPort::ReadBuffer(BYTE * bpBuffer, unsigned nBytes, BOOL nExtractFlag)
{
    HANDLE hMutex = CreateMutex(NULL, FALSE, m_szMutexName);
    if (hMutex == NULL) {
        return (-1);
    }
    if (WaitForSingleObject(hMutex, INFINITE) == WAIT_FAILED) {
        CloseHandle(hMutex);
        return (-1);
    }

	if (nBytes > m_nInBufSize) {
		nBytes = m_nInBufSize;
	}

    unsigned nInBufIndex = m_nInBufIndex;

	for (unsigned n = 0; n < nBytes; n ++) {
		*bpBuffer ++ = m_baInBuf[nInBufIndex];
		nInBufIndex = (nInBufIndex + 1) % IN_BUFFER_SIZE;
	}

    if (nExtractFlag) {
        m_nInBufIndex = nInBufIndex;
	    m_nInBufSize -= nBytes;
    }

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);

	return (nBytes);
}

//////////////////////////////////////////////////////////////////////
// DeleteBuffer
// return:   0  success
//          -1  error
//
int CAsyncPort::DeleteBuffer(unsigned nBytes)
{
    if (nBytes >= m_nInBufSize) {
        return (ClearInputBuffer());
	}

    HANDLE hMutex = CreateMutex(NULL, FALSE, m_szMutexName);
    if (hMutex == NULL) {
        return (-1);
    }
    if (WaitForSingleObject(hMutex, INFINITE) == WAIT_FAILED) {
        CloseHandle(hMutex);
        return (-1);
    }

	while (nBytes --) {
		m_nInBufIndex = (m_nInBufIndex + 1) % IN_BUFFER_SIZE;
        m_nInBufSize --;
	}

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);

    return (0);
}

//////////////////////////////////////////////////////////////////////
// WriteBuffer
// return:  n>=0	number of bytes written
//          -1		error
//
int CAsyncPort::WriteBuffer(BYTE * bpBuffer, unsigned nBytes)
{
	return (0 == nBytes ? 0 : WriteCommBlock(bpBuffer, nBytes));
}

//////////////////////////////////////////////////////////////////////
// ShowConfigDialog
// return:   0  success
//          -1  error
//
int CAsyncPort::ShowConfigDialog(HWND hWnd)
{
	COMMCONFIG commConfig;
	//memset(& commConfig, 0, sizeof (COMMCONFIG));
	//commConfig.dwSize = sizeof(COMMCONFIG);
	memcpy(& commConfig, & m_commConfig, sizeof (COMMCONFIG));

	if (! CommConfigDialog(m_szCommDevName, hWnd, & commConfig)) {
		return -1;
	}

	return SetupConnection(commConfig.dcb);
}

//////////////////////////////////////////////////////////////////////
// GetCommDevName
//
const char * CAsyncPort::GetCommDevName()
{
	return m_szCommDevName;
}

//////////////////////////////////////////////////////////////////////
// Debug
//
int CAsyncPort::Debug(const char * pszDirName)
{
	wsprintf(m_szInLogPath, "%s/ASYNCPORT_LOG_%s.IN", pszDirName, & m_szCommDevName[4]);
	wsprintf(m_szOutLogPath, "%s/ASYNCPORT_LOG_%s.OUT", pszDirName, & m_szCommDevName[4]);
	return 0;
}

//////////////////////////////////////////////////////////////////////
// GetCommState
//
BOOL CAsyncPort::GetCommState(DCB *lpDcb)
{
	lpDcb->DCBlength = sizeof (DCB) ;
	::GetCommState(m_hCommDev, lpDcb);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// FormatDcb
//
LPSTR CAsyncPort::FormatDcb(DCB *lpDcb, LPSTR lpszString, int nStringSize)
{
	wsprintf(lpszString, "%d,%d,%s,%s", lpDcb->BaudRate, lpDcb->ByteSize,
		lpDcb->Parity == NOPARITY ? "N" : 
		 lpDcb->Parity == EVENPARITY ? "E" : 
		  lpDcb->Parity == ODDPARITY ? "O" : 
		  lpDcb->Parity == SPACEPARITY ? "S" : "-",
		lpDcb->StopBits == ONESTOPBIT ? "1" : 
		 lpDcb->StopBits == ONE5STOPBITS ? "1.5" : 
		  lpDcb->StopBits == TWOSTOPBITS ? "2" : "-"
	);

	return lpszString;
}
