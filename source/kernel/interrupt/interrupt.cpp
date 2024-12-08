#include <kernel/cpu/cpu.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/print.hpp>
namespace QuantumNEC::Kernel {
using namespace std;

auto Interrupt::enable_interrupt( void ) -> InterruptStatus {
    InterruptStatus status { InterruptStatus::DISABLE };
    if ( get_interrupt( ) == InterruptStatus::ENABLE ) {
        status = InterruptStatus::ENABLE;
        return status;
    }
    else {
        status = InterruptStatus::DISABLE;
        CPU::sti( );
        return status;
    }
}

auto Interrupt::disable_interrupt( void ) -> InterruptStatus {
    InterruptStatus status { InterruptStatus::DISABLE };
    if ( get_interrupt( ) == InterruptStatus::ENABLE ) {
        status = InterruptStatus::ENABLE;
        CPU::cli( );
        return status;
    }
    else {
        status = InterruptStatus::DISABLE;
        return status;
    }
}

auto Interrupt::set_interrupt( IN const InterruptStatus &status ) -> InterruptStatus {
    return status == InterruptStatus::ENABLE ? enable_interrupt( ) : disable_interrupt( );
}

auto Interrupt::get_interrupt( void ) -> InterruptStatus {
    return ( CPU::get_rflags( ) & 0x00000200 ) ? InterruptStatus::ENABLE : InterruptStatus::DISABLE;
}
}     // namespace QuantumNEC::Kernel
