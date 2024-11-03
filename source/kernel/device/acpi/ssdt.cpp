#include <kernel/driver/acpi/acpi.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    using namespace Kernel;
    using namespace std;
    Ssdt::Ssdt( IN Xsdt & xsdt ) noexcept {
        this->ssdt = xsdt.find_table< Ssdt >( );
        if ( !this->ssdt ) {
            //  println< ostream::HeadLevel::ERROR >( "Can not find Ssdt." );
        }
    }
}