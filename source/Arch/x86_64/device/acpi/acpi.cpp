#include <Arch/x86_64/platform/platform.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    Acpi::Acpi( VOID ) noexcept {
        Xsdp xsdp { (uint64_t)__config.acpi_table.address };
        Xsdt xsdt { xsdp };
        Bgrt { xsdt };
        Dsdt { xsdt };
        Ssdt { xsdt };
        Fadt { xsdt };
        Madt { xsdt };
        Srat { xsdt };
    }
}