#pragma once

#include <kernel/memory/arch/memory_arch.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_collector.hpp>
#include <kernel/memory/page/page_manager.hpp>
#include <kernel/memory/heap/heap_allocater.hpp>
#include <kernel/memory/heap/heap_collector.hpp>
#include <kernel/memory/heap/heap_manager.hpp>

PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class Memory :
        public PageManager,
        public MemoryArch
    // HeapManager
    {
    public:
        explicit Memory( VOID ) noexcept = default;

    public:
        virtual ~Memory( VOID ) noexcept = default;
    };
}