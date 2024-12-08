#pragma once

namespace QuantumNEC::Architecture {
// inline auto syscall( IN SyscallFunction function, IN auto &&arg1, IN auto &&arg2, IN auto &&arg3, IN auto &&arg4, IN auto &&arg5, IN auto &&arg6 )->SyscallStatus {
//     register auto _a0 __asm__( "rax" ) = function;
//     register auto _a1 __asm__( "rdi" ) = arg1;
//     register auto _a2 __asm__( "rsi" ) = arg2;
//     register auto _a3 __asm__( "rdx" ) = arg3;
//     register auto _a4 __asm__( "rcx" ) = arg4;
//     register auto _a5 __asm__( "r8" ) = arg5;
//     register auto _a6 __asm__( "r9" ) = arg6;
//     SyscallStatus state { };
//     ASM( "INT $0x80\n\t"
//          : "=a"( state )
//          : "r"( _a0 ), "r"( _a1 ), "r"( _a2 ), "r"( _a3 ), "r"( _a4 ), "r"( _a5 ), "r"( _a6 )
//          : "memory", "cc", "r11" );
//     // RDI、RSI、RDX、RCX、R8、R9
//     return state;
// }     // namespace QuantumNEC::Architecture

// auto send_receive( IN SyscallFunction function, IN uint64_t source_destination, IN message_t & message )->SyscallStatus {
//     switch ( function ) {
//     case SyscallFunction::SYSCALL_MESSAGE_SEND:
//     case SyscallFunction::SYSCALL_MESSAGE_RECEIVE:
//         return syscall( function, source_destination, &message, 0, 0, 0, 0 );
//     case SyscallFunction::SYSCALL_MESSAGE_SEND_RECEIVE:
//         auto status = syscall( SyscallFunction::SYSCALL_MESSAGE_SEND, source_destination, &message, 0, 0, 0, 0 );
//         if ( status == SyscallStatus::SUCCESS ) {
//             status = syscall( SyscallFunction::SYSCALL_MESSAGE_RECEIVE, source_destination, &message, 0, 0, 0, 0 );
//         }
//         return status;
//     }
//     return SyscallStatus::NO_SYSCALL;
// }

}     // namespace QuantumNEC::Architecture