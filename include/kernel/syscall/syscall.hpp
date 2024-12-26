#pragma once
#include <kernel/interrupt/interrupt.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
constexpr auto NR_SYS_CALL { 20 };
class Syscall {
public:
    enum class Function : uint64_t {
        SYSCALL_MESSAGE_SEND,
        SYSCALL_MESSAGE_RECEIVE,
        SYSCALL_MESSAGE_SEND_RECEIVE
    };
    enum class Status : uint64_t {
        ERROR,
        SUCCESS,
        NO_SYSCALL,
        DEADLOCK,
        NO_DESTINATION,
        NO_SOURCE,
    };

public:
    /*
     * 这个结构是这样的
     * 可以有超过三个系统调用
     * 前三个是属于消息处理的
     * 后面可以接着加上
     * 顺序就是枚举类SyscallFunction里的枚举类型顺序
     * 调用约定：RAX->返回值，RAX->功能号（SyscallFunction），RDI、RSI、RDX、RCX、R8、R9（和linux差不多）
     */

    using SyscallEntry = FuncPtr< Interrupt::InterruptFrame *, Interrupt::InterruptFrame * >;

public:
    explicit Syscall( void ) noexcept;
    virtual ~Syscall( void ) noexcept = default;

public:
    static auto initializate( void ) noexcept -> void;

public:
    static auto set_syscall_table( IN uint64_t index, IN SyscallEntry entry ) -> void;
    static auto get_syscall_table( void ) -> SyscallEntry *;
};
}     // namespace QuantumNEC::Kernel
