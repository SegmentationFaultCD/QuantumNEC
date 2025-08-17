#pragma once
#include <cstdint>
#include <kernel/driver/acpi/table.hpp>
#include <kernel/driver/acpi/xsdt.hpp>
#include <limine.h>
namespace Driver {
struct [[gnu::packed]] XSDP {
    std::uint64_t signature;
    std::uint8_t  checksum;
    char          OEMID[ 6 ];
    std::uint8_t  revision;
    std::uint32_t rsdt_address;     // deprecated since version 2.0

    std::uint32_t Length;
    std::uint64_t xsdt_address;
    std::uint8_t  extended_checksum;
    std::uint8_t  reserved[ 3 ];

    explicit XSDP( void ) = default;

    auto get_xsdt( ) {
        return (XSDT *)this->xsdt_address;
    }

    consteval static auto get_signature( ) {
        return Table::SIGN_64( 'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ' );
    }
};
}     // namespace Driver