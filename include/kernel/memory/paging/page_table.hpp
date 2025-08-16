#pragma once
#include <cstdint>
#include <kernel/memory/allocator/page.hpp>
#include <limine.h>
namespace Memory {

class Paging {
public:
    class pmlxt {
    private:
        enum class Level {
            PML5 = 5,
            PML4 = 4,
            PDPT = 3,
            PD   = 2,
            PT   = 1,
        };

    public:
        constexpr static auto PT_SIZE { 0x1000 };

        constexpr static auto PAGE_SHIFT { 12 };

        /*
         * 内核层 ： 00000000 00000000 -> FFFFFFFF FFFFFFFF
         * 应用层 ： 00000000 02A00000 -> 00007FFF FFFFFFFF
         */

        constexpr static auto PAGE_XD { 1UL << 63 };
        constexpr static auto PAGE_PAT { 1UL << 7 };
        constexpr static auto PAGE_GLOBAL { 1UL << 8 };
        constexpr static auto PAGE_PS { 1UL << 7 };
        constexpr static auto PAGE_DIRTY { 1UL << 6 };
        constexpr static auto PAGE_ACCESSED { 1UL << 5 };
        constexpr static auto PAGE_PCD { 1UL << 4 };
        constexpr static auto PAGE_PWT { 1UL << 3 };
        constexpr static auto PAGE_PRESENT { 1UL << 0 };
        constexpr static auto PAGE_RW_W { 1ull << 1 };
        constexpr static auto PAGE_US_S { 0UL << 0 };
        constexpr static auto PAGE_US_U { 1ull << 2 };
        constexpr static auto PAGE_RW_R { 0UL << 0 };

        constexpr static auto USER_STACK_VIRTUAL_ADDRESS_TOP { 0x00007fffffffffffUL };

    public:
        explicit pmlxt( void ) noexcept;
        explicit pmlxt( uint64_t *_pmlx_table ) noexcept :
            pmlx_table { _pmlx_table }, can_allocate { false } {
        }
        virtual ~pmlxt( void ) noexcept;

    private:
        bool can_allocate;

    public:
        virtual auto flags_p( uint64_t index ) -> uint64_t                                  = 0;
        virtual auto flags_rw( uint64_t index ) -> uint64_t                                 = 0;
        virtual auto flags_us( uint64_t index ) -> uint64_t                                 = 0;
        virtual auto flags_pcd( uint64_t index ) -> uint64_t                                = 0;
        virtual auto flags_pwt( uint64_t index ) -> uint64_t                                = 0;
        virtual auto flags_a( uint64_t index ) -> uint64_t                                  = 0;
        virtual auto flags_xd( uint64_t index ) -> uint64_t                                 = 0;
        virtual auto flags_base( uint64_t index, Page::Type mode ) -> uint64_t              = 0;
        virtual auto flags_ps_pat( uint64_t index ) -> uint64_t                             = 0;
        virtual auto set_p( uint64_t index, bool bit ) -> void                              = 0;
        virtual auto set_rw( uint64_t index, bool bit ) -> void                             = 0;
        virtual auto set_us( uint64_t index, bool bit ) -> void                             = 0;
        virtual auto set_pcd( uint64_t index, bool bit ) -> void                            = 0;
        virtual auto set_pwt( uint64_t index, bool bit ) -> void                            = 0;
        virtual auto set_a( uint64_t index, bool bit ) -> void                              = 0;
        virtual auto set_xd( uint64_t index, bool bit ) -> void                             = 0;
        virtual auto set_base( uint64_t index, uint64_t address, Page::Type _mode ) -> void = 0;
        virtual auto set_ps_pat( uint64_t index, bool bit ) -> void                         = 0;

