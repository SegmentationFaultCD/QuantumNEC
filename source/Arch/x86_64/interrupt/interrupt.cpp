#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    using namespace Lib;
    Interrupt::Interrupt( VOID ) noexcept :
        PIC { },
        PIT { },
        InterruptEntryRegister { } {
        // Interrupt Management 初始化
        println< ostream::HeadLevel::OK >( "Initialize the Interrupt Management." );
    }
    Interrupt::~Interrupt( VOID ) noexcept {
    }
    auto Interrupt::enable_interrupt( VOID )->InterruptStatus {
        InterruptStatus status { InterruptStatus::INTERRUPT_DISABLE };
        if ( get_interrupt( ) == InterruptStatus::INTERRUPT_ENABLE ) {
            status = InterruptStatus::INTERRUPT_ENABLE;
            return status;
        }
        else {
            status = InterruptStatus::INTERRUPT_DISABLE;
            CPUs::sti( );
            return status;
        }
    }

    auto Interrupt::disable_interrupt( VOID )->InterruptStatus {
        InterruptStatus status { InterruptStatus::INTERRUPT_DISABLE };
        if ( get_interrupt( ) == InterruptStatus::INTERRUPT_ENABLE ) {
            status = InterruptStatus::INTERRUPT_ENABLE;
            CPUs::cli( );
            return status;
        }
        else {
            status = InterruptStatus::INTERRUPT_DISABLE;
            return status;
        }
    }

    auto Interrupt::set_interrupt( IN CONST InterruptStatus & status )->InterruptStatus {
        return status == InterruptStatus::INTERRUPT_ENABLE ? enable_interrupt( ) : disable_interrupt( );
    }

    auto Interrupt::get_interrupt( VOID )->InterruptStatus {
        return ( CPUs::get_rflags( ) & 0x00000200 ) ? InterruptStatus::INTERRUPT_ENABLE : InterruptStatus::INTERRUPT_DISABLE;
    }
}
