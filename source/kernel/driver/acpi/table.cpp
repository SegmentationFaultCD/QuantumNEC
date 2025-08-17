#include <kernel/driver/acpi/table.hpp>
#include <kernel/driver/acpi/xsdp.hpp>
namespace Driver {
auto initialize_acpi( limine_rsdp_response *rsdp ) -> void {
    auto xsdp = (XSDP *)rsdp->address;
    if ( xsdp->signature != XSDP::get_signature( ) ) {
    }
    auto xsdt = xsdp->get_xsdt( );
    if ( xsdt->signature != XSDT::get_signature( ) ) {
    }
    
}
}     // namespace Driver