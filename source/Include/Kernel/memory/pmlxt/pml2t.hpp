#pragma once
#include <Kernel/memory/pmlxt/pmlxt.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC class pml2t : public pmlxt
    {
        friend pmlxt;
        // 三级页表有两种情况
        // 一种是当分页模式为2M时继续找二级页表
        // 一种是当分页模式为1G时放弃二级页表，直接从三级页表把内存基地址写入
    public:
        explicit pml2t( IN pml2t_entry *pml2t_address, IN MemoryPageType _mode ) noexcept :
            pmlxt { Level::PD }, mode { _mode } {
            this->pmlx_entry[ Level::PD ] = (uint64_t *)pml2t_address;
        }
        explicit pml2t( IN MemoryPageType _mode ) noexcept :
            pmlxt { Level::PD }, mode { _mode } {
        }
        virtual ~pml2t( VOID ) noexcept {
        }

    public:
        virtual auto flags_p( IN uint64_t index ) -> uint64_t override {
            return ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].p;
        }
        virtual auto flags_rw( IN uint64_t index ) -> uint64_t override {
            return ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].rw;
        }

        virtual auto flags_us( IN uint64_t index ) -> uint64_t override {
            return ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].us;
        }

        virtual auto flags_pcd( IN uint64_t index ) -> uint64_t override {
            return ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].pcd;
        }

        virtual auto flags_pwt( IN uint64_t index ) -> uint64_t override {
            return ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].pwt;
        }

        virtual auto flags_a( IN uint64_t index ) -> uint64_t override {
            return ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].a;
        }
        virtual auto flags_ps( IN uint64_t index ) -> uint64_t {
            return ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].ps;
        }
        virtual auto flags_xd( IN uint64_t index ) -> uint64_t override {
            return ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].xd;
        }
        virtual auto flags_base( IN uint64_t index ) -> uint64_t override {
            if ( mode == MemoryPageType::PAGE_2M ) {
                return ( (pml2t_huge_entry *)this->pmlx_entry[ Level::PD ] )[ index ].base << PAGE_2M_SHIFT;
            }
            else {
                return ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].base << PAGE_4K_SHIFT;
            }
        }

        virtual auto set_p( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].p = bit;
        }

        virtual auto set_rw( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].rw = bit;
        }

        virtual auto set_us( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].us = bit;
        }

        virtual auto set_pcd( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].pcd = bit;
        }

        virtual auto set_pwt( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].pwt = bit;
        }
        virtual auto set_a( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].a = bit;
        }
        virtual auto set_ps( IN uint64_t index, IN BOOL bit ) -> VOID {
            ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].ps = bit;
        }
        virtual auto set_xd( IN uint64_t index, IN BOOL bit ) -> VOID override {
            ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].xd = bit;
        }

        virtual auto set_base( IN uint64_t index, IN uint64_t address ) -> VOID override {
            if ( mode == MemoryPageType::PAGE_2M ) {
                ( (pml2t_huge_entry *)this->pmlx_entry[ Level::PD ] )[ index ].base = address >> PAGE_2M_SHIFT;
            }
            else {
                ( (pml2t_entry *)this->pmlx_entry[ Level::PD ] )[ index ].base = address >> PAGE_4K_SHIFT;
            }
        }
        virtual auto operator=( IN std::tuple< uint64_t, uint64_t, uint64_t > group ) -> VOID {
            this->set_p( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_PRESENT ) );
            this->set_rw( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_RW_W ) );
            this->set_us( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_US_U ) );
            this->set_pwt( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_PWT ) );
            this->set_pcd( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_PCD ) );
            this->set_a( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_ACCESSED ) );
            if ( mode == MemoryPageType::PAGE_2M ) {     // 2M分配，那就是一个大页
                this->set_ps( std::get< 0 >( group ), 1 );
            }
            this->set_xd( std::get< 0 >( group ), !!( std::get< 2 >( group ) & PAGE_XD ) );
            this->set_base( std::get< 0 >( group ), std::get< 1 >( group ) );
        }
        virtual auto operator=( IN uint64_t table_address ) -> VOID {
            this->pmlx_entry[ Level::PD ] = (uint64_t *)table_address;
        }

        virtual auto get_table( VOID ) -> uint64_t * override {
            return this->pmlx_entry[ Level::PD ];
        }
        virtual auto get_address_index_in( IN VOID *address ) -> uint64_t {
            return ( ( (uint64_t)address >> PAGE_2M_SHIFT ) & 0x1ff );
        }
        auto operator=( IN pml2t_entry *entry ) -> pml2t & {
            this->pmlx_entry[ Level::PD ] = (uint64_t *)entry;
            return *this;
        }

    private:
        const MemoryPageType mode;     // 2M或者4K
    };
}