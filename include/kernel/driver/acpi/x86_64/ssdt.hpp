#pragma once
#include <kernel/driver/acpi/x86_64/xsdt.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class Ssdt {
public:
    struct _packed SSDT : Xsdt::ACPISDTHeader {
        explicit SSDT( void ) noexcept = default;
    };

    using table_type = SSDT;

public:
    explicit Ssdt( IN Xsdt &xsdt ) noexcept;

public:
    constexpr static auto signature { SIGN_32( 'S', 'S', 'D', 'T' ) };

private:
    inline static SSDT *ssdt;
};
}     // namespace QuantumNEC::Kernel::x86_64