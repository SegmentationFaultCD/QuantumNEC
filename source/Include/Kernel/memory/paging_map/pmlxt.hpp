#pragma once
#include <Lib/Uefi.hpp>
#include <Kernel/memory/page_allocater/page_allocaters.hpp>
#include <tuple>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC constexpr CONST auto PML_IDX { 512 };
    PUBLIC constexpr CONST auto PML_SIZE { PML_IDX * 2 };
    PUBLIC constexpr CONST auto PT_SIZE { 0x1000 };

    /*
     * 内核层 ： 00000000 00000000 -> FFFFFFFF FFFFFFFF
     * 应用层 ： 00000000 02A00000 -> 00007FFF FFFFFFFF
     */

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
    PUBLIC constexpr CONST auto PAGE_US_S { 0UL << 0 };
    PUBLIC constexpr CONST auto PAGE_US_U { 1ull << 2 };
    PUBLIC constexpr CONST auto PAGE_RW_R { 0UL << 0 };
    PUBLIC constexpr CONST auto PAGE_5th_SHIFT { 48 };
    PUBLIC constexpr CONST auto PAGE_4th_SHIFT { 39 };
    PUBLIC constexpr CONST auto PAGE_1G_SHIFT { 30 };
    PUBLIC constexpr CONST auto PAGE_2M_SHIFT { 21 };
    PUBLIC constexpr CONST auto PAGE_4K_SHIFT { 12 };
    PUBLIC constexpr CONST auto PAGE_2M_SIZE { ( 1UL << PAGE_2M_SHIFT ) };
    PUBLIC constexpr CONST auto PAGE_4K_SIZE { ( 1UL << PAGE_4K_SHIFT ) };
    PUBLIC constexpr CONST auto PAGE_1G_SIZE { ( 1UL << PAGE_1G_SHIFT ) };
    PUBLIC constexpr CONST auto PAGE_2M_MASK { ( ~( PAGE_2M_SIZE - 1 ) ) };
    PUBLIC constexpr CONST auto PAGE_4K_MASK { ( ~( PAGE_4K_SIZE - 1 ) ) };
    PUBLIC constexpr CONST auto PAGE_1G_MASK { ( ~( PAGE_1G_SIZE - 1 ) ) };
    PUBLIC inline constexpr auto page_2M_aligned( auto addr ) {
        return ( ( (unsigned long)( addr ) + PAGE_2M_SIZE - 1 ) & PAGE_2M_MASK );
    };
    PUBLIC inline constexpr auto page_4K_aligned( auto addr ) {
        return ( ( (unsigned long)( addr ) + PAGE_4K_SIZE - 1 ) & PAGE_4K_MASK );
    };
    PUBLIC inline constexpr auto page_1G_aligned( auto addr ) {
        return ( ( (unsigned long)( addr ) + PAGE_1G_SIZE - 1 ) & PAGE_1G_MASK );
    };

    PUBLIC struct pml1t_entry
    {
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
        explicit pml1t_entry( VOID ) noexcept = default;
    };
    PUBLIC struct pml2t_entry
    {
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
        explicit pml2t_entry( VOID ) noexcept = default;
    };
    PUBLIC struct pml2t_huge_entry
    {
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
        explicit pml2t_huge_entry( VOID ) noexcept = default;
    };
    PUBLIC struct pml3t_entry
    {
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
        explicit pml3t_entry( VOID ) noexcept = default;
    };
    PUBLIC struct pml3t_huge_entry
    {
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
        explicit pml3t_huge_entry( VOID ) noexcept = default;
    };
    PUBLIC struct pml4t_entry
    {
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
        explicit pml4t_entry( VOID ) noexcept = default;
    };
    PUBLIC struct pml5t_entry
    {
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
        explicit pml5t_entry( VOID ) noexcept = default;
    };

    enum class Level {
        PML5 = 5,
        PML4 = 4,
        PDPT = 3,
        PD = 2,
        PT = 1,

    };
    class pmlxt
    {
    public:
        explicit pmlxt( IN Level _level ) noexcept :
            level { _level } {
        }
        virtual ~pmlxt( VOID ) noexcept = default;

    public:
        virtual auto flags_p( IN uint64_t index ) -> uint64_t = 0;
        virtual auto flags_rw( IN uint64_t index ) -> uint64_t = 0;
        virtual auto flags_us( IN uint64_t index ) -> uint64_t = 0;
        virtual auto flags_pcd( IN uint64_t index ) -> uint64_t = 0;
        virtual auto flags_pwt( IN uint64_t index ) -> uint64_t = 0;
        virtual auto flags_a( IN uint64_t index ) -> uint64_t = 0;
        virtual auto flags_xd( IN uint64_t index ) -> uint64_t = 0;
        virtual auto flags_base( IN uint64_t index ) -> uint64_t = 0;
        virtual auto flags_ps_pat( IN uint64_t index ) -> uint64_t = 0;
        virtual auto set_p( IN uint64_t index, IN BOOL bit ) -> VOID = 0;
        virtual auto set_rw( IN uint64_t index, IN BOOL bit ) -> VOID = 0;
        virtual auto set_us( IN uint64_t index, IN BOOL bit ) -> VOID = 0;
        virtual auto set_pcd( IN uint64_t index, IN BOOL bit ) -> VOID = 0;
        virtual auto set_pwt( IN uint64_t index, IN BOOL bit ) -> VOID = 0;
        virtual auto set_a( IN uint64_t index, IN BOOL bit ) -> VOID = 0;
        virtual auto set_xd( IN uint64_t index, IN BOOL bit ) -> VOID = 0;
        virtual auto set_base( IN uint64_t index, IN uint64_t address ) -> VOID = 0;
        virtual auto set_ps_pat( IN uint64_t index, IN BOOL bit ) -> VOID = 0;

    public:
        /**
         * @brief 检查虚拟地址是否为规范格式vrt地址，如果无效，请进行调整
         * @param virtual_address 要检查的地址
         * @retval BOOL 状态
         */
        auto check_address( IN uint64_t *virtual_address ) {
            if ( ( *virtual_address >> 47 ) & 1 ) {
                if ( *virtual_address >> 48 == 0xFFFF ) {
                    return TRUE;
                }
                *virtual_address |= 0xFFFFULL << 48;
                return FALSE;
            }
            return TRUE;
        }
        auto check_page_size( IN MemoryPageType mode ) {
            using enum MemoryPageType;
            switch ( mode ) {
            case PAGE_2M:
                return PAGE_2M_SIZE;
            case PAGE_4K:
                return PAGE_4K_SIZE;
            case PAGE_1G:
                return PAGE_1G_SIZE;
            default:
                return 0ul;
            }
        }
        auto is_huge( IN MemoryPageType mode ) {
            if ( mode == MemoryPageType::PAGE_2M || mode == MemoryPageType::PAGE_1G )
                return PAGE_PS;
            return 0ul;
        };

        virtual auto operator=( IN std::tuple< uint64_t, uint64_t, uint64_t > group ) -> VOID = 0;
        virtual auto operator=( IN uint64_t table_address ) -> VOID = 0;

        virtual auto get_address_index_in( IN VOID *address ) -> uint64_t = 0;

        virtual auto get_table( VOID ) -> uint64_t * = 0;

    public:
        const Level level;

    protected:
        struct PmlxEntry
        {
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
    };
}