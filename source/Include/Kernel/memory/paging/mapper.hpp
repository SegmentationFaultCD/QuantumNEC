#pragma once
#include <Lib/Uefi.hpp>
#include <Kernel/memory/manager/page/page_manager.hpp>
#include <Kernel/memory/paging/page_table.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class __mapper
    {
    public:
        explicit __mapper( VOID ) = default;
        virtual ~__mapper( VOID ) = default;

    public:
        /**
         * @brief 映射页
         * @param physics_address 要映射的物理地址
         * @param virtual_address 将物理地址映射的指定虚拟地址
         * @param flags 映射标志
         * @param level 页级5/4/3/2/1
         * @param mode 映射页的模式 (4K/2M/1G)
         * @param pmlx_t 页表
         */
        auto __map( IN uint64_t physics_address, IN uint64_t virtual_address, IN uint64_t size, IN uint64_t flags, IN MemoryPageType mode, IN pmlxt &pmlx_t = *__paging::__kernel_page_table ) -> VOID;
        /**
         * @brief 取消映射页
         * @param virtual_address 将取消映射的指定虚拟地址
         * @param size 要取消映射的内存页大小
         * @param mode 内存页模式
         * @param pmlx_t 页表地址
         */
        auto __unmap( IN uint64_t virtual_address, IN size_t size, IN MemoryPageType mode, IN pmlxt &pmlx_t = *__paging::__kernel_page_table ) -> VOID;
    };
}