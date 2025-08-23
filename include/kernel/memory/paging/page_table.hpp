#pragma once
#include <cstdint>
#include <kernel/display/print.hpp>
#include <kernel/memory/allocator/page.hpp>
#include <limine.h>
namespace Memory {
// 史山代码。不想动了

struct Paging {
    struct pmlxt {
    public:
        constexpr static auto PAGE_XD { 1UL << 63 };
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
        constexpr static auto PT_SIZE { 4096 };

    public:
        explicit pmlxt( );
        explicit pmlxt( std::uint64_t address ) :
            pt { (uint64_t *)address }, can_allocate( false ) {}
        virtual ~pmlxt( );

    public:
        auto map( uint64_t physics_address, uint64_t virtual_address, uint64_t size, uint64_t flags, Page::Type mode ) -> void;

        auto unmap( uint64_t virtual_address, std::size_t size, Page::Type mode ) -> void;

        auto page_protect( bool flags ) -> void;

        auto activate( void ) -> void;

        auto find_physcial_address( std::uint64_t virtual_address, Page::Type mode ) -> void *;

        auto copy( pmlxt & ) -> void;

        auto get_offset( std::uint64_t level ) {
            // virtual address
            // 63 ~ 58 | 57 ~ 49 | 48 ~ 39 | 38 ~ 30 | 29 ~ 21 | 20 ~ 12 | 11 ~ 0
            // res               | pml5t   | pml4t   | pml3t   | pml2t   | pml1t
            // 每层512个目录项, 2^9
            return ( level - 1 ) * 9 + 12;
        }
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

        virtual auto get_virtual_index( uint64_t address ) -> std::uint64_t = 0;

        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t > group ) -> pmlxt & = 0;

        auto operator=( uint64_t *addr ) -> pmlxt & {
            this->pt = addr;
            return *this;
        }
        auto empty( ) {
            return !this->pt;
        }
        const auto get( ) const {
            return this->pt;
        }
        virtual auto flags_p( uint64_t index ) -> uint64_t {
            return ( this->pt )[ index ] & PAGE_PRESENT;
        }
        virtual auto flags_rw( uint64_t index ) -> uint64_t {
            return ( this->pt )[ index ] & PAGE_RW_W;
        }
        virtual auto flags_us( uint64_t index ) -> uint64_t {
            return ( this->pt )[ index ] & PAGE_US_U;
        }
        virtual auto flags_pcd( uint64_t index ) -> uint64_t {
            return ( this->pt )[ index ] & PAGE_PCD;
        }
        virtual auto flags_pwt( uint64_t index ) -> uint64_t {
            return ( this->pt )[ index ] & PAGE_PWT;
        }
        virtual auto flags_a( uint64_t index ) -> uint64_t {
            return ( this->pt )[ index ] & PAGE_ACCESSED;
        }
        virtual auto flags_d( uint64_t index ) -> uint64_t {
            return ( this->pt )[ index ] & PAGE_DIRTY;
        }
        virtual auto flags_g( uint64_t index ) -> uint64_t {
            return ( this->pt )[ index ] & PAGE_GLOBAL;
        }
        virtual auto flags_xd( uint64_t index ) -> uint64_t {
            return ( this->pt )[ index ] & PAGE_XD;
        }
        virtual auto flags_base( uint64_t index ) -> uint64_t = 0;
        virtual auto flags_ps_pat( uint64_t index ) -> uint64_t = 0;
        virtual auto set_p( uint64_t index, bool bit ) -> void {
            this->pt[ index ] |= (uint64_t)bit << 0;
        }
        virtual auto set_rw( uint64_t index, bool bit ) -> void {
            this->pt[ index ] |= (uint64_t)bit << 1;
        }
        virtual auto set_us( uint64_t index, bool bit ) -> void {
            this->pt[ index ] |= (uint64_t)bit << 2;
        }
        virtual auto set_pwt( uint64_t index, bool bit ) -> void {
            this->pt[ index ] |= (uint64_t)bit << 3;
        }
        virtual auto set_pcd( uint64_t index, bool bit ) -> void {
            this->pt[ index ] |= (uint64_t)bit << 4;
        }
        virtual auto set_a( uint64_t index, bool bit ) -> void {
            this->pt[ index ] |= (uint64_t)bit << 5;
        }
        virtual auto set_d( uint64_t index, bool bit ) -> void {
            this->pt[ index ] |= (uint64_t)bit << 6;
        }
        virtual auto set_g( uint64_t index, bool bit ) -> void {
            this->pt[ index ] |= (uint64_t)bit << 8;
        }
        virtual auto set_xd( uint64_t index, bool bit ) -> void {
            this->pt[ index ] |= (uint64_t)bit << 63;
        }
        virtual auto set_base( uint64_t index, uint64_t address ) -> void = 0;
        virtual auto set_ps_pat( uint64_t index, bool bit ) -> void = 0;

    protected:
        std::uint64_t *pt;

