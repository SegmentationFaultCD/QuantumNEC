#pragma once
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_manager.hpp>
#include <kernel/print.hpp>
#include <lib/Uefi.hpp>
#include <tuple>
namespace QuantumNEC::Kernel::x86_64 {

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

enum class Level {
    PML5 = 5,
    PML4 = 4,
    PDPT = 3,
    PD   = 2,
    PT   = 1,

};

class pmlxt {
public:
    constexpr static auto PT_SIZE { 0x1000 };

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
    constexpr static auto PAGE_5th_SHIFT { 48 };
    constexpr static auto PAGE_4th_SHIFT { 39 };
    constexpr static auto PAGE_1G_SHIFT { 30 };
    constexpr static auto PAGE_2M_SHIFT { 21 };
    constexpr static auto PAGE_4K_SHIFT { 12 };
    constexpr static auto USER_STACK_VIRTUAL_ADDRESS_TOP { 0x00007fffffffffffUL };

public:
    explicit pmlxt( void ) noexcept {
    }
    virtual ~pmlxt( void ) noexcept {
    }

public:
    virtual auto flags_p( IN uint64_t index ) -> uint64_t                                            = 0;
    virtual auto flags_rw( IN uint64_t index ) -> uint64_t                                           = 0;
    virtual auto flags_us( IN uint64_t index ) -> uint64_t                                           = 0;
    virtual auto flags_pcd( IN uint64_t index ) -> uint64_t                                          = 0;
    virtual auto flags_pwt( IN uint64_t index ) -> uint64_t                                          = 0;
    virtual auto flags_a( IN uint64_t index ) -> uint64_t                                            = 0;
    virtual auto flags_xd( IN uint64_t index ) -> uint64_t                                           = 0;
    virtual auto flags_base( IN uint64_t index, MemoryPageType mode ) -> uint64_t                    = 0;
    virtual auto flags_ps_pat( IN uint64_t index ) -> uint64_t                                       = 0;
    virtual auto set_p( IN uint64_t index, IN bool bit ) -> void                                     = 0;
    virtual auto set_rw( IN uint64_t index, IN bool bit ) -> void                                    = 0;
    virtual auto set_us( IN uint64_t index, IN bool bit ) -> void                                    = 0;
    virtual auto set_pcd( IN uint64_t index, IN bool bit ) -> void                                   = 0;
    virtual auto set_pwt( IN uint64_t index, IN bool bit ) -> void                                   = 0;
    virtual auto set_a( IN uint64_t index, IN bool bit ) -> void                                     = 0;
    virtual auto set_xd( IN uint64_t index, IN bool bit ) -> void                                    = 0;
    virtual auto set_base( IN uint64_t index, IN uint64_t address, IN MemoryPageType _mode ) -> void = 0;
    virtual auto set_ps_pat( IN uint64_t index, IN bool bit ) -> void                                = 0;

public:
    /**
     * @brief 检查虚拟地址是否为规范格式vrt地址，如果无效，请进行调整
     * @param virtual_address 要检查的地址
     * @retval BOOL 状态
     */
    auto check_address( IN uint64_t *virtual_address ) {
        if ( ( *virtual_address >> 47 ) & 1 ) {
            if ( *virtual_address >> 48 == 0xFFFF ) {
                return true;
            }
            *virtual_address |= 0xFFFFULL << 48;
            return false;
        }
        return true;
    }
    auto check_page_size( IN MemoryPageType mode ) {
        using enum MemoryPageType;
        switch ( mode ) {
        case PAGE_2M:
            return PageAllocater::__page_size__< PAGE_2M >;
        case PAGE_4K:
            return PageAllocater::__page_size__< PAGE_4K >;
        case PAGE_1G:
            return PageAllocater::__page_size__< PAGE_1G >;
        default:
            return 0ul;
        }
    }
    auto is_huge( IN MemoryPageType mode ) {
        if ( mode == MemoryPageType::PAGE_2M || mode == MemoryPageType::PAGE_1G )
            return PAGE_PS;
        return 0ul;
    };

    virtual auto operator=( IN std::tuple< uint64_t, uint64_t, uint64_t, MemoryPageType > group ) -> void = 0;
    virtual auto operator=( IN uint64_t table_address ) -> void                                           = 0;

    virtual auto get_address_index_in( IN void *address ) -> uint64_t = 0;

    virtual auto get_table( void ) -> uint64_t * = 0;

protected:
    struct PmlxEntry {
    private:
        uint64_t *nul { };

        uint64_t *pml1t_entries { };
        uint64_t *pml2t_entries { };
        uint64_t *pml3t_entries { };
        uint64_t *pml4t_entries { };
        uint64_t *pml5t_entries { };

