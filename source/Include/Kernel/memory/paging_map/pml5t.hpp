#pragma once
#include <Kernel/memory/paging_map/pmlxt.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class pml5t : public pmlxt
    {
        friend pmlxt;
        // 五级页表是最大的了
    public:
        explicit pml5t( IN pml5t_entry *pml5t_address ) noexcept :
            pmlxt { Level::PML5 } {
            this->pmlx_entry[ Level::PML5 ] = (uint64_t *)( pml5t_address );
        }

        explicit pml5t( IN pml5t &pml5t ) noexcept :
            pmlxt { Level::PML5 } {
            this->pmlx_entry = pml5t.pmlx_entry;
        }
        explicit pml5t( VOID ) noexcept :
            pmlxt { Level::PML5 } {
        }
        virtual ~pml5t( VOID ) noexcept {
        }

    public:
        virtual auto flags_p( IN uint64_t index ) -> uint64_t override {
            return ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].p;
        }
        virtual auto flags_rw( IN uint64_t index ) -> uint64_t override {
            return ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].rw;
        }
        virtual auto flags_us( IN uint64_t index ) -> uint64_t override {
            return ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].us;
        }
        virtual auto flags_pcd( IN uint64_t index ) -> uint64_t override {
            return ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].pcd;
        }
        virtual auto flags_pwt( IN uint64_t index ) -> uint64_t override {
            return ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].pwt;
        }
        virtual auto flags_a( IN uint64_t index ) -> uint64_t override {
            return ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].a;
        }
        virtual auto flags_xd( IN uint64_t index ) -> uint64_t override {
            return ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].xd;
        }
        virtual auto flags_base( IN uint64_t index ) -> uint64_t override {
            return ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].base << PAGE_4K_SHIFT;
        }
        virtual auto set_p( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].p = bit;
        }
        virtual auto set_rw( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].rw = bit;
        }
        virtual auto set_us( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].us = bit;
        }
        virtual auto set_pcd( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].pcd = bit;
        }
        virtual auto set_pwt( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].pwt = bit;
        }
        virtual auto set_a( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].a = bit;
        }
        virtual auto set_xd( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].xd = bit;
        }
        virtual auto set_base( IN uint64_t index, IN uint64_t address ) -> VOID override {
            ( (pml5t_entry *)this->pmlx_entry[ Level::PML5 ] )[ index ].base = address >> PAGE_4K_SHIFT;
        }
        virtual auto operator=( IN std::tuple< uint64_t, uint64_t, uint64_t > group ) -> VOID {
            this->set_p( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_PRESENT ) );
            this->set_rw( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_RW_W ) );
            this->set_us( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_US_U ) );
            this->set_pwt( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_PWT ) );
            this->set_pcd( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_PCD ) );
            this->set_a( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_ACCESSED ) );
            this->set_xd( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_XD ) );
            this->set_base( std::get< 0 >( group ), std::get< 1 >( group ) );
        }
        virtual auto operator=( IN uint64_t table_address ) -> VOID {
            this->pmlx_entry[ Level::PML5 ] = (uint64_t *)table_address;
        }

        virtual auto get_table( VOID ) -> uint64_t * override {
            return this->pmlx_entry[ Level::PML5 ];
        }
        virtual auto get_address_index_in( IN VOID *address ) -> uint64_t {
            return ( ( (uint64_t)address >> PAGE_5th_SHIFT ) & 0x1ff );
        }
        auto operator=( IN pml5t &&pml5 ) -> pml5t & {
            this->pmlx_entry = pml5.pmlx_entry;
            return *this;
        };
        auto operator=( IN CONST pml5t *pml5 ) -> pml5t & {
            this->pmlx_entry = pml5->pmlx_entry;
            return *this;
        };
        auto operator=( IN pml5t_entry *entry ) -> pml5t & {
            this->pmlx_entry[ Level::PML5 ] = (uint64_t *)entry;
            return *this;
        };
    };
}