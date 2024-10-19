#pragma once
#include <lib/Uefi.hpp>

#include <kernel/driver/acpi/xsdt.hpp>
#include <kernel/driver/acpi/bgrt.hpp>
#include <kernel/driver/acpi/fadt.hpp>
#include <kernel/driver/acpi/dsdt.hpp>
#include <kernel/driver/acpi/madt.hpp>
#include <kernel/driver/acpi/srat.hpp>
#include <kernel/driver/acpi/xsdp.hpp>
#include <kernel/driver/acpi/ssdt.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class Acpi

    {
    public:
        explicit Acpi( VOID ) noexcept;

    public:
        Xsdp *xsdp;
        Xsdt *xsdt;
        Madt *madt;
        Bgrt *bgrt;
        Fadt *fadt;
        Dsdt *dsdt;
        Ssdt *ssdt;
        Srat *srat;
    };
}
