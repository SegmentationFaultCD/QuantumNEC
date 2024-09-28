#pragma once
#include <Arch/x86_64/abi/builtins.hpp>
#include <Arch/x86_64/cpu/cpu.hpp>
#include <Arch/x86_64/smp/smp.hpp>
#include <Arch/x86_64/interrupt/interrupt.hpp>
#include <Arch/x86_64/platform/global.hpp>
#include <Arch/x86_64/syscall/syscall.hpp>
#include <Arch/x86_64/device/device.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    PUBLIC class x86_64Architecture :
        // 架构组织
        public Device,
        public CPUs,
        public Interrupt,
        public Syscall,
        public SymmetricMultiprocessing
    {
    public:
        using InterruptFrame = InterruptDescriptorTable::InterruptFrame;

    public:
        explicit x86_64Architecture( VOID ) noexcept;
        virtual ~x86_64Architecture( VOID ) noexcept = default;

    public:
        inline STATIC byte_t *kernel_stack_space;
    };
    PUBLIC inline BootConfig __config { };
    PUBLIC [[noreturn]] auto apu_entry( IN limine_smp_info * cpu )->VOID;
}