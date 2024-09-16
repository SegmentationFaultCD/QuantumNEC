#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
#include <Lib/IO/Stream/iostream>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC constexpr CONST auto PML_IDX { 512 };
    PUBLIC constexpr CONST auto PML_SIZE { PML_IDX * 2 };
    PUBLIC constexpr CONST auto PT_SIZE { 0x1000 };
    PUBLIC constexpr CONST auto KERNEL_PHYSICAL_ADDRESS { 0x100000ULL };
    PUBLIC constexpr CONST auto KERNEL_VIRTUAL_ADDRESS { 0xffff800000100000 };
    PUBLIC inline Lib::Types::size_t KERNEL_VRAM_PHYSICAL_ADDRESS;
    PUBLIC constexpr CONST auto KERNEL_VRAM_VIRTUAL_ADDRESS { 0xffff800000600000 };
    PUBLIC constexpr CONST auto KERNEL_FONT_MEMORY_PHYSICAL_ADDRESS { 0x0400000 };
    PUBLIC constexpr CONST auto KERNEL_FONT_MEMORY_VIRTUAL_ADDRESS { 0xffff800000400000 };
    PUBLIC inline Lib::Types::size_t KERNEL_FREE_MEMORY_PHYSICAL_ADDRESS;
    PUBLIC inline Lib::Types::size_t KERNEL_FREE_MEMORY_VIRTUAL_ADDRESS;
    PUBLIC constexpr CONST auto KERNEL_TASK_PCB_PHYSICAL_ADDRESS { 0x0200000ull };
    PUBLIC constexpr CONST auto KERNEL_TASK_PCB_VIRTUAL_ADDRESS { 0xffff800000200000 };
    PUBLIC constexpr CONST auto KERNEL_PAGE_TABLE_PHYSICAL_ADDRESS { 0x2500000ull };
    PUBLIC constexpr CONST auto KERNEL_PAGE_TABLE_VIRTUAL_ADDRESS { 0xffff800002500000 };
    PUBLIC inline Lib::Types::size_t KERNEL_I_O_APIC_PHYSICAL_ADDRESS;
    PUBLIC constexpr CONST auto KERNEL_I_O_APIC_VIRTUAL_ADDRESS { 0xffff800002410000 };
    PUBLIC inline Lib::Types::size_t KERNEL_LOCAL_APIC_PHYSICAL_ADDRESS;
    PUBLIC constexpr CONST auto KERNEL_LOCAL_APIC_VIRTUAL_ADDRESS { 0xffff800002411000 };
    /*
     * 内核层 ： 00000000 00000000 -> FFFFFFFF FFFFFFFF
     * 应用层 ： 00000000 02A00000 -> 00007FFF FFFFFFFF
     */
    PUBLIC constexpr CONST auto KERNEL_STACK_VIRTUAL_START_ADDRESS { 0x0000000000000000ULL };
    PUBLIC constexpr CONST auto KERNEL_STACK_VIRTUAL_END_ADDRESS { 0xffffffffffffffffULL };
    PUBLIC constexpr CONST auto USER_STACK_VIRTUAL_START_ADDRESS { 0x0000000002A00000ULL };
    PUBLIC constexpr CONST auto USER_STACK_VIRTUAL_END_ADDRESS { 0x00007FFFFFFFFFFFULL };
    PUBLIC enum class MemoryPageType : Lib::Types::uint64_t {
        PAGE_4K = 65536,
        PAGE_2M = 128,
        PAGE_1G = 1,
    };
    PUBLIC struct pml_t
    {
        Lib::Types::Ptr< Lib::Types::uint64_t > pml;
        auto make_mplt( IN Lib::Types::Ptr< Lib::Types::uint64_t > address, IN Lib::Types::uint64_t attribute ) {
            *this->pml = ( reinterpret_cast< Lib::Types::uint64_t >( address ) | attribute );
            return pml;
        }
        auto set_mplt( IN Lib::Types::Ptr< Lib::Types::uint64_t > mplval ) {
            this->pml = mplval;
            return;
        }
    };
    PUBLIC struct pdpt_t
    {
        Lib::Types::Ptr< Lib::Types::uint64_t > pdpt;
        auto make_pdpt( IN Lib::Types::Ptr< Lib::Types::uint64_t > address, IN Lib::Types::uint64_t attribute ) {
            *this->pdpt = ( reinterpret_cast< Lib::Types::uint64_t >( address ) | attribute );
            return this->pdpt;
        }
        auto set_pdpt( IN Lib::Types::Ptr< Lib::Types::uint64_t > pdptval ) {
            this->pdpt = pdptval;
            return;
        }
    };
    PUBLIC struct pdt_t
    {
        Lib::Types::Ptr< Lib::Types::uint64_t > pdt;
        auto make_pdt( IN Lib::Types::Ptr< Lib::Types::uint64_t > address, IN Lib::Types::uint64_t attribute ) {
            *this->pdt = ( reinterpret_cast< Lib::Types::uint64_t >( address ) | attribute );
            return this->pdt;
        }
        auto set_pdt( IN Lib::Types::Ptr< Lib::Types::uint64_t > pdtval ) {
            this->pdt = pdtval;
            return;
        }
    };
    PUBLIC struct pt_t
    {
        Lib::Types::Ptr< Lib::Types::uint64_t > pt;
        auto make_pt( IN Lib::Types::Ptr< Lib::Types::uint64_t > address, IN Lib::Types::uint64_t attribute ) {
            *this->pt = ( reinterpret_cast< Lib::Types::uint64_t >( address ) | attribute );
            return this->pt;
        }
        auto set_pt( IN Lib::Types::Ptr< Lib::Types::uint64_t > ptval ) {
            this->pt = ptval;
            return;
        }
    };
    PUBLIC constexpr CONST auto PAGE_XD { 1UL << 63 };
    PUBLIC constexpr CONST auto PAGE_PAT { 1UL << 7 };
    PUBLIC constexpr CONST auto PAGE_GLOBAL { 1UL << 8 };
    PUBLIC constexpr CONST auto PAGE_PS { 1UL << 7 };
    PUBLIC constexpr CONST auto PAGE_DIRTY { 1UL << 6 };
    PUBLIC constexpr CONST auto PAGE_ACCESSED { 1UL << 5 };
    PUBLIC constexpr CONST auto PAGE_PCD { 1UL << 4 };
    PUBLIC constexpr CONST auto PAGE_PWT { 1UL << 3 };
    PUBLIC constexpr CONST auto PAGE_PRESENT { 1UL << 0 };
    PUBLIC constexpr CONST auto PAGE_RW_W { 1ull << 1 };
    PUBLIC constexpr CONST auto PAGE_US_S { 1UL << 0 };
    PUBLIC constexpr CONST auto PAGE_US_U { 1ull << 2 };
    PUBLIC constexpr CONST auto PAGE_RW_R { 1UL << 0 };

    PUBLIC class Table
    {
    private:
        Lib::Types::uint64_t present : 1;
        Lib::Types::uint64_t read_write : 1;
        Lib::Types::uint64_t user_supervisor : 1;
        Lib::Types::uint64_t write_through : 1;
        Lib::Types::uint64_t cache_disable : 1;
        Lib::Types::uint64_t accessed : 1;
        Lib::Types::uint64_t dirty : 1;
        Lib::Types::uint64_t page_attribute_table_page_size : 1;
        Lib::Types::uint64_t global : 1;
        Lib::Types::uint64_t available : 3;
        Lib::Types::uint64_t address : 40;
        Lib::Types::uint64_t reserved : 7;
        Lib::Types::uint64_t protection_key : 4;
        Lib::Types::uint64_t execute_disable : 1;

    public:
        constexpr auto flags_p( VOID ) {
            return 1ull << 0;
        }
        constexpr auto flags_rw( VOID ) {
            return 1ull << 1;
        }
        constexpr auto flags_us( VOID ) {
            return 1ull << 2;
        }
        constexpr auto flags_pwt( VOID ) {
            return 1ull << 3;
        }
        constexpr auto flags_pcd( VOID ) {
            return 1ull << 4;
        }
        constexpr auto flags_a( VOID ) {
            return 1ull << 5;
        }
        constexpr auto flags_d( VOID ) {
            return 1ull << 6;
        }
        constexpr auto flags_pat_ps( VOID ) {
            return 1ull << 7;
        }
        constexpr auto flags_g( VOID ) {
            return 1ull << 9;
        }
        constexpr auto flags_avl( VOID ) {
            return 1ull << 8;
        }
        constexpr auto flags_xd( VOID ) {
            return 1ull << 63;
        }
        constexpr auto set_p( IN Lib::Types::uint64_t bit ) {
            this->present = bit;
        }
        constexpr auto set_rw( IN Lib::Types::uint64_t bit ) {
            this->read_write = bit;
        }
        constexpr auto set_us( IN Lib::Types::uint64_t bit ) {
            this->user_supervisor = bit;
        }
        constexpr auto set_pwt( IN Lib::Types::uint64_t bit ) {
            this->write_through = bit;
        }
        constexpr auto set_pcd( IN Lib::Types::uint64_t bit ) {
            this->cache_disable = bit;
        }
        constexpr auto set_a( IN Lib::Types::uint64_t bit ) {
            this->accessed = bit;
        }
        constexpr auto set_d( IN Lib::Types::uint64_t bit ) {
            this->dirty = bit;
        }
        constexpr auto set_ps_pat( IN Lib::Types::uint64_t bit ) {
            this->page_attribute_table_page_size = bit;
        }

        constexpr auto set_g( IN Lib::Types::uint64_t bit ) {
            this->global = bit;
        }
        constexpr auto set_avl( IN Lib::Types::uint64_t bit ) {
            this->available = bit;
        }

        constexpr auto set_pk( IN Lib::Types::uint64_t bit ) {
            this->protection_key = bit;
        }
        constexpr auto set_xd( IN Lib::Types::uint64_t bit ) {
            this->execute_disable = bit;
        }
        constexpr auto get_p( VOID ) {
            return this->present;
        }
        constexpr auto get_rw( VOID ) {
            return this->read_write;
        }
        constexpr auto get_us( VOID ) {
            return this->user_supervisor;
        }
        constexpr auto get_pwt( VOID ) {
            return this->write_through;
        }
        constexpr auto get_pcd( VOID ) {
            return this->cache_disable;
        }
        constexpr auto get_a( VOID ) {
            return this->accessed;
        }
        constexpr auto get_d( VOID ) {
            return this->dirty;
        }
        constexpr auto get_ps_pat( VOID ) {
            return this->page_attribute_table_page_size;
        }

        constexpr auto get_g( VOID ) {
            return this->global;
        }
        constexpr auto get_avl( VOID ) {
            return this->available;
        }

        constexpr auto get_pk( VOID ) {
            return this->protection_key;
        }
        constexpr auto get_xd( VOID ) {
            return this->execute_disable;
        }
        constexpr auto get_address( VOID ) {
            return this->address;
        }
        auto set_address( IN Lib::Types::uint64_t bit ) {
            *reinterpret_cast< Lib::Types::uint64_t * >( this ) |= bit;
        }

    public:
        auto set_table( IN Lib::Types::Ptr< VOID > address, IN Lib::Types::uint64_t flags ) -> VOID;
    };

    PUBLIC class PageDirectoryTable
    {
    public:
        explicit PageDirectoryTable( VOID ) noexcept = default;

    public:
        /**
         * @brief 设置512个页表到512个pd
         * @param flags 设置位
         * @param type 内存页内存
         */
        auto make( IN Lib::Types::uint64_t flags, IN MemoryPageType type ) noexcept -> VOID;

    private:
        Table page_directory_table[ 512 ];
    };
    PUBLIC class PageDirectoryPointerTable
    {
    public:
        explicit PageDirectoryPointerTable( VOID ) noexcept = default;

    public:
        /**
         * @brief 设置512个pd到512个pdp
         * @param flags 设置位
         * @param type 内存页内存
         */
        auto make( IN Lib::Types::uint64_t flags, IN MemoryPageType type ) noexcept -> VOID;

    private:
        Table page_directory_pointer_table[ 512 ];
        PageDirectoryTable page_dircetory_table[ 512 ];
    };
    /**
     * @brief 映射类型
     */
    enum class MapLevel {
        PML = 4,
        PDPT = 3,
        PD = 2,
        PT = 1,
        PG = 0,
    };
    PUBLIC class PageMapLevel4Table
    {
    public:
        explicit PageMapLevel4Table( VOID ) noexcept;
        explicit PageMapLevel4Table( IN PageMapLevel4Table *pml4_t ) noexcept;

    public:
        auto make( IN Lib::Types::uint64_t flags, IN MemoryPageType type ) noexcept -> VOID;

    public:
        /**
         * @brief 获取整个页表大小
         */
        constexpr auto size( VOID ) {
            return ( ( 4096 * 512 ) + 4096 ) * 512 + 4096;
        }
        /**
         * @brief 获取入口
         * @param level 获取的东西类型
         * @retval 入口地址
         */
        auto get_table_entry( IN Lib::Types::Ptr< VOID > address, IN MapLevel level ) -> Lib::Types::Ptr< Lib::Types::uint64_t >;
        /**
         * @brief 检查虚拟地址是否为规范格式vrt地址，如果无效，请进行调整
         * @param virtual_address 要检查的地址
         * @retval BOOL 状态
         */
        auto check( IN Lib::Types::Ptr< Lib::Types::uint64_t > virtual_address ) -> Lib::Types::BOOL;

    private:
        Table page_map_level4_table[ 512 ];
        PageDirectoryPointerTable page_directory_pointer_table[ 512 ];
    };

    PUBLIC class MemoryMap
    {
    private:
    public:
        /**
         * @brief 内存页模式
         */
        enum class MapMode {
            MEMORY_MAP_4K = 0x1,      // PTE
            MEMORY_MAP_2M = 0x2,      // PDE
            MEMORY_MAP_1G = 0x3,      // PDPTE
            MEMORY_MAP_END = 0x4,     // End
        };

    public:
        explicit MemoryMap( IN Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept;

    public:
        virtual ~MemoryMap( VOID ) noexcept = default;

    public:
        /**
         * @brief 映射页
         * @param physics_address 要映射的物理地址
         * @param virtual_address 将物理地址映射的指定虚拟地址
         * @param size 要映射的内存页大小
         * @param flags 映射标志
         * @param mode 映射页的模式 (4K/2M/1G)
         */
        auto map( IN Lib::Types::uint64_t physics_address, IN Lib::Types::uint64_t virtual_address, IN Lib::Types::size_t size, IN Lib::Types::uint16_t flags, IN MapMode mode, IN Lib::Types::Ptr< PageMapLevel4Table > pml = kernel_page_table ) -> VOID;
        /**
         * @brief 取消映射页
         * @param virtual_address 将取消映射的指定虚拟地址
         * @param size 要取消映射的内存页大小
         */
        auto unmap( IN Lib::Types::uint64_t virtual_address, IN Lib::Types::size_t size, IN Lib::Types::Ptr< PageMapLevel4Table > pml = kernel_page_table ) -> VOID;

    public:
        /**
         * @brief 控制页保护开关
         * @param flags 如果为true那么开启页保护，如果为false那么关闭页保护
         */
        auto page_table_protect( IN Lib::Types::BOOL flags ) -> VOID;
        /**
         * @brief 制作页表
         * @return 制作的页目录地址
         */
        auto make_page_table( VOID ) -> Lib::Types::Ptr< PageMapLevel4Table >;
        /**
         * @brief 激活页表
         * @param page_directory 页表地址
         */
        auto activate_page_table( IN Lib::Types::Ptr< VOID > page_directory_table_address ) -> VOID;
        /**
         * @brief 获取当前页表
         */
        auto get_current_page_tabel( VOID ) -> Lib::Types::Ptr< Lib::Types::uint64_t >;
        /**
         * @brief 获取规范地址掩码
         */
        auto get_canonical_address_mask( VOID ) -> Lib::Types::uint64_t;

        inline STATIC PageMapLevel4Table *kernel_page_table { };
    };
}
