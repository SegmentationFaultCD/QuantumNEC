#pragma once
#include <concepts>
#include <lib/Uefi.hpp>
#include <libcxx/expected.hpp>
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

template < typename SchedulerType, typename BindFunction >
concept standard_scheduler_interface = conform_to_scheduler_standard< SchedulerType > && requires( BindFunction f, SchedulerType::view v ) {
    { f( v ) } -> std::same_as< typename SchedulerType::view >;
};

template < typename SchedulerType, typename BindFunction >
    requires standard_scheduler_interface< SchedulerType, BindFunction >
class scheduler_inserter {
public:
    explicit scheduler_inserter( void ) = delete;

    ~scheduler_inserter( ) {
    }

    explicit scheduler_inserter( BindFunction insert_interface ) :
        inserter { insert_interface } {
    }

    auto operator( )( typename SchedulerType::view block ) {
        return inserter( block );
    }

private:
    BindFunction inserter;
};
template < typename SchedulerType, typename BindFunction >
    requires standard_scheduler_interface< SchedulerType, BindFunction >
class scheduler_wakeuper {
public:
    explicit scheduler_wakeuper( void ) = delete;

    ~scheduler_wakeuper( ) {
    }

    explicit scheduler_wakeuper( BindFunction insert_interface ) :
        inserter { insert_interface } {
    }

    auto operator( )( typename SchedulerType::view block ) {
        return inserter( block );
    }

private:
    BindFunction inserter;
};
template < typename SchedulerType, typename BindFunction >
    requires standard_scheduler_interface< SchedulerType, BindFunction >
class scheduler_hanger {
public:
    explicit scheduler_hanger( void ) = delete;

    ~scheduler_hanger( ) {
    }

    explicit scheduler_hanger( BindFunction insert_interface ) :
        inserter { insert_interface } {
    }

    auto operator( )( typename SchedulerType::view block ) {
        return inserter( block );
    }

private:
    BindFunction inserter;
};
template < typename SchedulerType, typename BindFunction >
    requires standard_scheduler_interface< SchedulerType, BindFunction >
class scheduler_remover {
public:
    explicit scheduler_remover( void ) = delete;

    ~scheduler_remover( ) {
    }

    explicit scheduler_remover( BindFunction insert_interface ) :
        inserter { insert_interface } {
    }

    auto operator( )( typename SchedulerType::view block ) {
        return inserter( block );
    }

private:
    BindFunction inserter;
};
}     // namespace QuantumNEC::Kernel