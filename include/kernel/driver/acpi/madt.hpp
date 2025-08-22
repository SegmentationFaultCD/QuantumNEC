#pragma once
#include <cstdint>
#include <kernel/driver/acpi/table.hpp>
namespace Driver {
struct [[gnu::packed]] MADT : Table::ACPISDTHeader {
    std::uint32_t local_APIC_address;
    std::uint32_t flags;

    enum class ICSAttribute : uint8_t {
        PROCESSOR_LOCAL_APIC              = 0,
        IO_APIC                           = 1,
        INTERRUPT_SOURCE_OVERRIDE         = 2,
        NO_MASKABLE_INTERRUPT_SOURCE      = 3,
        LOCAL_APIC_NO_MASKABLE_INTERRUPTS = 4,
        LOCAL_APIC_ADDRESS_OVERRIDE       = 5,
        IO_SAPIC                          = 6,
        LOCAL_SAPIC                       = 7,
        PLATFORM_INTERRUPT_SOURCE         = 8,
        PROCESSOR_LOCAL_x2APIC            = 9,
        LOCAL_X2APIC_NMI                  = 10,
    };
    struct [[gnu::packed]] MadtICS {
        ICSAttribute type;
        std::uint8_t length;
    };
    // This type represents a single logical processor and its local interrupt controller.
    struct [[gnu::packed]] LocalApic : MadtICS {
        std::uint8_t ACPI_processor_ID;
        std::uint8_t APIC_ID;
        // If flags bit 0 is set the CPU is able to be enabled, if it is not set you need to check bit 1.
        // If that one is set you can still enable it, if it is not the CPU can not be enabled and the OS should not try.
        std::uint32_t flags;
    };
    // This type represents a I/O APIC.
    // The global system interrupt base is the first interrupt number that this I/O APIC handles.
    // You can see how many interrupts it handles using the register by getting the number of redirection entries from register 0x01, as described in IO APIC Registers.
    struct [[gnu::packed]] IOApic : MadtICS {
        std::uint8_t  IOApic_ID;
        std::uint8_t  reserved;
        std::uint32_t IOApic_address;
        std::uint32_t global_system_interrupt_base;
    };
    // This entry type contains the data for an Interrupt Source Override.
    // This explains how IRQ sources are mapped to global system interrupts.
    // For example, IRQ source for the timer is 0, and the global system interrupt will usually be 2.
    // So you could look for the I/O APIC with the base below 2 and within its redirection entries, then make the redirection entry for (2 - base) to be the timer interrupt.
    struct [[gnu::packed]] InterruptSourceOverride : MadtICS {
        std::uint8_t  bus;
        std::uint8_t  source;
        std::uint32_t global_system_interrupt;
        struct {
            std::uint16_t polarity : 2;
            /*
             * Polarity of the APIC I/O input signals:
             * 00 Conforms to the specifications of the bus (for example,
             * EISA is active-low for level-triggered interrupts).
             * 01 Active high
             * 10 Reserved
             * 11 Active low
             */
            std::uint16_t trigger_mode : 2;
            /*
             * Trigger mode of the APIC I/O Input signals:
             * 00 Conforms to specifications of the bus (For example, ISA
             * is edge-triggered)
             * 01 Edge-triggered
             * 10 Reserved
             * 11 Level-triggered
             */
            std::uint16_t : 12;
        };
    };
    // Specifies which I/O APIC interrupt inputs should be enabled as non-maskable.
    struct [[gnu::packed]] NoMaskableInterruptSource : MadtICS {
        std::uint16_t flags;
        std::uint32_t global_system_interrupt;
    };
    // Configure these with the LINT0 and LINT1 entries in the Local vector table of the relevant processor(')s(') local APIC.
    struct [[gnu::packed]] LocalNoMaskableInterrupts : MadtICS {
        std::uint8_t  ACPI_processor_ID;
        std::uint16_t flags;
        std::uint8_t  local_APIC_LINT;
    };
    // Provides 64 bit systems with an override of the physical address of the Local APIC.
    // There can only be one of these defined in the MADT.
    // If this structure is defined, the 64-bit Local APIC address stored within it should be used instead of the 32-bit Local APIC address stored in the MADT header.
    struct [[gnu::packed]] LocalApicAddressOverride : MadtICS {
        std::uint16_t reserved;
        std::uint64_t _64bit_physical_address_of_local_APIC;
    };
    struct [[gnu::packed]] IOSApic : MadtICS {
        std::uint8_t  ACPI_processor_ID;
        std::uint8_t  local_SApic_ID;
        std::uint8_t  local_SApic_EID;
        std::uint8_t  reserved1;
        std::uint16_t reserved2;
        // If flags bit 0 is set the CPU is able to be enabled, if it is not set you need to check bit 1.
        // If that one is set you can still enable it, if it is not the CPU can not be enabled and the OS should not try.
        std::uint32_t flags;
        std::uint32_t ACPI_processor_UID_value;
        char          ACPI_Processor_UID_string[ 0 ];
    };
    struct [[gnu::packed]] LocalSApic : MadtICS {
        std::uint8_t  IOApic_ID;
        std::uint8_t  reserved;
        std::uint32_t global_system_interrupt_base;
        std::uint64_t IOSApic_address;
    };
    struct [[gnu::packed]] PlatformInterruptSource : MadtICS {
        std::uint16_t flags;
        std::uint8_t  interrupt_type;
        std::uint8_t  processor_ID;
        std::uint8_t  processor_EID;
        std::uint8_t  IOSApic_vector;
        std::uint32_t global_system_interrupt;
        struct {
            std::uint32_t CPEI_processor_override : 1;
            std::uint32_t : 31;
        } platform_interrupt_source_flags;
    };

    // Represents a physical processor and its Local x2APIC.
    // Identical to Local APIC; used only when that struct would not be able to hold the required values.
    struct [[gnu::packed]] ProcessorLocalx2APIC : MadtICS {
        std::uint16_t reserved;
        std::uint32_t processor_local_x2APIC_ID;
        std::uint32_t flags;
        std::uint32_t ACPI_ID;
    };

    struct [[gnu::packed]] Localx2APICNoMaskableInterrupts : MadtICS {
        std::uint16_t flags;
        std::uint32_t ACPI_processor_ID;
        std::uint8_t  local_x2apic_LINT;
    };

    auto operator[]( std::size_t size ) {
        auto ics = (MadtICS *)( this + 1 );
        for ( auto i = 0; i < size; ++i ) {
            ics = (MadtICS *)( (std::uint64_t)ics + ics->length );
        }
        return ics;
    }

    auto size( ) {
        return this->length;
    }

    consteval static auto get_signature( ) {
        return Table::SIGN_32( 'A', 'P', 'I', 'C' );
    }
};

}     // namespace Driver