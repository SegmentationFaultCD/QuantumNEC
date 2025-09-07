#pragma once
#include <kernel/syscall/syscall.hpp>
#include <kernel/task/task.hpp>
namespace Kernel {
class FilesystemServcie final : public Syscall::Interface {
public:
    virtual auto handle( Interrupt::IDT::Frame *frame ) -> Interrupt::IDT::Frame * override;
    virtual ~FilesystemServcie( void ) {}

    FilesystemServcie( Task::PCB *pcb ) :
        service { pcb } {
    }

private:
    Task::PCB *service;
};
}     // namespace Kernel