#include <kernel/display/print.hpp>
#include <kernel/driver/cpu/io.hpp>
#include <kernel/driver/cpu/smp.hpp>
#include <kernel/driver/cpu/sse.hpp>
#include <kernel/interrupt/apic.hpp>
#include <kernel/interrupt/idt.hpp>
#include <kernel/memory/segment/gdt.hpp>
#include <kernel/syscall/syscall.hpp>
#include <kernel/task/task.hpp>
namespace Driver {
auto initialize_smp( limine_smp_response *smp ) -> void {
    for ( std::uint64_t i = 1; i < smp->cpu_count; ++i ) {
        smp->cpus[ i ]->goto_address = smp_start;
    }
}

[[noreturn]] auto smp_start( limine_smp_info *info ) -> void {
    Memory::gdt->initialize( info->lapic_id );
    Interrupt::idt->initialize( info->lapic_id );
    Driver::initialize_sse( );
    Interrupt::apic.initialize( false );
    Task::initialize_task( info->processor_id );
    Kernel::syscall->initialize( );
    Interrupt::idt->enable_interrupt( );

    while ( true );
}
}     // namespace Driver