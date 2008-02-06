// base64.cpp
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "base64.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ENCODE(b) \
( \
	((b) < 26) ? ('A' + (b)) : \
	((b) < 52) ? ('a' + ((b) - 26)) : \
	((b) < 62) ? ('0' + ((b) - 52)) : \
	((b) == 62) ? '+' : '/' \
)

#define DECODE(c) \
( \
	((c) >= 'A' && (c) <= 'Z') ? ((c) - 'A') : \
	((c) >= 'a' && (c) <= 'z') ? ((c) - 'a' + 26) : \
	((c) >= '0' && (c) <= '9') ? ((c) - '0' + 52) : \
	((c) == '+') ? 62 : 63 \
)

#define ISBASE64(c) \
( \
	((c) >= 'A' && (c) <= 'Z') ? TRUE : \
	((c) >= 'a' && (c) <= 'z') ? TRUE : \
	((c) >= '0' && (c) <= '9') ? TRUE : \
	((c) == '+') ? TRUE : \
	((c) == '/') ? TRUE : \
	((c) == '=') ? TRUE : FALSE \
)

BOOL b64encode(LPBYTE src, DWORD len, LPSTR dest)
{
	int c1, c2, c3;
	BYTE b1, b2, b3, b4;
	DWORD i = 0;
	int spare = (len % 3);
	len -= spare;
	while (i < len) {
		c1 = src[i++];
		c2 = src[i++];
		c3 = src[i++];
		b1 = (c1 >> 2);
		b2 = ((c1 & 0x03) << 4) | (c2 >> 4);
		b3 = ((c2 & 0x0f) << 2) | (c3 >> 6);
		b4 = (c3 & 0x3f);
		*dest++ = ENCODE(b1);
		*dest++ = ENCODE(b2);
		*dest++ = ENCODE(b3);
		*dest++ = ENCODE(b4);
/*		if (i % (76 / 4 * 3) == 0) {
			*dest++ ='\r';
			*dest++ ='\n';
		}
*/	}
	switch (spare) {
	case 1:
		c1 = src[i++];
		b1 = (c1 >> 2);
		b2 = ((c1 & 0x03) << 4) & 0xf0;
		*dest++ = ENCODE(b1);
		*dest++ = ENCODE(b2);
		*dest++ = '=';
		*dest++ = '=';
		break;
	case 2:
		c1 = src[i++];
		c2 = src[i++];
		b1 = (c1 >> 2);
		b2 = ((c1 & 0x03) << 4) | (c2 >> 4);
		b3 = ((c2 & 0x0f) << 2) & 0xfc;
		*dest++ = ENCODE(b1);
		*dest++ = ENCODE(b2);
		*dest++ = ENCODE(b3);
		*dest++ = '=';
		break;
	}
	*dest = 0;
	return TRUE;
}

BOOL b64decode(LPSTR src, LPBYTE dest, LPDWORD len)
{
	int slen = strlen(src);
	*len = 0;
	if (slen % 4) {
		return FALSE;
	}
	for (int j = 0; j < slen; j++) {
		if (!ISBASE64(src[j])) {
			return FALSE;
		}
	}
	int c1, c2, c3, c4;
	int b1, b2, b3, b4;
	int i = 0;
	int dlen = 0;
	while (i < slen) {
		c1 = src[i++];
		c2 = src[i++];
		c3 = src[i++];
		c4 = src[i++];
		b1 = DECODE(c1);
		b2 = DECODE(c2);
		*dest++ = (b1 << 2) | (b2 >> 4);
		dlen ++;
		if (c3 != '=') {
			b3 = DECODE(c3);
			*dest++ = ((b2 & 0x0f) << 4) | (b3 >> 2);
			dlen ++;
			if (c4 != '=') {
				b4 = DECODE(c4);
				*dest++ = ((b3 & 0x03) << 6) | b4;
				dlen ++;
			}
		}
	}
	*len = dlen;
	return TRUE;
}
