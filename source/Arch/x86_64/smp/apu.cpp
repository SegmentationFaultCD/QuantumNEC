
#include <Arch/x86_64/platform/platform.hpp>
#include <Lib/spin_lock.hpp>
#include <Kernel/print.hpp>
using namespace QuantumNEC;
using namespace Architecture;
using namespace std;
PRIVATE Lib::SpinLock lock { };
PUBLIC [[noreturn]] auto Architecture::apu_entry( IN limine_smp_info *cpu ) -> VOID {
    println< ostream::HeadLevel::SYSTEM >( "Initialize the APU : {}", cpu->processor_id );
    // 挂载这个核心的GDT IDT TSS
    // 很重要所以必须加上锁
    lock.acquire( );
    // 减一是因为limine给的不是0，1，2号而是1，2，3号
    x86_64Architecture::CPUs::idt->load( cpu->processor_id );
    x86_64Architecture::CPUs::gdt->load( cpu->processor_id );
    x86_64Architecture::CPUs::gdt->tss[ cpu->processor_id ].load_tr( SELECTOR_TSS );
    x86_64Architecture::enable_x2apic( );
    lock.release( );
    while ( TRUE ) {
        CPUs::hlt( );
    }
}
