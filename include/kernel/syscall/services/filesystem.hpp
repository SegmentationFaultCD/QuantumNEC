#pragma once
#include <kernel/syscall/services/interface.hpp>
#include <kernel/syscall/syscall.hpp>
#include <kernel/task/task.hpp>
namespace Kernel {
class FilesystemServcie final : public Syscall::Interface, public Servicer {
public:
    virtual auto handle( Interrupt::IDT::Frame *frame ) -> Interrupt::IDT::Frame * override;
    virtual ~FilesystemServcie( void ) {}

    FilesystemServcie( Task::PCB *pcb ) :
        Servicer { pcb } {
    }
};
}     // namespace Kernel