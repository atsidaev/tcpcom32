// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tcpcom32.h"
#include "SettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TCPCOM32_USE_REGISTRY
#define TCPCOM32_REG_KEY "SOFTWARE\\mep\\TcpCom32\\Settings\\%s"

#define DEFAULT_SERVER_MODE "TRUE"
#define DEFAULT_LOCAL_IP "localhost"
#define DEFAULT_LOCAL_PORT "232"
#define DEFAULT_CLIENT_MODE "FALSE"
#define DEFAULT_REMOTE_IP ""
#define DEFAULT_REMOTE_PORT ""
#define DEFAULT_PROXY_ENABLED "FALSE"
#define DEFAULT_PROXY_IP ""
#define DEFAULT_PROXY_PORT ""
#define DEFAULT_AUTH_ENABLED "FALSE"
#define DEFAULT_AUTHSCHEME "Basic"
#define DEFAULT_USERNAME ""
#define DEFAULT_PASSWORD ""
#define DEFAULT_WORKGROUP ""
#define DEFAULT_WORKSTATION ""
#define DEFAULT_LOG_ENABLED "FALSE"
#define DEFAULT_LOG_DIR "C:\\WINDOWS\\Temp"
#define DEFAULT_PORT_DISABLED "FALSE"


/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog


CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsDlg)
	//}}AFX_DATA_INIT
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDlg)
	DDX_Control(pDX, IDC_RADIO_NTLMV2, m_radioNTLMv2);
	DDX_Control(pDX, IDC_RADIO_NTLM, m_radioNTLM);
	DDX_Control(pDX, IDC_RADIO_BASIC, m_radioBasic);
	DDX_Control(pDX, IDC_CHECK_AUTH, m_checkAuth);
	DDX_Control(pDX, IDC_EDIT_WORKSTATION, m_editWorkstation);
	DDX_Control(pDX, IDC_EDIT_WORKGROUP, m_editWorkgroup);
	DDX_Control(pDX, IDC_CHECK_SERVER_MODE, m_checkServerMode);
	DDX_Control(pDX, IDC_CHECK_PROXY, m_checkProxy);
	DDX_Control(pDX, IDC_CHECK_LOG, m_checkLog);
	DDX_Control(pDX, IDC_CHECK_CLIENT_MODE, m_checkClientMode);
	DDX_Control(pDX, IDC_EDIT_LOCAL_PORT, m_editLocalPort);
	DDX_Control(pDX, IDC_EDIT_LOCAL_IP, m_editLocalIp);
	DDX_Control(pDX, IDC_EDIT_REMOTE_PORT, m_editRemotePort);
	DDX_Control(pDX, IDC_EDIT_REMOTE_IP, m_editRemoteIp);
	DDX_Control(pDX, IDC_EDIT_PROXY_PORT, m_editProxyPort);
	DDX_Control(pDX, IDC_EDIT_PROXY_IP, m_editProxyIp);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_editUserName);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_EDIT_LOG_DIR, m_editLogDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingsDlg)
	ON_BN_CLICKED(IDC_CHECK_LOG, OnCheckLogEnabled)
	ON_BN_CLICKED(IDC_CHECK_SERVER_MODE, OnCheckServerMode)
	ON_BN_CLICKED(IDC_CHECK_CLIENT_MODE, OnCheckClientMode)
	ON_BN_CLICKED(IDC_CHECK_PROXY, OnCheckProxy)
	ON_BN_CLICKED(IDC_CHECK_AUTH, OnCheckAuth)
	ON_BN_CLICKED(IDC_RADIO_BASIC, OnRadioBasic)
	ON_BN_CLICKED(IDC_RADIO_NTLM, OnRadioNtlm)
	ON_BN_CLICKED(IDC_RADIO_NTLMV2, OnRadioNtlmv2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers

BOOL CSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	UpdateControls(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsDlg::UpdateControls(BOOL bSaveAndValidate)
{
	char buf[255];

	if (bSaveAndValidate) {

		UpdateData(TRUE);
		
		SetServerMode(m_checkServerMode.GetCheck() == BST_CHECKED);
		m_editLocalIp.GetWindowText(buf, 255);
		SetLocalIpAddress(buf);
		m_editLocalPort.GetWindowText(buf, 255);
		SetLocalTcpPort(buf);

		m_editRemoteIp.GetWindowText(buf, 255);
		SetRemoteIpAddress(buf);
		m_editRemotePort.GetWindowText(buf, 255);
		SetRemoteTcpPort(buf);
		
		SetProxyEnabled(m_checkProxy.GetCheck() == BST_CHECKED);
		m_editProxyIp.GetWindowText(buf, 255);
		SetProxyIpAddress(buf);
		m_editProxyPort.GetWindowText(buf, 255);
		SetProxyTcpPort(buf);
		
		SetAuthEnabled(m_checkAuth.GetCheck() == BST_CHECKED);
		if (m_radioBasic.GetCheck() == BST_CHECKED) {
			SetAuthScheme("Basic");
		} else if (m_radioNTLM.GetCheck() == BST_CHECKED) {
			SetAuthScheme("NTLM");
		} else if (m_radioNTLMv2.GetCheck() == BST_CHECKED) {
			SetAuthScheme("NTLMv2");
		}
		m_editUserName.GetWindowText(buf, 255);
		SetUserName(buf);
		m_editPassword.GetWindowText(buf, 255);
		SetPassword(buf);
		m_editWorkgroup.GetWindowText(buf, 255);
		SetWorkgroup(buf);
		m_editWorkstation.GetWindowText(buf, 255);
		SetWorkstation(buf);
		
		SetLogEnabled(m_checkLog.GetCheck() == BST_CHECKED);
		m_editLogDir.GetWindowText(buf, 255);
		SetLogDir(buf);

	} else {

		BOOL isServerMode = IsServerMode();
		m_checkServerMode.SetCheck(isServerMode ? BST_CHECKED : BST_UNCHECKED);
		m_editLocalIp.SetWindowText(GetLocalIpAddress(buf, 32));
		m_editLocalIp.EnableWindow(isServerMode);
		m_editLocalPort.SetWindowText(GetLocalTcpPort(buf, 32));
		m_editLocalPort.EnableWindow(isServerMode);

		m_checkClientMode.SetCheck(isServerMode ? BST_UNCHECKED : BST_CHECKED);
		m_editRemoteIp.SetWindowText(GetRemoteIpAddress(buf, 32));
		m_editRemoteIp.EnableWindow(!isServerMode);
		m_editRemotePort.SetWindowText(GetRemoteTcpPort(buf, 32));
		m_editRemotePort.EnableWindow(!isServerMode);

		BOOL isProxyEnabled = IsProxyEnabled();
		m_checkProxy.SetCheck(isProxyEnabled ? BST_CHECKED : BST_UNCHECKED);
		m_checkProxy.EnableWindow(isProxyEnabled && !isServerMode);
		m_editProxyIp.SetWindowText(GetProxyIpAddress(buf, 32));
		m_editProxyIp.EnableWindow(isProxyEnabled && !isServerMode);
		m_editProxyPort.SetWindowText(GetProxyTcpPort(buf, 32));
		m_editProxyPort.EnableWindow(isProxyEnabled && !isServerMode);
		
		BOOL isAuthEnabled = IsAuthEnabled();
		m_checkAuth.SetCheck(isAuthEnabled ? BST_CHECKED : BST_UNCHECKED);
		m_checkAuth.EnableWindow(isProxyEnabled && !isServerMode);
		m_editUserName.SetWindowText(GetUserName(buf, 32));
		m_editUserName.EnableWindow(isAuthEnabled && isProxyEnabled && !isServerMode);
		m_editPassword.SetWindowText(GetPassword(buf, 32));
		m_editPassword.EnableWindow(isAuthEnabled && isProxyEnabled && !isServerMode);
		m_editWorkgroup.SetWindowText(GetWorkgroup(buf, 32));
		m_editWorkgroup.EnableWindow(isAuthEnabled && isProxyEnabled && !isServerMode);
		m_editWorkstation.SetWindowText(GetWorkstation(buf, 32));
		m_editWorkstation.EnableWindow(isAuthEnabled && isProxyEnabled && !isServerMode);
		GetAuthScheme(buf, 32);
		if (!stricmp(buf, "Basic")) {
			m_radioBasic.SetCheck(BST_CHECKED);
			m_radioNTLM.SetCheck(BST_UNCHECKED);
			m_radioNTLMv2.SetCheck(BST_UNCHECKED);
		} else if (!stricmp(buf, "NTLM")) {
			m_radioBasic.SetCheck(BST_UNCHECKED);
			m_radioNTLM.SetCheck(BST_CHECKED);
			m_radioNTLMv2.SetCheck(BST_UNCHECKED);
		} else if (!stricmp(buf, "NTLMv2")) {
			m_radioBasic.SetCheck(BST_UNCHECKED);
			m_radioNTLM.SetCheck(BST_UNCHECKED);
			m_radioNTLMv2.SetCheck(BST_CHECKED);
		}
		m_radioBasic.EnableWindow(isAuthEnabled && isProxyEnabled && !isServerMode);
		m_radioNTLM.EnableWindow(isAuthEnabled && isProxyEnabled && !isServerMode);
		m_radioNTLMv2.EnableWindow(isAuthEnabled && isProxyEnabled && !isServerMode);

		BOOL isLogEnabled = IsLogEnabled();
		m_editLogDir.EnableWindow(isLogEnabled);
		m_editLogDir.SetWindowText(GetLogDir(buf, 32));
		UpdateData(FALSE);

	}

	EnableControls();
}

BOOL CSettingsDlg::ReadConfigString(LPSTR lpszSection, LPSTR lpszName, LPSTR lpszBuffer, DWORD dwBufferSize)
{
#ifdef TCPCOM32_USE_REGISTRY

	HKEY hKey;
	char szKeyName[255];

	wsprintf(szKeyName, TCPCOM32_REG_KEY, lpszSection);

	LONG nRet = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE, 
		TEXT( szKeyName ), 
		0, 
		KEY_READ, 
		&hKey
	);
	if (ERROR_SUCCESS != nRet) {
		return FALSE;
	}

	dwBufferSize --;
	nRet = RegQueryValueEx(
	  hKey,
	  TEXT( lpszName ),
	  NULL,
	  NULL,
	  (LPBYTE)lpszBuffer,
	  &dwBufferSize
	);

	RegCloseKey(hKey);

	if (ERROR_SUCCESS == nRet) {
		lpszBuffer[dwBufferSize+1] = 0;
		return TRUE;
	}
	return FALSE;

#else // TCPCOM32_USE_REGISTRY

	DWORD dwLength = GetPrivateProfileString (
		TEXT( lpszSection ), 
		TEXT( lpszName ), 
		TEXT( "\r\n" ), 
		lpszBuffer, nBufferSize, 
		TEXT( m_iniFilePath )
	);

	return strcmp(lpszBuffer, "\r\n") ? TRUE : FALSE;

#endif //TCPCOM32_USE_REGISTRY
}

BOOL CSettingsDlg::WriteConfigString(LPSTR lpszSection, LPSTR lpszName, LPSTR lpszBuffer, BOOL bCreate)
{
#ifdef TCPCOM32_USE_REGISTRY

	HKEY hKey;
	char szKeyName[255];
	LONG nRet;

	wsprintf(szKeyName, TCPCOM32_REG_KEY, lpszSection);

	if (bCreate) {
		nRet = RegCreateKeyEx(
			HKEY_LOCAL_MACHINE,
			TEXT( szKeyName ),
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_WRITE,
			NULL,
			&hKey,
			NULL
		);
	} else {
		nRet = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE, 
			TEXT( szKeyName ), 
			KEY_WRITE, 
			0,
			&hKey
		);

	}
	if (ERROR_SUCCESS != nRet) {
		return FALSE;
	}

	nRet = RegSetValueEx(
		hKey,
		TEXT( lpszName ),
		0,
		REG_SZ,
		(LPBYTE)lpszBuffer,
		1+strlen(lpszBuffer)
	);

	RegCloseKey(hKey);

	return (ERROR_SUCCESS == nRet);

#else // TCPCOM32_USE_REGISTRY

	return WritePrivateProfileString(
        TEXT( lpszSection ), 
        TEXT( lpszName ), 
        TEXT( lpszBuffer ), 
        TEXT( m_iniFilePath )
	); 

#endif //TCPCOM32_USE_REGISTRY
}

