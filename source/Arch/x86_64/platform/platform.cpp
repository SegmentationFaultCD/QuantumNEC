#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/IO/Stream/iostream>
#include <Kernel/task.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    x86_64Architecture::x86_64Architecture( IN Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept :
        Device { _config },
        CPUs { },
        Interrupt { } {
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::INFO ] << "QuantumNEC-System-Architecture-Mode : intel IA-32e(x86-64)." << Lib::IO::endl;
    }
    ControlRegisterFrame::ControlRegisterFrame( VOID ) noexcept {
        auto _cr0 = x86_64Architecture::read_cr0( );
        auto _cr2 = x86_64Architecture::read_cr2( );
        auto _cr3 = x86_64Architecture::read_cr3( );
        auto _cr4 = x86_64Architecture::read_cr4( );
        auto _cr8 = x86_64Architecture::read_cr8( );
        this->cr0 = *(unsigned long long *)( &_cr0 );
        this->cr2 = *(unsigned long long *)( &_cr2 );
        this->cr3 = *(unsigned long long *)( &_cr3 );
        this->cr4 = *(unsigned long long *)( &_cr4 );
        this->cr8 = *(unsigned long long *)( &_cr8 );
    }
    ControlRegisterFrame::~ControlRegisterFrame( VOID ) noexcept {
    }
    _C_LINK auto save_current_frame( IN CONST Kernel::ProcessFrame * frame )->VOID {
        *Kernel::get_current< Kernel::Process >( )->cpu_frame = *frame;
    }
}