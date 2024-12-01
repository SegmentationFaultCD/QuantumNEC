#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
static int is_digit( char c ) {
    if ( c >= '0' && c <= '9' ) {
        return 1;     // 是数字字符，返回非零值
    }
    else {
        return 0;     // 不是数字字符，返回0
    }
}
#ifdef __cplusplus
}
#endif