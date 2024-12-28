#pragma once
#include <kernel/global/x86_64/global.hpp>
#include <lib/Uefi.hpp>

namespace QuantumNEC::Kernel::x86_64 {
class Apic {
public:
    using irq_t = uint64_t;

    struct IOApicInformation {
        uint64_t io_apic_address;
        void    *io_apic_index_address;
        void    *io_apic_EOI_address;
        void    *io_apic_data_address;
    };
    struct ApicInformation {
        uint64_t          local_apic_address;
        uint8_t           core_count;
        uint8_t           local_apic_ID[ NR_CPUS ];
        uint64_t          ioapic_count;
        IOApicInformation ioapic[ 8 ];     // ioapic最多可以有8个
    };
    enum class SupportState {
        SUPPORT_x2APIC,
        SUPPORT_xAPIC,
        DOES_NOT_SUPPORT
    };
    enum class ApicType {
        IO_APIC    = 0,
        LOCAL_APIC = 1
    };
    struct _packed SpuriousInterruptVectorRegister {
        uint32_t : 8;
        uint32_t enable_apic : 1;
        uint32_t : 4;
        uint32_t mask_eoi : 1;
        uint32_t : 18;
        explicit SpuriousInterruptVectorRegister( void ) = default;
        explicit SpuriousInterruptVectorRegister( IN uint32_t value ) {
            *this = *reinterpret_cast< SpuriousInterruptVectorRegister * >( &value );
        }
        explicit SpuriousInterruptVectorRegister( IN uint64_t value ) {
            *this = *reinterpret_cast< SpuriousInterruptVectorRegister * >( &value );
        }
        operator uint32_t( ) {
            return *( (uint32_t *)this );
        }
    };
    constexpr static auto SVR_ENABLE_APIC  = 1;
    constexpr static auto SVR_DISABLE_APIC = 0;
    constexpr static auto SVR_EOI_MASK     = 1;
    constexpr static auto SVR_EOI_UNMASK   = 0;

    struct _packed LocalVectorTableRegisters {
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
        explicit LocalVectorTableRegisters( IN uint32_t value ) {
            *this = *reinterpret_cast< LocalVectorTableRegisters * >( &value );
        }
        explicit LocalVectorTableRegisters( IN uint64_t value ) {
            *this = *reinterpret_cast< LocalVectorTableRegisters * >( &value );
        }

        auto operator=( IN uint32_t value ) -> const LocalVectorTableRegisters & {
            *reinterpret_cast< uint32_t * >( this ) = value;
            return *this;
        }
        auto operator|=( IN uint32_t value ) -> const LocalVectorTableRegisters & {
            *this = value | uint32_t( *this );
            return *this;
        }
    };

    struct _packed InterruptCommandRegister {
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
        auto operator=( IN uint64_t value ) -> const InterruptCommandRegister & {
            *reinterpret_cast< uint64_t * >( this ) = value;
            return *this;
        }
    };

    struct _packed IOApicRedirectionEntry {
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

public:
    explicit Apic( void ) noexcept;
    virtual ~Apic( void ) noexcept;

public:
    static auto write_apic( IN uint16_t index, IN uint64_t value, IN ApicType type ) -> void;
    static auto read_apic( IN uint16_t index, IN ApicType type ) -> uint64_t;
    static auto eoi( IN const irq_t irq ) -> void;
    static auto enable_xapic( void ) -> void;
    static auto enable_x2apic( void ) -> void;
    /**
     * @brief 检测CPU是否支持APIC
     * @return 如果CPU支持且未在MSR中禁用，则返回 TRUE 值
     */
    static auto check_apic( void ) -> SupportState;
    static auto irq_set_mask( IN irq_t irq ) -> void;
    static auto irq_clear_mask( IN irq_t irq, IN uint8_t vector, IN uint8_t ) -> void;
    static auto enable_ioapic( IN IN irq_t irq ) -> void;
    static auto disable_ioapic( IN irq_t irq ) -> void;
    static auto install_ioapic( IN Apic::irq_t irq, IN void *entry ) -> uint64_t;
    static auto uninstall_ioapic( IN irq_t irq ) -> void;
    static auto ioapic_level_ack( IN irq_t irq ) -> void;
    static auto ioapic_edge_ack( IN irq_t irq ) -> void;
    static auto apic_id( void ) -> uint64_t;

    static auto cpu_id( void ) {
        return apic_id( );
    }

    /**
     * @brief make IOApicRedirectionEntry struct
     * @param entry 结构体
     * @param vector 中断向量
     * @param deliver_mode 投递模式
     * @param dest_mode 目标模式
     * @param deliver_status 投递状态
     * @param polarity 电平触发极性
     * @param irr 远程IRR标志位（只读）
     * @param trigger 触发模式
     * @param mask 屏蔽标志位，（0为未屏蔽， 1为已屏蔽）
     * @param dest_apicID 目标apicID
     */
    static auto make_ioapic_rte_entry( uint8_t vector, uint8_t deliver_mode, uint8_t dest_mode, uint8_t deliver_status, uint8_t polarity, uint8_t irr, uint8_t trigger, uint8_t mask, uint8_t dest_apicID ) {
        IOApicRedirectionEntry entry;
        entry.vector         = vector;
        entry.deliver_mode   = deliver_mode;
        entry.dest_mode      = dest_mode;
        entry.deliver_status = deliver_status;
        entry.polarity       = polarity;
        entry.irr            = irr;
        entry.trigger        = trigger;
        entry.mask           = mask;
        if ( dest_mode == x86_64::ICR_IOAPIC_DELV_PHYSICAL ) {
            entry.destination.physical.phy_dest = dest_apicID;
        }
        else {
            entry.destination.logical.logical_dest = dest_apicID;
        }
        return entry;
    }
    static auto make_icr_entry( uint8_t vector, uint8_t deliver_mode, uint8_t dest_mode, uint8_t deliver_status, uint8_t level, uint8_t dest_shorthand, uint8_t trigger, uint64_t destination ) {
        InterruptCommandRegister icr;
        icr.vector         = vector;
        icr.deliver_mode   = deliver_mode;
        icr.dest_mode      = dest_mode;
        icr.deliver_status = deliver_status;
        icr.level          = level;
        icr.dest_shorthand = dest_shorthand;
        icr.trigger        = trigger;
        if ( apic_flags == SupportState::SUPPORT_x2APIC ) {
            icr.destination.x2apic_destination = destination;
        }
        else {
            icr.destination.apic_destination.dest_field = destination;
        }
        return icr;
    }

public:
    inline static ApicInformation apic_map;

public:
    inline static SupportState apic_flags { };
};
}     // namespace QuantumNEC::Kernel::x86_64