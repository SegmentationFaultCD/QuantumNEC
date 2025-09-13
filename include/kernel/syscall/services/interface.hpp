#pragma once
#include <kernel/task/schedule/scheduler.hpp>
namespace Kernel {
class Servicer {
public:
    auto enable( ) {
    }
    auto disable( ) {
    }
    Servicer( Task::PCB *pcb ) :
        service { pcb } {}

private:
    Task::PCB *service;
};

}     // namespace Kernel