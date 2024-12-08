#pragma once
#include <kernel/driver/acpi/x86_64/xsdt.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class Dsdt {
public:
    struct _packed DSDT : Xsdt::ACPISDTHeader {
        explicit DSDT( void ) noexcept = default;
    };

    using table_type = DSDT;

public:
    explicit Dsdt( IN Xsdt &xsdt ) noexcept;

public:
    constexpr static auto signature { SIGN_32( 'D', 'S', 'D', 'T' ) };

private:
    inline static DSDT *dsdt;
};
}     // namespace QuantumNEC::Kernel::x86_64