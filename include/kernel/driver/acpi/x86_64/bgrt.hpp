#pragma once
#include <kernel/driver/acpi/x86_64/xsdt.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel::x86_64 {
class Bgrt {
public:
    struct _packed BGRT : Xsdt::ACPISDTHeader {
        uint16_t version;
        uint8_t  status;
        uint8_t  imageType;
        void    *image;
        uint32_t xOffset;
        uint32_t yOffset;
    };
    using table_type = BGRT;

public:
    explicit Bgrt( IN Xsdt &xsdt ) noexcept;

public:
    constexpr static auto signature { SIGN_32( 'B', 'G', 'R', 'T' ) };

private:
    inline static BGRT *bgrt;
};
}     // namespace QuantumNEC::Kernel::x86_64