    public:
        constexpr auto operator[]( IN Level level ) -> uint64_t *& {
            using enum Level;
            switch ( level ) {
            case PML5:
                return this->pml5t_entries;
            case PML4:
                return this->pml4t_entries;
            case PDPT:
                return this->pml3t_entries;
            case PD:
                return this->pml2t_entries;
            case PT:
                return this->pml1t_entries;
            }
            return this->nul;
        }
    } pmlx_entry;

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
    auto map( IN uint64_t physics_address, IN uint64_t virtual_address, IN uint64_t size, IN uint64_t flags, IN MemoryPageType mode ) -> void;
    /**
     * @brief 取消映射页
     * @param virtual_address 将取消映射的指定虚拟地址
     * @param size 要取消映射的内存页大小
     * @param mode 内存页模式
     * @param pmlx_t 页表地址
     */
    auto unmap( IN uint64_t virtual_address, IN size_t size, IN MemoryPageType mode ) -> void;
    /**
     * @brief 控制页保护开关
     * @param flags 如果为true那么开启页保护，如果为false那么关闭页保护
     */
    auto page_protect( IN bool flags ) -> void;
    /**
     * @brief 制作页表
     * @param flags 页属性
     * @param level 页表等级（5/4/3/2/1）
     * @param mode 内存页模式
     * @return 制作的页目录地址
     */
    auto make( IN uint64_t flags, IN Level level, IN MemoryPageType mode ) -> pmlxt &;
    /**
     * @brief 激活页表
     * @param pmlx_t 页表地址
     */
    auto activate( ) -> void;
    /**
     * @brief 根据页表和虚拟地址找出物理地址
     * @param virtual_address 虚拟地址
     * @param mode 内存页模式
     * @param pmlx_t 页表地址
     */
    auto VTP_from( IN void *virtual_address, IN MemoryPageType mode ) -> void *;
    /**
     * @brief 复制页表
     */
    auto copy( pmlxt & ) -> void;
};
class pml1t : public pmlxt {
    friend pmlxt;

public:
    using page_table_entry      = pml1t_entry;
    using huge_page_table_entry = void;

public:
    explicit pml1t( IN pml1t_entry *pml1t_address ) noexcept :
        pmlxt { } {
        this->pmlx_entry[ Level::PML4 ] = (uint64_t *)( pml1t_address );
    }

    explicit pml1t( IN pml1t &pml1t ) noexcept :
        pmlxt { } {
        this->pmlx_entry = pml1t.pmlx_entry;
    }
    explicit pml1t( void ) noexcept :
        pmlxt { } {
    }
    virtual ~pml1t( void ) noexcept = default;

public:
    virtual auto flags_p( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].p;
    }
    virtual auto flags_rw( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].rw;
    }
    virtual auto flags_us( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].us;
    }
    virtual auto flags_pcd( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pcd;
    }
    virtual auto flags_pwt( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pwt;
    }
    virtual auto flags_a( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].a;
    }
    virtual auto flags_xd( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].xd;
    }
    virtual auto flags_base( IN uint64_t index, IN [[maybe_unused]] MemoryPageType _mode ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].base << this->PAGE_4K_SHIFT;
    }
    virtual auto flags_ps_pat( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pat;
    }
    virtual auto set_p( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].p = bit;
    }
    virtual auto set_rw( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].rw = bit;
    }
    virtual auto set_us( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].us = bit;
    }
    virtual auto set_pcd( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pcd = bit;
    }
    virtual auto set_pwt( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pwt = bit;
    }
    virtual auto set_a( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].a = bit;
    }
    virtual auto set_xd( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].xd = bit;
    }
    virtual auto set_base( IN uint64_t index, IN uint64_t address, IN [[maybe_unused]] MemoryPageType _mode ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].base = address >> PAGE_4K_SHIFT;
    }
    virtual auto set_ps_pat( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pat = bit;
    }
    virtual auto operator=( IN std::tuple< uint64_t, uint64_t, uint64_t, MemoryPageType > group ) -> void override {
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
    virtual auto operator=( IN uint64_t table_address ) -> void override {
        this->pmlx_entry[ Level::PT ] = (uint64_t *)table_address;
    }
    virtual auto get_table( void ) -> uint64_t * override {
        return this->pmlx_entry[ Level::PT ];
    }
    virtual auto get_address_index_in( IN void *address ) -> uint64_t override {
        return ( ( (uint64_t)address >> PAGE_4K_SHIFT ) & 0x1ff );
    }
    auto operator=( IN pml1t &&pml1 ) -> pml1t & {
        this->pmlx_entry = pml1.pmlx_entry;
        return *this;
    };
    auto operator=( IN pml1t *pml1 ) -> pml1t & {
        this->pmlx_entry = pml1->pmlx_entry;
        return *this;
    };
    auto operator=( IN page_table_entry *entry ) -> pml1t & {
        this->pmlx_entry[ Level::PT ] = (uint64_t *)entry;
        return *this;
    };
};
class pml2t : public pmlxt {
    friend pmlxt;

public:
    using page_table_entry      = pml2t_entry;
    using huge_page_table_entry = pml2t_huge_entry;