void CSettingsDlg::SetIniFilePath(CString iniFilePath)
{
	m_iniFilePath = iniFilePath;
}

BOOL CSettingsDlg::IsServerMode()
{
	char buf[8];

	if (!ReadConfigString("Server Mode", "Enabled", buf, 8)) {
		strcpy(buf, DEFAULT_SERVER_MODE);
	}
	return !stricmp(buf, "TRUE");
}

void CSettingsDlg::SetServerMode(BOOL isServerMode)
{
	WriteConfigString("Server Mode", "Enabled",
		isServerMode ? TEXT( "TRUE" ) : TEXT( "FALSE" )); 

	WriteConfigString("Client Mode", "Enabled",
		isServerMode ? TEXT( "FALSE" ) : TEXT( "TRUE" )); 
}

BOOL CSettingsDlg::IsClientMode()
{
	char buf[8];

	if (!ReadConfigString("Client Mode", "Enabled", buf, 8)) {
		strcpy(buf, DEFAULT_CLIENT_MODE);
	}
	return !stricmp(buf, "TRUE");
}

void CSettingsDlg::SetClientMode(BOOL isClientMode)
{
	WriteConfigString("Client Mode", "Enabled",
		isClientMode ? TEXT( "TRUE" ) : TEXT( "FALSE" )); 

	WriteConfigString("Server Mode", "Enabled",
		isClientMode ? TEXT( "FALSE" ) : TEXT( "TRUE" )); 
}

