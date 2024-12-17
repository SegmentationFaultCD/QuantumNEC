#include <kernel/interrupt/x86_64/entry/entry_apic_cmci.hpp>
#include <kernel/interrupt/x86_64/entry/entry_apic_error.hpp>
#include <kernel/interrupt/x86_64/entry/entry_apic_lint0.hpp>
#include <kernel/interrupt/x86_64/entry/entry_apic_lint1.hpp>
#include <kernel/interrupt/x86_64/entry/entry_apic_performance_counter.hpp>
#include <kernel/interrupt/x86_64/entry/entry_apic_thermal_monitor.hpp>
#include <kernel/interrupt/x86_64/entry/entry_apic_timer.hpp>
#include <kernel/interrupt/x86_64/entry/entry_cascade_timer.hpp>
#include <kernel/interrupt/x86_64/entry/entry_clock.hpp>
#include <kernel/interrupt/x86_64/entry/entry_rtc.hpp>
#include <kernel/interrupt/x86_64/entry/entry_smp_switch_cpu.hpp>
#include <kernel/interrupt/x86_64/entry/entry_syscall.hpp>
#include <kernel/interrupt/x86_64/entry/entry_trap.hpp>
#include <kernel/interrupt/x86_64/entry/register.hpp>
#include <kernel/print.hpp>
namespace QuantumNEC::Kernel::x86_64 {
/*  ------------------- Interrupts ------------------- */

ApicErrorEntry              apic_error_entry;
ApicLINT1Entry              apic_lint1_entry;
ApicLINT0Entry              apic_lint0_entry;
ApicCMCIEntry               apic_cmci_entry;
ApicTimerEntry              apic_timer_entry;
ApicPerformanceCounterEntry apic_performance_counter_entry;
ApicThermalMonitorEntry     apic_thermal_monitor_entry;

CascadeTimerEntry                      cascade_timer_entry;
ClockEntry                             clock_entry;
RTCEntry                               rtc_entry;
SymmetricMultiprocessingSwitchCPUEntry smp_switch_cpu_entry;
SystemcallEntry                        syscall_entry;

/*  ------------------- Exceptions ------------------- */

DivisionError                DE;        // Fault
Debug                        DB;        // Fault/Trap
NonMaskableInterrupt         NMI;       // Interrupt
Breakpoint                   BP;        // Trap
Overflow                     OF;        // Trap
BoundRangeExceeded           BR;        // Fault
InvalidOpcode                UD;        // Fault
DeviceNotAvailable           NM;        // Fault
DoubleFault                  DF;        // Abort
CoprocessorSegmentOverrun    CSO;       // Fault
InvalidTSS                   TS;        // Fault
SegmentNotPresent            NP;        // Fault
StackSegmentFault            SS;        // Fault
GeneralProtectionFault       GP;        // Fault
PageFault                    PF;        // Fault
X87FloatingPointException    MF;        // Fault
AlignmentCheck               AC;        // Fault
MachinCheck                  MC;        // Abort
SIMDFloatingPointException   XM_XF;     // Fault
VirtualizationException      VE;        // Fault
ControlProtectionException   CP;        // Fault
HypervisorInjectionException HV;        // Fault
VMMCommunicationException    VC;        // Fault
SecurityException            SX;        // Fault

InterruptEntryRegister::InterruptEntryRegister( void ) noexcept {
    this->entry[ 0 ]  = &DE;
    this->entry[ 1 ]  = &DB;
    this->entry[ 2 ]  = &NMI;
    this->entry[ 3 ]  = &BP;
    this->entry[ 4 ]  = &OF;
    this->entry[ 5 ]  = &BR;
    this->entry[ 6 ]  = &UD;
    this->entry[ 7 ]  = &NM;
    this->entry[ 8 ]  = &DF;
    this->entry[ 9 ]  = &CSO;
    this->entry[ 10 ] = &TS;
    this->entry[ 11 ] = &NP;
    this->entry[ 12 ] = &SS;
    this->entry[ 13 ] = &GP;
    this->entry[ 14 ] = &PF;
    this->entry[ 15 ] = NULL;
    this->entry[ 16 ] = &MF;
    this->entry[ 17 ] = &AC;
    this->entry[ 18 ] = &MC;
    this->entry[ 19 ] = &XM_XF;
    this->entry[ 20 ] = &VE;
    this->entry[ 21 ] = &CP;
    for ( auto i = 22; i < 28; ++i ) {
        this->entry[ i ] = NULL;
    }
    this->entry[ 28 ]                               = &HV;
    this->entry[ 29 ]                               = &VC;
    this->entry[ 30 ]                               = &SX;
    this->entry[ 31 ]                               = NULL;
    this->entry[ IRQ_CLOCK ]                        = &clock_entry;
    this->entry[ IRQ_CASCADE_TIMER ]                = &cascade_timer_entry;
    this->entry[ IRQ_CMOS_RTC ]                     = &rtc_entry;
    this->entry[ IRQ_SYSTEM_CALL ]                  = &syscall_entry;
    this->entry[ IRQ_APIC_LVT_CMCI ]                = &apic_cmci_entry;
    this->entry[ IRQ_APIC_LVT_TIMER ]               = &apic_timer_entry;
    this->entry[ IRQ_APIC_LVT_THERMAL_MONITOR ]     = &apic_thermal_monitor_entry;
    this->entry[ IRQ_APIC_LVT_PERFORMANCE_COUNTER ] = &apic_performance_counter_entry;
    this->entry[ IRQ_APIC_LVT_LINT0 ]               = &apic_lint1_entry;
    this->entry[ IRQ_APIC_LVT_LINT1 ]               = &apic_lint0_entry;
    this->entry[ IRQ_APIC_LVT_ERROR ]               = &apic_error_entry;
    this->entry[ IRQ_SMP_SWITCH_CPU ]               = &smp_switch_cpu_entry;

    for ( auto interrupt : this->entry ) {
        if ( interrupt ) {
            interrupt->do_register( );
        }
    }
}
}     // namespace QuantumNEC::Kernel::x86_64