#pragma once
#include <Kernel/memory/paging_map/pmlxt.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class pml1t : public pmlxt
    {
        friend pmlxt;

    public:
        explicit pml1t( IN pml1t_entry *pml1t_address ) noexcept :
            pmlxt { Level::PT } {
            this->pmlx_entry[ Level::PML4 ] = (uint64_t *)( pml1t_address );
        }

        explicit pml1t( IN pml1t &pml1t ) noexcept :
            pmlxt { Level::PML4 } {
            this->pmlx_entry = pml1t.pmlx_entry;
        }
        explicit pml1t( VOID ) noexcept :
            pmlxt { Level::PT } {
        }
        virtual ~pml1t( VOID ) noexcept {
        }

    public:
        virtual auto flags_p( IN uint64_t index ) -> uint64_t override {
            return ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].p;
        }
        virtual auto flags_rw( IN uint64_t index ) -> uint64_t override {
            return ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].rw;
        }
        virtual auto flags_us( IN uint64_t index ) -> uint64_t override {
            return ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].us;
        }
        virtual auto flags_pcd( IN uint64_t index ) -> uint64_t override {
            return ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pcd;
        }
        virtual auto flags_pwt( IN uint64_t index ) -> uint64_t override {
            return ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pwt;
        }
        virtual auto flags_a( IN uint64_t index ) -> uint64_t override {
            return ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].a;
        }
        virtual auto flags_xd( IN uint64_t index ) -> uint64_t override {
            return ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].xd;
        }
        virtual auto flags_base( IN uint64_t index ) -> uint64_t override {
            return ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].base << PAGE_4K_SHIFT;
        }
        virtual auto flags_ps_pat( IN uint64_t index ) -> uint64_t override {
            return ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pat;
        }
        virtual auto set_p( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].p = bit;
        }
        virtual auto set_rw( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].rw = bit;
        }
        virtual auto set_us( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].us = bit;
        }
        virtual auto set_pcd( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pcd = bit;
        }
        virtual auto set_pwt( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pwt = bit;
        }
        virtual auto set_a( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].a = bit;
        }
        virtual auto set_xd( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].xd = bit;
        }
        virtual auto set_base( IN uint64_t index, IN uint64_t address ) -> VOID override {
            ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].base = address >> PAGE_4K_SHIFT;
        }
        virtual auto set_ps_pat( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml1t_entry *)this->pmlx_entry[ Level::PT ] )[ index ].pat = bit;
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
            this->set_ps_pat( index, 0 );
        }
        virtual auto operator=( IN uint64_t table_address ) -> VOID override {
            this->pmlx_entry[ Level::PT ] = (uint64_t *)table_address;
        }
        virtual auto get_table( VOID ) -> uint64_t * override {
            return this->pmlx_entry[ Level::PT ];
        }
        virtual auto get_address_index_in( IN VOID *address ) -> uint64_t override {
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
        auto operator=( IN pml1t_entry *entry ) -> pml1t & {
            this->pmlx_entry[ Level::PT ] = (uint64_t *)entry;
            return *this;
        };
    };
}