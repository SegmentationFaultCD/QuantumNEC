#pragma once
#include <Kernel/memory/page_allocater/page_allocaters.hpp>
#include <Kernel/memory/paging_map/pmlxt.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class PageAllocater1G : public PageAllocaters< PAGE_1G_SIZE >
    {
    public:
        constexpr static auto page_size = PAGE_1G_SIZE;

    public:
        explicit PageAllocater1G( VOID ) noexcept = default;
        virtual ~PageAllocater1G( VOID ) noexcept = default;
        virtual auto allocate( IN uint64_t size ) -> VOID * override;
        virtual auto free( IN VOID *address, IN uint64_t size ) -> VOID override;

    public:
        inline STATIC Lib::ListTable page_header_group_list { };

    private:
        inline STATIC uint64_t global_memory { PAGE_1G_SIZE };     // 前1G已经被占用了
    };
}