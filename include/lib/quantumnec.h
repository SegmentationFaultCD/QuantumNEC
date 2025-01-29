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

inline uint64_t syscall( servicer servicer_index, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6 ) {
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
/*
0000000000401000 <main>:
  401000:       55                      push   %rbp
  401001:       48 89 e5                mov    %rsp,%rbp
  401004:       48 8d 05 f9 ff ff ff    lea    -0x7(%rip),%rax        # 401004 <main+0x4>
  40100b:       49 bb e4 2f 00 00 00    movabs $0x2fe4,%r11
  401012:       00 00 00
  401015:       4c 01 d8                add    %r11,%rax
  401018:       48 83 ec 08             sub    $0x8,%rsp
  40101c:       6a 00                   push   $0x0
  40101e:       41 b9 00 00 00 00       mov    $0x0,%r9d
  401024:       41 b8 00 00 00 00       mov    $0x0,%r8d
  40102a:       b9 00 00 00 00          mov    $0x0,%ecx
  40102f:       ba 00 00 00 00          mov    $0x0,%edx
  401034:       be 00 00 00 00          mov    $0x0,%esi
  401039:       bf 00 00 00 00          mov    $0x0,%edi
  40103e:       49 ba 6d d0 ff ff ff    movabs $0xffffffffffffd06d,%r10
  401045:       ff ff ff
  401048:       4a 8d 04 10             lea    (%rax,%r10,1),%rax
  40104c:       ff d0                   call   *%rax
  40104e:       48 83 c4 10             add    $0x10,%rsp
  401052:       90                      nop
  401053:       eb fd                   jmp    401052 <main+0x52>

0000000000401000 <main>:
  401000:       55                      push   %rbp
  401001:       48 89 e5                mov    %rsp,%rbp
  401004:       48 8d 05 f9 ff ff ff    lea    -0x7(%rip),%rax        # 401004 <main+0x4>
  40100b:       49 bb e4 2f 00 00 00    movabs $0x2fe4,%r11
  401012:       00 00 00
  401015:       4c 01 d8                add    %r11,%rax
  401018:       48 83 ec 08             sub    $0x8,%rsp
  40101c:       6a 00                   push   $0x0
  40101e:       41 b9 00 00 00 00       mov    $0x0,%r9d
  401024:       41 b8 00 00 00 00       mov    $0x0,%r8d
  40102a:       b9 00 00 00 00          mov    $0x0,%ecx
  40102f:       ba 00 00 00 00          mov    $0x0,%edx
  401034:       be 00 00 00 00          mov    $0x0,%esi
  401039:       bf 00 00 00 00          mov    $0x0,%edi
  40103e:       49 ba 71 d0 ff ff ff    movabs $0xffffffffffffd071,%r10
  401045:       ff ff ff
  401048:       4a 8d 04 10             lea    (%rax,%r10,1),%rax
  40104c:       ff d0                   call   *%rax
  40104e:       48 83 c4 10             add    $0x10,%rsp
  401052:       b8 00 00 00 00          mov    $0x0,%eax
  401057:       c9                      leave
  401058:       c3                      ret


*/
#ifdef __cplusplus
}
#endif
