#pragma once
#include <Lib/Uefi.hpp>
#include <Arch/x86_64/cpu/cpu.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC constexpr CONST auto NR_SYS_CALL { 20 };
    PUBLIC class Syscall
    {
    public:
        /*
         * 这个结构是这样的
         * 可以有超过三个系统调用
         * 前三个是属于消息处理的
         * 后面可以接着加上
         * 顺序就是枚举类SyscallFunction里的枚举类型顺序
         * 调用约定：RAX->返回值，RAX->功能号（SyscallFunction），RDI、RSI、RDX、RCX、R8、R9（和linux差不多）
         */

        using SyscallEntry = FuncPtr< InterruptDescriptorTable::InterruptFrame *, InterruptDescriptorTable::InterruptFrame * >;

    public:
        explicit Syscall( VOID ) noexcept;
        virtual ~Syscall( VOID ) noexcept = default;

    public:
        STATIC auto set_syscall_table( IN uint64_t index, IN SyscallEntry entry ) -> VOID;
        STATIC auto get_syscall_table( VOID ) -> SyscallEntry *;
    };
}
#include <Arch/x86_64/syscall/message.hpp>