#pragma once
#include <Kernel/memory/page_allocater/page_allocaters.hpp>
#include <Kernel/memory/paging_map/pmlxt.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class PageAllocater2M : public PageAllocaters< PAGE_2M_SIZE >
    {
    public:
        constexpr static auto page_size = PAGE_2M_SIZE;

    public:
        explicit PageAllocater2M( VOID ) noexcept = default;
        virtual ~PageAllocater2M( VOID ) noexcept = default;

        virtual auto allocate( IN uint64_t size ) -> VOID * override;
        virtual auto free( IN VOID *address, IN uint64_t size ) -> VOID override;

    public:
        inline STATIC Lib::ListTable page_header_group_list { };
    };
}