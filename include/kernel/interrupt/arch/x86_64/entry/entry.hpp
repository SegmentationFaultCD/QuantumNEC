#pragma once
#include <lib/Uefi.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry_clock.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry_syscall.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry_apic_error.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry_rtc.hpp>
#include <kernel/interrupt/arch/x86_64/entry/entry_cascade_timer.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
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