#include <kernel/driver/cpu/io.hpp>
#include <kernel/driver/cpu/mmio.hpp>
#include <kernel/interrupt/apic.hpp>
namespace Interrupt {

auto Apic::write( std::uint32_t index, std::uint32_t reg, std::uint64_t value ) -> void {
    using namespace Driver;
    MMIO< std::uint8_t > io_apic_index { this->ioapic[ index ].ioapic_index_address };
    MMIO< std::uint32_t > io_apic_data { this->ioapic[ index ].ioapic_data_address };
    // write to low 32 bits
    io_apic_index[ 0 ] = reg;
    IO::mfence( );
    io_apic_data[ 0 ] = value & 0xffffffff;
    IO::mfence( );
    // value offset
    value >>= 32;
    // write to high 32 bits
    io_apic_index[ 0 ] = reg + 1;
    IO::mfence( );
    io_apic_data[ 0 ] = value & 0xffffffff;
    IO::mfence( );
}

auto Apic::write( std::uint32_t reg, std::uint64_t value ) -> void {
    Driver::IO::wrmsr( reg, value );
}
auto Apic::read( std::uint32_t reg ) -> std::uint64_t {
    return Driver::IO::rdmsr( reg );
}
auto Apic::read( std::uint32_t index, std::uint32_t reg ) -> std::uint64_t {
    using namespace Driver;
    MMIO< std::uint8_t > io_apic_index { this->ioapic[ index ].ioapic_index_address };
    MMIO< std::uint32_t > io_apic_data { this->ioapic[ index ].ioapic_data_address };
    // read high 32 bits
    io_apic_index[ 0 ] = index + 1;
    IO::mfence( );
    std::uint64_t return_value = io_apic_data[ 0 ];
    return_value <<= 32;
    IO::mfence( );
    // read low 32 bits
    io_apic_index[ 0 ] = index;
    IO::mfence( );
    return_value = io_apic_data[ 0 ];
    IO::mfence( );
    return return_value;
}
auto Apic::find_ioapic( std::uint32_t gsi ) -> std::int64_t {
    for ( size_t i = 0; i < this->ioapic_count; i++ ) {
        if ( gsi >= this->ioapic[ i ].gsi_base && gsi < this->ioapic[ i ].gsi_base + this->ioapic[ i ].irq_count ) {
            return i;
        }
    }
    return -1;
}
auto Apic::enable( std::uint8_t vector, std::uint32_t irq ) -> void {
    auto ioapic = this->find_ioapic( irq );
    if ( ioapic < 0 )
        return;

    uint64_t index = 0x10 + ( ( vector - 32 ) * 2 );
    auto value = this->read( ioapic, index );
    value &= ( ~0x10000UL );
    this->write( ioapic, index );
}
auto Apic::apic_id( void ) -> std::uint64_t {
    return ( Driver::IO::cpuid( { 1, 0, 0, 0, 0, 0 } ).rdx >> 24 ) & 0xff;
}
auto Apic::install( std::uint8_t vector, std::uint32_t irq ) -> void {
    auto ioapic = find_ioapic( irq );
    if ( ioapic < 0 )
        return;

    auto irq0 = irq - this->ioapic[ ioapic ].gsi_base;
    auto ioredtbl = 0x10 + irq0 * 2;
    auto redirect = vector | ( (uint64_t)( this->read( LOCAL_APIC_MSR_ID ) ) << 32 );
    this->write( ioapic, ioredtbl, redirect );
}
auto initialize_apic( void ) -> void {
    
}
}     // namespace Interrupt