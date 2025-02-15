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
    uint64_t state = 0;
    __asm__ __volatile__(
        "MOVQ %0, %%RAX\n\t"
        "MOVQ %1, %%RDI\n\t"
        "MOVQ %2, %%RSI\n\t"
        "MOVQ %3, %%RDX\n\t"
        "MOVQ %4, %%RCX\n\t"
        "MOVQ %5, %%R8\n\t"
        "MOVQ %6, %%R9\n\t"
        "syscall" : "=a"( state ) : "g"( (uint64_t)servicer_index ), "g"( arg1 ), "g"( arg2 ), "g"( arg3 ), "g"( arg4 ), "g"( arg5 ), "g"( arg6 ) : "memory", "cc", "r11" );
    return state;
}

#ifdef __cplusplus
}
#endif
