#include <kernel/driver/acpi/acpi.hpp>
#include <kernel/print.hpp>
namespace QuantumNEC::Kernel::x86_64 {
using namespace Kernel;
using namespace std;
Ssdt::Ssdt( IN Xsdt &xsdt ) noexcept {
    this->ssdt = xsdt.find_table< Ssdt >( );
    if ( !this->ssdt ) {
        println< print_level::ERROR >( "Can not find Ssdt." );
    }
}
}     // namespace QuantumNEC::Kernel::x86_64