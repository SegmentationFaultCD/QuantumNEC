#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    using namespace Kernel;
    using namespace std;
    Bgrt::Bgrt( IN Xsdt & xsdt ) noexcept {
        this->brgt = xsdt.find_table< Bgrt >( );
        if ( !this->brgt ) {
            println< ostream::HeadLevel::ERROR >( "Can not find bgrt." );
            while ( true );
        }
    }
}