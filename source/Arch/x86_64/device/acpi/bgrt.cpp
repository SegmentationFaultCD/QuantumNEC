#include <Arch/x86_64/platform/platform.hpp>
#include <Kernel/print.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    using namespace Kernel;
    using namespace std;
    Bgrt::Bgrt( IN Xsdt & xsdt ) noexcept {
        this->bgrt = xsdt.find_table< Bgrt >( );
        if ( !this->bgrt ) {
            println< ostream::HeadLevel::ERROR >( "Can not find bgrt." );
        }
    }
}