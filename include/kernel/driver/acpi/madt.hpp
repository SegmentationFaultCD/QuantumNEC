#pragma once
#include <kernel/driver/acpi/xsdt.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class Madt {
    private:
        enum class ICSAttribute : uint8_t {
            PROCESSOR_LOCAL_APIC                 = 0,
            IO_APIC                              = 1,
            IO_APIC_INTERRUPT_SOURCE_OVERRIDE    = 2,
            IO_APICNON_MASKABLE_INTERRUPT_SOURCE = 3,
            LOCAL_APIC_NON_MASKABLE_INTERRUPTS   = 4,
            LOCAL_APIC_ADDRESS_OVERRIDE          = 5,
            PROCESSOR_LOCAL_x2APIC               = 9
        };

        struct _packed MadtICS {
            explicit MadtICS( VOID ) noexcept = default;
            ICSAttribute type;
            uint8_t      length;
        };
        // This type represents a single logical processor and its local interrupt controller.
        struct _packed LocalApic : MadtICS {
            explicit LocalApic( VOID ) noexcept = default;
            uint8_t ACPI_processor_ID;
            uint8_t APIC_ID;
            // If flags bit 0 is set the CPU is able to be enabled, if it is not set you need to check bit 1.
            // If that one is set you can still enable it, if it is not the CPU can not be enabled and the OS should not try.
            uint32_t flags;
        };
        // This type represents a I/O APIC.
        // The global system interrupt base is the first interrupt number that this I/O APIC handles.
        // You can see how many interrupts it handles using the register by getting the number of redirection entries from register 0x01, as described in IO APIC Registers.
        struct _packed IOApic : MadtICS {
            explicit IOApic( VOID ) noexcept = default;
            uint8_t  IOApic_ID;
            uint8_t  reserved;
            uint32_t IOApic_address;
            uint32_t global_system_interrupt_base;
        };
        // This entry type contains the data for an Interrupt Source Override.
        // This explains how IRQ sources are mapped to global system interrupts.
        // For example, IRQ source for the timer is 0, and the global system interrupt will usually be 2.
        // So you could look for the I/O APIC with the base below 2 and within its redirection entries, then make the redirection entry for (2 - base) to be the timer interrupt.
        struct _packed IOApicInterruptSourceOverride : MadtICS {
            explicit IOApicInterruptSourceOverride( VOID ) noexcept = default;
            uint8_t  bus_source;
            uint8_t  irq_source;
            uint32_t global_system_interrupt;
            uint16_t flags;
        };
        // Specifies which I/O APIC interrupt inputs should be enabled as non-maskable.
        struct _packed IOAPICNonMaskableInterruptSource : MadtICS {
            explicit IOAPICNonMaskableInterruptSource( VOID ) noexcept = default;
            uint8_t  nmi_source;
            uint8_t  reserved;
            uint16_t flags;
            uint32_t global_system_interrupt;
        };
        // Configure these with the LINT0 and LINT1 entries in the Local vector table of the relevant processor(')s(') local APIC.
        struct _packed LocalAPICNonMaskableInterrupts : MadtICS {
            explicit LocalAPICNonMaskableInterrupts( VOID ) noexcept = default;
            uint8_t  ACPI_processor_ID;
            uint16_t flags;
            uint8_t  LINT;
        };
        // Provides 64 bit systems with an override of the physical address of the Local APIC.
        // There can only be one of these defined in the MADT.
        // If this structure is defined, the 64-bit Local APIC address stored within it should be used instead of the 32-bit Local APIC address stored in the MADT header.
        struct _packed LocalAPICAddressOverride : MadtICS {
            explicit LocalAPICAddressOverride( VOID ) noexcept = default;
            uint16_t reserved;
            uint64_t _64bit_physical_address_of_local_APIC;
        };
        // Represents a physical processor and its Local x2APIC.
        // Identical to Local APIC; used only when that struct would not be able to hold the required values.
        struct _packed ProcessorLocalx2APIC : MadtICS {
            explicit ProcessorLocalx2APIC( VOID ) noexcept = default;
            uint16_t reserved;
            uint32_t processor_local_x2APIC_ID;
            uint32_t flags;
            uint32_t ACPI_ID;
        };

    public:
        struct _packed MADT : Xsdt::ACPISDTHeader {
            explicit MADT( VOID ) noexcept = default;
            uint32_t local_apic_address;
            uint32_t flags;
        };

        using table_type = MADT;

        struct _packed HPET : Xsdt::ACPISDTHeader {
            explicit HPET( VOID ) noexcept = default;
            uint8_t                       hardware_rev_id;
            uint8_t                       comparator_count : 5;
            uint8_t                       counter_size : 1;
            uint8_t                       reserved : 1;
            uint8_t                       legacy_replacement : 1;
            uint16_t                      pci_vendor_id;
            Xsdt::GenericAddressStructure hpet_address;
            uint8_t                       hpet_number;
            uint16_t                      minimum_tick;
            uint8_t                       page_protection;
        };

    public:
        explicit Madt( IN Xsdt &xsdt ) noexcept;

    private:
        class
        {
        private:
            uint8_t globalSystemInterrupt_[ 24 ];

        public:
            auto operator[]( IN uint8_t source ) -> uint8_t & {
                return globalSystemInterrupt_[ source ];
            }
            auto get( IN uint8_t source ) -> uint8_t {
                return globalSystemInterrupt_[ source ] != 0xff ? globalSystemInterrupt_[ source ] : source;
            }
        } inline STATIC global_system_interrupt;

    public:
        constexpr static auto signature { SIGN_32( 'A', 'P', 'I', 'C' ) };

        constexpr static auto hpet_signature { SIGN_32( 'H', 'P', 'E', 'T' ) };

    private:
        inline STATIC MADT *madt;

    public:
        inline STATIC HPET *hpet;
    };
}