#pragma once
#include <Lib/Uefi.hpp>
#include <Arch/x86_64/device/acpi/xsdt.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class Ssdt
    {
    public:
        struct SSDT : Xsdt::ACPISDTHeader
        {
            explicit SSDT( VOID ) noexcept = default;

        } _packed;

        using table_type = SSDT;

    public:
        explicit Ssdt( IN Xsdt &xsdt ) noexcept;

    public:
        constexpr static auto signature { SIGN_32( 'S', 'S', 'D', 'T' ) };

    private:
        inline STATIC SSDT *ssdt;
    };
}