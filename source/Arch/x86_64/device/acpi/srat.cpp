#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    using namespace Kernel;
    using namespace std;
    Srat::Srat( IN Xsdt & xsdt ) noexcept {
        this->srat = xsdt.find_table< Srat >( );
        if ( !this->srat ) {
            println< ostream::HeadLevel::ERROR >( "Can not find Srat." );
            while ( true );
        }
    }
}