    private:
        bool can_allocate;
    };
    struct pml1t : pmlxt {
        struct page_entry {
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
        };
        explicit pml1t( ) :
            pmlxt { } {
        }
        explicit pml1t( uint64_t address ) :
            pmlxt { address } {}
        virtual ~pml1t( ) = default;

        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t > group ) -> pmlxt & override {
            auto &[ index, base, flags ] = group;
            this->pt[ index ] = 0;
            this->set_p( index, !!( flags & PAGE_PRESENT ) );
            this->set_rw( index, !!( flags & PAGE_RW_W ) );
            this->set_us( index, !!( flags & PAGE_US_U ) );
            this->set_pwt( index, !!( flags & PAGE_PWT ) );
            this->set_pcd( index, !!( flags & PAGE_PCD ) );
            this->set_a( index, !!( flags & PAGE_ACCESSED ) );
            this->set_xd( index, !!( flags & PAGE_XD ) );
            this->set_ps_pat( index, 0 );
            this->set_base( index, base );
            return *this;
        }
        virtual auto get_virtual_index( uint64_t virtual_address ) -> std::uint64_t override {
            return ( virtual_address ) >> this->get_offset( 1 ) & 0x1ff;
        }

        virtual auto flags_base( uint64_t index ) -> uint64_t override {
            return ( (page_entry *)this->pt )[ index ].base << this->get_offset( 1 );
        }
        virtual auto flags_ps_pat( uint64_t index ) -> uint64_t override {
            return 0;
        }

