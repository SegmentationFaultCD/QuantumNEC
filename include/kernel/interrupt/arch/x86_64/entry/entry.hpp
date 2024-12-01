#pragma once
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class InterruptEntry {
    protected:
        using Frame = InterruptDescriptorTable::InterruptFrame;

    private:
        virtual auto name( VOID ) noexcept -> VOID               = 0;
        virtual auto handler( Frame *frame ) noexcept -> Frame * = 0;
        virtual auto error_code( uint64_t error_code ) noexcept -> VOID;

    protected:
        auto registers( Frame *frame ) noexcept -> VOID;
        auto task( VOID ) noexcept -> VOID;
        auto cpu( VOID ) noexcept -> VOID;

    public:
        virtual auto do_register( VOID ) -> VOID = 0;

    public:
        explicit InterruptEntry( VOID ) noexcept = default;
        virtual ~InterruptEntry( VOID ) noexcept = default;

    public:
        auto do_IRQ( Frame *frame ) -> Frame * {
            this->name( );
            this->error_code( frame->error_code );
            return this->handler( frame );
        }

    private:
        struct ErrorCode {
            uint32_t external : 1;
            uint32_t tbl : 2;
            uint32_t index : 13;
            uint32_t reserved : 16;
            ErrorCode( uint64_t error_code ) {
                *reinterpret_cast< uint64_t * >( this ) = error_code;
            }
        };
    };
    // PUBLIC class InterruptEntryRegister :
    //     public ClockEntry,             // 时间中断(0x20号中断)入口注册
    //     public SystemcallEntry,        // 系统调用(0x80号中断)入口注册
    //     public ApicErrorEntry,         // 本地APIC(0x81与0x82号中断)入口注册
    //     public RTCEntry,               // 实时时钟(0x28号中断)入口注册
    //     public Cascade_TimerEntry,     // 联级(0x22号中断)入口注册
    //     public SymmetricMultiprocessingSwitchCPUEntry {
    // public:
    //     explicit InterruptEntryRegister( VOID ) noexcept;
    //     virtual ~InterruptEntryRegister( VOID ) noexcept;

    // private:
    // };
}