// NTLM.cpp
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <time.h>
#include "des_crypt.h"
#include "apr_md4.h"
#include "apr_md5.h"
#include "NTLM.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Flags
#define NTLM_Negotiate_Unicode				0x00000001 // Indicates that Unicode strings are supported for use in security buffer data.  
#define NTLM_Negotiate_OEM					0x00000002 // Indicates that OEM strings are supported for use in security buffer data.  
#define NTLM_Request_Target					0x00000004 // Requests that the server's authentication realm be included in the Type 2 message.  
#define NTLM_Negotiate_Sign					0x00000010 // Specifies that authenticated communication between the client and server should carry a digital signature (message integrity).  
#define NTLM_Negotiate_Seal					0x00000020 // Specifies that authenticated communication between the client and server should be encrypted (message confidentiality).  
#define NTLM_Negotiate_Datagram_Style		0x00000040 // Indicates that datagram authentication is being used.  
#define NTLM_Negotiate_Lan_Manager_Key		0x00000080 // Indicates that the Lan Manager Session Key should be used for signing and sealing authenticated communications.  
#define NTLM_Negotiate_Netware				0x00000100 // This flag's usage has not been identified.  
#define NTLM_Negotiate_NTLM					0x00000200 // Indicates that NTLM authentication is being used.  
#define NTLM_Negotiate_Anonymous			0x00000800 // Sent by the client in the Type 3 message to indicate that an anonymous context has been established. This also affects the response fields (as detailed in the "Anonymous Response" section).  
#define NTLM_Negotiate_Domain_Supplied		0x00001000 // Sent by the client in the Type 1 message to indicate that the name of the domain in which the client workstation has membership is included in the message. This is used by the server to determine whether the client is eligible for local authentication.  
#define NTLM_Negotiate_Workstation_Supplied	0x00002000 // Sent by the client in the Type 1 message to indicate that the client workstation's name is included in the message. This is used by the server to determine whether the client is eligible for local authentication.  
#define NTLM_Negotiate_Local_Call			0x00004000 // Sent by the server to indicate that the server and client are on the same machine. Implies that the client may use the established local credentials for authentication instead of calculating a response to the challenge.  
#define NTLM_Negotiate_Always_Sign			0x00008000 // Indicates that authenticated communication between the client and server should be signed with a "dummy" signature.  
#define NTLM_Target_Type_Domain				0x00010000 // Sent by the server in the Type 2 message to indicate that the target authentication realm is a domain.  
#define NTLM_Target_Type_Server				0x00020000 // Sent by the server in the Type 2 message to indicate that the target authentication realm is a server.  
#define NTLM_Target_Type_Share				0x00040000 // Sent by the server in the Type 2 message to indicate that the target authentication realm is a share. Presumably, this is for share-level authentication. Usage is unclear.  
#define NTLM_Negotiate_NTLM2_Key			0x00080000 // Indicates that the NTLM2 signing and sealing scheme should be used for protecting authenticated communications. Note that this refers to a particular session security scheme, and is not related to the use of NTLMv2 authentication. This flag can, however, have an effect on the response calculations (as detailed in the "NTLM2 Session Response" section).  
#define NTLM_Request_Init_Response			0x00100000 // This flag's usage has not been identified.  
#define NTLM_Request_Accept_Response		0x00200000 // This flag's usage has not been identified.  
#define NTLM_Request_NonNT_Session_Key		0x00400000 // This flag's usage has not been identified.  
#define NTLM_Negotiate_Target_Info			0x00800000 // Sent by the server in the Type 2 message to indicate that it is including a Target Information block in the message. The Target Information block is used in the calculation of the NTLMv2 response.  
#define NTLM_0x02000000						0x02000000 // Unknown.
#define NTLM_Negotiate_128					0x20000000 // Indicates that 128-bit encryption is supported.  
#define NTLM_Negotiate_Key_Exchange			0x40000000 // Indicates that the client will provide an encrypted master key in the "Session Key" field of the Type 3 message.  
#define NTLM_Negotiate_56					0x80000000 // Indicates that 56-bit encryption is supported.  


