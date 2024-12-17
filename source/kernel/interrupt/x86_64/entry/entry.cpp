#include <kernel/interrupt/x86_64/entry/entry.hpp>
#include <kernel/interrupt/x86_64/entry/idt.hpp>
#include <kernel/interrupt/x86_64/entry/register.hpp>
#include <kernel/interrupt/x86_64/pic/apic.hpp>
#include <kernel/print.hpp>
#include <kernel/task/general/pcb/pcb.hpp>
using namespace QuantumNEC;
namespace QuantumNEC::Kernel::x86_64 {
using namespace std;
auto InterruptEntry::registers( Frame *frame ) noexcept -> void {
    ControlRegisterFrame control_registers_frame { };
    println< print_level::ERROR >( "Rflags -> {:x}\n"
                                   "\t\t\t RIP -> {} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                   "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                   "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                   "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                   "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                   "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}",
                                   frame->rflags,
                                   (void *)frame->rip,
                                   (uint64_t)frame->rsp, (uint64_t)frame->ss, (uint64_t)frame->cs,
                                   (uint64_t)frame->regs.rax, (uint64_t)frame->regs.rbx, (uint64_t)frame->regs.rcx, (uint64_t)frame->regs.rdx,
                                   (uint64_t)frame->regs.r8, (uint64_t)frame->regs.r9,
                                   (uint64_t)frame->regs.r10,
                                   (uint64_t)frame->regs.r11,
                                   (uint64_t)frame->regs.r12,
                                   (uint64_t)frame->regs.r13,
                                   (uint64_t)frame->regs.r14,
                                   (uint64_t)frame->regs.r15,
                                   (uint64_t)frame->regs.rdi, (uint64_t)frame->regs.rsi, (uint64_t)frame->regs.rbp,
                                   (uint64_t)frame->regs.ds, (uint64_t)frame->regs.es, (uint64_t)frame->regs.fs, (uint64_t)frame->regs.gs,
                                   *( (uint64_t *)&control_registers_frame.cr0 ),
                                   control_registers_frame.cr2.PFLA,
                                   *( (uint64_t *)&control_registers_frame.cr3 ),
                                   *( (uint64_t *)&control_registers_frame.cr4 ),
                                   *( (uint64_t *)&control_registers_frame.cr8 ) );
}
auto InterruptEntry::cpu( void ) noexcept -> void {
    ControlRegisterFrame control_registers_frame { };
    println< print_level::ERROR >( "CPU -> {}", Apic::apic_id( ) );
}
auto InterruptEntry::task( void ) noexcept -> void {
}
auto InterruptEntry::error_code( uint64_t error_code ) noexcept -> void {
    ErrorCode code { error_code };

    if ( code.external ) {
        std::println< print_level::ERROR >( "The exception originated externally to the processor." );
    }
    const char *tbl;
    switch ( code.tbl ) {
    case 0b00:
        std::println< print_level::ERROR >( "The selector index references a descriptor in the GDT. " );
        tbl = R"(GDT)";
        break;
    case 0b01:
    case 0b11:
        std::println< print_level::ERROR >( "The selector index references a descriptor in the IDT. " );
        tbl = R"(IDT)";
        break;
    case 0b10:
        std::println< print_level::ERROR >( "The selector index references a descriptor in the LDT. " );
        tbl = R"(LDT)";
        break;
    default:
        break;
    }
    std::println< print_level::ERROR >( "The selector index in the {} is {}.", tbl, (uint32_t)code.index );
}

_C_LINK _asmcall auto do_IRQ( IN InterruptDescriptorTable::InterruptFrame *frame ) -> IN InterruptDescriptorTable::InterruptFrame * {
    if ( auto entry = InterruptEntryRegister::entry[ frame->vector ]; entry ) {
        return entry->do_IRQ( frame );
    };
    return frame;
}
}     // namespace QuantumNEC::Kernel::x86_64
