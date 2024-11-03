#include <kernel/driver/acpi/acpi.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    using namespace Kernel;
    using namespace std;
    Bgrt::Bgrt( IN Xsdt & xsdt ) noexcept {
        this->bgrt = xsdt.find_table< Bgrt >( );
        if ( !this->bgrt ) {
          //  println< ostream::HeadLevel::ERROR >( "Can not find bgrt." );
        }
    }
}