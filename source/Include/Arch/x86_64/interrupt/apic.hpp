#pragma once
#include <Lib/Types/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class Apic
    {
    public:
        using irq_t = Lib::Types::uint8_t;

        struct ApicInformation
        {
            Lib::Types::uint64_t local_apic_address;
            Lib::Types::uint32_t io_apic_address;
            Lib::Types::Ptr< VOID > io_apic_index_address;
            Lib::Types::Ptr< VOID > io_apic_EOI_address;
            Lib::Types::Ptr< VOID > io_apic_data_address;
            Lib::Types::uint8_t core_count;
            Lib::Types::uint8_t local_apic_ID[ 256 ];
        };

        enum class ApicType {
            IO_APIC = 0,
            LOCAL_APIC = 1
        };

    public:
        explicit Apic( VOID ) noexcept;
        virtual ~Apic( VOID ) noexcept;

    public:
        STATIC auto write_apic( IN Lib::Types::uint16_t index, IN Lib::Types::uint32_t value, IN ApicType type ) -> VOID;
        STATIC auto read_apic( IN Lib::Types::uint16_t index, IN ApicType type ) -> Lib::Types::uint64_t;
        STATIC auto enable_ioapic( IN Lib::Types::uint64_t pin, IN Lib::Types::uint64_t vector ) -> VOID;
        STATIC auto eoi( IN CONST irq_t irq ) -> VOID;

        /**
         * @brief 检测CPU是否支持APIC
         * @return 如果CPU支持且未在MSR中禁用，则返回 TRUE 值
         */
        STATIC auto check_apic( VOID ) -> Lib::Types::BOOL;
        STATIC auto irq_set_mask( IN irq_t irq ) -> VOID;
        STATIC auto irq_clear_mask( IN irq_t irq, IN Lib::Types::uint8_t vector, IN Lib::Types::uint8_t apicID ) -> VOID;

        STATIC auto enable_ioapic( IN irq_t irq ) -> VOID;
        STATIC auto disable_ioapic( IN irq_t irq ) -> VOID;
        STATIC auto install_ioapic( IN irq_t irq, IN VOID *arg ) -> VOID;
        STATIC auto uninstall_ioapic( IN irq_t irq ) -> VOID;
        STATIC auto edge_ack( IN ApicType type ) -> VOID;

    public:
        inline STATIC ApicInformation apic_map;
    };
}