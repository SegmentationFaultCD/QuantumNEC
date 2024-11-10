#pragma once
#include <kernel/memory/heap/kheap/kheap_manager.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class HeapManager : public KHeapManager {
    public:
        explicit HeapManager( VOID ) noexcept = default;
        virtual ~HeapManager( VOID ) noexcept = default;
    };
}