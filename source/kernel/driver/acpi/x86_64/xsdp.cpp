#include <kernel/driver/acpi/acpi.hpp>
#include <kernel/driver/acpi/x86_64/xsdp.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/print.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Kernel::x86_64;
using namespace std;
Xsdp::Xsdp( IN uint64_t xsdp_address ) noexcept {
    this->xsdp = (XSDP *)xsdp_address;

    if ( std::strncmp( this->xsdp->signature, "RSD PTR ", 8 ) || this->xsdp->revision != 2 ) {
        println< print_level::ERROR >( "XSDP found is not valid." );
    }
    if ( auto sum { bytes_sum_total( reinterpret_cast< const uint8_t * >( this->xsdp ), 20 ) }; sum ) {
        println< print_level::ERROR >( "XSDP found is not valid." );
    }
    if ( auto sum { bytes_sum_total( reinterpret_cast< const uint8_t * >( this->xsdp ), 36 ) }; sum ) {
        println< print_level::ERROR >( "XSDP found is not valid." );
    }
}