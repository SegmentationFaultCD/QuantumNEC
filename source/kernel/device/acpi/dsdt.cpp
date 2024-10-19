#include <kernel/driver/acpi/acpi.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    using namespace Kernel;
    using namespace std;
    Dsdt::Dsdt( IN Xsdt & xsdt ) noexcept {
        this->dsdt = xsdt.find_table< Dsdt >( );
        if ( !this->dsdt ) {
            println< ostream::HeadLevel::ERROR >( "Can not find Dsdt." );
        }
    }
}