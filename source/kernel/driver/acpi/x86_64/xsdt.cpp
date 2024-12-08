#include <kernel/driver/acpi/acpi.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/print.hpp>
namespace QuantumNEC::Kernel::x86_64 {
using namespace Kernel;
using namespace std;
Xsdt::Xsdt( IN Xsdp &xsdp ) noexcept {
    this->xsdt = (XSDT *)uint64_t( Kernel::x86_64::physical_to_virtual( xsdp.get_rsdp( )->xsdt_address ) );
    if ( !this->xsdt || this->xsdt->signature != this->signature || !Xsdp::check_sum( this->xsdt, this->xsdt->length ) ) {
        println< print_level::ERROR >( "XSDT is not valid." );
        while ( true );
    }
}
}     // namespace QuantumNEC::Kernel::x86_64