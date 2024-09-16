#pragma once
#include <Kernel/Memory/heap.hpp>
#include <Kernel/Memory/map.hpp>
#include <Kernel/Memory/page.hpp>
#include <Kernel/Memory/stack.hpp>
#include <Lib/Types/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class Memory
    {
    public:
        explicit Memory( Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept;

    public:
        virtual ~Memory( VOID ) noexcept = default;

        inline STATIC HeapMemory *heap;
        inline STATIC PageMemory *page;
        inline STATIC MemoryMap *memory_paging;
    };
}