#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
    RESTART_SYSCALL,
    EXIT,
    FORK,
    READ,
    WRITE,
    OPEN,
    CLOSE,
    WAITPID,
    CREAT,
    LINK,
    UNLINK,
    EXECVE,
    CHDIR
} servicer;

static inline uint64_t syscall( servicer servicer, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6 ) {
    register uint64_t _a0 __asm__( "rax" ) = (uint64_t)servicer;
    register uint64_t _a1 __asm__( "rdi" ) = arg1;
    register uint64_t _a2 __asm__( "rsi" ) = arg2;
    register uint64_t _a3 __asm__( "rdx" ) = arg3;
    register uint64_t _a4 __asm__( "rcx" ) = arg4;
    register uint64_t _a5 __asm__( "r8" )  = arg5;
    register uint64_t _a6 __asm__( "r9" )  = arg6;
    uint64_t          state                = 0;
    __asm__ __volatile__( "syscall" : "=a"( state ) : "r"( _a0 ), "r"( _a1 ), "r"( _a2 ), "r"( _a3 ), "r"( _a4 ), "r"( _a5 ), "r"( _a6 ) : "memory", "cc", "r11" );
    return state;
}

#ifdef __cplusplus
}
#endif
