#pragma once
#include <Lib/Uefi.hpp>
#include <Arch/x86_64/device/acpi/xsdt.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class Bgrt
    {
    public:
        struct BGRT : Xsdt::ACPISDTHeader
        {
            uint16_t version;
            uint8_t status;
            uint8_t imageType;
            VOID *image;
            uint32_t xOffset;
            uint32_t yOffset;
        } _packed;
        using table_type = BGRT;

    public:
        explicit Bgrt( IN Xsdt &xsdt ) noexcept;

    public:
        inline STATIC constexpr auto signature { Xsdp::SIGN_32( 'B', 'R', 'G', 'T' ) };

    private:
        inline STATIC BGRT *brgt;
    };
}