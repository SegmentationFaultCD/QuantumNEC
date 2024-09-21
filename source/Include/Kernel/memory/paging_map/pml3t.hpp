#pragma once
#include <Kernel/memory/paging_map/pmlxt.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class pml3t : public pmlxt
    {
        friend pmlxt;
        // 三级页表有两种情况
        // 一种是当分页模式为2M时继续找二级页表
        // 一种是当分页模式为1G时放弃二级页表，直接从三级页表把内存基地址写入
    public:
        explicit pml3t( IN pml3t_entry *pml3t_address, IN MemoryPageType _mode ) noexcept :
            pmlxt { Level::PDPT }, mode { _mode } {
            this->pmlx_entry[ Level::PDPT ] = (uint64_t *)pml3t_address;
            this->pmlx_entry[ Level::PT ] = (uint64_t *)pml3t_address;
        }
        explicit pml3t( IN MemoryPageType _mode ) noexcept :
            pmlxt { Level::PDPT }, mode { _mode } {
        }
        virtual ~pml3t( VOID ) noexcept {
        }

    public:
        virtual auto flags_p( IN uint64_t index ) -> uint64_t override {
            return ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].p;
        }
        virtual auto flags_rw( IN uint64_t index ) -> uint64_t override {
            return ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].rw;
        }

        virtual auto flags_us( IN uint64_t index ) -> uint64_t override {
            return ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].us;
        }

        virtual auto flags_pcd( IN uint64_t index ) -> uint64_t override {
            return ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].pcd;
        }

        virtual auto flags_pwt( IN uint64_t index ) -> uint64_t override {
            return ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].pwt;
        }

        virtual auto flags_a( IN uint64_t index ) -> uint64_t override {
            return ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].a;
        }
        virtual auto flags_ps( IN uint64_t index ) -> uint64_t {
            return ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].ps;
        }
        virtual auto flags_xd( IN uint64_t index ) -> uint64_t override {
            return ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].xd;
        }
        virtual auto flags_base( IN uint64_t index ) -> uint64_t override {
            if ( mode == MemoryPageType::PAGE_1G ) {
                return ( (pml3t_huge_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].base << PAGE_1G_SHIFT;
            }
            else {
                return ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].base << PAGE_4K_SHIFT;
            }
        }

        virtual auto set_p( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].p = bit;
        }

        virtual auto set_rw( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].rw = bit;
        }

        virtual auto set_us( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].us = bit;
        }

        virtual auto set_pcd( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].pcd = bit;
        }

        virtual auto set_pwt( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].pwt = bit;
        }
        virtual auto set_a( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].a = bit;
        }

        virtual auto set_xd( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].xd = bit;
        }
        virtual auto set_ps( IN uint64_t index, IN BOOL bit ) -> VOID {
            ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].ps = bit;
        }

        virtual auto set_base( IN uint64_t index, IN uint64_t address ) -> VOID override {
            if ( mode == MemoryPageType::PAGE_1G ) {
                ( (pml3t_huge_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].base = address >> PAGE_1G_SHIFT;
            }
            else {
                ( (pml3t_entry *)this->pmlx_entry[ Level::PDPT ] )[ index ].base = address >> PAGE_4K_SHIFT;
            }
        }
        virtual auto operator=( IN std::tuple< uint64_t, uint64_t, uint64_t > group ) -> VOID {
            this->set_p( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_PRESENT ) );
            this->set_rw( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_RW_W ) );
            this->set_us( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_US_U ) );
            this->set_pwt( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_PWT ) );
            this->set_pcd( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_PCD ) );
            this->set_a( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_ACCESSED ) );
            if ( this->mode == MemoryPageType::PAGE_1G ) {
                this->set_ps( std::get< 0 >( group ), 1 );
            }
            this->set_xd( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_XD ) );
            this->set_base( std::get< 0 >( group ), std::get< 1 >( group ) );
        }
        virtual auto operator=( IN uint64_t table_address ) -> VOID {
            this->pmlx_entry[ Level::PDPT ] = (uint64_t *)table_address;
        }

        virtual auto get_table( VOID ) -> uint64_t * override {
            return this->pmlx_entry[ Level::PDPT ];
        }
        virtual auto get_address_index_in( IN VOID *address ) -> uint64_t {
            return ( ( (uint64_t)address >> PAGE_1G_SHIFT ) & 0x1ff );
        }
        auto operator=( IN pml3t_entry *entry ) -> pml3t & {
            this->pmlx_entry[ Level::PDPT ] = (uint64_t *)entry;
            return *this;
        }

    private:
        const MemoryPageType mode;
    };
}