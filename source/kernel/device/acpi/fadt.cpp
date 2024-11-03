#include <kernel/driver/acpi/acpi.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    using namespace Kernel;
    using namespace std;
    Fadt::Fadt( IN Xsdt & xsdt ) noexcept {
        this->fadt = xsdt.find_table< Fadt >( );
        if ( !this->fadt ) {
            // println< ostream::HeadLevel::ERROR >( "Can not fadt Fadt." );
        }
    }
}