#pragma once
#include <lib/Uefi.hpp>

#include <kernel/driver/acpi/x86_64/bgrt.hpp>
#include <kernel/driver/acpi/x86_64/dsdt.hpp>
#include <kernel/driver/acpi/x86_64/fadt.hpp>
#include <kernel/driver/acpi/x86_64/madt.hpp>
#include <kernel/driver/acpi/x86_64/srat.hpp>
#include <kernel/driver/acpi/x86_64/ssdt.hpp>
#include <kernel/driver/acpi/x86_64/xsdp.hpp>
#include <kernel/driver/acpi/x86_64/xsdt.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class Acpi

{
public:
    explicit Acpi( void ) noexcept;

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
}     // namespace QuantumNEC::Kernel::x86_64
