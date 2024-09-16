#pragma once
#include <Arch/x86_64/abi/builtins.hpp>
#include <Arch/x86_64/cpu/cpu.hpp>
#include <Arch/x86_64/cpu/xdt.hpp>
#include <Arch/x86_64/cpu/smp.hpp>
#include <Arch/x86_64/interrupt/interrupt.hpp>
#include <Arch/x86_64/platform/global.hpp>
#include <Arch/x86_64/platform/syscall.hpp>
#include <Arch/x86_64/device/device.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class x86_64Architecture :
        // 架构组织
        public Device,
        public CPUs,
        public Interrupt,
        public Syscall
    {
    public:
        explicit x86_64Architecture( IN Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept;
        virtual ~x86_64Architecture( VOID ) noexcept = default;

    public:
        inline STATIC Lib::Types::Ptr< Lib::Types::byte_t > kernel_stack_space;
    };
}