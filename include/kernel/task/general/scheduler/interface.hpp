#pragma once
#include <concepts>
#include <expected>
#include <lib/Uefi.hpp>
#include <libcxx/functional.hpp>
namespace QuantumNEC::Kernel {
class SchedulerInterface;

template < typename Scheduler >
concept conform_to_scheduler_standard = std::derived_from< Scheduler, SchedulerInterface >
                                        && requires {
                                               typename Scheduler::Schedule;
                                               typename Scheduler::view;
                                           };     // must derived from base class __Scheduler__

class SchedulerInterface {
public:
    explicit SchedulerInterface( void ) = default;
    ~SchedulerInterface( void )         = default;

public:
    auto insert( this auto &&self, auto &schedule ) {
        return self.__insert__( schedule );
    }
    auto hang( this auto &&self, auto &schedule ) {
        self.__hang__( schedule );
    }
    auto wake_up( this auto &&self, auto &schedule ) noexcept {
        return self.__wake_up__( schedule );
    }
    auto remove( this auto &&self, auto &schedule ) {
        self.__remove__( schedule );
    }

public:
    auto search( this auto &&self, uint64_t priority, uint64_t ID ) {
        return self.__search__( priority, ID );
    }
    auto search( this auto &&self, uint64_t ID ) {
        return self.__search__( ID );
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
};
template < typename SchedulerType >
class scheduler_utils {
public:
    explicit scheduler_utils( void ) = delete;

    ~scheduler_utils( ) {
    }
    template < typename F >
    explicit scheduler_utils( F &&interface ) :
        f { std::forward< F >( interface ) } {
    }

    auto operator( )( typename SchedulerType::view block ) {
        return f( block );
    }

private:
    std::function< typename SchedulerType::view, typename SchedulerType::view > f;
};

}     // namespace QuantumNEC::Kernel