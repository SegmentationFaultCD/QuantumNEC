#include <Arch/x86_64/platform/platform.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PRIVATE auto ASMCALL local_apic_spurious_handler( IN Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > frame )->Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > {
        return frame;
    }

    PRIVATE auto ASMCALL local_apic_error_handler( IN Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > frame )->Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > {
        return frame;
    }
    LocalApicEntry::LocalApicEntry( VOID ) noexcept {
        Architecture::InterruptDescriptorTable::set_interrupt_handler( IRQ_LOCAL_APIC_SPURIOUS, local_apic_spurious_handler );     // 注册Local APIC 中断入口函数
        Architecture::InterruptDescriptorTable::set_interrupt_handler( IRQ_LOCAL_APIC_ERROR, local_apic_error_handler );           // 注册Local APIC 中断入口函数
    }
}