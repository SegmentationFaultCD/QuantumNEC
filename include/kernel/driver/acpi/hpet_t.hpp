#pragma once
#include <cstdint>
#include <kernel/driver/acpi/table.hpp>
namespace Driver {
class [[gnu::packed]] HPET : Table::ACPISDTHeader {
    uint8_t hardware_rev_id;
    uint8_t comparator_count : 5;
    uint8_t counter_size : 1;
    uint8_t reserved : 1;
    uint8_t legacy_replacement : 1;
    uint16_t pci_vendor_id;
    Table::GenericAddressStructure base_address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;

public:
    auto size( ) {
        return this->length;
    }

    consteval static auto get_signature( ) {
        return Table::SIGN_32( 'H', 'P', 'E', 'T' );
    }

    explicit HPET( void ) noexcept;
};
}     // namespace Driver