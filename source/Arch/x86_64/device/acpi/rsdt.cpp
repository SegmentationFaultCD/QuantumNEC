#include <Arch/x86_64/device/acpi/rsdp.hpp>
#include <Arch/x86_64/device/acpi/rsdt.hpp>
#include <Kernel/print.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    using namespace Kernel;
    using namespace std;
    Rsdt::Rsdt( IN Rsdp & rsdp ) noexcept {
        this->rsdt = (Rsdt::RSDT *)uint64_t( rsdp.get_rsdp( )->rsdt_address );
        if ( !this->rsdt || this->rsdt->signature != this->signature || !rsdp.check_sum( this->rsdt, this->rsdt->length ) ) {
            println< ostream::HeadLevel::ERROR >( "RSDT is not valid." );
            while ( true );
        }
    }
}