    private:
        virtual auto set_base( uint64_t index, uint64_t address ) -> void override {
            ( (page_entry *)this->pt )[ index ].base = address >> this->get_offset( 1 );
        }
        virtual auto set_ps_pat( uint64_t index, bool bit ) -> void override {
            ( (page_entry *)this->pt )[ index ].pat = bit;
        }
    };
    struct pml2t : pmlxt {
        struct page_entry {
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
        };
        struct huge_page_entry {
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
        };
        explicit pml2t( ) :
            pmlxt { } {
        }
        explicit pml2t( uint64_t address ) :
            pmlxt { address } {}
        virtual ~pml2t( ) = default;

        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t > group ) -> pmlxt & override {
            auto &[ index, base, flags ] = group;
            this->pt[ index ] = 0;
            this->set_p( index, !!( flags & PAGE_PRESENT ) );
            this->set_rw( index, !!( flags & PAGE_RW_W ) );
            this->set_us( index, !!( flags & PAGE_US_U ) );
            this->set_pwt( index, !!( flags & PAGE_PWT ) );
            this->set_pcd( index, !!( flags & PAGE_PCD ) );
            this->set_a( index, !!( flags & PAGE_ACCESSED ) );
            this->set_xd( index, !!( flags & PAGE_XD ) );
            this->set_ps_pat( index, !!( flags & PAGE_PS ) );
            this->set_base( index, base );
            return *this;
        }
        virtual auto get_virtual_index( uint64_t virtual_address ) -> std::uint64_t override {
            return ( virtual_address ) >> this->get_offset( 2 ) & 0x1ff;
        }

        virtual auto flags_base( uint64_t index ) -> uint64_t override {
            if ( ( (page_entry *)this->pt )[ index ].ps ) {
                return (uint64_t)( (huge_page_entry *)this->pt )[ index ].base << this->get_offset( 2 );
            }
            else {
                return (uint64_t)( (page_entry *)this->pt )[ index ].base << this->get_offset( 1 );
            }
        }
        virtual auto flags_ps_pat( uint64_t index ) -> uint64_t override {
            return ( (page_entry *)this->pt )[ index ].ps;
        }

    private:
        virtual auto set_base( uint64_t index, uint64_t address ) -> void override {
            if ( ( (page_entry *)this->pt )[ index ].ps ) {
                ( (huge_page_entry *)this->pt )[ index ].base = ( address >> this->get_offset( 2 ) );
            }
            else {
                ( (page_entry *)this->pt )[ index ].base = ( address >> this->get_offset( 1 ) );
            }
        }
        virtual auto set_ps_pat( uint64_t index, bool bit ) -> void override {
            ( (page_entry *)this->pt )[ index ].ps = bit;
        }
    };
    struct pml3t : pmlxt {
        struct page_entry {
            uint64_t p : 1;
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
        };
        struct huge_page_entry {
            uint64_t p : 1;
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
        };
        explicit pml3t( ) :
            pmlxt { } {
        }
        explicit pml3t( uint64_t address ) :
            pmlxt { address } {}
        virtual ~pml3t( ) = default;

        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t > group ) -> pmlxt & override {
            auto &[ index, base, flags ] = group;
            this->pt[ index ] = 0;
            this->set_p( index, !!( flags & PAGE_PRESENT ) );
            this->set_rw( index, !!( flags & PAGE_RW_W ) );
            this->set_us( index, !!( flags & PAGE_US_U ) );
            this->set_pwt( index, !!( flags & PAGE_PWT ) );
            this->set_pcd( index, !!( flags & PAGE_PCD ) );
            this->set_a( index, !!( flags & PAGE_ACCESSED ) );
            this->set_xd( index, !!( flags & PAGE_XD ) );
            this->set_ps_pat( index, !!( flags & PAGE_PS ) );
            this->set_base( index, base );
            return *this;
        }
        virtual auto get_virtual_index( uint64_t virtual_address ) -> std::uint64_t override {
            return ( virtual_address ) >> this->get_offset( 3 ) & 0x1ff;
        }

        virtual auto flags_base( uint64_t index ) -> uint64_t override {
            if ( ( (page_entry *)this->pt )[ index ].ps ) {
                return (uint64_t)( (huge_page_entry *)this->pt )[ index ].base << this->get_offset( 3 );
            }
            else {
                return (uint64_t)( (page_entry *)this->pt )[ index ].base << this->get_offset( 1 );
            }
        }
        virtual auto flags_ps_pat( uint64_t index ) -> uint64_t override {
            return ( (page_entry *)this->pt )[ index ].ps;
        }

    private:
        virtual auto set_base( uint64_t index, uint64_t address ) -> void override {
            if ( ( (page_entry *)this->pt )[ index ].ps ) {
                ( (huge_page_entry *)this->pt )[ index ].base = address >> this->get_offset( 3 );
            }
            else {
                ( (page_entry *)this->pt )[ index ].base = address >> this->get_offset( 1 );
            }
        }
        virtual auto set_ps_pat( uint64_t index, bool bit ) -> void override {
            ( (page_entry *)this->pt )[ index ].ps = bit;
        }
    };
    struct pml4t : pmlxt {
        struct page_entry {
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
        };
        explicit pml4t( ) :
            pmlxt { } {
        }
        explicit pml4t( uint64_t address ) :
            pmlxt { address } {}
        virtual ~pml4t( ) = default;

        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t > group ) -> pmlxt & override {
            auto &[ index, base, flags ] = group;
            this->pt[ index ] = 0;
            this->set_p( index, !!( flags & PAGE_PRESENT ) );
            this->set_rw( index, !!( flags & PAGE_RW_W ) );
            this->set_us( index, !!( flags & PAGE_US_U ) );
            this->set_pwt( index, !!( flags & PAGE_PWT ) );
            this->set_pcd( index, !!( flags & PAGE_PCD ) );
            this->set_a( index, !!( flags & PAGE_ACCESSED ) );
            this->set_xd( index, !!( flags & PAGE_XD ) );
            this->set_ps_pat( index, 0 );
            this->set_base( index, base );
            return *this;
        }
        virtual auto get_virtual_index( uint64_t virtual_address ) -> std::uint64_t override {
            return ( virtual_address ) >> this->get_offset( 4 ) & 0x1ff;
        }

        virtual auto flags_base( uint64_t index ) -> uint64_t override {
            return ( (page_entry *)this->pt )[ index ].base << this->get_offset( 1 );
        }
        virtual auto flags_ps_pat( uint64_t index ) -> uint64_t override {
            return 0;
        }

    private:
        virtual auto set_base( uint64_t index, uint64_t address ) -> void override {
            ( (page_entry *)this->pt )[ index ].base = address >> this->get_offset( 1 );
        }
        virtual auto set_ps_pat( uint64_t index, bool bit ) -> void override {
        }
    };
    struct pml5t : pmlxt {
        struct page_entry {
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
        };
        explicit pml5t( ) :
            pmlxt { } {
        }
        explicit pml5t( uint64_t address ) :
            pmlxt { address } {}
        virtual ~pml5t( ) = default;

        virtual auto operator=( std::tuple< uint64_t, uint64_t, uint64_t > group ) -> pmlxt & override {
            auto &[ index, base, flags ] = group;
            this->pt[ index ] = 0;
            this->set_p( index, !!( flags & PAGE_PRESENT ) );
            this->set_rw( index, !!( flags & PAGE_RW_W ) );
            this->set_us( index, !!( flags & PAGE_US_U ) );
            this->set_pwt( index, !!( flags & PAGE_PWT ) );
            this->set_pcd( index, !!( flags & PAGE_PCD ) );
            this->set_a( index, !!( flags & PAGE_ACCESSED ) );
            this->set_xd( index, !!( flags & PAGE_XD ) );
            this->set_ps_pat( index, 0 );
            this->set_base( index, base );
            return *this;
        }
        virtual auto get_virtual_index( uint64_t virtual_address ) -> std::uint64_t override {
            return ( virtual_address ) >> this->get_offset( 5 ) & 0x1ff;
        }

        virtual auto flags_base( uint64_t index ) -> uint64_t override {
            return (std::uint64_t)( (page_entry *)this->pt )[ index ].base << this->get_offset( 1 );
        }
        virtual auto flags_ps_pat( uint64_t index ) -> uint64_t override {
            return 0;
        }

    private:
        virtual auto set_base( uint64_t index, uint64_t address ) -> void override {
            ( (page_entry *)this->pt )[ index ].base = address >> this->get_offset( 1 );
        }
        virtual auto set_ps_pat( uint64_t index, bool bit ) -> void override {
        }
    };

    Paging( void ) noexcept;
    static auto initialize( limine_paging_mode_response *pg ) -> void;

    inline static pmlxt *kernel_page_table;
    inline static bool support_5level_paging { };
};
}     // namespace Memory