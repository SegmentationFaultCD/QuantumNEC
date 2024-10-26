#pragma once
#include <lib/Uefi.hpp>
#include <kernel/driver/acpi/xsdt.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class Srat
    {
    public:
        struct SRAT : Xsdt::ACPISDTHeader
        {
            explicit SRAT( VOID ) noexcept = default;
            uint8_t reserved[ 12 ];
        } _packed;
        using table_type = SRAT;
        struct ProcessorLocalApicAffinityStructure
        {
            uint8_t type;           // 0x0 for this type of structure
            uint8_t length;         // 16
            uint8_t lo_DM;          // Bits [0:7] of the proximity domain
            uint8_t APIC_ID;        // Processor's APIC ID
            uint32_t flags;         // Haha the most useless thing ever
            uint8_t SAPIC_EID;      // The processor's local SAPIC EID. Don't even bother.
            uint8_t hi_DM[ 3 ];     // Bits [8:31] of the proximity domain
            uint32_t _CDM;          // The clock domain which the processor belongs to (more jargon)
        } _packed;
        struct MemoryAffinityStructure
        {
            uint8_t type;               // 0x1 for this type of structure
            uint8_t length;             // 40
            uint32_t domain;            // The domain to which this memory region belongs to
            uint8_t reserved1[ 2 ];     // Reserved
            uint32_t lo_base;           // Low 32 bits of the base address of the memory range
            uint32_t hi_base;           // High 32 bits of the base address of the memory range
            uint32_t lo_length;         // Low 32 bits of the length of the range
            uint32_t hi_length;         // High 32 bits of the length
            uint8_t reserved2[ 4 ];     // Reserved
            uint32_t flags;             // Flags
            uint8_t reserved3[ 8 ];     // Reserved
        } _packed;
        struct ProcessorLocalx2ApicAffinityStructure
        {
            uint8_t type;               // 0x2 for this type of structure
            uint8_t length;             // 24
            uint8_t reserved1[ 2 ];     // Must be zero
            uint32_t domain;            // The proximity domain which the logical processor belongs to
            uint32_t x2APIC_ID;         // Processor's x2APIC ID
            uint32_t flags;             // Haha the most useless thing ever
            uint32_t _CDM;              // The clock domain which the processor belongs to (more jargon)
            uint8_t reserved2[ 4 ];     // Reserved.
        } _packed;

    public:
        explicit Srat( IN Xsdt &xsdt ) noexcept;

    public:
        constexpr static auto signature { SIGN_32( 'S', 'R', 'A', 'T' ) };

    private:
        inline STATIC SRAT *srat;
    };
}