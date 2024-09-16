#pragma once
#include <Kernel/memory/memory_page_map/memory_page_map.hpp>
#include <Kernel/memory/memory_page_map/memory_map_1G.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class MemoryPageMap2M : public MemoryPageMaping< PAGE_2M_SIZE >
    {
    public:
        constexpr static auto page_size = PAGE_2M_SIZE;

    public:
        explicit MemoryPageMap2M( VOID ) noexcept = default;
        virtual ~MemoryPageMap2M( VOID ) noexcept = default;

        virtual auto allocate( IN uint64_t size ) -> VOID * override;
        virtual auto free( IN VOID *address, IN uint64_t size ) -> VOID override;

    private:
        inline STATIC Lib::ListTable memory_page_table_2M { };
    };
}