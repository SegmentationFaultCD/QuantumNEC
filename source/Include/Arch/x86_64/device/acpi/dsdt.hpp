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
        inline STATIC constexpr auto signature { Xsdp::SIGN_32( 'S', 'S', 'D', 'T' ) };

    private:
        inline STATIC DSDT *dsdt;
    };
}