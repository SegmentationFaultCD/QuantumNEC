#include <kernel/driver/acpi/acpi.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    using namespace Kernel;
    using namespace std;
    Srat::Srat( IN Xsdt & xsdt ) noexcept {
        this->srat = xsdt.find_table< Srat >( );
        if ( !this->srat ) {
            println< print_level::ERROR >( "Can not find Srat." );
        }
    }
}