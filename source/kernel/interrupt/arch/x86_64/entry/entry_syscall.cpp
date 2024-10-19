#include <kernel/interrupt/arch/x86_64/entry/entry.hpp>
#include <kernel/print.hpp>
#include <kernel/global/arch/x86_64/global.hpp>
#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/interrupt/arch/x86_64/pic/pic.hpp>
#include <kernel/syscall/syscall.hpp>
using namespace QuantumNEC;
PUBLIC namespace QuantumNEC::Kernel::x86_64 {
    _C_LINK auto save_current_frame( IN CONST InterruptDescriptorTable::InterruptFrame * frame ) -> VOID;

    _C_LINK auto _asmcall syscall_entry( IN CONST InterruptDescriptorTable::InterruptFrame * frame, IN uint64_t ) -> CONST InterruptDescriptorTable::InterruptFrame * {
        // if ( !( Kernel::get_current< Kernel::Process >( )->flags ) ) {
        //     save_current_frame( frame );
        // }
        auto entry = Syscall::get_syscall_table( )[ frame->regs.rax ];
        if ( !entry ) {
            const_cast< InterruptDescriptorTable::InterruptFrame * >( frame )->regs.rax = static_cast< uint64_t >( Syscall::Status::NO_SYSCALL );
            return frame;
        }

        return entry( const_cast< InterruptDescriptorTable::InterruptFrame * >( frame ) );
    }

    SystemcallEntry::SystemcallEntry( VOID ) noexcept {
        InterruptDescriptorTable::InterruptFunctionController controller {
            .install = Apic::install_ioapic,
            .uninstall = Apic::uninstall_ioapic,
            .enable = Apic::enable_ioapic,
            .disable = Apic::disable_ioapic,
            .ack = Apic::eoi
        };

        Apic::IOApicRedirectionEntry entry { };
        entry.vector = IRQ_SYSTEM_CALL;
        entry.deliver_mode = APIC_ICR_IOAPIC_FIXED;
        entry.dest_mode = ICR_IOAPIC_DELV_PHYSICAL;
        entry.deliver_status = APIC_ICR_IOAPIC_IDLE;
        entry.polarity = APIC_IOAPIC_POLARITY_HIGH;
        entry.irr = APIC_IOAPIC_IRR_RESET;
        entry.trigger = APIC_ICR_IOAPIC_EDGE;
        entry.mask = APIC_ICR_IOAPIC_MASKED;
        InterruptDescriptorTable::register_irq( IRQ_SYSTEM_CALL, &entry, syscall_entry, 0, "SYSTEM CALL", &controller );
    }
}
