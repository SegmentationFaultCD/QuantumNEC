#pragma once

#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/heap/heap.hpp>
#include <kernel/memory/page/page_manager.hpp>

PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class Memory :
        public MemoryArch,
        public PageManager,
        public HeapManager {
    public:
        explicit Memory( VOID ) noexcept = default;

        virtual ~Memory( VOID ) noexcept = default;
    };
}