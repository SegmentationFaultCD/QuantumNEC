#pragma once
#include <Kernel/memory/memory_page_map/memory_page_map.hpp>
#include <Kernel/memory/memory_page_map/memory_map_2M.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class MemoryPageMap4K : public MemoryPageMaping< PAGE_4K_SIZE >
    {
    public:
        constexpr static auto page_size = PAGE_4K_SIZE;

    public:
        explicit MemoryPageMap4K( VOID ) noexcept = default;
        virtual ~MemoryPageMap4K( VOID ) noexcept = default;

        virtual auto allocate( IN uint64_t size ) -> VOID * override;
        virtual auto free( IN VOID *address, IN uint64_t size ) -> VOID override;

    private:
        inline STATIC Lib::ListTable memory_page_table_4k { };
    };
}