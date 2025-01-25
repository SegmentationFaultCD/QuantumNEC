#include <kernel/cpu/cpu.hpp>
#include <kernel/cpu/smp/x86_64/x86_64smp.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/print.hpp>
#include <kernel/syscall/syscall.hpp>
#include <kernel/task/task.hpp>
#include <lib/spinlock.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Kernel;
using namespace std;

[[noreturn]] auto x86_64::apu_entry( IN limine_smp_info *cpu ) -> void {
    Lib::spinlock lock { };
    println< print_level::SYSTEM >( "Initialize the APU : {}", cpu->processor_id );
    // 挂载这个核心的GDT IDT TSS
    // 很重要所以必须加上锁
    lock.acquire( );
    Interrupt::idt->load( Interrupt::cpu_id( ) );
    Memory::gdt->load( Interrupt::cpu_id( ) );
    ProcessManager::main_process_install( Interrupt::cpu_id( ) );
    Memory::gdt->tss[ Interrupt::cpu_id( ) ].load_tr( SELECTOR_TSS );

    Interrupt::enable_x2apic( );
    Sse::activate( );     // 激活SSE
    Syscall::initializate( );

    lock.release( );

    Interrupt::enable_interrupt( );
    CPU::hlt( );
    while ( true ) {
    }
}
