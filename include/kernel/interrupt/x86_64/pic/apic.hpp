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

    enum class ApicType {
        IO_APIC    = 0,
        LOCAL_APIC = 1
    };
    struct _packed ApicLocalVectorTableRegisters {
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

        explicit ApicLocalVectorTableRegisters( void ) = default;
        explicit ApicLocalVectorTableRegisters( IN uint32_t value ) {
            *this = *reinterpret_cast< ApicLocalVectorTableRegisters * >( &value );
        }
        explicit ApicLocalVectorTableRegisters( IN uint64_t value ) {
            *this = *reinterpret_cast< ApicLocalVectorTableRegisters * >( &value );
        }

        auto operator=( IN uint32_t value ) -> const ApicLocalVectorTableRegisters & {
            *reinterpret_cast< uint32_t * >( this ) = value;
            return *this;
        }
        auto operator|=( IN uint32_t value ) -> const ApicLocalVectorTableRegisters & {
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
        uint32_t reserved : 15;

        union {
            struct
            {
                uint32_t : 24;
                uint32_t phy_dest : 4;
                uint32_t reserved2 : 4;
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
    static auto check_apic( void ) -> int16_t;
    static auto irq_set_mask( IN irq_t irq ) -> void {
        ApicLocalVectorTableRegisters lvt { };
        lvt.mask = APIC_ICR_IOAPIC_MASKED;
        write_apic( IOAPIC_REG_TABLE + 2 * irq, lvt, ApicType::IO_APIC );
    }
    static auto irq_clear_mask( IN irq_t irq, IN uint8_t vector, IN uint8_t ) -> void {
        // 标记中断边缘触发，高电平有效，
        // 启用并路由到给定的cpunum，
        // 恰好是该cpu的APIC ID
        ApicLocalVectorTableRegisters lvt { };
        lvt.vector = vector;
        lvt.mask   = APIC_ICR_IOAPIC_UNMASKED;
        write_apic( IOAPIC_REG_TABLE + 2 * irq, lvt, ApicType::IO_APIC );
    }
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

public:
    inline static ApicInformation apic_map;

private:
    inline static int16_t apic_flags { };
};
}     // namespace QuantumNEC::Kernel::x86_64