    public:
        /**
         * @brief 检查虚拟地址是否为规范格式vrt地址，如果无效，请进行调整
         * @param virtual_address 要检查的地址
         * @retval BOOL 状态
         */
        auto check_address( uint64_t *virtual_address ) {
            if ( ( *virtual_address >> 47 ) & 1 ) {
                if ( *virtual_address >> 48 == 0xFFFF ) {
                    return true;
                }
                *virtual_address |= 0xFFFFULL << 48;
                return false;
            }
            return true;
        }
        auto check_page_size( Page::Type mode ) {
            using enum Page::Type;
            using namespace Page;
            switch ( mode ) {
            case P2Mib:
                return 2_MB;
            case P4Kib:
                return 4_KB;
            case P1Gib:
                return 1_GB;
            default:
                return 0ull;
            }
        }
        auto is_huge( Page::Type mode ) {
            if ( mode == Page::Type::P2Mib || mode == Page::Type::P1Gib )
                return PAGE_PS;
            return 0ul;
        };

        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t, Page::Type > group ) -> void = 0;
        virtual auto get_address_index_in( void *virtual_address ) -> uint64_t                         = 0;
        virtual auto get_table( void ) -> uint64_t *                                                   = 0;

    public:
        auto operator=( uint64_t table_address ) -> void {
            this->pmlx_table   = (uint64_t *)table_address;
            this->can_allocate = false;
        }

    protected:
        auto operator=( auto entry ) -> pmlxt &
            requires std::is_pointer_v< decltype( entry ) >
        {
            this->pmlx_table   = (uint64_t *)entry;
            this->can_allocate = false;
            return *this;
        }

    protected:
        uint64_t *pmlx_table;

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
        auto map( uint64_t physics_address, uint64_t virtual_address, uint64_t size, uint64_t flags, Page::Type mode ) -> void;
        /**
         * @brief 取消映射页
         * @param virtual_address 将取消映射的指定虚拟地址
         * @param size 要取消映射的内存页大小
         * @param mode 内存页模式
         * @param pmlx_t 页表地址
         */
        auto unmap( uint64_t virtual_address, std::size_t size, Page::Type mode ) -> void;
        /**
         * @brief 控制页保护开关
         * @param flags 如果为true那么开启页保护，如果为false那么关闭页保护
         */
        auto page_protect( bool flags ) -> void;

        /**
         * @brief 激活页表
         * @param pmlx_t 页表地址
         */
        auto activate( void ) -> void;
        /**
         * @brief 根据页表和虚拟地址找出物理地址
         * @param virtual_address 虚拟地址
         * @param mode 内存页模式
         * @param pmlx_t 页表地址
         */
        auto find_physcial_address( void *virtual_address, Page::Type mode ) -> void *;
        /**
         * @brief 复制页表
         */
        auto copy( pmlxt & ) -> void;
    };
    class pml1t : public pmlxt {
        friend pmlxt;

    private:
        struct pml1t_entry {
            uint64_t p : 1;     // 页是否存在，1为在，0为不在
            uint64_t rw : 1;
            uint64_t us : 1;
            uint64_t pwt : 1;
            uint64_t pcd : 1;
            uint64_t a : 1;
            uint64_t d : 1;
            uint64_t pat : 1;
            uint64_t g : 1;
            uint64_t avl : 3;
            uint64_t base : 40;
            uint64_t : 7;
            uint64_t pk : 4;
            uint64_t xd : 1;
            explicit pml1t_entry( void ) noexcept = default;
        };

    public:
        using page_table_entry      = pml1t_entry;
        using huge_page_table_entry = void;

    public:
    public:
        explicit pml1t( page_table_entry *pml1t_address ) noexcept :
            pmlxt { (uint64_t *)( pml1t_address ) } {
        }
        explicit pml1t( pml1t &pml1t ) noexcept :
            pmlxt { pml1t.pmlx_table } {
        }
        explicit pml1t( void ) noexcept :
            pmlxt { } {
        }
        virtual ~pml1t( void ) noexcept {
        }

