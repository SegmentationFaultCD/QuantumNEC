#include <kernel/interrupt/interrupt.hpp>
#include <kernel/cpu/cpu.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    using namespace std;

    auto InterruptArch::enable_interrupt( VOID ) -> InterruptStatus {
        InterruptStatus status { InterruptStatus::INTERRUPT_DISABLE };
        if ( get_interrupt( ) == InterruptStatus::INTERRUPT_ENABLE ) {
            status = InterruptStatus::INTERRUPT_ENABLE;
            return status;
        }
        else {
            status = InterruptStatus::INTERRUPT_DISABLE;
            CPU::sti( );
            return status;
        }
    }

    auto InterruptArch::disable_interrupt( VOID ) -> InterruptStatus {
        InterruptStatus status { InterruptStatus::INTERRUPT_DISABLE };
        if ( get_interrupt( ) == InterruptStatus::INTERRUPT_ENABLE ) {
            status = InterruptStatus::INTERRUPT_ENABLE;
            CPU::cli( );
            return status;
        }
        else {
            status = InterruptStatus::INTERRUPT_DISABLE;
            return status;
        }
    }

    auto InterruptArch::set_interrupt( IN CONST InterruptStatus & status ) -> InterruptStatus {
        return status == InterruptStatus::INTERRUPT_ENABLE ? enable_interrupt( ) : disable_interrupt( );
    }

    auto InterruptArch::get_interrupt( VOID ) -> InterruptStatus {
        return ( CPU::get_rflags( ) & 0x00000200 ) ? InterruptStatus::INTERRUPT_ENABLE : InterruptStatus::INTERRUPT_DISABLE;
    }
}
