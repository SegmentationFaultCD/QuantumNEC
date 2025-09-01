#include <kernel/display/print.hpp>
#include <kernel/driver/cpu/io.hpp>
#include <kernel/driver/cpu/mmio.hpp>
#include <kernel/interrupt/apic.hpp>
#include <kernel/interrupt/entry.hpp>
#include <kernel/interrupt/hpet.hpp>
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
auto Apic::register_ioapic( std::uint8_t vector, std::uint32_t irq ) -> void {
    auto gsi = irq;
    for ( std::size_t i = 0; i < this->iso_count; i++ ) {
        if ( this->iso[ i ].source == irq ) {
            gsi = this->iso[ i ].global_system_interrupt;
        }
    }

    this->install( vector, gsi );
    this->enable( vector, gsi );
}
auto Apic::eoi( void ) -> void {
    this->write( this->LOCAL_APIC_MSR_EOI, 0 );
}
class Clock : public GeneralInterruptHandle {
    virtual auto name( std::uint64_t ) noexcept -> void override {
    }
    virtual auto registers( IDT::Frame *frame ) noexcept -> void override {
    }
    virtual auto error_code( std::uint64_t code ) noexcept -> void override {
    }
    virtual auto task( ) noexcept -> void override {
    }
    virtual auto cpu( ) noexcept -> void override {
    }
    virtual auto handler( IDT::Frame *frame ) noexcept -> IDT::Frame * override {
        apic.eoi( );
        Driver::SerialPort { }.print( "ssconst char *str\n" );
        return frame;
    }
} clock;

auto initialize_apic( bool bsp ) -> void {
    // ban 8259A pic
    if ( bsp ) {
        Driver::IO::out8( 0x21, 0xff );
        Driver::IO::out8( 0xa1, 0xff );
        Driver::IO::out8( 0x20, 0x20 );
        Driver::IO::out8( 0xa0, 0x20 );
        Driver::IO::out8( 0x22, 0x70 );
        Driver::IO::out8( 0x23, 0x01 );
    }
    using namespace Driver;
    // enable x2apic
    auto base = IO::rdmsr( apic.IA32_APIC_BASE_MSR );
    base |= 1 << 10;
    base |= 1 << 11;
    IO::wrmsr( apic.IA32_APIC_BASE_MSR, base );

    // enable SVR
    Apic::SpuriousInterruptVectorRegister svr { (std::uint32_t)apic.read( apic.LOCAL_APIC_MSR_SVR ) };

    svr.enable_apic = apic.SVR_ENABLE_APIC;
    svr.vector = IDT::APIC_SPURIOUS;
    if ( apic.read( apic.LOCAL_APIC_MSR_VERSION ) >> 24 & 1 ) {
        svr.mask_eoi = apic.SVR_EOI_MASK;
    }
    else {
        svr.mask_eoi = apic.SVR_EOI_UNMASK;
    }
    apic.write( apic.LOCAL_APIC_MSR_SVR, svr );

    // 6个lvt我就不屏蔽了

    Apic::LocalVectorTableRegisters lvt { (std::uint32_t)apic.read( apic.LOCAL_APIC_MSR_LVT_ERROR ) };
    lvt.vector = IDT::APIC_ERROR;
    lvt.mask = apic.APIC_ICR_IOAPIC_UNMASKED;
    apic.write( apic.LOCAL_APIC_MSR_LVT_ERROR, lvt );

    // apic timer， 使用hpet进行校正
    apic.write( apic.LOCAL_APIC_MSR_TDCR, 11 );
    lvt = apic.read( apic.LOCAL_APIC_MSR_LVT_TIMER );
    lvt.vector = IDT::CLOCK;
    lvt.mask = apic.APIC_ICR_IOAPIC_UNMASKED;
    lvt.timer_mode = apic.APIC_LVT_TIMER_PERIODIC;
    apic.write( apic.LOCAL_APIC_MSR_LVT_TIMER, lvt );

    auto b = hpet->elapsed( );

    apic.write( apic.LOCAL_APIC_MSR_TICR, 0xffffffff );

    static std::uint64_t calibrated_timer_initial;

    while ( true ) {
        if ( hpet->elapsed( ) - b >= 1000000 ) {
            break;
        }
    }
    auto lapic_timer = 0xffffffff - apic.read( apic.LOCAL_APIC_MSR_TCCR );
    calibrated_timer_initial = (uint64_t)( (uint64_t)( lapic_timer * 1000 ) / apic.TIMER_SPEED );
    
    apic.write( apic.LOCAL_APIC_MSR_TICR, calibrated_timer_initial );
    GeneralInterruptHandle::register_handle( IDT::CLOCK, &clock );
    apic.register_ioapic( IDT::CLOCK, 0 );

    // ioapic initialize
}
}     // namespace Interrupt