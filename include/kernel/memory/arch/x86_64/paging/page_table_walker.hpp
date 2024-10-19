#pragma once
#include <lib/Uefi.hpp>
#include <kernel/memory/page/page_manager.hpp>
#include <kernel/memory/arch/x86_64/paging/paging.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    PUBLIC class PageTableWalker
    {
    public:
        explicit PageTableWalker( VOID ) = default;
        virtual ~PageTableWalker( VOID ) = default;

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
        auto map( IN uint64_t physics_address, IN uint64_t virtual_address, IN uint64_t size, IN uint64_t flags, IN MemoryPageType mode, IN pmlxt &pmlx_t = *Paging::__kernel_page_table ) -> VOID;
        /**
         * @brief 取消映射页
         * @param virtual_address 将取消映射的指定虚拟地址
         * @param size 要取消映射的内存页大小
         * @param mode 内存页模式
         * @param pmlx_t 页表地址
         */
        auto unmap( IN uint64_t virtual_address, IN size_t size, IN MemoryPageType mode, IN pmlxt &pmlx_t = *Paging::__kernel_page_table ) -> VOID;
        /**
         * @brief 控制页保护开关
         * @param flags 如果为true那么开启页保护，如果为false那么关闭页保护
         */
        auto page_protect( IN BOOL flags ) -> VOID;
        /**
         * @brief 制作页表
         * @param flags 页属性
         * @param level 页表等级（5/4/3/2/1）
         * @param mode 内存页模式
         * @return 制作的页目录地址
         */
        auto make( IN uint64_t flags, IN Level level, IN MemoryPageType mode, IN pmlxt &pmlx_t = *Paging::__kernel_page_table ) -> pmlxt &;
        /**
         * @brief 激活页表
         * @param pmlx_t 页表地址
         */
        auto activate( IN pmlxt &pmlx_t ) -> VOID;
        /**
         * @brief 根据页表和虚拟地址找出物理地址
         * @param virtual_address 虚拟地址
         * @param mode 内存页模式
         * @param pmlx_t 页表地址
         */
        auto VTP_from( IN VOID *virtual_address, IN MemoryPageType mode, IN pmlxt &pmlx_t = *Paging::__kernel_page_table ) -> VOID *;
        /**
         * @brief 复制页表
         */
        auto copy( OUT pmlxt &to, IN pmlxt &from = *Paging::__kernel_page_table ) -> VOID;
    };
}