#include <kernel/interrupt/arch/x86_64/entry/idt.hpp>
#include <kernel/cpu/cpu.hpp>
#include <kernel/print.hpp>
#include <lib/spin_lock.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Kernel::x86_64;
using namespace std;
using namespace Lib;
PRIVATE SpinLock lock { };

extern "C" auto do_divide_error( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    // 显示中断名

    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_debug( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_nmi( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> CONST InterruptDescriptorTable::InterruptFrame * {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
    return frame;
}
extern "C" auto do_int3( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_overflow( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_bounds( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {} PID -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_undefined_opcode( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {} PID -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}

extern "C" auto do_dev_not_available( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {} PID -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_double_fault( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {} PID -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_coprocessor_segment_overrun( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {} PID -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_invalid_TSS( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x} : {}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t Error code -> {:x}\n"
                                          "\t\t\t RIP -> {} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}\n"
                                          "\t\t\t {}\n"
                                          "\t\t\t {}"
                                          "\t\t\t Segment Selector Index : {:x}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->error_code,
                                          frame->rip,
                                          frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ),
                                          ( frame->error_code & 0x01 ) ? "The exception occurred during delivery of an event external to the program,such as an interrupt or an earlier exception." : ( ( frame->error_code & 0x02 ) ? "Refers to a gate descriptor in the IDT." : "Refers to a descriptor in the GDT or the current LDT." ),
                                          ( !( frame->error_code & 0x02 ) ) ? ( ( frame->error_code & 0x04 ) ? "Refers to a segment or gate descriptor in the LDT.\n" : "Refers to a descriptor in the current GDT.\n" ) : "\r",
                                          frame->error_code & 0xfff8 );
}
extern "C" auto do_segment_not_present( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x} : {}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t Error code -> {:x}\n"
                                          "\t\t\t RIP -> {} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}\n"
                                          "\t\t\t {}\n"
                                          "\t\t\t {}"
                                          "\t\t\t Segment Selector Index : {:x}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->error_code,
                                          frame->rip,
                                          frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ),
                                          ( frame->error_code & 0x01 ) ? "The exception occurred during delivery of an event external to the program,such as an interrupt or an earlier exception." : ( ( frame->error_code & 0x02 ) ? "Refers to a gate descriptor in the IDT." : "Refers to a descriptor in the GDT or the current LDT." ),
                                          ( !( frame->error_code & 0x02 ) ) ? ( ( frame->error_code & 0x04 ) ? "Refers to a segment or gate descriptor in the LDT.\n" : "Refers to a descriptor in the current GDT.\n" ) : "\r",
                                          frame->error_code & 0xfff8 );
}
extern "C" auto do_stack_segment_fault( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x} : {}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t Error code -> {:x}\n"
                                          "\t\t\t RIP -> {} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}\n"
                                          "\t\t\t {}\n"
                                          "\t\t\t {}"
                                          "\t\t\t Segment Selector Index : {:x}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->error_code,
                                          frame->rip,
                                          frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ),
                                          ( frame->error_code & 0x01 ) ? "The exception occurred during delivery of an event external to the program,such as an interrupt or an earlier exception." : ( ( frame->error_code & 0x02 ) ? "Refers to a gate descriptor in the IDT." : "Refers to a descriptor in the GDT or the current LDT." ),
                                          ( !( frame->error_code & 0x02 ) ) ? ( ( frame->error_code & 0x04 ) ? "Refers to a segment or gate descriptor in the LDT.\n" : "Refers to a descriptor in the current GDT.\n" ) : "\r",
                                          frame->error_code & 0xfff8 );
}
extern "C" auto do_general_protection( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x} : {}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t Error code -> {:x}\n"
                                          "\t\t\t RIP -> {} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}\n"
                                          "\t\t\t {}\n"
                                          "\t\t\t {}"
                                          "\t\t\t Segment Selector Index : {:x}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->error_code,
                                          frame->rip,
                                          frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ),
                                          ( frame->error_code & 0x01 ) ? "The exception occurred during delivery of an event external to the program,such as an interrupt or an earlier exception." : ( ( frame->error_code & 0x02 ) ? "Refers to a gate descriptor in the IDT." : "Refers to a descriptor in the GDT or the current LDT." ),
                                          ( !( frame->error_code & 0x02 ) ) ? ( ( frame->error_code & 0x04 ) ? "Refers to a segment or gate descriptor in the LDT.\n" : "Refers to a descriptor in the current GDT.\n" ) : "\r",
                                          frame->error_code & 0xfff8 );
}
extern "C" auto do_page_fault( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2(PFLA) -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}\n"
                                          "\t\t\t {}{}{}{}{}{}{}{}.",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ),
                                          ( !( frame->error_code & ( 1 << 0 ) ) ) ? "Page Not-Present " : "",
                                          ( frame->error_code & ( 1 << 1 ) ) ? "Write Cause Fault " : "Read Cause Fault ",
                                          ( frame->error_code & ( 1 << 2 ) ) ? "Fault in user(3) " : "Fault in supervisor(0,1,2) ",
                                          ( frame->error_code & ( 1 << 3 ) ) ? "Reserved Bit Cause Fault " : "",
                                          ( frame->error_code & ( 1 << 4 ) ) ? "Instruction fetch Cause Fault " : "",
                                          ( frame->error_code & ( 1 << 5 ) ) ? "Protection Key Cause Fault " : "",
                                          ( frame->error_code & ( 1 << 6 ) ) ? "Shadow Stack Cause Fault" : "",
                                          ( frame->error_code & ( 1 << 7 ) ) ? "Software Guard Extensions Cause Fault" : "" );
}
extern "C" auto do_x87_FPU_error( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_alignment_check( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_machine_check( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_SIMD_exception( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_virtualization_exception( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_control_protection_exception( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_exception_injected_by_the_virtual_machine( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_VMM_communication_failed( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
extern "C" auto do_security_exception( IN CONST InterruptDescriptorTable::InterruptFrame *frame ) -> VOID {
    ControlRegisterFrame control_registers_frame { };
    println< ostream::HeadLevel::ERROR >( "IRQ_{:x}:{}\n"
                                          "\t\t\t Rflags -> {:x}\n"
                                          "\t\t\t RIP -> {:x} RSP -> {:x} SS -> {:x} CS -> {:x}\n"
                                          "\t\t\t RAX -> {:x} RBX -> {:x} RCX -> {:x} RDX -> {:x}\n"
                                          "\t\t\t R8  -> {:x} R9  -> {:x} R10 -> {:x} R11 -> {:x} R12 -> {:x} R13 -> {:x} R14 -> {:x} R15 -> {:x}\n"
                                          "\t\t\t RDI -> {:x} RSI -> {:x} RBP -> {:x}\n"
                                          "\t\t\t DS -> {:x} ES -> {:x} FS -> {:x} GS -> {:x}\n"
                                          "\t\t\t CR0 -> {:x} CR2 -> {:x} CR3 -> {:x} CR4 -> {:x} CR8 -> {:x}\n"
                                          "\t\t\t CPU -> {}",
                                          frame->vector,
                                          InterruptDescriptorTable::interrupt_name[ frame->vector ],
                                          frame->rflags,
                                          frame->rip, frame->rsp, frame->ss, frame->cs,
                                          frame->regs.rax, frame->regs.rbx, frame->regs.rcx, frame->regs.rdx,
                                          frame->regs.r8, frame->regs.r9,
                                          frame->regs.r10,
                                          frame->regs.r11,
                                          frame->regs.r12,
                                          frame->regs.r13,
                                          frame->regs.r14,
                                          frame->regs.r15,
                                          frame->regs.rdi, frame->regs.rsi, frame->regs.rbp,
                                          frame->regs.ds, frame->regs.es, frame->regs.fs, frame->regs.gs,
                                          *( (uint64_t *)&control_registers_frame.cr0 ), control_registers_frame.cr2.PFLA, *( (uint64_t *)&control_registers_frame.cr3 ), *( (uint64_t *)&control_registers_frame.cr4 ), *( (uint64_t *)&control_registers_frame.cr8 ),
                                          Apic::apic_id( ) );
}
