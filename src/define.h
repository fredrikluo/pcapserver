/**
 * This is where to put all type defines
 */
#ifndef _DEFINE_H
#define _DEFINE_H

#ifdef UNICODE
#include <wstring>
#define stdstring std::wstring
#define _T(x) L#x
#define TCHAR wchar_t
#else
#include <string>
#define _T(x) x
#define stdstring std::string
#define TCHAR char
#endif

typedef unsigned int u_int;
typedef unsigned char u_char;

//Don't request NOSIGNAL on systems where this is not implemented.
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0x0
#endif

#endif // _DEFINE_H
