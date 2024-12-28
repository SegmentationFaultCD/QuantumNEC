#include <kernel/cpu/cpu.hpp>
#include <kernel/interrupt/x86_64/pic/8259a.hpp>
#include <kernel/interrupt/x86_64/pic/apic.hpp>
#include <kernel/print.hpp>
#include <lib/mmio.hpp>
#include <lib/spin_lock.hpp>
namespace QuantumNEC::Kernel::x86_64 {
using namespace std;
auto Apic::enable_xapic( void ) -> void {
}
auto Apic::enable_x2apic( void ) -> void {
    // enable x2apic
    auto base = CPU::rdmsr( IA32_APIC_BASE_MSR );
    base |= 4 << 8;
    base |= 1 << 11;
    CPU::wrmsr( IA32_APIC_BASE_MSR, base );
    // enable SVR
    SpuriousInterruptVectorRegister svr { CPU::rdmsr( LOCAL_APIC_MSR_SVR ) };
    svr.enable_apic = SVR_ENABLE_APIC;
    if ( CPU::rdmsr( LOCAL_APIC_MSR_VERSION ) >> 24 & 1 ) {
        svr.mask_eoi = SVR_EOI_MASK;
    }
    else {
        svr.mask_eoi = SVR_EOI_UNMASK;
    }
    CPU::wrmsr( LOCAL_APIC_MSR_SVR, svr );
    // disable seven lvt registers

    // I don't know what causes #GP fault when I use msr register to write CMCI register.
    // But everything gose well when I use mmio function to write CMCI.
    LocalVectorTableRegisters lvt { read_apic( LOCAL_BASE_APIC_LVT_CMCI, ApicType::LOCAL_APIC ) };
    lvt.mask           = APIC_ICR_IOAPIC_MASKED;
    lvt.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
    write_apic( LOCAL_BASE_APIC_LVT_CMCI, lvt, ApicType::LOCAL_APIC );

    lvt                = CPU::rdmsr( LOCAL_APIC_MSR_LVT_TIMER );
    lvt.mask           = APIC_ICR_IOAPIC_MASKED;
    lvt.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
    CPU::wrmsr( LOCAL_APIC_MSR_LVT_TIMER, lvt );

    lvt                = CPU::rdmsr( LOCAL_APIC_MSR_LVT_ERROR );
    lvt.mask           = APIC_ICR_IOAPIC_MASKED;
    lvt.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
    CPU::wrmsr( LOCAL_APIC_MSR_LVT_ERROR, lvt );

    lvt                = CPU::rdmsr( LOCAL_APIC_MSR_LVT_TS );
    lvt.mask           = APIC_ICR_IOAPIC_MASKED;
    lvt.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
    CPU::wrmsr( LOCAL_APIC_MSR_LVT_TS, lvt );

    lvt                = CPU::rdmsr( LOCAL_APIC_MSR_LVT_PMC );
    lvt.mask           = APIC_ICR_IOAPIC_MASKED;
    lvt.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
    CPU::wrmsr( LOCAL_APIC_MSR_LVT_PMC, lvt );

    lvt                = CPU::rdmsr( LOCAL_APIC_MSR_LVT_LINT0 );
    lvt.mask           = APIC_ICR_IOAPIC_MASKED;
    lvt.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
    CPU::wrmsr( LOCAL_APIC_MSR_LVT_LINT0, lvt );

    lvt                = CPU::rdmsr( LOCAL_APIC_MSR_LVT_LINT1 );
    lvt.mask           = APIC_ICR_IOAPIC_MASKED;
    lvt.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
    CPU::wrmsr( LOCAL_APIC_MSR_LVT_LINT1, lvt );
}
Apic::Apic( void ) noexcept {
    // disable 8259A PIC
    PIC8259A::disable_8259A_pic( );
    // enable IMCR
    CPU::io_out8( 0x22, 0x70 );
    CPU::io_out8( 0x23, 0x01 );
    // check it is sure whether the machine suopport the x2apic or xapic.
    this->apic_flags = this->check_apic( );
    // enable local apic
    switch ( this->apic_flags ) {
    case SupportState::SUPPORT_x2APIC:
        this->enable_x2apic( );
        break;
    case SupportState::SUPPORT_xAPIC:
        this->enable_xapic( );
        break;
    case SupportState::DOES_NOT_SUPPORT:
        println< print_level::SYSTEM >( "Can not enable apic." );
        while ( true );
    }
    // enable IO APIC
    IOApicRedirectionEntry entry { };
    for ( auto i { 0ull }; i < ( ( read_apic( IOAPIC_REG_VER, ApicType::IO_APIC ) >> 16 ) & 0xFF ); i++ ) {
        entry.vector = IDT_ENTRY_IRQ_0 + i;
        entry.mask   = APIC_ICR_IOAPIC_MASKED;
        write_apic( IOAPIC_REG_TABLE + i * 2, entry, ApicType::IO_APIC );
    }
    // flush CR8 register
    ASM( "MOVQ %0, %%CR8" ::"r"( 0ull ) );
}
Apic::~Apic( void ) noexcept {
}
auto Apic::eoi( IN [[maybe_unused]] irq_t irq ) -> void {
    switch ( apic_flags ) {
    case SupportState::SUPPORT_x2APIC:
        CPU::wrmsr( LOCAL_APIC_MSR_EOI, 0 );
        break;
    case SupportState::SUPPORT_xAPIC:
        write_apic( LOCAL_BASE_APIC_EOI, 0, ApicType::LOCAL_APIC );
        break;
    case SupportState::DOES_NOT_SUPPORT:
        println< print_level::SYSTEM >( "You shouln't call the function" );
        while ( true );
    }
}
auto Apic::check_apic( void ) -> SupportState {
    CPU::CpuidStatus status { 1, 0, 0, 0, 0, 0 };
    CPU::cpuid( status );
    if ( ( status.rcx & CPU::CpuidStatus::CPUID_FEAT_RCX_X2APIC ) ) {
        return SupportState::SUPPORT_x2APIC;     // support x2APIC
    }
    else if ( ( status.rdx & CPU::CpuidStatus::CPUID_FEAT_RDX_APIC ) ) {
        return SupportState::SUPPORT_xAPIC;     // support xAPIC
    }

    return SupportState::DOES_NOT_SUPPORT;     // doesn't support apic
}
auto Apic::read_apic( IN uint16_t index, IN ApicType type ) -> uint64_t {
    uint64_t return_value { };
    if ( type == ApicType::LOCAL_APIC ) {
        Lib::MMIO< uint32_t > local_apic { apic_map.local_apic_address };
        return_value = local_apic[ index ];
        CPU::mfence( );
        return return_value;
    }
    else {
        Lib::MMIO< uint8_t >  io_apic_index { apic_map.ioapic[ 0 ].io_apic_index_address };
        Lib::MMIO< uint32_t > io_apic_data { apic_map.ioapic[ 0 ].io_apic_data_address };
        // read high 32 bits
        io_apic_index[ 0 ] = index + 1;
        CPU::mfence( );
        return_value = io_apic_data[ 0 ];
        return_value <<= 32;
        CPU::mfence( );
        // read low 32 bits
        io_apic_index[ 0 ] = index;
        CPU::mfence( );
        return_value = io_apic_data[ 0 ];
        CPU::mfence( );
        return return_value;
    }

    return 0;
}
auto Apic::write_apic( IN uint16_t index, IN uint64_t value, IN ApicType type ) -> void {
    if ( type == ApicType::LOCAL_APIC ) {
        Lib::MMIO< uint32_t > local_apic { apic_map.local_apic_address };
        local_apic[ index ] = static_cast< uint32_t >( value );
        CPU::mfence( );
    }
    else {
        Lib::MMIO< uint8_t >  io_apic_index { apic_map.ioapic[ 0 ].io_apic_index_address };
        Lib::MMIO< uint32_t > io_apic_data { apic_map.ioapic[ 0 ].io_apic_data_address };
        // write to low 32 bits
        io_apic_index[ 0 ] = index;
        CPU::mfence( );
        io_apic_data[ 0 ] = value & 0xffffffff;
        CPU::mfence( );
        // value offset
        value >>= 32;
        // write to high 32 bits
        io_apic_index[ 0 ] = index + 1;
        CPU::mfence( );
        io_apic_data[ 0 ] = value & 0xffffffff;
        CPU::mfence( );
    }
}
auto Apic::enable_ioapic( IN IN irq_t vector ) -> void {
    IOApicRedirectionEntry entry { read_apic( ( vector - IDT_ENTRY_IRQ_0 ) * 2 + IOAPIC_REG_TABLE, ApicType::IO_APIC ) };
    entry.mask = APIC_ICR_IOAPIC_UNMASKED;
    write_apic( ( vector - IDT_ENTRY_IRQ_0 ) * 2 + IOAPIC_REG_TABLE, entry, ApicType::IO_APIC );
}
auto Apic::disable_ioapic( IN irq_t irq ) -> void {
    IOApicRedirectionEntry entry { read_apic( ( irq - IDT_ENTRY_IRQ_0 ) * 2 + IOAPIC_REG_TABLE, ApicType::IO_APIC ) };
    entry.mask = APIC_ICR_IOAPIC_MASKED;
    write_apic( ( irq - IDT_ENTRY_IRQ_0 ) * 2 + IOAPIC_REG_TABLE, entry, ApicType::IO_APIC );
}
auto Apic::install_ioapic( IN irq_t irq, IN void *entry ) -> uint64_t {
    write_apic( ( irq - IDT_ENTRY_IRQ_0 ) * 2 + IOAPIC_REG_TABLE, *reinterpret_cast< uint64_t * >( entry ), Apic::ApicType::IO_APIC );
    return 1;
}
auto Apic::uninstall_ioapic( IN irq_t irq ) -> void {
    write_apic( ( irq - IDT_ENTRY_IRQ_0 ) * 2 + IOAPIC_REG_TABLE, INT_DISABLED, ApicType::IO_APIC );
}
auto Apic::ioapic_level_ack( IN irq_t irq ) -> void {
    eoi( irq );
    Lib::MMIO< uint32_t > io_apic_eoi { apic_map.ioapic[ 0 ].io_apic_EOI_address };
    io_apic_eoi[ 0 ] = irq;
}
auto Apic::ioapic_edge_ack( IN irq_t irq ) -> void {
    eoi( irq );
}
auto Apic::irq_set_mask( irq_t irq ) -> void {
    LocalVectorTableRegisters lvt { };
    lvt.mask = APIC_ICR_IOAPIC_MASKED;
    write_apic( IOAPIC_REG_TABLE + 2 * irq, lvt, ApicType::IO_APIC );
}
auto Apic::irq_clear_mask( IN irq_t irq, IN uint8_t vector, IN uint8_t ) -> void {
    // 标记中断边缘触发，高电平有效，
    // 启用并路由到给定的cpunum，
    // 恰好是该cpu的APIC ID
    LocalVectorTableRegisters lvt { };
    lvt.vector = vector;
    lvt.mask   = APIC_ICR_IOAPIC_UNMASKED;
    write_apic( IOAPIC_REG_TABLE + 2 * irq, lvt, ApicType::IO_APIC );
}
auto Apic::apic_id( ) -> uint64_t {
    CPU::CpuidStatus status { 1, 0, 0, 0, 0, 0 };
    CPU::cpuid( status );
    return ( status.rbx >> 24 ) & 0xff;
}
}     // namespace QuantumNEC::Kernel::x86_64
