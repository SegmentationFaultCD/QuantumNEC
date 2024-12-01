#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdarg.h>
typedef unsigned long long size_t;

int vsprintf( char *buf, const char *fmt, va_list args );
int vsnprintf( char *__restrict __s, size_t __maxlen, const char *__restrict __format, va_list __arg );
int sprintf( char *str, const char *format, ... );
int snprintf( char *str, size_t maxlen, const char *format, ... );
#ifdef __cplusplus
}
#endif