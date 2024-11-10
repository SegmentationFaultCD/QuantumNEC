#pragma once
#include <lib/Uefi.hpp>
#include <kernel/driver/acpi/xsdt.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class Ssdt
    {
    public:
        struct _packed SSDT : Xsdt::ACPISDTHeader {
            explicit SSDT( VOID ) noexcept = default;
        };

        using table_type = SSDT;

    public:
        explicit Ssdt( IN Xsdt &xsdt ) noexcept;

    public:
        constexpr static auto signature { SIGN_32( 'S', 'S', 'D', 'T' ) };

    private:
        inline STATIC SSDT *ssdt;
    };
}