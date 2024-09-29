#pragma once
#include <Lib/Uefi.hpp>
#include <Arch/x86_64/device/acpi/xsdt.hpp>
PUBLIC namespace QuantumNEC::Architecture {
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