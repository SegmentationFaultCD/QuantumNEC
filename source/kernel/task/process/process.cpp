#include <kernel/memory/heap/kheap/kheap_walker.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/task/process/process.hpp>
namespace QuantumNEC::Kernel {
Process::Process( const char_t *name, uint64_t priority, void *entry, PCB::Flags::Type type ) noexcept {
    if ( type != PCB::Flags::Type::THREAD ) {
        PCB::Flags flags {
            .fpu_enable = PCB::Flags::Fpu::ENABLE,
            .fpu_used   = PCB::Flags::Fpu::USED,
            .task_type  = type,
        };
        this->pcb = new ( KHeapWalker { }.allocate( sizeof( PCB ) ) ) PCB {
            name,
            priority,
            flags,
            entry,
            0
        };
        this->has_inserted = false;
    }
}
Process::Process( const Modules::ModuleLoader::FileInformation &file, uint64_t priority, PCB::Flags::Type type ) noexcept {
    if ( type != PCB::Flags::Type::THREAD ) {
        PCB::Flags flags {
            .fpu_enable = PCB::Flags::Fpu::ENABLE,
            .fpu_used   = PCB::Flags::Fpu::USED,
            .task_type  = type,
        };
        if ( type == PCB::Flags::Type::KERNEL_PROCESS ) {
            this->pcb = new ( KHeapWalker { }.allocate( sizeof( PCB ) ) ) PCB {
                file.module_name,
                priority,
                flags,
                physical_to_virtual( file.executable_start ),
                0
            };
        }
        else {
            this->pcb = new ( KHeapWalker { }.allocate( sizeof( PCB ) ) ) PCB {
                file.module_name,
                priority,
                flags,
                (void *)this->__user_process_text_segment_start__,
                0
            };

            auto &upage_table = this->pcb->memory_manager.page_table;
            upage_table.map(
                file.executable_start,
                this->__user_process_text_segment_start__,
                Lib::DIV_ROUND_UP( file.executable_end - file.executable_start, PageAllocater::__page_size__< MemoryPageType::PAGE_4K > ),
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
    if ( !this->has_inserted ) {
        scheduler.insert( this->pcb->schedule );
        this->has_inserted = true;
    }

    scheduler.wake_up( this->pcb->schedule );
}
}     // namespace QuantumNEC::Kernel