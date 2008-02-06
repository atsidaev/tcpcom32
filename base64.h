// base64.h
//////////////////////////////////////////////////////////////////////


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef BASE64_H_INCLUDED
#define BASE64_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

BOOL b64encode(LPBYTE src, DWORD len, LPSTR dest);
BOOL b64decode(LPSTR src, LPBYTE dest, LPDWORD len);

#ifdef __cplusplus
}
#endif


#endif // BASE64_H_INCLUDED

