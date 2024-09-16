#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    using namespace Kernel;
    using namespace Lib;
    Dsdt::Dsdt( IN Xsdt & xsdt ) noexcept {
        this->dsdt = xsdt.find_table< Dsdt >( );
        if ( !this->dsdt ) {
            println< ostream::HeadLevel::ERROR >( "Can not find Dsdt." );
            while ( true );
        }
    }
}