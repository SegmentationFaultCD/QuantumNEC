#pragma once
#include <lib/Uefi.hpp>
#include <concepts>
#include <type_traits>
#include <kernel/task/scheduler/bfs.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    template < typename SchedulerType >
        requires std::is_compound_v< SchedulerType >
                 && std::derived_from< SchedulerType, ScheduleSource >
                 && std::invocable< decltype( SchedulerType::wake_up ), PCB * >
                 && std::invocable< decltype( SchedulerType::sleep ) >
                 && std::invocable< decltype( SchedulerType::scheduler ) >
                 && std::invocable< decltype( SchedulerType::insert ), PCB * >
    class Scheduler : public SchedulerType
    {
    public:
        explicit Scheduler( VOID ) noexcept = default;
        virtual ~Scheduler( VOID ) noexcept = default;
    };
}