/*
[OS Version Structure]
Offset		Description						Content 
0			Major Version Number			1 byte 
1			Minor Version Number			1 byte 
2			Build Number					short 
4			Unknown							0x0000000f 

WindowsXP sp2 example
0x05 (major version 5) 
0x01 (minor version 1; Windows XP) 
0x280a (build number 2600 in hexadecimal little-endian) 
0x0000000f (unknown/reserved) 


[The Type 1 Message]

Offset		Description						Content 
0			NTLMSSP Signature				Null-terminated ASCII "NTLMSSP" (0x4e544c4d53535000) 
8			NTLM Message Type				long (0x01000000) 
12			Flags							long 
(16)		Supplied Domain (Optional)		security buffer 
(24)		Supplied Workstation (Optional) security buffer 
(32)		OS Version Structure (Optional)	8 bytes 
(32)(40)	start of data block if required 


[The Type 2 Message]

Offset		Description						Content 
0			NTLMSSP Signature				Null-terminated ASCII "NTLMSSP" (0x4e544c4d53535000) 
8			NTLM Message Type				long (0x02000000) 
12			Target Name						security buffer 
20			Flags							long 
24			Challenge						8 bytes 
(32)		Context (optional)				8 bytes (two consecutive longs) 
(40)		Target Information (optional)	security buffer 
(48)		OS Version Structure (Optional)	8 bytes 
32(48)(56)	start of data block 


[The Type 3 Message]

Offset		Description						Content 
0			NTLMSSP Signature				Null-terminated ASCII "NTLMSSP" (0x4e544c4d53535000) 
8			NTLM Message Type				long (0x03000000) 
12			LM/LMv2 Response				security buffer 
20			NTLM/NTLMv2 Response			security buffer 
28			Target Name						security buffer 
36			User Name						security buffer 
44			Workstation Name				security buffer 
(52)		Session Key (optional)			security buffer 
(60)		Flags (optional)				long 
(64)		OS Version Structure (Optional)	8 bytes 
52(64)(72)	start of data block 

*/

static DWORD FormatSecurityBuffer(LPBYTE lpSecurity, WORD wSecurityLength, LPVOID lpBuffer, DWORD dwBufferSize, DWORD dwPtrOffset, DWORD dwRecordOffset)
{
	LPBYTE buf = (LPBYTE)lpBuffer;
	if (wSecurityLength && lpSecurity) {
		memcpy(buf+dwRecordOffset, lpSecurity, wSecurityLength);
	}
	*((LPWORD)(buf + dwPtrOffset)) = wSecurityLength;
	*((LPWORD)(buf + dwPtrOffset + 2)) = wSecurityLength;
	*((LPDWORD)(buf + dwPtrOffset + 4)) = dwRecordOffset;
	return wSecurityLength;
}

static DWORD FormatOEMSecurityBuffer(LPSTR lpszSecurity, LPVOID lpBuffer, DWORD dwBufferSize, DWORD dwPtrOffset, DWORD dwRecordOffset)
{
	LPBYTE buf = (LPBYTE)lpBuffer;
	WORD len = 0;
	while (*lpszSecurity) {
		buf[dwRecordOffset+len] = (*lpszSecurity >= 'a' && *lpszSecurity<= 'z') ?
			*lpszSecurity + 'A' - 'a' : *lpszSecurity;
		lpszSecurity ++;
		len ++;
	}
	*((LPWORD)(buf + dwPtrOffset)) = len;
	*((LPWORD)(buf + dwPtrOffset + 2)) = len;
	*((LPDWORD)(buf + dwPtrOffset + 4)) = dwRecordOffset;
	return len;
}

static DWORD FormatUnicodeSecurityBuffer(LPSTR lpszSecurity, LPVOID lpBuffer, DWORD dwBufferSize, DWORD dwPtrOffset, DWORD dwRecordOffset)
{
	LPBYTE buf = (LPBYTE)lpBuffer;
	WORD len = 0;
	while (*lpszSecurity) {
		buf[dwRecordOffset+len] = *lpszSecurity;
		buf[dwRecordOffset+len+1] = 0;
		lpszSecurity ++;
		len += 2;
	}
	*((LPWORD)(buf + dwPtrOffset)) = len;
	*((LPWORD)(buf + dwPtrOffset + 2)) = len;
	*((LPDWORD)(buf + dwPtrOffset + 4)) = dwRecordOffset;
	return len;
}

