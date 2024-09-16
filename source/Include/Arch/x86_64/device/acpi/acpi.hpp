#pragma once

#include <Lib/Uefi.hpp>

#include <Arch/x86_64/device/acpi/xsdt.hpp>
#include <Arch/x86_64/device/acpi/bgrt.hpp>
#include <Arch/x86_64/device/acpi/fadt.hpp>
#include <Arch/x86_64/device/acpi/dsdt.hpp>
#include <Arch/x86_64/device/acpi/madt.hpp>
#include <Arch/x86_64/device/acpi/srat.hpp>
#include <Arch/x86_64/device/acpi/xsdp.hpp>
#include <Arch/x86_64/device/acpi/ssdt.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class Acpi :
        public Bgrt,
        public Dsdt,
        public Ssdt,
        public Fadt,
        public Madt,
        public Srat
    {
    public:
        explicit Acpi( VOID ) noexcept;

    private:
        Xsdp xsdp;
        Xsdt xsdt;
    };
}
