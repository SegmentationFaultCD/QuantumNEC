#pragma once
#include <Lib/Uefi.hpp>
#include <concepts>
#include <type_traits>
#include <Kernel/task/scheduler/bfs.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    template < typename SchedulerType >
        requires std::is_compound_v< SchedulerType > && requires( SchedulerType &sche ) {
            { sche.wake_up( (PCB *)0 ) } -> std::same_as< PCB * >;
            { sche.sleep( ) } -> std::same_as< PCB * >;
            { sche.schedule( ) } -> std::same_as< PCB * >;
            { sche.insert( (PCB *)0 ) } -> std::same_as< PCB * >;
        } && std::derived_from< SchedulerType, ScheduleSource >
    class Scheduler : public SchedulerType
    {
    public:
        explicit Scheduler( VOID ) noexcept = default;
        virtual ~Scheduler( VOID ) noexcept = default;
    };
    inline Scheduler< BrainFuckScheduler > scheduler { };
}
