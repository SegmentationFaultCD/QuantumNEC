#include <kernel/cpu/cpu.hpp>
#include <kernel/interrupt/x86_64/pit/apic_timer.hpp>
#include <kernel/print.hpp>
namespace QuantumNEC::Kernel::x86_64 {

ApicTimer::ApicTimer( void ) noexcept {
    // this->disable( );
    // this->set_current_count( 0xFFFFFFFF );
    // CPU::wrmsr( LOCAL_APIC_MSR_TDCR, 0xFFFFFFFF );
    // Apic::ApicLocalVectorTableRegisters lvt;
    // lvt.timer_mode = APIC_LVT_TIMER_PERIODIC;
    // lvt.mask       = APIC_ICR_IOAPIC_UNMASKED;
    // lvt.vector     = IRQ_APIC_LVT_TIMER;
    // CPU::wrmsr( LOCAL_APIC_MSR_LVT_TIMER, lvt );
    // CPU::wrmsr( LOCAL_APIC_MSR_TDCR, 3 );
    // CPU::wrmsr( LOCAL_APIC_MSR_TICR, 10 );
}
auto ApicTimer::enable( void ) -> void {     // 启动apic定时器
    Apic::ApicLocalVectorTableRegisters value;
    switch ( Apic::apic_flags ) {
    case Apic::SupportState::SUPPORT_x2APIC:
        value      = CPU::rdmsr( LOCAL_APIC_MSR_LVT_TIMER );
        value.mask = APIC_ICR_IOAPIC_UNMASKED;
        CPU::wrmsr( LOCAL_APIC_MSR_LVT_TIMER, value );
        break;
    case Apic::SupportState::SUPPORT_xAPIC:
        value      = Apic::read_apic( LOCAL_BASE_APIC_LVT_TIMER, Apic::ApicType::LOCAL_APIC );
        value.mask = APIC_ICR_IOAPIC_UNMASKED;
        Apic::write_apic( LOCAL_BASE_APIC_LVT_TIMER, value, Apic::ApicType::LOCAL_APIC );
        break;
    case Apic::SupportState::DOES_NOT_SUPPORT:
        std::println< std::print_level::SYSTEM >( "You shouln't call the function" );
        while ( true );
    }
}
auto ApicTimer::disable( void ) -> void {
    Apic::ApicLocalVectorTableRegisters value;
    switch ( Apic::apic_flags ) {
    case Apic::SupportState::SUPPORT_x2APIC:
        value      = CPU::rdmsr( LOCAL_APIC_MSR_LVT_TIMER );
        value.mask = APIC_ICR_IOAPIC_MASKED;
        CPU::wrmsr( LOCAL_APIC_MSR_LVT_TIMER, value );
        break;
    case Apic::SupportState::SUPPORT_xAPIC:
        value      = Apic::read_apic( LOCAL_BASE_APIC_LVT_TIMER, Apic::ApicType::LOCAL_APIC );
        value.mask = APIC_ICR_IOAPIC_MASKED;
        Apic::write_apic( LOCAL_BASE_APIC_LVT_TIMER, value, Apic::ApicType::LOCAL_APIC );
        break;
    case Apic::SupportState::DOES_NOT_SUPPORT:
        std::println< std::print_level::SYSTEM >( "You shouln't call the function" );
        while ( true );
    }
}
auto ApicTimer::install( uint64_t tick ) -> void {
    disable( );
    switch ( Apic::apic_flags ) {
    case Apic::SupportState::SUPPORT_x2APIC: {
        CPU::wrmsr( LOCAL_APIC_MSR_TDCR, DIVISOR );
        CPU::wrmsr( LOCAL_APIC_MSR_TICR, tick );
        Apic::ApicLocalVectorTableRegisters lvt;
        lvt.timer_mode = APIC_LVT_TIMER_PERIODIC;
        lvt.mask       = APIC_ICR_IOAPIC_MASKED;
        lvt.vector     = IRQ_APIC_LVT_TIMER;
        CPU::wrmsr( LOCAL_APIC_MSR_LVT_TIMER, lvt );
    } break;
    case Apic::SupportState::SUPPORT_xAPIC: {
        Apic::write_apic( LOCAL_BASE_APIC_TDCR, DIVISOR, Apic::ApicType::LOCAL_APIC );
        Apic::write_apic( LOCAL_BASE_APIC_TICR, tick, Apic::ApicType::LOCAL_APIC );
        Apic::ApicLocalVectorTableRegisters lvt;
        lvt.timer_mode = APIC_LVT_TIMER_PERIODIC;
        lvt.mask       = APIC_ICR_IOAPIC_MASKED;
        lvt.vector     = IRQ_APIC_LVT_TIMER;
        Apic::write_apic( LOCAL_BASE_APIC_LVT_TIMER, lvt, Apic::ApicType::LOCAL_APIC );
    } break;
    case Apic::SupportState::DOES_NOT_SUPPORT:
        std::println< std::print_level::SYSTEM >( "You shouln't call the function" );
        while ( true );
    }
}
auto ApicTimer::uninstall( void ) -> void {
    disable( );
}
auto ApicTimer::get_current_count( void ) -> uint64_t {
    switch ( Apic::apic_flags ) {
    case Apic::SupportState::SUPPORT_x2APIC: {
        return CPU::rdmsr( LOCAL_APIC_MSR_LVT_TIMER );
    } break;
    case Apic::SupportState::SUPPORT_xAPIC: {
        return Apic::read_apic( LOCAL_BASE_APIC_LVT_TIMER, Apic::ApicType::LOCAL_APIC );
    } break;
    case Apic::SupportState::DOES_NOT_SUPPORT:
        std::println< std::print_level::SYSTEM >( "You shouln't call the function" );
        while ( true );
    }
    return 0;
}
auto ApicTimer::set_current_count( uint64_t count ) -> void {
    switch ( Apic::apic_flags ) {
    case Apic::SupportState::SUPPORT_x2APIC: {
        CPU::wrmsr( LOCAL_APIC_MSR_LVT_TIMER, count );
    } break;
    case Apic::SupportState::SUPPORT_xAPIC: {
        Apic::write_apic( LOCAL_BASE_APIC_LVT_TIMER, count, Apic::ApicType::LOCAL_APIC );
    } break;
    case Apic::SupportState::DOES_NOT_SUPPORT:
        std::println< std::print_level::SYSTEM >( "You shouln't call the function" );
        while ( true );
    }
}
}     // namespace QuantumNEC::Kernel::x86_64