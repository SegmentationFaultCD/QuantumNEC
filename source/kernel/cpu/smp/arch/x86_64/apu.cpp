#include <kernel/cpu/cpu.hpp>
#include <kernel/cpu/smp/arch/x86_64/smp.hpp>
#include <kernel/interrupt/interrupt.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/print.hpp>
#include <lib/spin_lock.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Kernel;
using namespace std;
PRIVATE Lib::SpinLock    lock { };
PUBLIC [[noreturn]] auto x86_64::apu_entry( IN limine_smp_info *cpu ) -> VOID {
    // println< ostream::HeadLevel::SYSTEM >( "Initialize the APU : {}", cpu->processor_id );
    // 挂载这个核心的GDT IDT TSS
    // 很重要所以必须加上锁
    lock.acquire( );
    Interrupt::idt->load( cpu->processor_id );
    // Memory::gdt->load( cpu->processor_id );
    // Memory::gdt->tss[ cpu->processor_id ].load_tr( SELECTOR_TSS );
    Interrupt::enable_x2apic( );
    Sse::activate_sse( );     // 激活FPU
    lock.release( );
    while ( TRUE ) {
        CPU::hlt( );
    }
}
