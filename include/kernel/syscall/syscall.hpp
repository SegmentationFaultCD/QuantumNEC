#pragma once
#include <array>
#include <cstdint>
#include <kernel/interrupt/idt.hpp>
namespace Kernel {
class Syscall {
public:
    struct Interface {
        virtual auto handle( Interrupt::IDT::Frame *frame ) -> Interrupt::IDT::Frame * = 0;
        virtual ~Interface( void ) {}
    };

    constexpr static auto NUMBER_OF_SYSCALL { 300 };

private:
    Syscall( void ) = default;

public:
    auto initialize( void ) -> Syscall *;

    auto register_syscall( std::uint64_t index, Interface *face ) -> void {
        if ( index < NUMBER_OF_SYSCALL ) {
            this->table[ index ] = face;
        }
    }

    // 调用约定：RAX->返回值 & 功能号，RDI、RSI、RDX、R10、R8、R9依次传入(遵循POSIX)
    auto call( Interrupt::IDT::Frame *frame ) -> Interrupt::IDT::Frame *;

private:
    std::array< Interface *, NUMBER_OF_SYSCALL > table;
} inline *syscall;
}     // namespace Kernel