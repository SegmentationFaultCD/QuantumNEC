#include <Arch/x86_64/platform/platform.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PRIVATE auto ASMCALL rtc_handler( IN Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > frame )->Lib::Types::Ptr< CONST InterruptDescriptorTable::InterruptFrame > {
        Architecture::Interrupt::eoi( frame->vector );
        return frame;
    }

    RTCEntry::RTCEntry( VOID ) noexcept {
        Architecture::InterruptDescriptorTable::set_interrupt_handler( IRQ_CMOS_RTC, rtc_handler );     // 注册RTC中断入口函数
    }
}