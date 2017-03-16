#ifndef UTILITY_H
#define UTILITY_H

#include <assert.h>
#include <string>

void TRACE(bool withlineinfo, const char* file, int line, const char* format, ...);

#define ERROR_OUT(format, ...)                                   \
    {                                                            \
        TRACE(false, __FILE__, __LINE__, format, ##__VA_ARGS__); \
    }
#define INFO_OUT(format, ...)                                    \
    {                                                            \
        TRACE(false, __FILE__, __LINE__, format, ##__VA_ARGS__); \
    }

#ifdef DEBUG
#define DEBUG_TRACE(format, ...)                                \
    {                                                           \
        TRACE(true, __FILE__, __LINE__, format, ##__VA_ARGS__); \
    }
#else
#define DEBUG_TRACE(format, ...)
#endif

template <typename T1, typename T2>
static void vec_append(T1& t1, T2 &t2) {
    t1.insert(t1.end(), t2.begin(), t2.end());
}

template <typename T>
static T reverse_endian(const T src)
{
    T dest = 0;

    for (unsigned int i = 0; i < sizeof(T); i++)
        ((unsigned char*)&dest)[sizeof(T) - i - 1] = ((unsigned char*)&src)[i];

    return dest;
}

std::string url_abs_to_rel(const std::string& url);
#endif
