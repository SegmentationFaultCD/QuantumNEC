#include <kernel/driver/acpi/acpi.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/print.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    using namespace Kernel;
    using namespace std;
    Madt::Madt( IN Xsdt & xsdt ) noexcept {
        this->madt = xsdt.find_table< Madt >( );
        if ( !this->madt ) {
            println< ostream::HeadLevel::ERROR >( "Can not find Madt." );
            while ( true );
        }

        auto length { this->madt->length - sizeof *this->madt };
        auto ICS { reinterpret_cast< MadtICS * >( (uint64_t)( this->madt ) + sizeof *this->madt ) };
        std::memset( &this->global_system_interrupt, 0xFF, 24 );
        while ( length > 0 ) {
            switch ( ICS->type ) {
            case ICSAttribute::PROCESSOR_LOCAL_APIC: {
                auto entry { reinterpret_cast< LocalApic * >( ICS ) };
                println< ostream::HeadLevel::SYSTEM >( "Processor Local APIC : Acpi Processor UID <=> {} Apic ID <=> {} Flags <=> {:x}", (uint64_t)entry->ACPI_processor_ID, (uint64_t)entry->APIC_ID, (uint64_t)entry->flags );
                Apic::apic_map.local_apic_ID[ Apic::apic_map.core_count++ ] = entry->APIC_ID;
            } break;
            case ICSAttribute::IO_APIC: {     // 最多不超过8个
                auto entry { reinterpret_cast< IOApic * >( ICS ) };
                println< ostream::HeadLevel::SYSTEM >( "I/O APIC : I/O Apic ID <=> {} I/O Apic Address <=> {:x} Global System Interrupt Base <=> {:x}", (uint64_t)entry->IOApic_ID, (uint64_t)entry->IOApic_address, (uint64_t)entry->global_system_interrupt_base );
                auto &ioapic                 = Apic::apic_map.ioapic[ Apic::apic_map.ioapic_count ];
                ioapic.io_apic_address       = reinterpret_cast< uint64_t >( Kernel::physical_to_virtual( entry->IOApic_address ) );
                ioapic.io_apic_index_address = reinterpret_cast< VOID * >( ioapic.io_apic_address );
                ioapic.io_apic_data_address  = reinterpret_cast< VOID  *>( ioapic.io_apic_address + 0x10UL );
                ioapic.io_apic_EOI_address   = reinterpret_cast< VOID   *>( ioapic.io_apic_address + 0x40UL );
                Apic::apic_map.ioapic_count++;
            } break;
            case ICSAttribute::IO_APIC_INTERRUPT_SOURCE_OVERRIDE: {
                auto entry { reinterpret_cast< IOApicInterruptSourceOverride * >( ICS ) };
                println< ostream::HeadLevel::SYSTEM >( "I/O APIC Interrupt Source Override : Bus Source <=> {} Irq Source <=> {} Flags <=> {} Global System Interrupt <=> {}", entry->bus_source, entry->irq_source, (uint64_t)entry->flags, (uint64_t)entry->global_system_interrupt );
                this->global_system_interrupt[ entry->irq_source ] = entry->global_system_interrupt;
            } break;
            case ICSAttribute::IO_APICNON_MASKABLE_INTERRUPT_SOURCE: {
                auto entry { reinterpret_cast< IOAPICNonMaskableInterruptSource * >( ICS ) };
                println< ostream::HeadLevel::SYSTEM >(
                    "I/O APIC Non-maskable Interrupt Source : NMI Source <=> {} Flags <=> {} Global System Interrupt <=> {}",
                    (uint64_t)entry->nmi_source, (uint64_t)entry->flags, (uint64_t)entry->global_system_interrupt );
            } break;
            case ICSAttribute::LOCAL_APIC_NON_MASKABLE_INTERRUPTS: {
                auto entry { reinterpret_cast< LocalAPICNonMaskableInterrupts * >( ICS ) };
                println< ostream::HeadLevel::SYSTEM >(
                    "Local APIC Non-maskable interrupts : ACPI Processor ID <=> {} Flags <=> {} LINT# <=> {}",
                    (uint64_t)entry->ACPI_processor_ID, (uint64_t)entry->flags, (uint64_t)entry->LINT );
            } break;
            case ICSAttribute::LOCAL_APIC_ADDRESS_OVERRIDE: {
                auto entry { reinterpret_cast< LocalAPICAddressOverride * >( ICS ) };
                println< ostream::HeadLevel::SYSTEM >(
                    "Local APIC Address Override : 64-bit physical address of Local APIC <=> {}",
                    (uint64_t)entry->_64bit_physical_address_of_local_APIC );
            } break;
            case ICSAttribute::PROCESSOR_LOCAL_x2APIC: {
                auto entry { reinterpret_cast< ProcessorLocalx2APIC * >( ICS ) };
                println< ostream::HeadLevel::SYSTEM >(
                    "Processor Local x2APIC : Processor\'s local x2APIC ID <=> {} Flags <=> {} ACPI ID <=> {}",
                    (uint64_t)entry->processor_local_x2APIC_ID, (uint64_t)entry->flags, (uint64_t)entry->ACPI_ID );
            } break;
            }
            length -= ICS->length;
            ICS = reinterpret_cast< decltype( ICS ) >( (char *)( ICS ) + ICS->length );
        }

        Apic::apic_map.local_apic_address = reinterpret_cast< uint64_t >( ( Kernel::physical_to_virtual( this->madt->local_apic_address ) ) );

        this->hpet = (HPET *)xsdt.find_table< Madt >( hpet_signature );

        println< ostream::HeadLevel::OK >( "Initialize the advanced configuration power interface." );
    }
}
