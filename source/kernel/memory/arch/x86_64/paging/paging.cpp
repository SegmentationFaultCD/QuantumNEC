#include <kernel/memory/arch/x86_64/paging/paging.hpp>
#include <kernel/memory/arch/x86_64/paging/ptv.hpp>
#include <kernel/memory/arch/x86_64/paging/page_table_walker.hpp>
#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/cpu/cpu.hpp>
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    pml1t kernel_l1_page_table { };
    pml2t kernel_l2_page_table { MemoryPageType::PAGE_2M };
    pml3t kernel_l3_page_table { MemoryPageType::PAGE_2M };
    pml4t kernel_l4_page_table { };
    pml5t kernel_l5_page_table { };
    Paging::Paging( VOID ) noexcept {
        if ( __config.paging_mode.mode == LIMINE_PAGING_MODE_X86_64_5LVL ) {
            kernel_l5_page_table = (pml5t_entry *)physical_to_virtual( CPU::get_page_table( ) );
            this->__kernel_page_table = &kernel_l5_page_table;
            this->support_5level_paging = TRUE;
        }
        else {
            kernel_l4_page_table = (pml4t_entry *)physical_to_virtual( CPU::get_page_table( ) );
            this->__kernel_page_table = &kernel_l4_page_table;
            this->support_5level_paging = FALSE;
        }
        PageTableWalker { }.page_protect( FALSE );
    }
}
