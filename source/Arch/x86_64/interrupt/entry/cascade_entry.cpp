#include <Arch/x86_64/platform/platform.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PRIVATE auto ASMCALL cascade_handler( IN Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > frame )->Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > {
        return frame;
    }
    CascadeEntry::CascadeEntry( VOID ) noexcept {
        Architecture::InterruptDescriptorTable::set_interrupt_handler( IRQ_CASCADE, cascade_handler );     // 注册Local APIC 中断入口函数
    }
}