#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>

PUBLIC namespace QuantumNEC::Architecture {
    using namespace std;
    x86_64Architecture::x86_64Architecture( VOID ) noexcept :
        Device { },
        CPUs { },
        Interrupt { },
        Syscall { },
        SymmetricMultiprocessing { } {
        println< ostream::HeadLevel::INFO >( "QuantumNEC-System-Architecture-Mode : intel IA-32e(x86-64)." );
    }
    ControlRegisterFrame::ControlRegisterFrame( VOID ) noexcept {
        this->cr0 = x86_64Architecture::read_cr0( );
        this->cr2 = x86_64Architecture::read_cr2( );
        this->cr3 = x86_64Architecture::read_cr3( );
        this->cr4 = x86_64Architecture::read_cr4( );
        this->cr8 = x86_64Architecture::read_cr8( );
    }
}