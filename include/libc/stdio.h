#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdarg.h>
typedef unsigned long long size_t;
#ifdef __cplusplus
int vsprintf( char       *buf,
              const char *fmt,
              va_list     args );
int vsnprintf( char *s, size_t maxlen, const char *fmt, va_list va )
    __attribute__( format( printf, 3, 0 ) );
}
#endif