#include <kernel/driver/acpi/x86_64/rsdt.hpp>
#include <kernel/print.hpp>
namespace QuantumNEC::Kernel::x86_64 {
using namespace Kernel;
using namespace std;
Rsdt::Rsdt( IN Rsdp &rsdp ) noexcept {
    this->rsdt = (Rsdt::RSDT *)uint64_t( rsdp.get_rsdp( )->rsdt_address );
    if ( !this->rsdt || this->rsdt->signature != this->signature || !rsdp.check_sum( this->rsdt, this->rsdt->length ) ) {
        println< print_level::ERROR >( "RSDT is not valid." );
    }
}
}     // namespace QuantumNEC::Kernel::x86_64