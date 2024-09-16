#pragma once
#include <Arch/x86_64/cpu/xdt.hpp>
#include <Lib/Types/Uefi.hpp>
#include <Lib/Base/deflib.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    /**
     * @brief CPU管理类
     */
    PUBLIC class CPUs
    {
    public:
        struct CpuidStatus
        {
            Lib::Types::uint64_t mop;
            Lib::Types::uint64_t sop;
            Lib::Types::uint64_t rax;
            Lib::Types::uint64_t rbx;
            Lib::Types::uint64_t rcx;
            Lib::Types::uint64_t rdx;
        };

    public:
        explicit CPUs( VOID ) noexcept;
        virtual ~CPUs( VOID ) noexcept = default;

    public:
        STATIC auto cpuid( IN CpuidStatus &status ) -> CpuidStatus &;
        STATIC auto switch_to( IN OUT Lib::Types::Ptr< VOID > current, IN Lib::Types::Ptr< VOID > next ) -> VOID;
        STATIC auto port_insw( IN Lib::Types::uint64_t port, IN Lib::Types::Ptr< VOID > buffer, IN Lib::Types::uint64_t nr ) -> VOID;
        STATIC auto port_outsw( IN Lib::Types::uint64_t port, IN Lib::Types::Ptr< VOID > buffer, IN Lib::Types::uint64_t nr ) -> VOID;
        STATIC auto cli( VOID ) -> VOID;
        STATIC auto sti( VOID ) -> VOID;
        STATIC auto hlt( VOID ) -> VOID;
        STATIC auto nop( VOID ) -> VOID;
        STATIC auto rdmsr( IN Lib::Types::uint64_t address ) -> Lib::Types::uint64_t;
        STATIC auto wrmsr( IN Lib::Types::uint64_t address, IN Lib::Types::uint64_t value ) -> VOID;
        STATIC auto get_rsp( VOID ) -> Lib::Types::uint64_t;
        STATIC auto get_rflags( VOID ) -> Lib::Types::uint64_t;
        STATIC auto io_in8( IN Lib::Types::uint16_t port ) -> Lib::Types::uint8_t;
        STATIC auto io_in16( IN Lib::Types::uint16_t port ) -> Lib::Types::uint16_t;
        STATIC auto io_in32( IN Lib::Types::uint16_t port ) -> Lib::Types::uint32_t;
        STATIC auto io_out8( IN Lib::Types::uint16_t port, IN Lib::Types::uint8_t value ) -> VOID;
        STATIC auto io_out16( IN Lib::Types::uint16_t port, IN Lib::Types::uint16_t value ) -> VOID;
        STATIC auto io_out32( IN Lib::Types::uint16_t port, IN Lib::Types::uint32_t value ) -> VOID;

        STATIC auto read_cr8( VOID ) -> ControlRegisters::CR8;
        STATIC auto write_cr8( IN ControlRegisters::CR8 cr8 ) -> VOID;
        STATIC auto read_cr4( VOID ) -> ControlRegisters::CR4;
        STATIC auto write_cr4( IN ControlRegisters::CR4 cr4 ) -> VOID;
        STATIC auto read_cr3( VOID ) -> ControlRegisters::CR3;
        STATIC auto write_cr3( IN ControlRegisters::CR3 cr3 ) -> VOID;
        STATIC auto read_cr2( VOID ) -> ControlRegisters::CR2;
        STATIC auto write_cr2( IN ControlRegisters::CR2 cr2 ) -> VOID;
        STATIC auto read_cr0( VOID ) -> ControlRegisters::CR0;
        STATIC auto write_cr0( IN ControlRegisters::CR0 cr0 ) -> VOID;
        STATIC auto invlpg( IN Lib::Types::Ptr< VOID > address ) -> VOID;
        STATIC auto pause( VOID ) -> VOID;
        STATIC auto mfence( VOID ) -> VOID;
        STATIC auto lfence( VOID ) -> VOID;
        STATIC auto sfence( VOID ) -> VOID;
        STATIC auto set_page_table( IN Lib::Types::Ptr< Lib::Types::uint64_t > mmap ) -> VOID;
        STATIC auto flush_tlb( VOID ) -> VOID;
    };

    // cpuid
}