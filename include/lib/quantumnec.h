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

static inline uint64_t syscall_entry( servicer servicer_index, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6 ) {
    uint64_t          state           = 0;
    register uint64_t a0 asm( "rax" ) = servicer_index;
    register uint64_t a1 asm( "rdi" ) = arg1;
    register uint64_t a2 asm( "rsi" ) = arg2;
    register uint64_t a3 asm( "rdx" ) = arg3;
    register uint64_t a4 asm( "rcx" ) = arg4;
    register uint64_t a5 asm( "r8" )  = arg5;
    register uint64_t a6 asm( "r9" )  = arg6;
    __asm__ __volatile__( "syscall" : "=a"( state ) : "r"( a0 ), "r"( a1 ), "r"( a2 ), "r"( a3 ), "r"( a4 ), "r"( a5 ), "r"( a6 ) : "memory", "cc", "r11" );
    return state;
}

#ifdef __cplusplus
}
#endif
