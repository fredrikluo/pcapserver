#include "utility.h"
#include <iostream>
#include <stdio.h>

using namespace std;

#include <stdarg.h>

void TRACE(bool withlineinfo, const char* file, int line, const char* format, ...)
{
    static char logmsg[20480];
    va_list ap;
    va_start(ap, format);

    vsnprintf(logmsg, 20480, format, ap);
    va_end(ap);
    if (withlineinfo)
        cerr << "FILE:" << file << " LINE:" << line << "   ";

    cerr << logmsg << endl;
}

string url_abs_to_rel(const string& url)
{
    if (url[0] == '/')
        return url;

    string protocol = url.substr(0, url.find(":"));

    if (protocol != "HTTP" && protocol != "http") {
        return string("");
    } else {
        int pos = url.find(":") + 3; //skip "//"

        pos = url.find("/", pos);

        if (pos != -1) {
            return url.substr(pos, url.size());
        } else
            return string("");
    }

    return string("");
}
