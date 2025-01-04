#pragma once
#include <concepts>
#include <lib/Uefi.hpp>
#include <libcxx/expected.hpp>
namespace QuantumNEC::Kernel {
class SchedulerInterface;

template < typename Scheduler >
concept conform_to_scheduler_standard = std::derived_from< Scheduler, SchedulerInterface >;     // must derived from base class __Scheduler__

class SchedulerInterface {
public:
    explicit SchedulerInterface( void ) = default;
    ~SchedulerInterface( void )         = default;

public:
    auto insert( this auto &&self, auto &schedule ) {
        return self.__insert__( schedule );
    }
    auto hang( this auto &&self, auto &schedule ) {
        self.__hang__( );
    }
    auto wake_up( this auto &&self, auto &schedule ) noexcept {
        return self.__wake_up__( schedule );
    }

private:
    auto pick_next( this auto &&self ) {
        return self.__pick_next__( );
    }

public:
    // Use the function in ISR
    // You shouldn't use the function in any place except in ISR
    auto schedule( this auto &&self ) {
        return self.__schedule__( );
    }
    auto remove( this auto &&self, auto &schedule ) {
        self.__remove__( schedule );
    }
};

}     // namespace QuantumNEC::Kernel