LPSTR CSettingsDlg::GetLocalTcpPort(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("Server Mode", "TcpPort", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_LOCAL_PORT);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetLocalTcpPort(LPSTR lpszBuffer)
{
	WriteConfigString("Server Mode", "TcpPort", lpszBuffer); 
}

LPSTR CSettingsDlg::GetLocalIpAddress(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("Server Mode", "IpAddress", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_LOCAL_IP);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetLocalIpAddress(LPSTR lpszBuffer)
{
	WriteConfigString("Server Mode", "IpAddress", lpszBuffer); 
}

LPSTR CSettingsDlg::GetRemoteTcpPort(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("Client Mode", "TcpPort", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_REMOTE_PORT);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetRemoteTcpPort(LPSTR lpszBuffer)
{
	WriteConfigString("Client Mode", "TcpPort", lpszBuffer); 
}


LPSTR CSettingsDlg::GetRemoteIpAddress(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("Client Mode", "IpAddress", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_REMOTE_IP);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetRemoteIpAddress(LPSTR lpszBuffer)
{
	WriteConfigString("Client Mode", "IpAddress", lpszBuffer); 
}

BOOL CSettingsDlg::IsProxyEnabled()
{
	char buf[8];

	if (!ReadConfigString("HTTP Proxy", "Enabled", buf, 8)) {
		strcpy(buf, DEFAULT_PROXY_ENABLED);
	}
	return !stricmp(buf, "TRUE");
}

void CSettingsDlg::SetProxyEnabled(BOOL isProxyEnabled)
{
	WriteConfigString("HTTP Proxy", "Enabled",
		isProxyEnabled ? TEXT( "TRUE" ) : TEXT( "FALSE" )); 
}

LPSTR CSettingsDlg::GetProxyTcpPort(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("HTTP Proxy", "TcpPort", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_PROXY_PORT);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetProxyTcpPort(LPSTR lpszBuffer)
{
	WriteConfigString("HTTP Proxy", "TcpPort", lpszBuffer); 
}

LPSTR CSettingsDlg::GetProxyIpAddress(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("HTTP Proxy", "IpAddress", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_PROXY_IP);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetProxyIpAddress(LPSTR lpszBuffer)
{
	WriteConfigString("HTTP Proxy", "IpAddress", lpszBuffer); 
}

BOOL CSettingsDlg::IsAuthEnabled()
{
	char buf[8];

	if (!ReadConfigString("HTTP Proxy", "AuthEnabled", buf, 8)) {
		strcpy(buf, DEFAULT_AUTH_ENABLED);
	}
	return !stricmp(buf, "TRUE");
}

void CSettingsDlg::SetAuthEnabled(BOOL isAuthEnabled)
{
	WriteConfigString("HTTP Proxy", "AuthEnabled",
		isAuthEnabled ? TEXT( "TRUE" ) : TEXT( "FALSE" )); 
}

LPSTR CSettingsDlg::GetAuthScheme(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("HTTP Proxy", "AuthScheme", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_AUTHSCHEME);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetAuthScheme(LPSTR lpszBuffer)
{
	WriteConfigString("HTTP Proxy", "AuthScheme", lpszBuffer); 
}

LPSTR CSettingsDlg::GetUserName(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("HTTP Proxy", "UserName", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_USERNAME);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetUserName(LPSTR lpszBuffer)
{
	WriteConfigString("HTTP Proxy", "UserName", lpszBuffer); 
}

LPSTR CSettingsDlg::GetPassword(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("HTTP Proxy", "Password", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_PASSWORD);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetPassword(LPSTR lpszBuffer)
{
	WriteConfigString("HTTP Proxy", "Password", lpszBuffer); 
}

LPSTR CSettingsDlg::GetWorkgroup(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("HTTP Proxy", "Workgroup", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_WORKGROUP);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetWorkgroup(LPSTR lpszBuffer)
{
	WriteConfigString("HTTP Proxy", "Workgroup", lpszBuffer); 
}


LPSTR CSettingsDlg::GetWorkstation(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("HTTP Proxy", "Workstation", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_WORKSTATION);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetWorkstation(LPSTR lpszBuffer)
{
	WriteConfigString("HTTP Proxy", "Workstation", lpszBuffer); 
}

BOOL CSettingsDlg::IsLogEnabled()
{
	char buf[8];

	if (!ReadConfigString("Logging", "LogEnabled", buf, 8)) {
		strcpy(buf, DEFAULT_LOG_ENABLED);
	}
	return !stricmp(buf, "TRUE");
}

void CSettingsDlg::SetLogEnabled(BOOL isLogEnabled)
{
	WriteConfigString("Logging", "LogEnabled", 
		isLogEnabled ? TEXT( "TRUE" ) : TEXT( "FALSE" )); 
}

LPSTR CSettingsDlg::GetLogDir(LPSTR lpszBuffer, int nBufferSize)
{
	if (!ReadConfigString("Logging", "LogDir", lpszBuffer, nBufferSize)) {
		strcpy(lpszBuffer, DEFAULT_LOG_DIR);
	}
	return lpszBuffer;
}

void CSettingsDlg::SetLogDir(LPSTR lpszLogDir)
{
	WriteConfigString("Logging", "LogDir", lpszLogDir); 
}

BOOL CSettingsDlg::IsPortDisabled(LPSTR lpszPortName)
{
	char buf[8];

	if(!ReadConfigString(lpszPortName, "Disabled", buf, 8)) {
		strcpy(buf, DEFAULT_PORT_DISABLED);
	}
	return !stricmp(buf, "TRUE");
}

void CSettingsDlg::SetPortDisabled(LPSTR lpszPortName, BOOL isDisabled)
{
	WriteConfigString(lpszPortName, "Disabled",
		isDisabled ? "TRUE" : "FALSE"); 
}

BOOL CSettingsDlg::SetPortConfig(LPSTR lpszPortName, DCB *lpDcb)
{
	char buf[32];

	wsprintf(buf, "%d", lpDcb->BaudRate);
	WriteConfigString(lpszPortName, "BaudRate", buf); 

	wsprintf(buf, "%d", lpDcb->ByteSize);
	WriteConfigString(lpszPortName, "ByteSize", buf); 

	switch (lpDcb->Parity) {
	case NOPARITY:
		strcpy(buf, "NOPARITY");
		break;
	case MARKPARITY:
		strcpy(buf, "MARKPARITY");
		break;
	case EVENPARITY:
		strcpy(buf, "EVENPARITY");
		break;
	case ODDPARITY:
		strcpy(buf, "ODDPARITY");
		break;
	case SPACEPARITY:
		strcpy(buf, "SPACEPARITY");
		break;
	}
	WriteConfigString(lpszPortName, "Parity", buf); 

	switch (lpDcb->StopBits) {
	case ONESTOPBIT:
		strcpy(buf, "1");
		break;
	case ONE5STOPBITS:
		strcpy(buf, "1.5");
		break;
	case TWOSTOPBITS:
		strcpy(buf, "2");
		break;
	}
	WriteConfigString(lpszPortName, "StopBits", buf); 

	WriteConfigString(lpszPortName, "fBinary",
		lpDcb->fBinary ? "TRUE" : "FALSE"); 

	WriteConfigString(lpszPortName, "fParity",
		lpDcb->fParity ? "TRUE" : "FALSE"); 

	WriteConfigString(lpszPortName, "fOutxCtsFlow",
		lpDcb->fOutxCtsFlow ? "TRUE" : "FALSE"); 

	WriteConfigString(lpszPortName, "fOutxDsrFlow",
		lpDcb->fOutxDsrFlow ? "TRUE" : "FALSE"); 

	switch (lpDcb->fDtrControl) {
	case DTR_CONTROL_DISABLE:
		strcpy(buf, "DTR_CONTROL_DISABLE");
		break;
	case DTR_CONTROL_ENABLE:
		strcpy(buf, "DTR_CONTROL_ENABLE");
		break;
	case DTR_CONTROL_HANDSHAKE:
		strcpy(buf, "DTR_CONTROL_HANDSHAKE");
		break;
	}
	WriteConfigString(lpszPortName, "fDtrControl", buf); 

	WriteConfigString(lpszPortName, "fDsrSensitivity",
		lpDcb->fDsrSensitivity ? "TRUE" : "FALSE"); 

	switch (lpDcb->fRtsControl) {
	case RTS_CONTROL_DISABLE:
		strcpy(buf, "RTS_CONTROL_DISABLE");
		break;
	case RTS_CONTROL_ENABLE:
		strcpy(buf, "RTS_CONTROL_ENABLE");
		break;
	case RTS_CONTROL_HANDSHAKE:
		strcpy(buf, "RTS_CONTROL_HANDSHAKE");
		break;
	case RTS_CONTROL_TOGGLE:
		strcpy(buf, "RTS_CONTROL_TOGGLE");
		break;
	}
	WriteConfigString(lpszPortName, "fRtsControl", buf); 

	WriteConfigString(lpszPortName, "fTXContinueOnXoff",
		lpDcb->fTXContinueOnXoff ? "TRUE" : "FALSE"); 

	WriteConfigString(lpszPortName, "fOutX",
		lpDcb->fOutX ? "TRUE" : "FALSE"); 

	WriteConfigString(lpszPortName, "fInX",
		lpDcb->fInX ? "TRUE" : "FALSE"); 

	WriteConfigString(lpszPortName, "fErrorChar",
		lpDcb->fErrorChar ? "TRUE" : "FALSE"); 

	WriteConfigString(lpszPortName, "fNull",
		lpDcb->fNull ? "TRUE" : "FALSE"); 

	WriteConfigString(lpszPortName, "fAbortOnError",
		lpDcb->fAbortOnError ? "TRUE" : "FALSE"); 

	wsprintf(buf, "%d", lpDcb->XonLim);
	WriteConfigString(lpszPortName, "XonLim", buf); 

	wsprintf(buf, "%d", lpDcb->XoffLim);
	WriteConfigString(lpszPortName, "XoffLim", buf); 

	wsprintf(buf, "%d", lpDcb->XonChar);
	WriteConfigString(lpszPortName, "XonChar", buf); 

	wsprintf(buf, "%d", lpDcb->XoffChar);
	WriteConfigString(lpszPortName, "XoffChar", buf); 

	wsprintf(buf, "%d", lpDcb->ErrorChar);
	WriteConfigString(lpszPortName, "ErrorChar", buf); 

	wsprintf(buf, "%d", lpDcb->EofChar);
	WriteConfigString(lpszPortName, "EofChar", buf); 

	wsprintf(buf, "%d", lpDcb->EvtChar);
	WriteConfigString(lpszPortName, "EvtChar", buf); 

	return TRUE;
}

BOOL CSettingsDlg::GetPortConfig(LPSTR lpszPortName, DCB *lpDcb)
{
#define MASK_BAUDRATE 0x0001
#define MASK_BYTESIZE 0x0002
#define MASK_PARITY   0x0004
#define MASK_STOPBITS 0x0008

	char buf[32];
	DWORD dwMask = 0;

	DCB dcb;
	memset(&dcb, 0, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	if (ReadConfigString(lpszPortName, "BaudRate", buf, 32)) {
		dwMask |= MASK_BAUDRATE;
		dcb.BaudRate = atoi(buf);
	} 

	if (ReadConfigString(lpszPortName, "ByteSize", buf, 32)) {
		dwMask |= MASK_BYTESIZE;
		dcb.ByteSize = atoi(buf);
	}

	if (ReadConfigString(lpszPortName, "Parity", buf, 32)) {
		if (!stricmp(buf, "NOPARITY")) {
			dcb.Parity = NOPARITY;
		} else if (!stricmp(buf, "MARKPARITY")) {
			dcb.Parity = MARKPARITY;
		} else if (!stricmp(buf, "EVENPARITY")) {
			dcb.Parity = EVENPARITY;
		} else if (!stricmp(buf, "ODDPARITY")) {
			dcb.Parity = ODDPARITY;
		} else if (!stricmp(buf, "SPACEPARITY")) {
			dcb.Parity = SPACEPARITY;
		} else {
			return FALSE;
		}
		dwMask |= MASK_PARITY;
	}

	if (ReadConfigString(lpszPortName, "StopBits", buf, 32)) {
		if (!stricmp(buf, "1")) {
			dcb.StopBits = ONESTOPBIT;
		} else if (!stricmp(buf, "1.5")) {
			dcb.StopBits = ONE5STOPBITS;
		} else if (!stricmp(buf, "2")) {
			dcb.StopBits = TWOSTOPBITS;
		} else {
			return FALSE;
		}
		dwMask |= MASK_STOPBITS;
	}

	if (ReadConfigString(lpszPortName, "fBinary", buf, 32)) {
		dcb.fBinary = !stricmp(buf, "TRUE");
	}

	if (ReadConfigString(lpszPortName, "fParity", buf, 32)) {
		dcb.fParity = !stricmp(buf, "TRUE");
	}

	if (ReadConfigString(lpszPortName, "fOutxCtsFlow", buf, 32)) {
		dcb.fOutxCtsFlow = !stricmp(buf, "TRUE");
	}

	if (ReadConfigString(lpszPortName, "fOutxDsrFlow", buf, 32)) {
		dcb.fOutxDsrFlow = !stricmp(buf, "TRUE");
	}

	if (ReadConfigString(lpszPortName, "fDtrControl", buf, 32)) {
		if (!stricmp(buf, "DTR_CONTROL_DISABLE")) {
			dcb.fDtrControl = DTR_CONTROL_DISABLE;
		} else if (!stricmp(buf, "DTR_CONTROL_ENABLE")) {
			dcb.fDtrControl = DTR_CONTROL_ENABLE;
		} else if (!stricmp(buf, "DTR_CONTROL_HANDSHAKE")) {
			dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
		} else {
			return FALSE;
		}
	}

	if (ReadConfigString(lpszPortName, "fDsrSensitivity", buf, 32)) {
		dcb.fDsrSensitivity = !stricmp(buf, "TRUE");
	}

	if (ReadConfigString(lpszPortName, "fRtsControl", buf, 32)) {
		if (!stricmp(buf, "RTS_CONTROL_DISABLE")) {
			dcb.fRtsControl = RTS_CONTROL_DISABLE;
		} else if (!stricmp(buf, "RTS_CONTROL_ENABLE")) {
			dcb.fRtsControl = RTS_CONTROL_ENABLE;
		} else if (!stricmp(buf, "RTS_CONTROL_HANDSHAKE")) {
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
		} else if (!stricmp(buf, "RTS_CONTROL_TOGGLE")) {
			dcb.fRtsControl = RTS_CONTROL_TOGGLE;
		} else {
			return FALSE;
		}
	}

	if (ReadConfigString(lpszPortName, "fTXContinueOnXoff", buf, 32)) {
		dcb.fTXContinueOnXoff = !stricmp(buf, "TRUE");
	}

	if (ReadConfigString(lpszPortName, "fOutX", buf, 32)) {
		dcb.fOutX = !stricmp(buf, "TRUE");
	}

	if (ReadConfigString(lpszPortName, "fInX", buf, 32)) {
		dcb.fInX = !stricmp(buf, "TRUE");
	}

	if (ReadConfigString(lpszPortName, "fErrorChar", buf, 32)) {
		dcb.fErrorChar = !stricmp(buf, "TRUE");
	}

	if (ReadConfigString(lpszPortName, "fNull", buf, 32)) {
		dcb.fNull = !stricmp(buf, "TRUE");
	}

	if (ReadConfigString(lpszPortName, "fAbortOnError", buf, 32)) {
		dcb.fAbortOnError = !stricmp(buf, "TRUE");
	}

	if (ReadConfigString(lpszPortName, "XonLim", buf, 32)) {
		dcb.XonLim = atoi(buf);
	}

	if (ReadConfigString(lpszPortName, "XoffLim", buf, 32)) {
		dcb.XoffLim = atoi(buf);
	}

	if (ReadConfigString(lpszPortName, "XonChar", buf, 32)) {
		dcb.XonChar = atoi(buf);
	}

	if (ReadConfigString(lpszPortName, "XoffChar", buf, 32)) {
		dcb.XoffChar = atoi(buf);
	}

	if (ReadConfigString(lpszPortName, "ErrorChar", buf, 32)) {
		dcb.ErrorChar = atoi(buf);
	}

	if (ReadConfigString(lpszPortName, "EofChar", buf, 32)) {
		dcb.EofChar = atoi(buf);
	}

	if (ReadConfigString(lpszPortName, "EvtChar", buf, 32)) {
		dcb.EvtChar = atoi(buf);
	}

	// Check minimum params set
	if ((MASK_BAUDRATE | MASK_BYTESIZE | MASK_PARITY | MASK_STOPBITS) == dwMask) {
		*lpDcb = dcb;
		return TRUE;
	}
	
	// Default to 9600,8,n,1
	memset(&dcb, 0, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = 9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	*lpDcb = dcb;

	return FALSE;

#undef MASK_BAUDRATE
#undef MASK_BYTESIZE
#undef MASK_PARITY
#undef MASK_STOPBITS
}

void CSettingsDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	UpdateData(TRUE);

	char buf[255];

	// Server mode
	if (m_checkServerMode.GetCheck() == BST_CHECKED) {
	
		// Check Local IP Address
		m_editLocalIp.GetWindowText(buf, 255);
		if (INADDR_NONE == inet_addr(buf)) {
			if (NULL == gethostbyname(buf)) {
				MessageBox("Wrong Local IP Address", "Error");
				UpdateControls(FALSE);
				return;
			}
		}

		// Check Local TCP Port
		m_editLocalPort.GetWindowText(buf, 255);
		if (atoi(buf) < 1 || atoi(buf) > 65535) {
			MessageBox("Invalid Local TCP Port", "Error");
			UpdateControls(FALSE);
			return;
		}
	}

	// Client mode
	if (m_checkClientMode.GetCheck() == BST_CHECKED) {
	
		// Check Remote IP Address
		m_editRemoteIp.GetWindowText(buf, 255);
		if (INADDR_NONE == inet_addr(buf)) {
			if (NULL == gethostbyname(buf)) {
				MessageBox("Wrong Remote IP Address", "Error");
				UpdateControls(FALSE);
				return;
			}
		}

		// Check Remote TCP Port
		m_editRemotePort.GetWindowText(buf, 255);
		if (atoi(buf) < 1 || atoi(buf) > 65535) {
			MessageBox("Invalid Remote TCP Port", "Error");
			UpdateControls(FALSE);
			return;
		}

		// Proxy
		if (m_checkProxy.GetCheck() == BST_CHECKED) {
		
			// Check Proxy IP Address
			m_editProxyIp.GetWindowText(buf, 255);
			if (INADDR_NONE == inet_addr(buf)) {
				if (NULL == gethostbyname(buf)) {
					MessageBox("Wrong Proxy IP Address", "Error");
					UpdateControls(FALSE);
					return;
				}
			}

			// Check Proxy TCP Port
			m_editProxyPort.GetWindowText(buf, 255);
			if (atoi(buf) < 1 || atoi(buf) > 65535) {
				MessageBox("Invalid Proxy TCP Port", "Error");
				UpdateControls(FALSE);
				return;
			}
		}
	}

	if (m_checkLog.GetCheck() == BST_CHECKED) {
		m_editLogDir.GetWindowText(buf, 255);
		WIN32_FIND_DATA ffd;
		HANDLE hFind = FindFirstFile(buf, &ffd);
		if (INVALID_HANDLE_VALUE == hFind) {
			MessageBox("Invalid Log Directory Path", "Error");
			UpdateControls(FALSE);
			return;
		}
		FindClose(hFind);
	}

	UpdateControls(TRUE);

	CDialog::OnOK();
}

void CSettingsDlg::EnableControls()
{
	BOOL isServerMode = (m_checkServerMode.GetCheck() == BST_CHECKED);
	BOOL isProxyEnabled = (m_checkProxy.GetCheck() == BST_CHECKED);
	BOOL isAuthEnabled = (m_checkAuth.GetCheck() == BST_CHECKED);
	BOOL isAuthNTLM = (m_radioNTLM.GetCheck() == BST_CHECKED ||
						m_radioNTLMv2.GetCheck() == BST_CHECKED);
	BOOL isLogEnabled = (m_checkLog.GetCheck() == BST_CHECKED);

	GetDlgItem(IDC_STATIC_LOCAL_GROUP)->EnableWindow(isServerMode);
	GetDlgItem(IDC_STATIC_LOCAL_IP)->EnableWindow(isServerMode);
	GetDlgItem(IDC_STATIC_LOCAL_PORT)->EnableWindow(isServerMode);
	m_editLocalIp.EnableWindow(isServerMode);
	m_editLocalPort.EnableWindow(isServerMode);

	GetDlgItem(IDC_STATIC_REMOTE_GROUP)->EnableWindow(!isServerMode);
	GetDlgItem(IDC_STATIC_REMOTE_IP)->EnableWindow(!isServerMode);
	GetDlgItem(IDC_STATIC_REMOTE_PORT)->EnableWindow(!isServerMode);
	m_editRemoteIp.EnableWindow(!isServerMode);
	m_editRemotePort.EnableWindow(!isServerMode);

	GetDlgItem(IDC_STATIC_PROXY_GROUP)->EnableWindow(!isServerMode);
	GetDlgItem(IDC_STATIC_PROXY_IP)->EnableWindow(!isServerMode && isProxyEnabled);
	GetDlgItem(IDC_STATIC_PROXY_PORT)->EnableWindow(!isServerMode && isProxyEnabled);

	GetDlgItem(IDC_STATIC_AUTH_GROUP)->EnableWindow(!isServerMode && isProxyEnabled);
	GetDlgItem(IDC_STATIC_USERNAME)->EnableWindow(!isServerMode && isProxyEnabled && isAuthEnabled);
	GetDlgItem(IDC_STATIC_PASSWORD)->EnableWindow(!isServerMode && isProxyEnabled && isAuthEnabled);
	GetDlgItem(IDC_STATIC_WORKGROUP)->EnableWindow(!isServerMode && isProxyEnabled && isAuthEnabled && isAuthNTLM);
	GetDlgItem(IDC_STATIC_WORKSTATION)->EnableWindow(!isServerMode && isProxyEnabled && isAuthEnabled && isAuthNTLM);

	m_checkProxy.EnableWindow(!isServerMode);
	m_editProxyIp.EnableWindow(!isServerMode && isProxyEnabled);
	m_editProxyPort.EnableWindow(!isServerMode && isProxyEnabled);
	
	m_checkAuth.EnableWindow(!isServerMode && isProxyEnabled);
	m_radioBasic.EnableWindow(!isServerMode && isProxyEnabled && isAuthEnabled);
	m_radioNTLM.EnableWindow(!isServerMode && isProxyEnabled && isAuthEnabled);
	m_radioNTLMv2.EnableWindow(!isServerMode && isProxyEnabled && isAuthEnabled);
	m_editUserName.EnableWindow(!isServerMode && isProxyEnabled && isAuthEnabled);
	m_editPassword.EnableWindow(!isServerMode && isProxyEnabled && isAuthEnabled);
	m_editWorkgroup.EnableWindow(!isServerMode && isProxyEnabled && isAuthEnabled && isAuthNTLM);
	m_editWorkstation.EnableWindow(!isServerMode && isProxyEnabled && isAuthEnabled && isAuthNTLM);

	GetDlgItem(IDC_STATIC_LOGDIR)->EnableWindow(isLogEnabled);
	m_editLogDir.EnableWindow(isLogEnabled);
}

void CSettingsDlg::OnCheckServerMode() 
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	BOOL isServerMode = (m_checkServerMode.GetCheck() == BST_CHECKED);
	if (!isServerMode) {
		m_checkServerMode.SetCheck(BST_CHECKED);
	}
	m_checkClientMode.SetCheck(BST_UNCHECKED);
	EnableControls();
	UpdateData(FALSE);
	
}

void CSettingsDlg::OnCheckClientMode() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	BOOL isClientMode = (m_checkClientMode.GetCheck() == BST_CHECKED);
	if (!isClientMode) {
		m_checkClientMode.SetCheck(BST_CHECKED);
	}
	m_checkServerMode.SetCheck(BST_UNCHECKED);
	EnableControls();
	UpdateData(FALSE);

}

void CSettingsDlg::OnCheckLogEnabled() 
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	EnableControls();
	UpdateData(FALSE);
}

void CSettingsDlg::OnCheckProxy() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	EnableControls();
	UpdateData(FALSE);

}

void CSettingsDlg::OnCheckAuth() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	EnableControls();
	UpdateData(FALSE);
}

void CSettingsDlg::OnRadioBasic() 
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	if (m_radioBasic.GetCheck() == BST_CHECKED) {
		m_radioNTLM.SetCheck(BST_UNCHECKED);
		m_radioNTLMv2.SetCheck(BST_UNCHECKED);
	}
	EnableControls();
	UpdateData(FALSE);	
}

void CSettingsDlg::OnRadioNtlm() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	if (m_radioNTLM.GetCheck() == BST_CHECKED) {
		m_radioBasic.SetCheck(BST_UNCHECKED);
		m_radioNTLMv2.SetCheck(BST_UNCHECKED);
	}
	EnableControls();
	UpdateData(FALSE);
}

void CSettingsDlg::OnRadioNtlmv2() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	if (m_radioNTLMv2.GetCheck() == BST_CHECKED) {
		m_radioNTLM.SetCheck(BST_UNCHECKED);
		m_radioBasic.SetCheck(BST_UNCHECKED);
	}
	EnableControls();
	UpdateData(FALSE);
}