    // 三级页表有两种情况
    // 一种是当分页模式为2M时继续找二级页表
    // 一种是当分页模式为1G时放弃二级页表，直接从三级页表把内存基地址写入
public:
    explicit pml2t( IN page_table_entry *pml2t_address ) noexcept :
        pmlxt { } {
        this->pmlx_entry[ Level::PD ] = (uint64_t *)pml2t_address;
    }
    explicit pml2t( void ) noexcept :
        pmlxt { } {};
    virtual ~pml2t( void ) noexcept = default;

public:
    virtual auto flags_p( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].p;
    }
    virtual auto flags_rw( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].rw;
    }

    virtual auto flags_us( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].us;
    }

    virtual auto flags_pcd( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].pcd;
    }

    virtual auto flags_pwt( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].pwt;
    }

    virtual auto flags_a( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].a;
    }

    virtual auto flags_xd( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].xd;
    }
    virtual auto flags_base( IN uint64_t index, IN MemoryPageType _mode ) -> uint64_t override {
        if ( _mode == MemoryPageType::PAGE_2M ) {
            return ( (huge_page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].base << PAGE_2M_SHIFT;
        }
        else {
            return ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].base << PAGE_4K_SHIFT;
        }
    }
    virtual auto flags_ps_pat( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].ps;
    }
    virtual auto set_p( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].p = bit;
    }

    virtual auto set_rw( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].rw = bit;
    }

    virtual auto set_us( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].us = bit;
    }

    virtual auto set_pcd( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].pcd = bit;
    }

    virtual auto set_pwt( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].pwt = bit;
    }
    virtual auto set_a( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].a = bit;
    }

    virtual auto set_xd( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].xd = bit;
    }

    virtual auto set_base( IN uint64_t index, IN uint64_t address, IN [[maybe_unused]] MemoryPageType _mode ) -> void override {
        if ( _mode == MemoryPageType::PAGE_2M ) {
            ( (huge_page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].base = address >> PAGE_2M_SHIFT;
        }
        else {
            ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].base = address >> PAGE_4K_SHIFT;
        }
    }
    virtual auto set_ps_pat( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PD ] )[ index ].ps = bit;
    }

    virtual auto operator=( IN std::tuple< uint64_t, uint64_t, uint64_t, MemoryPageType > group ) -> void override {
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
    virtual auto operator=( IN uint64_t table_address ) -> void override {
        this->pmlx_entry[ Level::PD ] = (uint64_t *)table_address;
    }

    virtual auto get_table( void ) -> uint64_t * override {
        return this->pmlx_entry[ Level::PD ];
    }
    virtual auto get_address_index_in( IN void *address ) -> uint64_t override {
        return ( ( (uint64_t)address >> PAGE_2M_SHIFT ) & 0x1ff );
    }
    auto operator=( IN page_table_entry *entry ) -> pml2t & {
        this->pmlx_entry[ Level::PD ] = (uint64_t *)entry;
        return *this;
    }
};
class pml3t : public pmlxt {
    friend pmlxt;

public:
    using page_table_entry      = pml3t_entry;
    using huge_page_table_entry = pml3t_huge_entry;
    // 三级页表有两种情况
    // 一种是当分页模式为2M时继续找二级页表
    // 一种是当分页模式为1G时放弃二级页表，直接从三级页表把内存基地址写入
public:
    explicit pml3t( IN page_table_entry *pml3t_address ) noexcept :
        pmlxt { } {
        this->pmlx_entry[ Level::PDPT ] = (uint64_t *)pml3t_address;
    }
    explicit pml3t( void ) noexcept :
        pmlxt { } {};
    virtual ~pml3t( void ) noexcept = default;

public:
    virtual auto flags_p( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].p;
    }
    virtual auto flags_rw( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].rw;
    }

    virtual auto flags_us( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].us;
    }

    virtual auto flags_pcd( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].pcd;
    }

    virtual auto flags_pwt( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].pwt;
    }

    virtual auto flags_a( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].a;
    }
    virtual auto flags_ps_pat( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].ps;
    }
    virtual auto flags_xd( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].xd;
    }
    virtual auto flags_base( IN uint64_t index, IN MemoryPageType _mode ) -> uint64_t override {
        if ( _mode == MemoryPageType::PAGE_1G ) {
            return ( (huge_page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].base << PAGE_1G_SHIFT;
        }
        else {
            return ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].base << PAGE_4K_SHIFT;
        }
    }

    virtual auto set_p( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].p = bit;
    }

    virtual auto set_rw( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].rw = bit;
    }

    virtual auto set_us( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].us = bit;
    }

    virtual auto set_pcd( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].pcd = bit;
    }

    virtual auto set_pwt( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].pwt = bit;
    }
    virtual auto set_a( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].a = bit;
    }

    virtual auto set_xd( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].xd = bit;
    }
    virtual auto set_ps_pat( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].ps = bit;
    }

    virtual auto set_base( IN uint64_t index, IN uint64_t address, IN MemoryPageType _mode ) -> void override {
        if ( _mode == MemoryPageType::PAGE_1G ) {
            ( (huge_page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].base = address >> PAGE_1G_SHIFT;
        }
        else {
            ( (page_table_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].base = address >> PAGE_4K_SHIFT;
        }
    }
    virtual auto operator=( IN std::tuple< uint64_t, uint64_t, uint64_t, MemoryPageType > group ) -> void override {
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
    virtual auto operator=( IN uint64_t table_address ) -> void override {
        this->pmlx_entry[ Level::PDPT ] = (uint64_t *)table_address;
    }

    virtual auto get_table( void ) -> uint64_t * override {
        return this->pmlx_entry[ Level::PDPT ];
    }
    virtual auto get_address_index_in( IN void *address ) -> uint64_t override {
        return ( ( (uint64_t)address >> PAGE_1G_SHIFT ) & 0x1ff );
    }
    auto operator=( IN page_table_entry *entry ) -> pml3t & {
        this->pmlx_entry[ Level::PDPT ] = (uint64_t *)entry;
        return *this;
    }
};
class pml4t : public pmlxt {
    friend pmlxt;

public:
    using page_table_entry      = pml4t_entry;
    using huge_page_table_entry = void;
    // 五级页表是最大的了
public:
    explicit pml4t( IN page_table_entry *pml4t_address ) noexcept :
        pmlxt { } {
        this->pmlx_entry[ Level::PML4 ] = (uint64_t *)( pml4t_address );
    }

    explicit pml4t( IN pml4t &pml4t ) noexcept :
        pmlxt { } {
        this->pmlx_entry = pml4t.pmlx_entry;
    }
    explicit pml4t( void ) noexcept :
        pmlxt { } {

        };
    virtual ~pml4t( void ) noexcept = default;

public:
    virtual auto flags_p( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].p;
    }
    virtual auto flags_rw( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].rw;
    }
    virtual auto flags_us( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].us;
    }
    virtual auto flags_pcd( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].pcd;
    }
    virtual auto flags_pwt( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].pwt;
    }
    virtual auto flags_a( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].a;
    }
    virtual auto flags_xd( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].xd;
    }
    virtual auto flags_base( IN uint64_t index, IN [[maybe_unused]] MemoryPageType _mode ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].base << PAGE_4K_SHIFT;
    }
    virtual auto flags_ps_pat( [[maybe_unused]] IN uint64_t index ) -> uint64_t override {
        return 0;
    }
    virtual auto set_p( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].p = bit;
    }
    virtual auto set_rw( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].rw = bit;
    }
    virtual auto set_us( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].us = bit;
    }
    virtual auto set_pcd( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].pcd = bit;
    }
    virtual auto set_pwt( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].pwt = bit;
    }
    virtual auto set_a( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].a = bit;
    }
    virtual auto set_xd( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].xd = bit;
    }
    virtual auto set_base( IN uint64_t index, IN uint64_t address, IN [[maybe_unused]] MemoryPageType _mode ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].base = address >> PAGE_4K_SHIFT;
    }
    virtual auto set_ps_pat( [[maybe_unused]] IN uint64_t index, [[maybe_unused]] IN bool bit ) -> void override {
    }
    virtual auto operator=( IN std::tuple< uint64_t, uint64_t, uint64_t, MemoryPageType > group ) -> void override {
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
    virtual auto operator=( IN uint64_t table_address ) -> void override {
        this->pmlx_entry[ Level::PML4 ] = (uint64_t *)table_address;
    }
    virtual auto operator=( IN pml4t &pml4_t ) -> void {
        this->pmlx_entry[ Level::PML4 ] = (uint64_t *)pml4_t.pmlx_entry[ Level::PML4 ];
    }
    virtual auto get_table( void ) -> uint64_t * override {
        return this->pmlx_entry[ Level::PML4 ];
    }
    virtual auto get_address_index_in( IN void *address ) -> uint64_t override {
        return ( ( (uint64_t)address >> PAGE_4th_SHIFT ) & 0x1ff );
    }
    auto operator=( IN pml4t &&pml4 ) -> pml4t & {
        this->pmlx_entry = pml4.pmlx_entry;
        return *this;
    };
    auto operator=( IN const pml4t *pml4 ) -> pml4t & {
        this->pmlx_entry = pml4->pmlx_entry;
        return *this;
    };
    auto operator=( IN page_table_entry *entry ) -> pml4t & {
        this->pmlx_entry[ Level::PML4 ] = (uint64_t *)entry;
        return *this;
    };
};
class pml5t : public pmlxt {
    friend pmlxt;

public:
    using page_table_entry      = pml5t_entry;
    using huge_page_table_entry = void;
    // 五级页表是最大的了
public:
    explicit pml5t( IN page_table_entry *pml5t_address ) noexcept :
        pmlxt { } {
        this->pmlx_entry[ Level::PML5 ] = (uint64_t *)( pml5t_address );
    }

