#pragma once
#include <Kernel/memory/memory_page_map/memory_page_map.hpp>
#include <Kernel/memory/pmlxt/pmlxt.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class MemoryPageMap1G : public MemoryPageMaping< PAGE_1G_SIZE >
    {
    public:
        constexpr static auto page_size = PAGE_1G_SIZE;

    public:
        explicit MemoryPageMap1G( VOID ) noexcept = default;
        virtual ~MemoryPageMap1G( VOID ) noexcept = default;
        virtual auto allocate( IN uint64_t size ) -> VOID * override;
        virtual auto free( IN VOID *address, IN uint64_t size ) -> VOID override;

    private:
        inline STATIC Lib::ListTable memory_page_table_1G { };
        inline STATIC uint64_t global_memory { PAGE_1G_SIZE };     // 前1G已经被占用了
    };
}