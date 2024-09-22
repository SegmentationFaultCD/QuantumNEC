#pragma once
#include <Lib/Uefi.hpp>
#include <Kernel/task/pcb/pcb.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    class ScheduleSource
    {
    public:
        ScheduleSource( ) = default;
        virtual ~ScheduleSource( ) = default;

    public:
        virtual auto insert( IN PCB *pcb ) -> PCB * = 0;
        virtual auto sleep( ) -> PCB * = 0;
        virtual auto wake_up( PCB * ) -> PCB * = 0;
        virtual auto schedule( ) -> PCB * = 0;
    };
}