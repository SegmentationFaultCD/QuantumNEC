#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
#include <Arch/x86_64/cpu/cpu.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC constexpr CONST auto NR_SYS_CALL { 20 };
    PUBLIC class Syscall
    {
        /*
         * 这个结构是这样的
         * 可以有超过三个系统调用
         * 前三个是属于消息处理的
         * 后面可以接着加上
         * 顺序就是枚举类SyscallFunction里的枚举类型顺序
         * 调用约定：RAX->返回值，RAX->功能号（SyscallFunction），RDI、RSI、RDX、RCX、R8、R9（和linux差不多）
         */
    public:
        enum class SyscallStatus : Lib::Types::uint32_t {
            ERROR,
            SUCCESS,
            NO_SYSCALL,
            DEADLOCK,
            NO_DESTINATION,
            NO_SOURCE,
        };
        enum class SyscallFunction : Lib::Types::uint32_t {
            MESSAGE_SEND,
            MESSAGE_RECEIVE,
            MESSAGE_SEND_RECEIVE,
        };
        using SyscallEntry = Lib::Types::FuncPtr< Lib::Types::Ptr< Architecture::InterruptDescriptorTable::InterruptFrame >, Lib::Types::Ptr< Architecture::InterruptDescriptorTable::InterruptFrame > >;

    public:
        explicit Syscall( VOID ) noexcept;
        virtual ~Syscall( VOID ) noexcept = default;

    public:
        STATIC auto set_syscall_table( IN Lib::Types::uint64_t index, IN SyscallEntry entry ) -> VOID;
        STATIC auto get_syscall_table( VOID ) -> Lib::Types::Ptr< SyscallEntry >;
    };
    PUBLIC inline auto __syscall( IN Syscall::SyscallFunction function, IN auto arg1, IN auto arg2, IN auto arg3, IN auto arg4, IN auto arg5, IN auto arg6 ) {
        Syscall::SyscallStatus state { };
        register auto _a0 __asm__( "rax" ) = function;
        register auto _a1 __asm__( "rdi" ) = arg1;
        register auto _a2 __asm__( "rsi" ) = arg2;
        register auto _a3 __asm__( "rdx" ) = arg3;
        register auto _a4 __asm__( "rcx" ) = arg4;
        register auto _a5 __asm__( "r8" ) = arg5;
        register auto _a6 __asm__( "r9" ) = arg6;

        ASM( "INT $0x80\n\t" : "=a"( state ) : "r"( _a0 ), "r"( _a1 ), "r"( _a2 ), "r"( _a3 ), "r"( _a4 ), "r"( _a5 ), "r"( _a6 ) : "memory", "cc", "r11" );
        return state;
    }
}