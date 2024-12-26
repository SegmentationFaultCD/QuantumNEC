#include <kernel/driver/acpi/acpi.hpp>
#include <kernel/print.hpp>
namespace QuantumNEC::Kernel::x86_64 {
using namespace Kernel;
using namespace std;
Fadt::Fadt( IN Xsdt &xsdt ) noexcept {
    this->fadt = xsdt.find_table< Fadt >( );
    if ( !this->fadt ) {
        println< print_level::ERROR >( "Can not fadt Fadt." );
    }
}
}     // namespace QuantumNEC::Kernel::x86_64