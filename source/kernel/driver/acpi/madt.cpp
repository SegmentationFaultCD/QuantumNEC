#include <kernel/driver/acpi/madt.hpp>
#include <kernel/driver/cpu/mmio.hpp>
#include <kernel/interrupt/apic.hpp>
#include <kernel/memory/allocator/page.hpp>
#include <kernel/memory/paging/hhdm.hpp>
#include <kernel/memory/paging/page_table.hpp>
namespace Driver {
MADT::MADT( void ) noexcept {
    using namespace Memory;
    Interrupt::apic.lapic_address = physical_to_virtual( this->local_APIC_address );
    paging->kernel_page_table->map(
        this->local_APIC_address,
        physical_to_virtual( this->local_APIC_address ),
        1,
        paging->kernel_page_table->PAGE_PRESENT | paging->kernel_page_table->PAGE_RW_W | paging->kernel_page_table->PAGE_US_S,
        Page::Type::P4Kib );
    auto ics = (MADT::MadtICS *)( this + 1 );
    for ( auto length = 0ul; length <= this->length; length += ics->length, ics = (MADT::MadtICS *)( (std::uint64_t)ics + ics->length ) ) {
        using enum MADT::ICSAttribute;
        switch ( ics->type ) {
        case PROCESSOR_LOCAL_APIC: {
            Interrupt::apic.local_apic_ID[ Interrupt::apic.core_count++ ] = ( (MADT::ProcessorLocalApic *)ics )->APIC_ID;
        } break;
        case IO_APIC: {
            auto &ioapic = Interrupt::apic.ioapic[ Interrupt::apic.ioapic_count ];

            ioapic.ioapic_address = physical_to_virtual( ( (MADT::IOApic *)ics )->IOApic_address );

            paging->kernel_page_table->map(
                virtual_to_physical( ioapic.ioapic_address ),
                ioapic.ioapic_address,
                1,
                paging->kernel_page_table->PAGE_PRESENT | paging->kernel_page_table->PAGE_RW_W | paging->kernel_page_table->PAGE_US_S,
                Page::Type::P2Mib );

            ioapic.ioapic_index_address = reinterpret_cast< void * >( ioapic.ioapic_address );
            ioapic.ioapic_data_address = reinterpret_cast< void * >( ioapic.ioapic_address + 0x10UL );
            ioapic.ioapic_EOI_address = reinterpret_cast< void * >( ioapic.ioapic_address + 0x40UL );
            ioapic.ioapic_id = ( (MADT::IOApic *)ics )->IOApic_ID;
            ioapic.gsi_base = ( (MADT::IOApic *)ics )->global_system_interrupt_base;
            ioapic.irq_count = ( ( MMIO< std::uint64_t > { ioapic.ioapic_index_address }[ 1 ] >> 16 ) & 0xFF ) + 1;

            Interrupt::apic.ioapic_count++;

        } break;
        case INTERRUPT_SOURCE_OVERRIDE:
            Interrupt::apic.iso[ Interrupt::apic.iso_count++ ] = *( (MADT::InterruptSourceOverride *)ics );
            break;
        case NO_MASKABLE_INTERRUPT_SOURCE:
            break;
        case LOCAL_APIC_NO_MASKABLE_INTERRUPTS:
            break;
        case LOCAL_APIC_ADDRESS_OVERRIDE:
            break;
        case IO_SAPIC:
            break;
        case LOCAL_SAPIC:
            break;
        case PLATFORM_INTERRUPT_SOURCE:
            break;
        case PROCESSOR_LOCAL_x2APIC:
            break;
        case LOCAL_X2APIC_NMI:
            break;
        }
    }
}

}     // namespace Driver