BOOL FormatType1Message(BOOL useNTLMv2, LPSTR lpszDomain, LPSTR lpszWorkstation, LPVOID lpBuffer, DWORD dwBufferSize, LPDWORD lpMessageSize)
{
	LPBYTE buf = (LPBYTE)lpBuffer;
	memcpy(buf, "NTLMSSP", 8); // "NTLMSSP"
	*((LPDWORD)(buf +  8)) = 0x00000001; // Type1
	*((LPDWORD)(buf + 12)) = // Flags
		NTLM_Negotiate_Unicode |
		NTLM_Negotiate_OEM |
		NTLM_Request_Target |
		NTLM_Negotiate_NTLM |
		NTLM_Negotiate_Domain_Supplied |
		NTLM_Negotiate_Workstation_Supplied	|
		NTLM_Negotiate_Always_Sign |
		(useNTLMv2 ? NTLM_Negotiate_NTLM2_Key : 0) |
		NTLM_0x02000000 |
		(useNTLMv2 ? NTLM_Negotiate_128 : 0) |
		(useNTLMv2 ? NTLM_Negotiate_56 : 0) |
		0;
	DWORD dwOffset = 40;
	*((LPBYTE)(buf + 32)) = 5; // OS Major
	*((LPBYTE)(buf + 33)) = 1; // OS Minor
	*((LPWORD)(buf + 34)) = 2600;  // OS Build
	*((LPDWORD)(buf + 36)) = 0x0f000000; // OS Reserved
	dwOffset += FormatOEMSecurityBuffer(lpszWorkstation, lpBuffer, dwBufferSize, 24, dwOffset);
	dwOffset += FormatOEMSecurityBuffer(lpszDomain, lpBuffer, dwBufferSize, 16, dwOffset);
	*lpMessageSize = dwOffset;
	return TRUE;
}


LPSTR HexDump(LPVOID lpBuffer, DWORD dwBufferSize, LPSTR lpszHexDump)
{
	LPBYTE buf = (LPBYTE)lpBuffer;
	LPSTR hex = lpszHexDump;
	LPSTR asc = hex + (3*16);
	memset(hex, ' ', 4*16);
	for (DWORD dw=0; dw< dwBufferSize; dw ++, buf ++) {
		int nib = (*buf) >> 4;
		*hex ++ = (nib > 9 ? 'a' + nib - 10 : '0' + nib);
		nib = (*buf) & 0x0f;
		*hex ++ = (nib > 9 ? 'a' + nib - 10 : '0' + nib);
		*asc ++ = (*buf > 31 && *buf < 128) ? *buf : '.';
		if (dw % 16 == 15) {
			*asc ++ = '\r';
			*asc ++ = '\n';
			hex = asc;
			asc = hex + (3*16);
			memset(hex, ' ', 4*16);
		} else {
			*hex ++ = ' ';
		}
	}
	*asc = 0;
	return lpszHexDump;
}


// Applies odd parity to the given byte array.
static void oddParity(LPBYTE lpBuffer, DWORD dwBufferLength) 
{
    for (DWORD i = 0; i < dwBufferLength; i++) {
        BYTE b = lpBuffer[i];
        int needsParity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
							(b >> 4) ^ (b >> 3) ^ (b >> 2) ^
							(b >> 1)) & 0x01) == 0;
        if (needsParity) {
            lpBuffer[i] |= 0x01;
        } else {
            lpBuffer[i] &= 0xfe;
        }
    }
}

static void createDESKey(LPBYTE src, LPBYTE dest) 
{
	dest[0] = src[0];
	dest[1] = (BYTE) (src[0] << 7 | (src[1] & 0xff) >> 1);
	dest[2] = (BYTE) (src[1] << 6 | (src[2] & 0xff) >> 2);
	dest[3] = (BYTE) (src[2] << 5 | (src[3] & 0xff) >> 3);
	dest[4] = (BYTE) (src[3] << 4 | (src[4] & 0xff) >> 4);
	dest[5] = (BYTE) (src[4] << 3 | (src[5] & 0xff) >> 5);
	dest[6] = (BYTE) (src[5] << 2 | (src[6] & 0xff) >> 6);
	dest[7] = (BYTE) (src[6] << 1);
	oddParity(dest, 8);
}


static void lmHash(LPSTR lpszPassword, LPBYTE lpBuffer)
{
	BYTE key[14];
	memset(key, 0, 14);
	int length = 0;
	for (; length < 14 && *lpszPassword; length ++) {
		key[length] = (*lpszPassword >= 'a' && *lpszPassword <= 'z') ?
			*lpszPassword + 'A' - 'a' : *lpszPassword;
		lpszPassword ++;
	}
	BYTE lowKey[8]; 
	createDESKey(key, lowKey);
	BYTE highKey[8]; 
	createDESKey(key+7, highKey);
	memcpy(lpBuffer, "KGS!@#$%", 8);
	ecb_crypt((char *)lowKey, (char *)lpBuffer, 8, DES_ENCRYPT|DES_SW);
	memcpy(lpBuffer+8, "KGS!@#$%", 8);
	ecb_crypt((char *)highKey, (char *)lpBuffer+8, 8, DES_ENCRYPT|DES_SW);
}

