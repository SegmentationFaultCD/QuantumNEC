#pragma once
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/task/process/process.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {

class Syscall {
    constexpr static auto NR_SERVICERS { 20 };

public:
    enum class Servicer : uint64_t {
        RESTART_SYSCALL,
        EXIT,
        FORK,

        // FILE SYSTEM SERVICER
        READ,
        WRITE,
        OPEN,
        CLOSE,
        // TASK SERVICER
        WAITPID,
        CREAT,
        LINK,
        UNLINK,
        EXECVE,
        CHDIR
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
     * 调用约定：RAX->返回值，RAX->功能号，RDI、RSI、RDX、RCX、R8、R9（和linux差不多）
     */

    using SyscallEntry = FuncPtr< Interrupt::InterruptFrame *, Interrupt::InterruptFrame * >;

public:
    explicit Syscall( void ) noexcept;
    virtual ~Syscall( void ) noexcept = default;

public:
    static auto initializate( void ) noexcept -> void;

public:
    static auto save_servicer( Servicer index, Process &&servicer ) -> void {
        ( servicers[ std::to_underlying( index ) ] = std::move( servicer ) ).detach( );
    }
    static auto get_servicer( Servicer index ) -> const Process & {
        return servicers[ std::to_underlying( index ) ];
    }

private:
    inline static Process servicers[ NR_SERVICERS ] { };
};
}     // namespace QuantumNEC::Kernel
