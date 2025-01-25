#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/task/process/process.hpp>
namespace QuantumNEC::Kernel {
Process::Process( const char_t *name, uint64_t priority, void *entry, PCB::__flags__::__type__ type ) noexcept {
    if ( type != PCB::__flags__::__type__::THREAD ) {
        PCB::__flags__ flags {
            .fpu_enable = PCB::__flags__::__fpu_state__::ENABLE,
            .fpu_used   = PCB::__flags__::__fpu_state__::USED,
            .task_type  = type,
        };
        KHeapAllocator< PCB > pcb_allocator;
        this->pcb = std::allocator_traits< decltype( pcb_allocator ) >::allocate( pcb_allocator, 1 );
        std::allocator_traits< decltype( pcb_allocator ) >::construct( pcb_allocator, this->pcb,
                                                                       name,
                                                                       priority,
                                                                       flags,
                                                                       entry,
                                                                       0ul );
        this->has_inserted = false;
    }
}
Process::Process( const Modules::ModuleLoader::FileInformation &file, uint64_t priority, PCB::__flags__::__type__ type ) noexcept {
    if ( type != PCB::__flags__::__type__::THREAD ) {
        PCB::__flags__ flags {
            .fpu_enable = PCB::__flags__::__fpu_state__::ENABLE,
            .fpu_used   = PCB::__flags__::__fpu_state__::USED,
            .task_type  = type,
        };
        KHeapAllocator< PCB > pcb_allocator;

        if ( type == PCB::__flags__::__type__::KERNEL_PROCESS ) {
            this->pcb = std::allocator_traits< decltype( pcb_allocator ) >::allocate( pcb_allocator, 1 );
            std::allocator_traits< decltype( pcb_allocator ) >::construct( pcb_allocator, this->pcb,
                                                                           file.module_name,
                                                                           priority,
                                                                           flags,
                                                                           physical_to_virtual( file.executable_start ),
                                                                           0ul );
        }
        else {
            this->pcb = std::allocator_traits< decltype( pcb_allocator ) >::allocate( pcb_allocator, 1 );
            std::allocator_traits< decltype( pcb_allocator ) >::construct( pcb_allocator, this->pcb,
                                                                           file.module_name,
                                                                           priority,
                                                                           flags,
                                                                           (void *)this->__user_process_text_segment_start__,
                                                                           0ul );

            auto &upage_table = this->pcb->memory_manager.page_table;
            upage_table.map(
                file.executable_start,
                this->__user_process_text_segment_start__,
                Lib::DIV_ROUND_UP( file.executable_end - file.executable_start, PageAllocator< MemoryPageType::PAGE_4K >::__page_size__ ),
                upage_table.PAGE_PRESENT | upage_table.PAGE_RW_W | upage_table.PAGE_US_U,
                MemoryPageType::PAGE_4K );
        }

        this->has_inserted = false;
    }
}
Process::Process( Process &&process ) noexcept {
    this->pcb   = process.pcb;
    process.pcb = NULL;
}
auto Process::join( void ) noexcept -> void {
    Scheduler scheduler;
    auto      pcb_view = scheduler | this->pcb->schedule;
    if ( !this->has_inserted ) {
        pcb_view | scheduler_inserter { brain_fuck_scheduler_insert };
        this->has_inserted = true;
    }
    pcb_view | scheduler_wakeuper { brain_fuck_scheduler_wake_up };
    std::println( "{:x}", (void *)this->pcb->schedule.cpu_id );
    std::println( "{:x}", (void *)this->pcb->schedule.jiffies );
    std::println( "{:x}", (void *)this->pcb->schedule.virtual_deadline );
}
}     // namespace QuantumNEC::Kernel