static void lmResponse(LPBYTE lpHash, LPBYTE lpChallenge, LPBYTE lpBuffer)
{
	BYTE key[21];
	memset(key, 0, 21);
	memcpy(key, lpHash, 16);
	BYTE lowKey[8]; 
	createDESKey(key, lowKey);
	BYTE middleKey[8]; 
	createDESKey(key+7, middleKey);
	BYTE highKey[8]; 
	createDESKey(key+14, highKey);
	memcpy(lpBuffer, lpChallenge, 8);
	ecb_crypt((char *)lowKey, (char *)lpBuffer, 8, DES_ENCRYPT|DES_SW);
	memcpy(lpBuffer+8, lpChallenge, 8);
	ecb_crypt((char *)middleKey, (char *)lpBuffer+8, 8, DES_ENCRYPT|DES_SW);
	memcpy(lpBuffer+16, lpChallenge, 8);
	ecb_crypt((char *)highKey, (char *)lpBuffer+16, 8, DES_ENCRYPT|DES_SW);
}

static void getLMResponse(LPSTR lpszPassword, LPBYTE lpChallenge, LPBYTE lpBuffer)
{
	BYTE hash[16];
    lmHash(lpszPassword, hash);
    lmResponse(hash, lpChallenge, lpBuffer);
	//memset(lpBuffer+8, 0, 16);
}

static void getNTLMResponse(LPSTR lpszPassword, LPBYTE lpChallenge, LPBYTE lpBuffer)
{
	BYTE hash[16];
	int length = 0;
	for (; *lpszPassword; lpszPassword ++) {
		lpBuffer[length ++] = *lpszPassword;
		lpBuffer[length ++] = 0;
	}
	apr_md4(hash, lpBuffer, length);
    lmResponse(hash, lpChallenge, lpBuffer);
}

static void getLMv2Response(LPBYTE lpBuffer)
{
	srand(time(0) >> 1);
	for (int i = 0; i < 8; i ++) {
		lpBuffer[i] = (rand() & 0x0ff0) >> 4;
	}
	memset(lpBuffer + 8, 0, 16);
}

static void getNTLMv2Response(LPSTR lpszPassword, LPBYTE lpLMv2Response, LPBYTE lpChallenge, LPBYTE lpBuffer)
{
	BYTE hash[21];
	BYTE session[16];
	memcpy(hash, lpChallenge, 8);
	memcpy(hash+8, lpLMv2Response, 8);
	apr_md5(session, hash, 16);
//MessageBox(NULL, HexDump(session, 16, (char *)lpBuffer+512), "", MB_OK);
	int length = 0;
	for (; *lpszPassword; lpszPassword ++) {
		lpBuffer[length ++] = *lpszPassword;
		lpBuffer[length ++] = 0;
	}
	apr_md4(hash, lpBuffer, length);
//MessageBox(NULL, HexDump(hash, 16, (char *)lpBuffer+512), "", MB_OK);
	memset(hash+16, 0, 5);
   	BYTE lowKey[8]; 
	createDESKey(hash, lowKey);
//MessageBox(NULL, HexDump(lowKey, 8, (char *)lpBuffer+512), "", MB_OK);
	BYTE middleKey[8]; 
	createDESKey(hash+7, middleKey);
//MessageBox(NULL, HexDump(middleKey, 8, (char *)lpBuffer+512), "", MB_OK);
	BYTE highKey[8]; 
	createDESKey(hash+14, highKey);
//MessageBox(NULL, HexDump(highKey, 8, (char *)lpBuffer+512), "", MB_OK);
	memcpy(lpBuffer, session, 8);
	ecb_crypt((char *)lowKey, (char *)lpBuffer, 8, DES_ENCRYPT|DES_SW);
//MessageBox(NULL, HexDump(lpBuffer, 8, (char *)lpBuffer+512), "", MB_OK);
	memcpy(lpBuffer+8, session, 8);
	ecb_crypt((char *)middleKey, (char *)lpBuffer+8, 8, DES_ENCRYPT|DES_SW);
//MessageBox(NULL, HexDump(lpBuffer+8, 8, (char *)lpBuffer+512), "", MB_OK);
	memcpy(lpBuffer+16, session, 8);
	ecb_crypt((char *)highKey, (char *)lpBuffer+16, 8, DES_ENCRYPT|DES_SW);
//MessageBox(NULL, HexDump(lpBuffer+16, 8, (char *)lpBuffer+512), "", MB_OK);
}

