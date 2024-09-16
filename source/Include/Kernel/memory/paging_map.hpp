#pragma once
#include <Arch/Arch.hpp>
#include <Lib/Uefi.hpp>
#include <Kernel/print.hpp>
#include <Kernel/memory/pmlxt/pml1t.hpp>
#include <Kernel/memory/pmlxt/pml2t.hpp>
#include <Kernel/memory/pmlxt/pml3t.hpp>
#include <Kernel/memory/pmlxt/pml4t.hpp>
#include <Kernel/memory/pmlxt/pml5t.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC constexpr CONST uint64_t USER_STACK_VIRTUAL_ADDRESS_TOP { 0x00007fffffffffff };
    PUBLIC class PagingMap
    {
    public:
        explicit PagingMap( VOID ) noexcept;

    public:
        virtual ~PagingMap( VOID ) noexcept = default;

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
        auto map( IN uint64_t physics_address, IN uint64_t virtual_address, IN uint64_t size, IN uint64_t flags, IN MemoryPageType mode, IN pmlxt &pmlx_t = *kernel_page_table ) -> VOID;
        /**
         * @brief 取消映射页
         * @param virtual_address 将取消映射的指定虚拟地址
         * @param size 要取消映射的内存页大小
         * @param mode 内存页模式
         * @param pmlx_t 页表地址
         */
        auto unmap( IN uint64_t virtual_address, IN size_t size, IN MemoryPageType mode, IN pmlxt &pmlx_t = *kernel_page_table ) -> VOID;

        /**
         * @brief 对于map的一种处理
         */
        template < MemoryPageType type >
        auto map( IN uint64_t physics_address, IN uint64_t virtual_address, IN uint64_t size, IN uint64_t flags, IN pmlxt &pmlx_t = *kernel_page_table ) {
            using enum MemoryPageType;
            if constexpr ( type == PAGE_2M ) {
                // 内核主体以2m分页为主
                this->map( physics_address, virtual_address, size, flags, PAGE_2M, pmlx_t );
            }
            else if constexpr ( type == PAGE_4K ) {
                // 把一个2M拆开分成512个4K
                // 把一个指向一块2M内存的base改成指向一级页表的地址
                this->unmap( virtual_address, size, PAGE_2M, pmlx_t );
                this->map( physics_address, virtual_address, size, flags, PAGE_4K, pmlx_t );
            }
            else if constexpr ( type == PAGE_1G ) {
                // 把512个2M合并
                // 把指向2级页表的base改成指向一块1G内存地址
                this->unmap( virtual_address, size, PAGE_1G, pmlx_t );
                this->map( physics_address, virtual_address, size, flags, PAGE_1G, pmlx_t );
            }
        }
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
        auto make( IN uint64_t flags, IN Level level, IN MemoryPageType mode, IN pmlxt &pmlx_t = *kernel_page_table ) -> pmlxt &;
        /**
         * @brief 激活页表
         * @param pmlx_t 页表地址
         */
        auto activate( IN pmlxt &pmlx_t ) -> VOID;
        /**
         * @brief 根据页表和虚拟地址找出物理地址
         * @param address 虚拟地址
         * @param mode 内存页模式
         * @param pmlx_t 页表地址
         */
        auto VTP_address_from( IN VOID *virtual_address, IN MemoryPageType mode, IN pmlxt &pmlx_t = *kernel_page_table ) -> VOID *;

        /**
         * @brief 复制页表
         */
        auto copy( OUT pmlxt &to, IN pmlxt &from = *kernel_page_table ) -> VOID;

    private:
        pml1t kernel_l1_page_table;
        pml2t kernel_l2_page_table;
        pml3t kernel_l3_page_table;
        pml4t kernel_l4_page_table;
        pml5t kernel_l5_page_table;
        BOOL support_5level_paging;

    public:
        inline STATIC pmlxt *kernel_page_table { };
    };
}
