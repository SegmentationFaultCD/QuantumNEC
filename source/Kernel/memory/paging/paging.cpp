#include "Lib/Uefi.hpp"
#include <Kernel/memory/paging/paging.hpp>
#include <Kernel/memory/paging/page_table.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    pml1t kernel_l1_page_table { };
    pml2t kernel_l2_page_table { MemoryPageType::PAGE_2M };
    pml3t kernel_l3_page_table { MemoryPageType::PAGE_2M };
    pml4t kernel_l4_page_table { };
    pml5t kernel_l5_page_table { };
    __paging::__paging( VOID ) noexcept {
        using namespace Architecture;
        if ( Architecture::__config.paging_mode.mode == LIMINE_PAGING_MODE_X86_64_5LVL ) {
            kernel_l5_page_table = (pml5t_entry *)physical_to_virtual( ArchitectureManager< TARGET_ARCH >::get_page_table( ) );
            this->__kernel_page_table = &kernel_l5_page_table;
            this->support_5level_paging = TRUE;
        }
        else {
            kernel_l4_page_table = (pml4t_entry *)physical_to_virtual( ArchitectureManager< TARGET_ARCH >::get_page_table( ) );
            this->__kernel_page_table = &kernel_l4_page_table;
            this->support_5level_paging = FALSE;
        }
        __page_table { }.__page_protect( FALSE );
    }
}
