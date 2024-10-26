#include <kernel/interrupt/arch/x86_64/pic/apic.hpp>
#include <kernel/interrupt/arch/x86_64/pic/8259a.hpp>
#include <kernel/cpu/cpu.hpp>
#include <kernel/print.hpp>
#include <lib/spin_lock.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    using namespace std;
    auto Apic::enable_xapic( VOID ) -> VOID {
    }
    auto Apic::enable_x2apic( VOID ) -> VOID {
        // 开启  xapic
        InterruptCommandRegister icr { CPU::rdmsr( IA32_APIC_BASE_MSR ) };
        icr.deliver_mode = APIC_ICR_IOAPIC_NMI;
        icr.dest_mode = ICR_IOAPIC_DELV_LOGIC;
        CPU::wrmsr( IA32_APIC_BASE_MSR, icr );
        // 开启SVR
        ApicLocalVectorTableRegisters lvt { CPU::rdmsr( LOCAL_APIC_MSR_SVR ) };
        lvt.deliver_mode = IOAPIC_ICR_LOWEST_PRIORITY;
        if ( CPU::rdmsr( LOCAL_APIC_MSR_VERSION ) >> 24 & 1 ) {
            lvt.deliver_status = APIC_ICR_IOAPIC_SEND_PENDING;
        }
        CPU::wrmsr( LOCAL_APIC_MSR_SVR, lvt );
        // 屏蔽7个LVT寄存器
        lvt = read_apic( LOCAL_BASE_APIC_LVT_CMCI, ApicType::LOCAL_APIC );
        lvt.mask = APIC_ICR_IOAPIC_MASKED;
        lvt.deliver_mode = APIC_ICR_IOAPIC_FIXED;
        lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
        write_apic( LOCAL_BASE_APIC_LVT_CMCI, lvt, ApicType::LOCAL_APIC );

        lvt = CPU::rdmsr( LOCAL_APIC_MSR_LVT_TIMER );
        lvt.mask = APIC_ICR_IOAPIC_MASKED;
        lvt.deliver_mode = APIC_ICR_IOAPIC_FIXED;
        lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
        CPU::wrmsr( LOCAL_APIC_MSR_LVT_TIMER, lvt );

        lvt = CPU::rdmsr( LOCAL_APIC_MSR_LVT_ERROR );
        lvt.mask = APIC_ICR_IOAPIC_MASKED;
        lvt.deliver_mode = APIC_ICR_IOAPIC_FIXED;
        lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
        CPU::wrmsr( LOCAL_APIC_MSR_LVT_ERROR, lvt );

        lvt = CPU::rdmsr( LOCAL_APIC_MSR_LVT_TS );
        lvt.mask = APIC_ICR_IOAPIC_MASKED;
        lvt.deliver_mode = APIC_ICR_IOAPIC_FIXED;
        lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
        CPU::wrmsr( LOCAL_APIC_MSR_LVT_TS, lvt );

        lvt = CPU::rdmsr( LOCAL_APIC_MSR_LVT_PMC );
        lvt.mask = APIC_ICR_IOAPIC_MASKED;
        lvt.deliver_mode = APIC_ICR_IOAPIC_FIXED;
        lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
        CPU::wrmsr( LOCAL_APIC_MSR_LVT_PMC, lvt );

        lvt = CPU::rdmsr( LOCAL_APIC_MSR_LVT_LINT0 );
        lvt.mask = APIC_ICR_IOAPIC_MASKED;
        lvt.deliver_mode = APIC_ICR_IOAPIC_FIXED;
        lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
        CPU::wrmsr( LOCAL_APIC_MSR_LVT_LINT0, lvt );

        lvt = CPU::rdmsr( LOCAL_APIC_MSR_LVT_LINT1 );
        lvt.mask = APIC_ICR_IOAPIC_MASKED;
        lvt.deliver_mode = APIC_ICR_IOAPIC_FIXED;
        lvt.deliver_status = APIC_ICR_IOAPIC_IDLE;
        CPU::wrmsr( LOCAL_APIC_MSR_LVT_LINT1, lvt );
    }
    Apic::Apic( VOID ) noexcept {
        // 关闭8259A PIC

        PIC8259A::disable_8259A_pic( );
        // 开启 IMCR
        CPU::io_out8( 0x22, 0x70 );
        CPU::io_out8( 0x23, 0x01 );

        this->apic_flags = this->check_apic( );

        // 开启APIC
        if ( this->apic_flags == 0 ) {     // 检查是否支持APIC或者x2APIC
            this->enable_xapic( );
        }
        else if ( this->apic_flags == 1 ) {
            this->enable_x2apic( );
        }
        else {
            println< ostream::HeadLevel::SYSTEM >( "Can not enable apic." );
        }

        // 设置IO APIC
        IOApicRedirectionEntry entry { };
        for ( auto i { 0ull }; i < ( ( this->read_apic( IOAPIC_REG_VER, ApicType::IO_APIC ) >> 16 ) & 0xFF ); i++ ) {
            entry.vector = IDT_ENTRY_IRQ_0 + i;
            entry.mask = APIC_ICR_IOAPIC_MASKED;
            this->write_apic( IOAPIC_REG_TABLE + i * 2, entry, ApicType::IO_APIC );
        }

        ASM( "MOVQ %0, %%CR8" ::"r"( 0ull ) );
        println< ostream::HeadLevel::OK >( "Initialize the advanced programmable interrupt controller." );
    }
    Apic::~Apic( VOID ) noexcept {
    }
    auto Apic::eoi( IN CONST irq_t ) -> VOID {
        if ( apic_flags == 0 ) {
            write_apic( LOCAL_BASE_APIC_EOI, 0, ApicType::LOCAL_APIC );
        }
        else {
            CPU::wrmsr( LOCAL_APIC_MSR_EOI, 0 );
        }
    }
    auto Apic::check_apic( VOID ) -> int16_t {
        CPU::CpuidStatus status { 1, 0, 0, 0, 0, 0 };
        CPU::cpuid( status );
        if ( ( status.rcx & CPU::CpuidStatus::CPUID_FEAT_RCX_X2APIC ) ) {
            return 1;     // 支持x2APIC
        }
        else if ( ( status.rdx & CPU::CpuidStatus::CPUID_FEAT_RDX_APIC ) ) {
            return 0;     // 支持xAPIC
        }

        return -1;     // 不支持apic
    }
    auto Apic::read_apic( IN uint16_t index, IN ApicType type ) -> uint64_t {
        uint64_t return_value { };
        if ( type == ApicType::LOCAL_APIC ) {
            return_value = reinterpret_cast< uint32_t volatile * >( apic_map.local_apic_address )[ index / 4 ];
            CPU::mfence( );
            return return_value;
        }
        else {
            *reinterpret_cast< uint8_t * >( apic_map.ioapic[ 0 ].io_apic_index_address ) = index + 1;
            CPU::mfence( );
            return_value = *reinterpret_cast< uint32_t * >( apic_map.ioapic[ 0 ].io_apic_data_address );
            return_value <<= 32;
            CPU::mfence( );
            *reinterpret_cast< uint8_t * >( apic_map.ioapic[ 0 ].io_apic_index_address ) = index;
            CPU::mfence( );
            return_value |= *reinterpret_cast< uint32_t * >( apic_map.ioapic[ 0 ].io_apic_data_address );
            CPU::mfence( );
            return return_value;
        }

        return 0;
    }
    auto Apic::write_apic( IN uint16_t index, IN uint64_t value, IN ApicType type ) -> VOID {
        if ( type == ApicType::LOCAL_APIC ) {
            reinterpret_cast< uint32_t volatile * >( ( apic_map.local_apic_address ) )[ index / 4 ] = static_cast< uint32_t >( value );
            CPU::mfence( );
        }
        else {
            *reinterpret_cast< uint8_t * >( apic_map.ioapic[ 0 ].io_apic_index_address ) = index;
            CPU::mfence( );
            *reinterpret_cast< uint32_t * >( apic_map.ioapic[ 0 ].io_apic_data_address ) = value & 0xffffffff;
            value >>= 32;
            CPU::mfence( );
            *reinterpret_cast< uint8_t * >( apic_map.ioapic[ 0 ].io_apic_index_address ) = index + 1;
            CPU::mfence( );
            *reinterpret_cast< uint32_t * >( apic_map.ioapic[ 0 ].io_apic_data_address ) = value & 0xffffffff;
            CPU::mfence( );
        }
    }
    auto Apic::enable_ioapic( IN IN irq_t vector ) -> VOID {
        IOApicRedirectionEntry entry { read_apic( ( vector - IDT_ENTRY_IRQ_0 ) * 2 + IOAPIC_REG_TABLE, ApicType::IO_APIC ) };
        entry.mask = APIC_ICR_IOAPIC_UNMASKED;
        write_apic( ( vector - 32 ) * 2 + IOAPIC_REG_TABLE, entry, ApicType::IO_APIC );
    }
    auto Apic::disable_ioapic( IN irq_t irq ) -> VOID {
        IOApicRedirectionEntry entry { read_apic( ( irq - IDT_ENTRY_IRQ_0 ) * 2 + IOAPIC_REG_TABLE, ApicType::IO_APIC ) };
        entry.mask = APIC_ICR_IOAPIC_MASKED;
        write_apic( ( irq - IDT_ENTRY_IRQ_0 ) * 2 + IOAPIC_REG_TABLE, entry, ApicType::IO_APIC );
    }
    auto Apic::install_ioapic( IN irq_t irq, IN VOID * entry ) -> uint64_t {
        write_apic( ( irq - IDT_ENTRY_IRQ_0 ) * 2 + IOAPIC_REG_TABLE, *reinterpret_cast< uint64_t * >( entry ), Apic::ApicType::IO_APIC );
        return 1;
    }
    auto Apic::uninstall_ioapic( IN irq_t irq ) -> VOID {
        write_apic( ( irq - IDT_ENTRY_IRQ_0 ) * 2 + IOAPIC_REG_TABLE, INT_DISABLED, ApicType::IO_APIC );
    }
    auto Apic::ioapic_level_ack( IN irq_t irq ) -> VOID {
        eoi( irq );
        *reinterpret_cast< uint32_t * >( apic_map.ioapic[ 0 ].io_apic_EOI_address ) = irq;
    }
    auto Apic::ioapic_edge_ack( IN irq_t irq ) -> VOID {
        eoi( irq );
    }
    auto Apic::apic_id( ) -> uint64_t {
        CPU::CpuidStatus status { 1, 0, 0, 0, 0, 0 };
        CPU::cpuid( status );

        return ( status.rbx >> 24 ) & 0xff;
    }
}
