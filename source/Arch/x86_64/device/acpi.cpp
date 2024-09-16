#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/Debug/panic.hpp>
#include <Lib/STL/string>
#include <Lib/IO/Stream/iostream>
#include <Arch/Arch.hpp>
PUBLIC namespace {
    PUBLIC using namespace QuantumNEC;
    PUBLIC using namespace QuantumNEC::Lib::Types;
    PUBLIC using namespace QuantumNEC::Lib::IO;
    PRIVATE consteval auto SIGN_16( auto A, auto B )->decltype( auto ) {
        return ( A ) | ( B << 8 );
    }
    PRIVATE consteval auto SIGN_32( auto A, auto B, auto C, auto D )->decltype( auto ) {
        return SIGN_16( A, B ) | ( SIGN_16( C, D ) << 16 );
    }
    PRIVATE consteval auto SIGN_64( auto A, auto B, auto C, auto D, auto E, auto F, auto G, auto H )->decltype( auto ) {
        return SIGN_32( A, B, C, D ) | ( (Lib::Types::uint64_t)SIGN_32( E, F, G, H ) << 32 );
    }
    PRIVATE constexpr auto bytes_sum_total( IN Lib::Types::Ptr< CONST uint8_t > data, IN size_t bytes )->decltype( auto ) {
        uint8_t sum { };
        for ( size_t i { }; i < bytes; ++i ) {
            sum += data[ i ];
        }
        return sum;
    }
    PRIVATE constexpr CONST auto GLOBAL_SYSTEM_INTERRUPT_COUNT { 24 };
    PRIVATE constexpr CONST auto RSDP_SIGNED { SIGN_64( 'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ' ) };
    PRIVATE constexpr CONST auto XSDT_SIGNED { SIGN_32( 'X', 'S', 'D', 'T' ) };
    PRIVATE constexpr CONST auto MADT_SIGNED { SIGN_32( 'A', 'P', 'I', 'C' ) };
    PRIVATE constexpr CONST auto APIC_SIGNED { SIGN_32( 'A', 'P', 'I', 'C' ) };
    PRIVATE constexpr CONST auto FADT_SIGNED { SIGN_32( 'F', 'A', 'C', 'P' ) };
    PRIVATE constexpr CONST auto HPET_SIGNED { SIGN_32( 'H', 'P', 'E', 'T' ) };

    PRIVATE uint8_t global_system_interrupt[ GLOBAL_SYSTEM_INTERRUPT_COUNT ];
}
PUBLIC namespace QuantumNEC::Architecture {
    Acpi::Acpi( IN CONST Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept :
        rsdp { _config->acpi_table },
        xsdt { reinterpret_cast< Lib::Types::Ptr< Xsdt > >( this->rsdp->xsdtAddress ) } {
        using namespace Lib::Types;
        auto checkSum { []( IN Lib::Types::Ptr< VOID > buffer, IN Lib::Types::size_t size ) -> Lib::Types::BOOL {
            Lib::Types::uint8_t sum { };
            for ( size_t i = 0; i < size; i++ ) {
                sum += ( reinterpret_cast< Lib::Types::Ptr< Lib::Types::char_t > >( buffer ) )[ i ];
            }
            return !sum;
        } };
        auto getEntry = [ &, this ]( Lib::Types::uint32_t signature ) -> Lib::Types::Ptr< VOID > {
            Lib::Types::size_t size { this->xsdt->size( ) };
            Lib::Types::Ptr< SDTHeader > header { };
            Lib::Types::Ptr< Lib::Types::uint64_t > address { reinterpret_cast< decltype( address ) >( (Lib::Types::uint64_t)this->xsdt + sizeof( Xsdt ) ) };
            for ( size_t i { }; i < size; i++ ) {
                header = reinterpret_cast< decltype( header ) >( address[ i ] );
                if ( header->signature == signature ) {
                    if ( checkSum( header, header->length ) )
                        return reinterpret_cast< Lib::Types::Ptr< VOID > >( header );
                }
            }
            return NULL;
        };
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::START ] << "Initialize the advanced configuration power interface." << Lib::IO::endl;

        if ( !this->is_valid< Rsdp >( ) ) {
            Lib::IO::sout[ Lib::IO::ostream::HeadLevel::ERROR ] << "RSD pointer found is not valid." << Lib::IO::endl;
            while ( true )
                ;
        }
        if ( this->xsdt->signature != ::XSDT_SIGNED || !checkSum( this->xsdt, this->xsdt->length ) ) {
            Lib::IO::sout[ Lib::IO::ostream::HeadLevel::ERROR ] << "XSDT is not valid." << Lib::IO::endl;
            while ( true )
                ;
        }
        this->madt = reinterpret_cast< decltype( this->madt ) >( getEntry( ::APIC_SIGNED ) );
        if ( !madt ) {
            Lib::IO::sout[ Lib::IO::ostream::HeadLevel::ERROR ] << "Can not find madt." << Lib::IO::endl;
            while ( true )
                ;
        }
        Lib::Types::int64_t length { static_cast< decltype( length ) >( this->madt->length - sizeof *this->madt ) };
        Lib::Types::Ptr< MadtICS > ICS { reinterpret_cast< decltype( ICS ) >( ( Lib::Types::uint64_t )( this->madt ) + sizeof *this->madt ) };
        Lib::STL::memset( ::global_system_interrupt, 0xFF, ::GLOBAL_SYSTEM_INTERRUPT_COUNT );
        while ( length > 0 ) {
            if ( ICS->type == ICSAttribute::LOCAL_APIC ) {
                Lib::Types::Ptr< LocalApic > local_apic { reinterpret_cast< decltype( local_apic ) >( ICS ) };
                // Lib::IO::sout[ Lib::IO::ostream::HeadLevel::SYSTEM ].printk(
                //     DisplayColor::WHITE,
                //     DisplayColor::BLACK,
                //     "Local Apic ================>> | Acpi Processor UID <=> %8u | Apic ID <=> %27u | Flags <=> %41x |\n",
                //     local_apic->acpiProcessorUID, local_apic->apicID, local_apic->flags );
                Architecture::Apic::apic_map.local_apic_ID[ Architecture::Apic::apic_map.core_count++ ] = local_apic->apicID;
            }
            else if ( ICS->type == ICSAttribute::I_O_APIC ) {
                Lib::Types::Ptr< IOApic > io_apic { reinterpret_cast< decltype( io_apic ) >( ICS ) };
                //   Lib::IO::sout[ Lib::IO::ostream::HeadLevel::SYSTEM ].printk(
                //       DisplayColor::WHITE,
                //       DisplayColor::BLACK,
                //       "I/O Apic ==================>> | I/O Apic ID <=> %15u | I/O Apic Address <=> %#18x | Global System Interrupt Base <=> %18x |\n",
                //       io_apic->IOApicID, io_apic->IOApicAddress, io_apic->globalSystemInterruptBase );
                this->io_apic_address = reinterpret_cast< decltype( this->io_apic_address ) >( io_apic->IOApicAddress );
                Architecture::Apic::apic_map.io_apic_address = reinterpret_cast< Lib::Types::uint64_t >( this->io_apic_address );
            }
            else if ( ICS->type == ICSAttribute::INTERRUPT_SOURCE_OVERRIDE ) {
                Lib::Types::Ptr< InterruptSourceOverride > interrupt_source_override { reinterpret_cast< decltype( interrupt_source_override ) >( ICS ) };
                //  Lib::IO::sout[ Lib::IO::ostream::HeadLevel::SYSTEM ].printk(
                //      DisplayColor::WHITE,
                //      DisplayColor::BLACK,
                //      "Interrupt Source Override =>> | Bus <=> %23u | Source <=> %28u | Flags <=> %41x | Global System Interrupt <=> %8u |\n",
                //      interrupt_source_override->bus, interrupt_source_override->source, interrupt_source_override->flags, interrupt_source_override->globalSystemInterrupt );
                this->globalSystemInterrupt[ interrupt_source_override->source ] = interrupt_source_override->globalSystemInterrupt;
            }
            length -= ICS->length;
            ICS = reinterpret_cast< decltype( ICS ) >( (char *)( ICS ) + ICS->length );
        }
        this->local_apic_address = reinterpret_cast< decltype( this->local_apic_address ) >( this->madt->localApicAddress );
        Architecture::Apic::apic_map.local_apic_address = reinterpret_cast< Lib::Types::uint64_t >( this->local_apic_address );
        Architecture::Apic::apic_map.io_apic_index_address = reinterpret_cast< Lib::Types::Ptr< VOID > >( Architecture::Apic::apic_map.io_apic_address );
        Architecture::Apic::apic_map.io_apic_data_address = reinterpret_cast< Lib::Types::Ptr< VOID > >( Architecture::Apic::apic_map.io_apic_address + 0x10UL );
        Architecture::Apic::apic_map.io_apic_EOI_address = reinterpret_cast< Lib::Types::Ptr< VOID > >( Architecture::Apic::apic_map.io_apic_address + 0x40UL );
        this->hpet = reinterpret_cast< decltype( this->hpet ) >( getEntry( HPET_SIGNED ) );
        Lib::IO::sout[ Lib::IO::ostream::HeadLevel::OK ] << "Initialize the advanced configuration power interface." << Lib::IO::endl;
    }

    template <>
    auto Acpi::is_valid< Lib::Types::Rsdp >( VOID ) CONST->Lib::Types::BOOL {
        if ( Lib::STL::strncmp( this->rsdp->signature, "RSD PTR ", 8 ) || this->rsdp->revision != 2 ) {
            return FALSE;
        }
        if ( auto sum { bytes_sum_total( reinterpret_cast< Lib::Types::Ptr< CONST Lib::Types::uint8_t > >( this->rsdp ), 20 ) }; sum ) {
            return FALSE;
        }
        if ( auto sum { bytes_sum_total( reinterpret_cast< Lib::Types::Ptr< CONST Lib::Types::uint8_t > >( this->rsdp ), 36 ) }; sum ) {
            return FALSE;
        }
        return TRUE;
    }
}