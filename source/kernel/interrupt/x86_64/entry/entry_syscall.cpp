#include <kernel/cpu/cpu.hpp>
#include <kernel/global/x86_64/global.hpp>
#include <kernel/interrupt/x86_64/entry/entry_syscall.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/pic/pic.hpp>
#include <kernel/print.hpp>
#include <kernel/syscall/syscall.hpp>
using namespace QuantumNEC;
namespace QuantumNEC::Kernel::x86_64 {

auto SystemcallEntry::name( void ) noexcept -> void {
}
auto SystemcallEntry::error_code( [[maybe_unused]] uint64_t error_code ) noexcept -> void {
}
auto SystemcallEntry::handler( Frame *frame ) noexcept -> Frame * {
    Apic::eoi( frame->vector );

    constexpr auto shared_pages_size = 1;

    auto current  = ProcessControlBlock::get_running_task( );
    auto servicer = const_cast< ProcessControlBlock * >( Syscall::get_servicer( Syscall::Servicer( frame->regs.rax ) ).raw_control_block( ) );

    KHeapAllocator< Service::Order > order_allocater;
    auto                             order = std::allocator_traits< KHeapAllocator< Service::Order > >::allocate( order_allocater, 1 );

    if ( servicer->schedule.state == Scheduler::Schedule::State::RECEIVING ) {
        // 还未有任何请求的时候
        PageAllocator< MemoryPageType::PAGE_2M > allocater { };
        auto                                     shared_space = std::allocator_traits< PageAllocator< MemoryPageType::PAGE_2M > >::allocate( allocater, shared_pages_size );

        std::allocator_traits< KHeapAllocator< Service::Order > >::construct(
            order_allocater,
            order,
            shared_space,
            current->PID );

        current->services.send( order, &servicer->services );

        // 映射共享内存到固定的地方
        auto map_shared_memory = [ &shared_space, &shared_pages_size ]( ProcessControlBlock *pcb ) {
            pcb->memory_manager.page_table.map(
                (uint64_t)shared_space,
                Process::__user_process_shared_memory_space_start__,
                shared_pages_size,
                pcb->memory_manager.page_table.PAGE_PRESENT | pcb->memory_manager.page_table.PAGE_RW_W | pcb->memory_manager.page_table.PAGE_US_U,
                MemoryPageType::PAGE_2M );
        };

        map_shared_memory( current );
        map_shared_memory( servicer );
    }
    else {
        // 阻塞当前进程, 加入等候队列
    }
    return frame;
}
auto SystemcallEntry::do_register( void ) -> void {
    Apic::IOApicRedirectionEntry entry { };
    entry.vector         = IRQ_SYSTEM_CALL;
    entry.deliver_mode   = APIC_ICR_IOAPIC_FIXED;
    entry.dest_mode      = ICR_IOAPIC_DELV_PHYSICAL;
    entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
    entry.polarity       = APIC_IOAPIC_POLARITY_HIGH;
    entry.irr            = APIC_IOAPIC_IRR_RESET;
    entry.trigger        = APIC_ICR_IOAPIC_EDGE;
    entry.mask           = APIC_ICR_IOAPIC_MASKED;
    Apic::install_ioapic( IRQ_SYSTEM_CALL, &entry );
    Apic::enable_ioapic( IRQ_SYSTEM_CALL );
}

}     // namespace QuantumNEC::Kernel::x86_64
