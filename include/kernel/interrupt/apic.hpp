#pragma once
#include <cstdint>
#include <kernel/driver/acpi/madt.hpp>
#include <kernel/memory/segment/gdt.hpp>

namespace Interrupt {

inline struct Apic {
    constexpr static auto IA32_APIC_BASE_MSR { 0x1B };
    constexpr static auto IA32_APIC_BASE_MSR_BSP { 1UL << 8U };
    constexpr static auto IA32_APIC_BASE_MSR_ENABLE { 1UL << 11U };
    constexpr static auto IA32_APIC_BASE_MSR_BASE_ADDR_MSK { ~0xFFFUL };
    constexpr static auto IA32_EFER { 0xc0000080 };
    constexpr static auto IA32_STAR { 0xc0000081 };
    constexpr static auto IA32_LSTAR { 0xc0000082 };
    constexpr static auto IA32_FMASK { 0xc0000084 };
    constexpr static auto IA32_EFER_SCE { 1 };
    constexpr static auto IA32_KERNEL_GS_BASE { 0xc0000102 };
    constexpr static auto IA32_USER_GS_BASE { 0xc0000101 };
    constexpr static auto IA32_USER_FS_BASE { 0xc0000100 };

    /*
     Base Address   MSR address  Name                        Attribute
     0xfee00000     none         保留                         N/A
     0xfee00010     none         保留                         N/A
     0xfee00020     0x802        Local APIC ID寄存器          RW
     0xfee00030     0x803        Local APIC Version寄存器     RO
     0xfee00040     none         保留                         N/A
     ~
     0xfee00070     none         保留                         N/A
     0xfee00080     0x808        任务优先级寄存器TPR            RW
     0xfee00090     none         优先级仲裁寄存器APR            RO
     0xfee000a0     0x80a        处理器优先级寄存器PPR          RO
     0xfee000b0     0x80b        EOI寄存器                    WO
     0xfee000c0     none         远程读取寄存器RRD             RO
     0xfee000d0     0x80d        逻辑目标寄存器LDR             RW
     0xfee000e0     none         目标格式寄存器DFR             RW
     0xfee000f0     0x80f        伪中断向量寄存器SVR           RW
     0xfee00100     0x810        ISR寄存器(31:0)             RO
     0xfee00110     0x811        ISR寄存器(63:32)            RO
     0xfee00120     0x812        ISR寄存器(95:64)            RO
     0xfee00130     0x813        ISR寄存器(127:96)           RO
     0xfee00140     0x814        ISR寄存器(159:128)          RO
     0xfee00150     0x815        ISR寄存器(191:160)          RO
     0xfee00160     0x816        ISR寄存器(223:192)          RO
     0xfee00170     0x817        ISR寄存器(255:224)          RO
     0xfee00180     0x818        TMR寄存器(31:0)             RO
     0xfee00190     0x819        TMR寄存器(63:32)            RO
     0xfee001a0     0x81a        TMR寄存器(95:64)            RO
     0xfee001b0     0x81b        TMR寄存器(127:96)           RO
     0xfee001c0     0x81c        TMR寄存器(159:128)          RO
     0xfee001d0     0x81d        TMR寄存器(191:160)          RO
     0xfee001e0     0x81e        TMR寄存器(223:192)          RO
     0xfee001f0     0x81f        TMR寄存器(255:224)          RO
     0xfee00200     0x820        IRR寄存器(31:0)             RO
     0xfee00210     0x821        IRR寄存器(63:32)            RO
     0xfee00220     0x822        IRR寄存器(95:64)            RO
     0xfee00230     0x823        IRR寄存器(127:96)           RO
     0xfee00240     0x824        IRR寄存器(159:128)          RO
     0xfee00250     0x825        IRR寄存器(191:160)          RO
     0xfee00260     0x826        IRR寄存器(223:192)          RO
     0xfee00270     0x827        IRR寄存器(255:224)          RO
     0xfee00280     0x828        ESR错误状态寄存器             RO
     0xfee00290     none         保留                        N/A
     ~
     0xfee002e0     none         保留                        N/A
     0xfee002f0     0x82f        LVT CMCI寄存器              RW
     0xfee00300     0x830        中断命令寄存器ICR (31：0)     RW
     0xfee00310     0x831        中断命令寄存器ICR (63：32)    RW
     0xfee00320     0x832        LVT定时器寄存器              RW
     0xfee00330     0x833        LVT温度传感器寄存器           RW
     0xfee00340     0x834        LVT性能监控计数器寄存器        RW
     0xfee00350     0x835        LVTLINT0寄存器              RW
     0xfee00360     0x836        LVTLINT1寄存器              RW
     0xfee00370     0x837        LVT错误寄存器                RW
     0xfee00380     0x838        初始计数寄存器（定时器专用）    RW
     0xfee00390     0x839        当前计数寄存器（定时器专用）    RW
     0xfee003a0     none         保留                        N/A
     ~
     0xfee003d0     none         保留                        N/A
     0xfee003e0     0x83e        分频配置寄存器（定时器专用）    N/A
     N/A            0x83f        SELF IPI寄存器              N/A
 */
    constexpr static auto LOCAL_BASE_APIC_ID { 0x0020 };
    constexpr static auto LOCAL_BASE_APIC_VERSION { 0x0030 };
    constexpr static auto LOCAL_BASE_APIC_TPR { 0x0080 };
    constexpr static auto LOCAL_BASE_APIC_APR { 0x0090 };
    constexpr static auto LOCAL_BASE_APIC_PPR { 0x00A0 };
    constexpr static auto LOCAL_BASE_APIC_EOI { 0x00B0 };
    constexpr static auto LOCAL_BASE_APIC_RRD { 0x00C0 };
    constexpr static auto LOCAL_BASE_APIC_LDR { 0x00D0 };
    constexpr static auto LOCAL_BASE_APIC_DFR { 0x00E0 };
    constexpr static auto LOCAL_BASE_APIC_SVR { 0x00F0 };
    constexpr static auto LOCAL_BASE_APIC_ENABLE { 0x00000100 };
    constexpr static auto LOCAL_BASE_APIC_ESR { 0x0280 };
    constexpr static auto LOCAL_BASE_APIC_LVT_CMCI { 0x02F0 };
    constexpr static auto LOCAL_BASE_APIC_ICRL0 { 0x0300 };
    constexpr static auto LOCAL_BASE_APIC_ICRL1 { 0x0310 };
    constexpr static auto LOCAL_BASE_APIC_LVT_TIMER { 0x0320 };
    constexpr static auto LOCAL_BASE_APIC_LVT_TS { 0x0330 };
    constexpr static auto LOCAL_BASE_APIC_LVT_PMC { 0x0340 };
    constexpr static auto LOCAL_BASE_APIC_LVT_LINT0 { 0x0350 };
    constexpr static auto LOCAL_BASE_APIC_LVT_LINT1 { 0x0360 };
    constexpr static auto LOCAL_BASE_APIC_LVT_ERROR { 0x0370 };
    constexpr static auto LOCAL_BASE_APIC_TICR { 0x0380 };
    constexpr static auto LOCAL_BASE_APIC_TCCR { 0x0390 };
    constexpr static auto LOCAL_BASE_APIC_TDCR { 0x03E0 };
    
    constexpr static auto LOCAL_APIC_MSR_SVR { 0x80f };
    constexpr static auto LOCAL_APIC_MSR_VERSION { 0x803 };
    constexpr static auto LOCAL_APIC_MSR_ID { 0x802 };
    constexpr static auto LOCAL_APIC_MSR_LVT_CMCI { 0x82f };
    constexpr static auto LOCAL_APIC_MSR_ICRL0 { 0x830 };
    constexpr static auto LOCAL_APIC_MSR_ICRL1 { 0x831 };
    constexpr static auto LOCAL_APIC_MSR_LVT_TIMER { 0x832 };
    constexpr static auto LOCAL_APIC_MSR_LVT_TS { 0x833 };
    constexpr static auto LOCAL_APIC_MSR_LVT_PMC { 0x834 };
    constexpr static auto LOCAL_APIC_MSR_LVT_LINT0 { 0x835 };
    constexpr static auto LOCAL_APIC_MSR_LVT_LINT1 { 0x836 };
    constexpr static auto LOCAL_APIC_MSR_LVT_ERROR { 0x837 };
    constexpr static auto LOCAL_APIC_MSR_ESR { 0x828 };
    constexpr static auto LOCAL_APIC_MSR_EOI { 0x80b };
    constexpr static auto LOCAL_APIC_MSR_TPR { 0x808 };
    constexpr static auto LOCAL_APIC_MSR_TICR { 0x838 };
    constexpr static auto LOCAL_APIC_MSR_TCCR { 0x839 };
    constexpr static auto LOCAL_APIC_MSR_TDCR { 0x83e };

    constexpr static auto APIC_INIT { 0x00000500 };
    constexpr static auto APIC_STARTUP { 0x00000600 };
    constexpr static auto APIC_DELIVS { 0x00001000 };
    constexpr static auto APIC_ASSERT { 0x00004000 };
    constexpr static auto APIC_DEASSERT { 0x00000000 };
    constexpr static auto APIC_LEVEL { 0x00008000 };
    constexpr static auto APIC_BCAST { 0x00080000 };
    constexpr static auto APIC_BUSY { 0x00001000 };
    constexpr static auto APIC_FIXED { 0x00000000 };
    constexpr static auto APIC_X1 { 0x0000000B };
    constexpr static auto APIC_PERIODIC { 0x00020000 };
    constexpr static auto APIC_PCINT { 0x0340 };
    constexpr static auto APIC_MASKED { 0x00010000 };
    constexpr static auto IOAPIC_REG_ID { 0x00 };
    constexpr static auto IOAPIC_REG_VER { 0x01 };
    constexpr static auto IOAPIC_REG_TABLE { 0x10 };
    constexpr static auto INT_DISABLED { 0x00010000 };
    constexpr static auto INT_LEVEL { 0x00008000 };
    constexpr static auto INT_ACTIVELOW { 0x00002000 };
    constexpr static auto INT_LOGICAL { 0x00000800 };
    constexpr static auto APIC_ICR_IOAPIC_FIXED { 0 };
    constexpr static auto IOAPIC_ICR_LOWEST_PRIORITY { 1 };
    constexpr static auto APIC_ICR_IOAPIC_SMI { 2 };
    constexpr static auto APIC_ICR_IOAPIC_NMI { 4 };
    constexpr static auto APIC_ICR_IOAPIC_INIT { 5 };
    constexpr static auto ICR_START_UP { 6 };
    constexpr static auto IOAPIC_EXTINT { 7 };
    constexpr static auto APIC_LVT_TIMER_ONE_SHOT { 0 };
    constexpr static auto APIC_LVT_TIMER_PERIODIC { 1 };
    constexpr static auto APIC_LVT_TIMER_TSC_DEADLINE { 2 };
    constexpr static auto APIC_ICR_IOAPIC_MASKED { 1 };
    constexpr static auto APIC_ICR_IOAPIC_UNMASKED { 0 };
    constexpr static auto APIC_ICR_IOAPIC_EDGE { 0 };
    constexpr static auto APIC_ICR_IOAPIC_LEVEL { 1 };
    constexpr static auto APIC_ICR_IOAPIC_IDLE { 0 };
    constexpr static auto APIC_ICR_IOAPIC_SEND_PENDING { 1 };
    constexpr static auto ICR_NO_SHORTHAND { 0 };
    constexpr static auto ICR_SELF { 1 };
    constexpr static auto ICR_ALL_INCLUDE_SELF { 2 };
    constexpr static auto ICR_ALL_EXCLUDE_SELF { 3 };
    constexpr static auto ICR_IOAPIC_DELV_PHYSICAL { 0 };
    constexpr static auto ICR_IOAPIC_DELV_LOGIC { 1 };
    constexpr static auto ICR_LEVEL_DE_ASSERT { 0 };
    constexpr static auto ICR_LEVLE_ASSERT { 1 };
    constexpr static auto APIC_IOAPIC_IRR_RESET { 0 };
    constexpr static auto APIC_IOAPIC_IRR_ACCEPT { 1 };
    constexpr static auto APIC_IOAPIC_POLARITY_HIGH { 0 };
    constexpr static auto APIC_IOAPIC_POLARITY_LOW { 1 };
    using irq_t = uint64_t;
    struct [[gnu::packed]] SpuriousInterruptVectorRegister {
        uint32_t : 8;
        uint32_t enable_apic : 1;
        uint32_t : 3;
        uint32_t mask_eoi : 1;
        uint32_t : 19;
        explicit SpuriousInterruptVectorRegister( void ) = default;
        explicit SpuriousInterruptVectorRegister( uint32_t value ) {
            *this = *reinterpret_cast< SpuriousInterruptVectorRegister * >( &value );
        }

        operator uint32_t( ) {
            return *( (uint32_t *)this );
        }
    };

    constexpr static auto SVR_ENABLE_APIC = 1;
    constexpr static auto SVR_DISABLE_APIC = 0;
    constexpr static auto SVR_EOI_MASK = 1;
    constexpr static auto SVR_EOI_UNMASK = 0;

    struct [[gnu::packed]] LocalVectorTableRegisters {
        uint32_t vector : 8;             // 0~7	ALL
        uint32_t deliver_mode : 3;       // 8~10	      CMCI LINT0 LINT1 PerformCounter ThermalSensor
        uint32_t : 1;                    // 11
        uint32_t deliver_status : 1;     // 12	ALL
        uint32_t polarity : 1;           // 13	           LINT0 LINT1
        uint32_t irr : 1;                // 14	           LINT0 LINT1
        uint32_t trigger : 1;            // 15	           LINT0 LINT1
        uint32_t mask : 1;               // 16	ALL
        uint32_t timer_mode : 2;         // 17~18	Timer
        uint32_t resd : 13;              // 19~31
        operator uint32_t( ) {
            return *reinterpret_cast< uint32_t * >( this );
        }

        explicit LocalVectorTableRegisters( void ) = default;
        explicit LocalVectorTableRegisters( uint32_t value ) {
            *this = *reinterpret_cast< LocalVectorTableRegisters * >( &value );
        }
        explicit LocalVectorTableRegisters( uint64_t value ) {
            *this = *reinterpret_cast< LocalVectorTableRegisters * >( &value );
        }

        auto operator=( uint32_t value ) -> const LocalVectorTableRegisters & {
            *reinterpret_cast< uint32_t * >( this ) = value;
            return *this;
        }
        auto operator|=( uint32_t value ) -> const LocalVectorTableRegisters & {
            *this = value | uint32_t( *this );
            return *this;
        }
    };

    struct [[gnu::packed]] InterruptCommandRegister {
        uint32_t vector : 8;
        uint32_t deliver_mode : 3;
        uint32_t dest_mode : 1;
        uint32_t deliver_status : 1;
        uint32_t : 1;
        uint32_t level : 1;
        uint32_t trigger : 1;
        uint32_t : 2;
        uint32_t dest_shorthand : 2;
        uint32_t : 12;

        union {
            struct
            {
                uint32_t : 24;
                uint32_t dest_field : 8;
            } apic_destination;

            uint32_t x2apic_destination;
        } destination;
        operator uint64_t( ) {
            return *reinterpret_cast< uint64_t * >( this );
        }
        explicit InterruptCommandRegister( void ) = default;
        explicit InterruptCommandRegister( uint64_t value ) {
            *this = *reinterpret_cast< decltype( this ) >( &value );
        }
        auto operator=( uint64_t value ) -> const InterruptCommandRegister & {
            *reinterpret_cast< uint64_t * >( this ) = value;
            return *this;
        }
    };

    struct [[gnu::packed]] IOApicRedirectionEntry {
        uint32_t vector : 8;
        uint32_t deliver_mode : 3;
        uint32_t dest_mode : 1;
        uint32_t deliver_status : 1;
        uint32_t polarity : 1;
        uint32_t irr : 1;
        uint32_t trigger : 1;
        uint32_t mask : 1;
        uint32_t : 15;

        union {
            struct
            {
                uint32_t : 24;
                uint32_t phy_dest : 4;
                uint32_t : 4;
            } physical;

            struct
            {
                uint32_t : 24;
                uint32_t logical_dest : 8;
            } logical;
        } destination;
        explicit IOApicRedirectionEntry( void ) = default;
        explicit IOApicRedirectionEntry( uint64_t value ) {
            *this = *reinterpret_cast< decltype( this ) >( &value );
        }
        operator uint64_t( ) {
            return *reinterpret_cast< uint64_t * >( this );
        }
    };
    std::uint64_t lapic_address;
    std::uint64_t core_count;
    std::uint8_t local_apic_ID[ Memory::GDT::GDT_COUNT ];
    std::uint64_t ioapic_count;

    struct IOApic {
        std::uint64_t ioapic_id;
        std::uint64_t ioapic_address;
        void *ioapic_index_address;
        void *ioapic_EOI_address;
        void *ioapic_data_address;
        std::uint64_t gsi_base;
        std::uint64_t irq_count;
    } ioapic[ 8 ];

    std::uint32_t iso_count;
    Driver::MADT::InterruptSourceOverride iso[ 64 ];

    auto write( std::uint32_t index, std::uint32_t reg, std::uint64_t value ) -> void;
    auto read( std::uint32_t index, std::uint32_t reg ) -> std::uint64_t;
    auto write( std::uint32_t reg, std::uint64_t value ) -> void;
    auto read( std::uint32_t reg ) -> std::uint64_t;

    // 抄的

    auto find_ioapic( std::uint32_t gsi ) -> std::int64_t;
    auto enable( std::uint8_t vector, std::uint32_t irq ) -> void;
    auto install( std::uint8_t vector, std::uint32_t irq ) -> void;

    auto apic_id( void ) -> std::uint64_t;
} apic;

auto initialize_apic( void ) -> void;
}     // namespace Interrupt