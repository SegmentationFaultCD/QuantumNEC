#pragma once
#include <Lib/Uefi.hpp>
#include <Kernel/memory/paging/paging.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class __page_table
    {
    public:
        explicit __page_table( VOID ) noexcept = default;
        virtual ~__page_table( VOID ) noexcept = default;

    public:
        /**
         * @brief 控制页保护开关
         * @param flags 如果为true那么开启页保护，如果为false那么关闭页保护
         */
        auto __page_protect( IN BOOL flags ) -> VOID;
        /**
         * @brief 制作页表
         * @param flags 页属性
         * @param level 页表等级（5/4/3/2/1）
         * @param mode 内存页模式
         * @return 制作的页目录地址
         */
        auto __make( IN uint64_t flags, IN Level level, IN MemoryPageType mode, IN pmlxt &pmlx_t = *__paging::__kernel_page_table ) -> pmlxt &;
        /**
         * @brief 激活页表
         * @param pmlx_t 页表地址
         */
        auto __activate( IN pmlxt &pmlx_t ) -> VOID;
        /**
         * @brief 根据页表和虚拟地址找出物理地址
         * @param address 虚拟地址
         * @param mode 内存页模式
         * @param pmlx_t 页表地址
         */
        auto __VTP_address_from( IN VOID *virtual_address, IN MemoryPageType mode, IN pmlxt &pmlx_t = *__paging::__kernel_page_table ) -> VOID *;
        /**
         * @brief 复制页表
         */
        auto __copy( OUT pmlxt &to, IN pmlxt &from = *__paging::__kernel_page_table ) -> VOID;
    };
}