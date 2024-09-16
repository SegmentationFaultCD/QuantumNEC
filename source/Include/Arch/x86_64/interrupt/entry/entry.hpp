#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Arch/x86_64/interrupt/entry/clock_entry.hpp>
#include <Arch/x86_64/interrupt/entry/syscall_entry.hpp>
#include <Arch/x86_64/interrupt/entry/local_apic_entry.hpp>
#include <Arch/x86_64/interrupt/entry/rtc_entry.hpp>
#include <Arch/x86_64/interrupt/entry/cascade_entry.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class InterruptEntryRegister :
        ClockEntry,          // 时间中断(0x20号中断)入口注册
        SystemcallEntry,     // 系统调用(0x80号中断)入口注册
        LocalApicEntry,      // 本地APIC(0x81与0x82号中断)入口注册
        RTCEntry,            // 实时时钟(0x28号中断)入口注册
        CascadeEntry         // 联级(0x22号中断)入口注册
    {
    public:
        explicit InterruptEntryRegister( VOID ) noexcept;
        virtual ~InterruptEntryRegister( VOID ) noexcept;

    private:
    };
}