#pragma once
#include <Kernel/memory/paging_map/pmlxt.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class pml4t : public pmlxt
    {
        friend pmlxt;
        // 五级页表是最大的了
    public:
        explicit pml4t( IN pml4t_entry *pml4t_address ) noexcept :
            pmlxt { Level::PML4 } {
            this->pmlx_entry[ Level::PML4 ] = (uint64_t *)( pml4t_address );
        }

        explicit pml4t( IN pml4t &pml4t ) noexcept :
            pmlxt { Level::PML4 } {
            this->pmlx_entry = pml4t.pmlx_entry;
        }
        explicit pml4t( VOID ) noexcept :
            pmlxt { Level::PML4 } {
        }
        virtual ~pml4t( VOID ) noexcept {
        }

    public:
        virtual auto flags_p( IN uint64_t index ) -> uint64_t override {
            return ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].p;
        }
        virtual auto flags_rw( IN uint64_t index ) -> uint64_t override {
            return ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].rw;
        }
        virtual auto flags_us( IN uint64_t index ) -> uint64_t override {
            return ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].us;
        }
        virtual auto flags_pcd( IN uint64_t index ) -> uint64_t override {
            return ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].pcd;
        }
        virtual auto flags_pwt( IN uint64_t index ) -> uint64_t override {
            return ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].pwt;
        }
        virtual auto flags_a( IN uint64_t index ) -> uint64_t override {
            return ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].a;
        }
        virtual auto flags_xd( IN uint64_t index ) -> uint64_t override {
            return ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].xd;
        }
        virtual auto flags_base( IN uint64_t index ) -> uint64_t override {
            return ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].base << PAGE_4K_SHIFT;
        }
        virtual auto flags_ps_pat( IN uint64_t index ) -> uint64_t override {
            return 0;
        }
        virtual auto set_p( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].p = bit;
        }
        virtual auto set_rw( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].rw = bit;
        }
        virtual auto set_us( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].us = bit;
        }
        virtual auto set_pcd( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].pcd = bit;
        }
        virtual auto set_pwt( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].pwt = bit;
        }
        virtual auto set_a( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].a = bit;
        }
        virtual auto set_xd( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].xd = bit;
        }
        virtual auto set_base( IN uint64_t index, IN uint64_t address ) -> VOID override {
            ( (pml4t_entry *)this->pmlx_entry[ Level::PML4 ] )[ index ].base = address >> PAGE_4K_SHIFT;
        }
        virtual auto set_ps_pat( [[maybe_unused]] IN uint64_t index, [[maybe_unused]] IN BOOL bit ) -> VOID override {
        }
        virtual auto operator=( IN std::tuple< uint64_t, uint64_t, uint64_t > group ) -> VOID override {
            auto &[ index, base, flags ] = group;
            this->set_p( index, !!( flags & PAGE_PRESENT ) );
            this->set_rw( index, !!( flags & PAGE_RW_W ) );
            this->set_us( index, !!( flags & PAGE_US_U ) );
            this->set_pwt( index, !!( flags & PAGE_PWT ) );
            this->set_pcd( index, !!( flags & PAGE_PCD ) );
            this->set_a( index, !!( flags & PAGE_ACCESSED ) );
            this->set_xd( index, !!( flags & PAGE_XD ) );
            this->set_base( index, base );
            this->set_ps_pat( index, !!( flags & PAGE_PAT ) );
        }
        virtual auto operator=( IN uint64_t table_address ) -> VOID override {
            this->pmlx_entry[ Level::PML4 ] = (uint64_t *)table_address;
        }
        virtual auto operator=( IN pml4t &pml4_t ) -> VOID {
            this->pmlx_entry[ Level::PML4 ] = (uint64_t *)pml4_t.pmlx_entry[ Level::PML4 ];
        }
        virtual auto get_table( VOID ) -> uint64_t * override {
            return this->pmlx_entry[ Level::PML4 ];
        }
        virtual auto get_address_index_in( IN VOID *address ) -> uint64_t override {
            return ( ( (uint64_t)address >> PAGE_4th_SHIFT ) & 0x1ff );
        }
        auto operator=( IN pml4t &&pml4 ) -> pml4t & {
            this->pmlx_entry = pml4.pmlx_entry;
            return *this;
        };
        auto operator=( IN CONST pml4t *pml4 ) -> pml4t & {
            this->pmlx_entry = pml4->pmlx_entry;
            return *this;
        };
        auto operator=( IN pml4t_entry *entry ) -> pml4t & {
            this->pmlx_entry[ Level::PML4 ] = (uint64_t *)entry;
            return *this;
        };
    };
}