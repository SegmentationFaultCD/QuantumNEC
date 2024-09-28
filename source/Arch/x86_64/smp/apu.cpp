#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/spin_lock.hpp>
#include <Kernel/print.hpp>
using namespace QuantumNEC;
using namespace Architecture;
using namespace std;
PRIVATE Lib::SpinLock lock { };
PUBLIC [[noreturn]] auto Architecture::apu_entry( IN limine_smp_info *cpu ) -> VOID {
    lock.acquire( );
    // 挂载这个核心的GDT IDT TSS
    // 很重要所以必须加上锁
    x86_64Architecture::CPUs::idt->load( );
    x86_64Architecture::CPUs::gdt->load( );
    x86_64Architecture::CPUs::gdt->tss[ cpu->processor_id ].load_tr( SELECTOR_TSS );
    x86_64Architecture::enable_x2apic( );
    lock.release( );
    println< ostream::HeadLevel::SYSTEM >( "Initialize the APU : {}", cpu->processor_id );

    CPUs::sti( );

    while ( TRUE ) {
        CPUs::hlt( );
        println< ostream::HeadLevel::SYSTEM >( "APU {} Report!", cpu->processor_id );
    }
}
