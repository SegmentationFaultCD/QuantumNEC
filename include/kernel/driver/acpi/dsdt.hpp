#pragma once
#include <lib/Uefi.hpp>
#include <kernel/driver/acpi/xsdt.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class Dsdt
    {
    public:
        struct DSDT : Xsdt::ACPISDTHeader
        {
            explicit DSDT( VOID ) noexcept = default;

        } _packed;

        using table_type = DSDT;

    public:
        explicit Dsdt( IN Xsdt &xsdt ) noexcept;

    public:
        constexpr static auto signature { SIGN_32( 'D', 'S', 'D', 'T' ) };

    private:
        inline STATIC DSDT *dsdt;
    };
}