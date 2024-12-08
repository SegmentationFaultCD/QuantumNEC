#include <kernel/driver/acpi/acpi.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <new>
namespace QuantumNEC::Kernel::x86_64 {
Acpi::Acpi( void ) noexcept {
    static byte_t buffer[ sizeof( Xsdp ) + sizeof( Xsdt ) + sizeof( Madt ) + sizeof( Bgrt ) + sizeof( Fadt ) + sizeof( Dsdt ) + sizeof( Ssdt ) + sizeof( Srat ) ] { };
    auto          p = buffer;
    this->xsdp      = new ( p ) Xsdp { (uint64_t)__config.acpi_table.address };
    this->xsdp      = new ( p ) Xsdp { (uint64_t)__config.acpi_table.address };
    p += sizeof( Xsdp );
    this->xsdt = new ( p ) Xsdt { *this->xsdp };
    p += sizeof( Xsdt );
    this->madt = new ( p ) Madt { *this->xsdt };
    p += sizeof( Madt );
    this->fadt = new ( p ) Fadt { *this->xsdt };
    p += sizeof( Fadt );
    this->dsdt = new ( p ) Dsdt { *this->xsdt };
    p += sizeof( Dsdt );
    this->ssdt = new ( p ) Ssdt { *this->xsdt };
    p += sizeof( Ssdt );
    this->srat = new ( p ) Srat { *this->xsdt };
    p += sizeof( Srat );
    this->bgrt = new ( p ) Bgrt { *this->xsdt };
}
}     // namespace QuantumNEC::Kernel::x86_64