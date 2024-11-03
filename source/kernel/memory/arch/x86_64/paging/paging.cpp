#include <kernel/cpu/cpu.hpp>
#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/memory/arch/x86_64/paging/page_table_walker.hpp>
#include <kernel/memory/arch/x86_64/paging/paging.hpp>
#include <kernel/memory/arch/x86_64/paging/ptv.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    pml5t pml5_t;
    pml5t pml4_t;
    Paging::Paging( VOID ) noexcept {
        this->pml5_t = new ( &pml5_t ) pml5t { };
        this->pml4_t = new ( &pml4_t ) pml4t { };

        if ( __config.paging_mode.mode == LIMINE_PAGING_MODE_X86_64_5LVL ) {
            *this->pml5_t               = (pml5t_entry *)physical_to_virtual( CPU::get_page_table( ) );
            this->support_5level_paging = TRUE;
            this->kernel_page_table     = pml5_t;
        }
        else {
            *this->pml4_t               = (pml4t_entry *)physical_to_virtual( CPU::get_page_table( ) );
            this->support_5level_paging = FALSE;
            this->kernel_page_table     = pml4_t;
        }
        PageTableWalker { }.page_protect( FALSE );
    }
}
