#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    using namespace Kernel;
    using namespace Lib;
    Ssdt::Ssdt( IN Xsdt & xsdt ) noexcept {
        this->ssdt = xsdt.find_table< Ssdt >( );
        if ( !this->ssdt ) {
            println< ostream::HeadLevel::ERROR >( "Can not find Ssdt." );
            while ( true );
        }
    }
}