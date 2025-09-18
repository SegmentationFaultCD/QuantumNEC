#pragma once
#include <kernel/task/schedule/scheduler.hpp>
namespace Kernel {
class Servicer {
public:
    auto enable( ) -> void;
    auto disable( ) -> void;

    Servicer( Task::PCB *pcb ) :
        service { pcb } {}

private:
    Task::PCB *service;
};

}     // namespace Kernel