    explicit pml5t( IN pml5t &pml5t ) noexcept :
        pmlxt { } {
        this->pmlx_entry = pml5t.pmlx_entry;
    }
    explicit pml5t( void ) noexcept :
        pmlxt { } {
    }
    virtual ~pml5t( void ) noexcept = default;

public:
    virtual auto flags_p( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].p;
    }
    virtual auto flags_rw( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].rw;
    }
    virtual auto flags_us( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].us;
    }
    virtual auto flags_pcd( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].pcd;
    }
    virtual auto flags_pwt( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].pwt;
    }
    virtual auto flags_a( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].a;
    }
    virtual auto flags_xd( IN uint64_t index ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].xd;
    }
    virtual auto flags_ps_pat( [[maybe_unused]] IN uint64_t index ) -> uint64_t override {
        return 0;
    }
    virtual auto flags_base( IN uint64_t index, IN [[maybe_unused]] MemoryPageType _mode ) -> uint64_t override {
        return ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].base << PAGE_4K_SHIFT;
    }
    virtual auto set_p( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].p = bit;
    }
    virtual auto set_rw( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].rw = bit;
    }
    virtual auto set_us( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].us = bit;
    }
    virtual auto set_pcd( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].pcd = bit;
    }
    virtual auto set_pwt( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].pwt = bit;
    }
    virtual auto set_a( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].a = bit;
    }
    virtual auto set_xd( IN uint64_t index, IN bool bit ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].xd = bit;
    }
    virtual auto set_base( IN uint64_t index, IN uint64_t address, IN [[maybe_unused]] MemoryPageType _mode ) -> void override {
        ( (page_table_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].base = address >> PAGE_4K_SHIFT;
    }
    virtual auto set_ps_pat( [[maybe_unused]] IN uint64_t index, [[maybe_unused]] IN bool bit ) -> void override {
    }
    virtual auto operator=( IN std::tuple< uint64_t, uint64_t, uint64_t, MemoryPageType > group ) -> void override {
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
    virtual auto operator=( IN uint64_t table_address ) -> void override {
        this->pmlx_entry[ Level::PML5 ] = (uint64_t *)table_address;
    }

    virtual auto get_table( void ) -> uint64_t * override {
        return this->pmlx_entry[ Level::PML5 ];
    }
    virtual auto get_address_index_in( IN void *address ) -> uint64_t override {
        return ( ( (uint64_t)address >> PAGE_5th_SHIFT ) & 0x1ff );
    }
    auto operator=( IN pml5t &&pml5 ) -> pml5t & {
        this->pmlx_entry = pml5.pmlx_entry;
        return *this;
    };
    auto operator=( IN const pml5t *pml5 ) -> pml5t & {
        this->pmlx_entry = pml5->pmlx_entry;
        return *this;
    };
    auto operator=( IN page_table_entry *entry ) -> pml5t & {
        this->pmlx_entry[ Level::PML5 ] = (uint64_t *)entry;
        return *this;
    };
};

class Paging {
public:
    inline static pmlxt *kernel_page_table { };
    inline static bool   support_5level_paging { };

private:
    inline static pml4t *pml4_t { };
    inline static pml5t *pml5_t { };

public:
    explicit Paging( void ) noexcept;
};
}     // namespace QuantumNEC::Kernel::x86_64