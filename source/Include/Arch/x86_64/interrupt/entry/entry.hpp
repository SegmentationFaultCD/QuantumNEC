#pragma once
#include <Lib/Uefi.hpp>
#include <Arch/x86_64/interrupt/entry/clock_entry.hpp>
#include <Arch/x86_64/interrupt/entry/syscall_entry.hpp>
#include <Arch/x86_64/interrupt/entry/apic_error.hpp>
#include <Arch/x86_64/interrupt/entry/rtc_entry.hpp>
#include <Arch/x86_64/interrupt/entry/cascade_timer_handler.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class InterruptEntryRegister :
        public ClockEntry,            // 时间中断(0x20号中断)入口注册
        public SystemcallEntry,       // 系统调用(0x80号中断)入口注册
        public ApicErrorEntry,        // 本地APIC(0x81与0x82号中断)入口注册
        public RTCEntry,              // 实时时钟(0x28号中断)入口注册
        public Cascade_TimerEntry     // 联级(0x22号中断)入口注册
    {
    public:
        explicit InterruptEntryRegister( VOID ) noexcept;
        virtual ~InterruptEntryRegister( VOID ) noexcept;

    private:
    };
}