    public:
        virtual auto flags_p( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].p;
        }
        virtual auto flags_rw( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].rw;
        }
        virtual auto flags_us( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].us;
        }
        virtual auto flags_pcd( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].pcd;
        }
        virtual auto flags_pwt( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].pwt;
        }
        virtual auto flags_a( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].a;
        }
        virtual auto flags_xd( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].xd;
        }
        virtual auto flags_base( uint64_t index, [[maybe_unused]] Page::Type mode ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].base << this->PAGE_SHIFT;
        }
        virtual auto flags_ps_pat( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].pat;
        }
        virtual auto set_p( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].p = bit;
        }
        virtual auto set_rw( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].rw = bit;
        }
        virtual auto set_us( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].us = bit;
        }
        virtual auto set_pcd( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].pcd = bit;
        }
        virtual auto set_pwt( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].pwt = bit;
        }
        virtual auto set_a( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].a = bit;
        }
        virtual auto set_xd( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].xd = bit;
        }
        virtual auto set_base( uint64_t index, uint64_t address, [[maybe_unused]] Page::Type mode ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].base = address >> PAGE_SHIFT;
        }
        virtual auto set_ps_pat( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].pat = bit;
        }
        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t, Page::Type > group ) -> void override {
            auto &[ index, base, flags, _mode ] = group;
            this->set_p( index, !!( flags & PAGE_PRESENT ) );
            this->set_rw( index, !!( flags & PAGE_RW_W ) );
            this->set_us( index, !!( flags & PAGE_US_U ) );
            this->set_pwt( index, !!( flags & PAGE_PWT ) );
            this->set_pcd( index, !!( flags & PAGE_PCD ) );
            this->set_a( index, !!( flags & PAGE_ACCESSED ) );
            this->set_xd( index, !!( flags & PAGE_XD ) );
            this->set_base( index, base, _mode );
            this->set_ps_pat( index, 0 );
        }
        auto operator=( pml1t_entry *entry ) -> pml1t & {
            this->pmlxt::operator=( entry );
            return *this;
        }

        virtual auto get_table( void ) -> uint64_t * override {
            return this->pmlx_table;
        }
        virtual auto get_address_index_in( void *address ) -> uint64_t override {
            return ( ( (uint64_t)address >> PAGE_SHIFT ) & 0x1ff );
        }
    };
    class pml2t : public pmlxt {
        friend pmlxt;
        constexpr static auto PAGE_HUGE_SHIFT { 21 };

    private:
        struct pml2t_entry {
            uint64_t p : 1;     // 页是否存在，1为在，0为不在
            uint64_t rw : 1;
            uint64_t us : 1;
            uint64_t pwt : 1;
            uint64_t pcd : 1;
            uint64_t a : 1;
            uint64_t d : 1;
            uint64_t ps : 1;
            uint64_t g : 1;
            uint64_t avl : 3;
            uint64_t base : 40;
            uint64_t : 7;
            uint64_t pk : 4;
            uint64_t xd : 1;
            explicit pml2t_entry( void ) noexcept = default;
        };
        struct pml2t_huge_entry {
            uint64_t p : 1;     // 页是否存在，1为在，0为不在
            uint64_t rw : 1;
            uint64_t us : 1;
            uint64_t pwt : 1;
            uint64_t pcd : 1;
            uint64_t a : 1;
            uint64_t d : 1;
            uint64_t ps : 1;
            uint64_t g : 1;
            uint64_t avl : 3;
            uint64_t pat : 1;
            uint64_t : 8;
            uint64_t base : 31;
            uint64_t : 7;
            uint64_t pk : 4;
            uint64_t xd : 1;
            explicit pml2t_huge_entry( void ) noexcept = default;
        };

    public:
        using page_table_entry      = pml2t_entry;
        using huge_page_table_entry = pml2t_huge_entry;

        // 三级页表有两种情况
        // 一种是当分页模式为2M时继续找二级页表
        // 一种是当分页模式为1G时放弃二级页表，直接从三级页表把内存基地址写入
    public:
        explicit pml2t( page_table_entry *pml2t_address ) noexcept :
            pmlxt { (uint64_t *)( pml2t_address ) } {
        }
        explicit pml2t( pml2t &pml2t ) noexcept :
            pmlxt { pml2t.pmlx_table } {
        }
        explicit pml2t( void ) noexcept :
            pmlxt { } {
        }
        virtual ~pml2t( void ) noexcept {
        }

    public:
        virtual auto flags_p( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].p;
        }
        virtual auto flags_rw( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].rw;
        }

        virtual auto flags_us( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].us;
        }

        virtual auto flags_pcd( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].pcd;
        }

        virtual auto flags_pwt( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].pwt;
        }

        virtual auto flags_a( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].a;
        }

        virtual auto flags_xd( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].xd;
        }
        virtual auto flags_base( uint64_t index, Page::Type _mode ) -> uint64_t override {
            if ( _mode == Page::Type::P2Mib ) {
                return ( (huge_page_table_entry *)this->pmlx_table )[ index ].base << PAGE_HUGE_SHIFT;
            }
            else {
                return ( (page_table_entry *)this->pmlx_table )[ index ].base << PAGE_SHIFT;
            }
        }
        virtual auto flags_ps_pat( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].ps;
        }
        virtual auto set_p( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].p = bit;
        }

        virtual auto set_rw( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].rw = bit;
        }

        virtual auto set_us( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].us = bit;
        }

        virtual auto set_pcd( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].pcd = bit;
        }

        virtual auto set_pwt( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].pwt = bit;
        }
        virtual auto set_a( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].a = bit;
        }

        virtual auto set_xd( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].xd = bit;
        }

        virtual auto set_base( uint64_t index, uint64_t address, [[maybe_unused]] Page::Type mode ) -> void override {
            if ( mode == Page::Type::P2Mib ) {
                ( (huge_page_table_entry *)this->pmlx_table )[ index ].base = address >> PAGE_HUGE_SHIFT;
            }
            else {
                ( (page_table_entry *)this->pmlx_table )[ index ].base = address >> PAGE_SHIFT;
            }
        }
        virtual auto set_ps_pat( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].ps = bit;
        }

        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t, Page::Type > group ) -> void override {
            auto &[ index, base, flags, _mode ] = group;
            this->set_p( index, !!( flags & PAGE_PRESENT ) );
            this->set_rw( index, !!( flags & PAGE_RW_W ) );
            this->set_us( index, !!( flags & PAGE_US_U ) );
            this->set_pwt( index, !!( flags & PAGE_PWT ) );
            this->set_pcd( index, !!( flags & PAGE_PCD ) );
            this->set_a( index, !!( flags & PAGE_ACCESSED ) );
            this->set_xd( index, !!( flags & PAGE_XD ) );
            this->set_base( index, base, _mode );
            this->set_ps_pat( index, !!( flags & PAGE_PS ) );
        }
        auto operator=( pml2t_entry *entry ) -> pml2t & {
            this->pmlxt::operator=( entry );
            return *this;
        }
        virtual auto get_table( void ) -> uint64_t * override {
            return this->pmlx_table;
        }
        virtual auto get_address_index_in( void *address ) -> uint64_t override {
            return ( ( (uint64_t)address >> PAGE_HUGE_SHIFT ) & 0x1ff );
        }
    };
    class pml3t : public pmlxt {
        friend pmlxt;
        constexpr static auto PAGE_HUGE_SHIFT { 30 };

    private:
        struct pml3t_entry {
            uint64_t p : 1;     // 页是否存在，1为在，0为不在
            uint64_t rw : 1;
            uint64_t us : 1;
            uint64_t pwt : 1;
            uint64_t pcd : 1;
            uint64_t a : 1;
            uint64_t d : 1;
            uint64_t ps : 1;
            uint64_t g : 1;
            uint64_t avl : 3;
            uint64_t base : 40;
            uint64_t : 7;
            uint64_t pk : 4;
            uint64_t xd : 1;
            explicit pml3t_entry( void ) noexcept = default;
        };
        struct pml3t_huge_entry {
            uint64_t p : 1;     // 页是否存在，1为在，0为不在
            uint64_t rw : 1;
            uint64_t us : 1;
            uint64_t pwt : 1;
            uint64_t pcd : 1;
            uint64_t a : 1;
            uint64_t d : 1;
            uint64_t ps : 1;
            uint64_t g : 1;
            uint64_t avl : 3;
            uint64_t pat : 1;
            uint64_t : 17;
            uint64_t base : 31;
            uint64_t : 7;
            uint64_t pk : 4;
            uint64_t xd : 1;
            explicit pml3t_huge_entry( void ) noexcept = default;
        };

    public:
        using page_table_entry      = pml3t_entry;
        using huge_page_table_entry = pml3t_huge_entry;
        // 三级页表有两种情况
        // 一种是当分页模式为2M时继续找二级页表
        // 一种是当分页模式为1G时放弃二级页表，直接从三级页表把内存基地址写入
    public:
        explicit pml3t( page_table_entry *pml3t_address ) noexcept :
            pmlxt { (uint64_t *)( pml3t_address ) } {
        }
        explicit pml3t( pml3t &pml3t ) noexcept :
            pmlxt { pml3t.pmlx_table } {
        }
        explicit pml3t( void ) noexcept :
            pmlxt { } {
        }
        virtual ~pml3t( void ) noexcept {
        }

    public:
        virtual auto flags_p( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].p;
        }
        virtual auto flags_rw( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].rw;
        }

        virtual auto flags_us( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].us;
        }

        virtual auto flags_pcd( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].pcd;
        }

        virtual auto flags_pwt( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].pwt;
        }

        virtual auto flags_a( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].a;
        }
        virtual auto flags_ps_pat( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].ps;
        }
        virtual auto flags_xd( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].xd;
        }
        virtual auto flags_base( uint64_t index, Page::Type _mode ) -> uint64_t override {
            if ( _mode == Page::Type::P1Gib ) {
                return ( (huge_page_table_entry *)this->pmlx_table )[ index ].base << PAGE_HUGE_SHIFT;
            }
            else {
                return ( (page_table_entry *)this->pmlx_table )[ index ].base << PAGE_SHIFT;
            }
        }

        virtual auto set_p( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].p = bit;
        }

        virtual auto set_rw( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].rw = bit;
        }

        virtual auto set_us( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].us = bit;
        }

        virtual auto set_pcd( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].pcd = bit;
        }

        virtual auto set_pwt( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].pwt = bit;
        }
        virtual auto set_a( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].a = bit;
        }

        virtual auto set_xd( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].xd = bit;
        }
        virtual auto set_ps_pat( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].ps = bit;
        }

        virtual auto set_base( uint64_t index, uint64_t address, Page::Type _mode ) -> void override {
            if ( _mode == Page::Type::P1Gib ) {
                ( (huge_page_table_entry *)this->pmlx_table )[ index ].base = address >> PAGE_HUGE_SHIFT;
            }
            else {
                ( (page_table_entry *)this->pmlx_table )[ index ].base = address >> PAGE_SHIFT;
            }
        }
        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t, Page::Type > group ) -> void override {
            auto &[ index, base, flags, mode ] = group;
            this->set_p( index, !!( flags & PAGE_PRESENT ) );
            this->set_rw( index, !!( flags & PAGE_RW_W ) );
            this->set_us( index, !!( flags & PAGE_US_U ) );
            this->set_pwt( index, !!( flags & PAGE_PWT ) );
            this->set_pcd( index, !!( flags & PAGE_PCD ) );
            this->set_a( index, !!( flags & PAGE_ACCESSED ) );
            this->set_xd( index, !!( flags & PAGE_XD ) );
            this->set_base( index, base, mode );
            this->set_ps_pat( index, !!( flags & PAGE_PS ) );
        }
        auto operator=( pml3t_entry *entry ) -> pml3t & {
            this->pmlxt::operator=( entry );
            return *this;
        }
        virtual auto get_table( void ) -> uint64_t * override {
            return this->pmlx_table;
        }
        virtual auto get_address_index_in( void *address ) -> uint64_t override {
            return ( ( (uint64_t)address >> PAGE_HUGE_SHIFT ) & 0x1ff );
        }
    };
    class pml4t : public pmlxt {
        friend pmlxt;

    public:
        constexpr static auto PAGE_HUGE_SHIFT { 39 };

    private:
        struct pml4t_entry {
            uint64_t p : 1;     // 页是否存在，1为在，0为不在
            uint64_t rw : 1;
            uint64_t us : 1;
            uint64_t pwt : 1;
            uint64_t pcd : 1;
            uint64_t a : 1;
            uint64_t avl1 : 1;
            uint64_t : 1;
            uint64_t avl2 : 4;
            uint64_t base : 40;
            uint64_t avl3 : 11;
            uint64_t xd : 1;
            explicit pml4t_entry( void ) noexcept = default;
        };

    public:
        using page_table_entry      = pml4t_entry;
        using huge_page_table_entry = void;
        // 五级页表是最大的了
    public:
        explicit pml4t( page_table_entry *pml4t_address ) noexcept :
            pmlxt { (uint64_t *)( pml4t_address ) } {
        }
        explicit pml4t( pml4t &pml4t ) noexcept :
            pmlxt { pml4t.pmlx_table } {
        }
        explicit pml4t( void ) noexcept :
            pmlxt { } {
        }
        virtual ~pml4t( void ) noexcept {
        }

    public:
        virtual auto flags_p( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].p;
        }
        virtual auto flags_rw( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].rw;
        }
        virtual auto flags_us( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].us;
        }
        virtual auto flags_pcd( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].pcd;
        }
        virtual auto flags_pwt( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].pwt;
        }
        virtual auto flags_a( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].a;
        }
        virtual auto flags_xd( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].xd;
        }
        virtual auto flags_base( uint64_t index, [[maybe_unused]] Page::Type _mode ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].base << PAGE_SHIFT;
        }
        virtual auto flags_ps_pat( [[maybe_unused]] uint64_t index ) -> uint64_t override {
            return 0;
        }
        virtual auto set_p( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].p = bit;
        }
        virtual auto set_rw( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].rw = bit;
        }
        virtual auto set_us( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].us = bit;
        }
        virtual auto set_pcd( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].pcd = bit;
        }
        virtual auto set_pwt( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].pwt = bit;
        }
        virtual auto set_a( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].a = bit;
        }
        virtual auto set_xd( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].xd = bit;
        }
        virtual auto set_base( uint64_t index, uint64_t address, [[maybe_unused]] Page::Type _mode ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].base = address >> PAGE_SHIFT;
        }
        virtual auto set_ps_pat( [[maybe_unused]] uint64_t index, [[maybe_unused]] bool bit ) -> void override {
        }
        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t, Page::Type > group ) -> void override {
            auto &[ index, base, flags, _mode ] = group;
            this->set_p( index, !!( flags & PAGE_PRESENT ) );
            this->set_rw( index, !!( flags & PAGE_RW_W ) );
            this->set_us( index, !!( flags & PAGE_US_U ) );
            this->set_pwt( index, !!( flags & PAGE_PWT ) );
            this->set_pcd( index, !!( flags & PAGE_PCD ) );
            this->set_a( index, !!( flags & PAGE_ACCESSED ) );
            this->set_xd( index, !!( flags & PAGE_XD ) );
            this->set_base( index, base, _mode );
            this->set_ps_pat( index, !!( flags & PAGE_PAT ) );
        }
        auto operator=( pml4t_entry *entry ) -> pml4t & {
            this->pmlxt::operator=( entry );
            return *this;
        }
        virtual auto get_table( void ) -> uint64_t * override {
            return this->pmlx_table;
        }
        virtual auto get_address_index_in( void *address ) -> uint64_t override {
            return ( ( (uint64_t)address >> PAGE_HUGE_SHIFT ) & 0x1ff );
        }
    };
    class pml5t : public pmlxt {
        friend pmlxt;

    public:
        constexpr static auto PAGE_HUGE_SHIFT { 48 };

    private:
        struct pml5t_entry {
            uint64_t p : 1;     // 页是否存在，1为在，0为不在
            uint64_t rw : 1;
            uint64_t us : 1;
            uint64_t pwt : 1;
            uint64_t pcd : 1;
            uint64_t a : 1;
            uint64_t avl1 : 1;
            uint64_t : 1;
            uint64_t avl2 : 4;
            uint64_t base : 40;
            uint64_t avl3 : 11;
            uint64_t xd : 1;
            explicit pml5t_entry( void ) noexcept = default;
        };

    public:
        using page_table_entry      = pml5t_entry;
        using huge_page_table_entry = void;
        // 五级页表是最大的了
    public:
        explicit pml5t( page_table_entry *pml5t_address ) noexcept :
            pmlxt { (uint64_t *)( pml5t_address ) } {
        }
        explicit pml5t( pml5t &pml5t ) noexcept :
            pmlxt { pml5t.pmlx_table } {
        }
        explicit pml5t( void ) noexcept :
            pmlxt { } {
        }
        virtual ~pml5t( void ) noexcept {
        }

    public:
        virtual auto flags_p( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].p;
        }
        virtual auto flags_rw( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].rw;
        }
        virtual auto flags_us( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].us;
        }
        virtual auto flags_pcd( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].pcd;
        }
        virtual auto flags_pwt( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].pwt;
        }
        virtual auto flags_a( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].a;
        }
        virtual auto flags_xd( uint64_t index ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].xd;
        }
        virtual auto flags_ps_pat( [[maybe_unused]] uint64_t index ) -> uint64_t override {
            return 0;
        }
        virtual auto flags_base( uint64_t index, [[maybe_unused]] Page::Type _mode ) -> uint64_t override {
            return ( (page_table_entry *)this->pmlx_table )[ index ].base << PAGE_SHIFT;
        }
        virtual auto set_p( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].p = bit;
        }
        virtual auto set_rw( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].rw = bit;
        }
        virtual auto set_us( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].us = bit;
        }
        virtual auto set_pcd( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].pcd = bit;
        }
        virtual auto set_pwt( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].pwt = bit;
        }
        virtual auto set_a( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].a = bit;
        }
        virtual auto set_xd( uint64_t index, bool bit ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].xd = bit;
        }
        virtual auto set_base( uint64_t index, uint64_t address, [[maybe_unused]] Page::Type _mode ) -> void override {
            ( (page_table_entry *)this->pmlx_table )[ index ].base = address >> PAGE_SHIFT;
        }
        virtual auto set_ps_pat( [[maybe_unused]] uint64_t index, [[maybe_unused]] bool bit ) -> void override {
        }
        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t, Page::Type > group ) -> void override {
            auto &[ index, base, flags, _mode ] = group;
            this->set_p( index, !!( flags & PAGE_PRESENT ) );
            this->set_rw( index, !!( flags & PAGE_RW_W ) );
            this->set_us( index, !!( flags & PAGE_US_U ) );
            this->set_pwt( index, !!( flags & PAGE_PWT ) );
            this->set_pcd( index, !!( flags & PAGE_PCD ) );
            this->set_a( index, !!( flags & PAGE_ACCESSED ) );
            this->set_xd( index, !!( flags & PAGE_XD ) );
            this->set_base( index, base, _mode );
            this->set_ps_pat( index, !!( flags & PAGE_PAT ) );
        }

        virtual auto get_table( void ) -> uint64_t * override {
            return this->pmlx_table;
        }
        virtual auto get_address_index_in( void *address ) -> uint64_t override {
            return ( ( (uint64_t)address >> PAGE_HUGE_SHIFT ) & 0x1ff );
        }
        auto operator=( pml5t_entry *entry ) -> pml5t & {
            this->pmlxt::operator=( entry );
            return *this;
        }
    };

public:
    inline static pmlxt *kernel_page_table { };
    inline static bool   support_5level_paging { };

private:
    inline static pml4t *pml4_t { };
    inline static pml5t *pml5_t { };

public:
    explicit Paging( void ) noexcept;
    ~Paging( void ) noexcept = default;
    static auto initialize( limine_paging_mode_response *pg ) -> void;
};
}     // namespace Memory