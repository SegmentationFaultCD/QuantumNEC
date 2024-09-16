#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    using namespace Kernel;
    using namespace Lib;
    Fadt::Fadt( IN Xsdt & xsdt ) noexcept {
        this->fadt = xsdt.find_table< Fadt >( );
        if ( !this->fadt ) {
            println< ostream::HeadLevel::ERROR >( "Can not fadt Fadt." );
            while ( true );
        }
    }
}