BOOL FormatType3Message(BOOL useNTLMv2, LPSTR lpszDomain, LPSTR lpszWorkstation, LPSTR lpszUserName, LPSTR lpszPassword, LPBYTE lpChallenge, LPVOID lpBuffer, DWORD dwBufferSize, LPDWORD lpMessageSize)
{
	LPBYTE buf = (LPBYTE)lpBuffer;
	memcpy(buf, "NTLMSSP", 8); // "NTLMSSP"
	*((LPDWORD)(buf +  8)) = 0x00000003; // Type3
	LPSTR lpszTargetName = lpszUserName;
	if (lpszUserName = strchr(lpszUserName, '\\')) {
		*lpszUserName = 0;
		lpszUserName ++;
	} else {
		lpszUserName = lpszTargetName;
		lpszTargetName = lpszWorkstation;
	}
//	*((LPBYTE)(buf + 64)) = 5; // OS Major
//	*((LPBYTE)(buf + 65)) = 1; // OS Minor
//	*((LPWORD)(buf + 66)) = 2600; // OS Build
//	*((LPDWORD)(buf + 68)) = 0x0f000000; // OS Reserved
//	DWORD dwOffset = 72;
	DWORD dwOffset = 64;
	dwOffset += FormatUnicodeSecurityBuffer(lpszTargetName, buf, dwBufferSize, 28, dwOffset);
	dwOffset += FormatUnicodeSecurityBuffer(lpszUserName, buf, dwBufferSize, 36, dwOffset);
	dwOffset += FormatUnicodeSecurityBuffer(lpszWorkstation, buf, dwBufferSize, 44, dwOffset);
	dwOffset += FormatSecurityBuffer(NULL, 0, buf, dwBufferSize, 52, dwOffset); 
	BYTE response[24];
	if (useNTLMv2) {
		getLMv2Response(response);
		// web example
		//response[0] = 0xff;
		//response[1] = 0xff;
		//response[2] = 0xff;
		//response[3] = 0x00;
		//response[4] = 0x11;
		//response[5] = 0x22;
		//response[6] = 0x33;
		//response[7] = 0x44;
		// dump
		//response[0] = 0x2A;
		//response[1] = 0x06;
		//response[2] = 0xA8;
		//response[3] = 0x6B;
		//response[4] = 0x44;
		//response[5] = 0xEF;
		//response[6] = 0x8F;
		//response[7] = 0xE0;
		dwOffset += FormatSecurityBuffer(response, 24, buf, dwBufferSize, 12, dwOffset); 
		// web example
		//lpszPassword = "SecREt01";
		//lpChallenge[0] = 0x01;
		//lpChallenge[1] = 0x23;
		//lpChallenge[2] = 0x45;
		//lpChallenge[3] = 0x67;
		//lpChallenge[4] = 0x89;
		//lpChallenge[5] = 0xab;
		//lpChallenge[6] = 0xcd;
		//lpChallenge[7] = 0xef;
		// dump
		//lpChallenge[0] = 0x78;
		//lpChallenge[1] = 0x4B;
		//lpChallenge[2] = 0xB1;
		//lpChallenge[3] = 0x12;
		//lpChallenge[4] = 0x8F;
		//lpChallenge[5] = 0xB1;
		//lpChallenge[6] = 0x9E;
		//lpChallenge[7] = 0x69;
		getNTLMv2Response(lpszPassword, response, lpChallenge, response);
		dwOffset += FormatSecurityBuffer(response, 24, buf, dwBufferSize, 20, dwOffset); 
	} else {
		getLMResponse(lpszPassword, lpChallenge, response);
		dwOffset += FormatSecurityBuffer(response, 24, buf, dwBufferSize, 12, dwOffset); 
		getNTLMResponse(lpszPassword, lpChallenge, response);
		dwOffset += FormatSecurityBuffer(response, 24, buf, dwBufferSize, 20, dwOffset); 
	}
	*((LPDWORD)(buf + 60)) = // Flags
		NTLM_Negotiate_Unicode |
		NTLM_Request_Target |
		NTLM_Negotiate_NTLM |
		NTLM_Negotiate_Always_Sign |
		(useNTLMv2 ? NTLM_Negotiate_NTLM2_Key : 0) |
		0;
	*lpMessageSize = dwOffset;
	return TRUE;
}

