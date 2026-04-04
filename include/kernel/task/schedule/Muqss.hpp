#pragma once
#include <kernel/task/lock/spinlock.hpp>
#include <kernel/task/task.hpp>
#include <lib/queue>
namespace Task {
struct Schedule {
    std::uint64_t time_slice;

    enum class Priority : std::uint64_t {
        ISO = 100,
        NORMAL = 101,
        IDLEPRIO = 102
    };
    std::uint64_t priority;

    std::uint64_t nice;

    std::uint64_t virtual_deadline;

    std::uint64_t cpu;

    auto operator=( Schedule && ) -> Schedule & = default;
    Schedule( Schedule && ) = default;
    Schedule( ) = default;
};

struct Muqss {
    struct mCore {
        PCB running_task;
        PCB next;
        s_locks *lock;
        std::uint64_t cpu_id;
        struct compare {
            auto operator( )( const PCB &x, const PCB &y ) const -> bool;
        };
        struct compare_ptr {
            auto operator( )( const PCB *x, const PCB *y ) const -> bool;
        };

        mCore( ) :
            lock { new s_locks {} }, cpu_id { Interrupt::apic.apic_id( ) } {}
        mCore( mCore &&c ) :
            running_task { std::move( c.running_task ) }, next { std::move( c.next ) }, lock { new s_locks {} }, cpu_id { Interrupt::apic.apic_id( ) } {}
        auto operator=( mCore &&c ) -> mCore & {
            running_task = std::move( c.running_task );
            next = std::move( c.next );
            lock = new s_locks { };
            cpu_id = c.cpu_id;
            return *this;
        }

        ~mCore( ) {
            if ( this->lock ) {
                delete this->lock;
            }
        }
        std::priority_queue< PCB, std::cxxvector< PCB >, compare > scheduler_queue;
        std::cxxqueue< PCB > RT_task_queue;
    };

    friend auto initialize_task( std::uint64_t core ) -> void;
    // MuQSS - The Multiple Queue Skiplist Scheduler by Con Kolivas.
    // MuQSS is a per-cpu runqueue variant of the original BFS scheduler with
    // one 8 level skiplist per runqueue, and fine grained locking for much more
    // scalability.

public:
    constexpr static auto rr_interval = 6ul;     // 6ms,这个一般作为时间片填充
    /*
     *  The value is in milliseconds, and the default value is set to 6. Valid values
     *  are from 1 to 1000 Decreasing the value will decrease latencies at the cost of
     *  decreasing throughput, while increasing it will improve throughput, but at the
     *  cost of worsening latencies. It is based on the fact that humans can detect
     *  jitter at approximately 7ms, so aiming for much lower latencies is pointless
     *  under most circumstances. It is worth noting this fact when comparing the
     *  latency performance of MuQSS to other schedulers. Worst case latencies being
     *  higher than 7ms are far worse than average latencies not being in the
     *  microsecond range.
     */

    // nice默认为0，如要更改使用系统调用, 更改优先级，重新计算VD
    constexpr static auto default_nice = 0;
    // nice有40个
    constexpr static auto min_nice = -20;
    constexpr static auto max_nice = 40;

    // Virtual deadline:

    // The key to achieving low latency, scheduling fairness, and "nice level"
    // distribution in MuQSS is entirely in the virtual deadline mechanism. The one
    // tunable in MuQSS is the rr_interval, or "round robin interval". This is the
    // maximum time two SCHED_OTHER (or SCHED_NORMAL, the common scheduling policy)
    // tasks of the same nice level will be running for, or looking at it the other
    // way around, the longest duration two tasks of the same nice level will be
    // delayed for. When a task requests cpu time, it is given a quota (time_slice)
    // equal to the rr_interval and a virtual deadline. The virtual deadline is
    // offset from the current time in niffies by this equation:
    //     niffies + (prio_ratio * rr_interval)
    // The prio_ratio is determined as a ratio compared to the baseline of nice -20
    // and increases by 10% per nice level. The deadline is a virtual one only in that
    // no guarantee is placed that a task will actually be scheduled by this time, but
    // it is used to compare which task should go next. There are three components to
    // how a task is next chosen. First is time_slice expiration. If a task runs out
    // of its time_slice, it is descheduled, the time_slice is refilled, and the
    // deadline reset to that formula above. Second is sleep, where a task no longer
    // is requesting CPU for whatever reason. The time_slice and deadline are _not_
    // adjusted in this case and are just carried over for when the task is next
    // scheduled. Third is preemption, and that is when a newly waking task is deemed
    // higher priority than a currently running task on any cpu by virtue of the fact
    // that it has an earlier virtual deadline than the currently running task. The
    // earlier deadline is the key to which task is next chosen for the first and
    // second cases.

    constexpr static double min_prio_ratio = 1.0;     // 静态优先级在时间片计算的权重

    auto get_prio_ratio( std::uint64_t nice ) {
        auto prio_ratio = this->min_prio_ratio;
        for ( auto i = 1; i <= nice - this->min_nice; ++i ) {
            prio_ratio *= 1.1;
        }
        return prio_ratio;
    }

    // VD(Virtual Deadline) 计算公式为 niffies(纳秒级最小时间间隔计数) + (prio_ratio * rr_interval)

    auto get_virtual_deadline( std::uint64_t now_time, double prio_ratio ) {
        return now_time + prio_ratio * this->rr_interval;
    }

    /*
     * Niffies are a monotonic forward moving timer not unlike the "jiffies" but are
     * of nanosecond resolution. Niffies are calculated per-runqueue from the high
     * resolution TSC timers, and in order to maintain fairness are synchronised
     * between CPUs whenever both runqueues are locked concurrently.
     */
    // PS: niffies可以放hpet里面算

    auto schedule( void ) -> void;
    auto yield( ) -> void;
    auto wake_up( PCB &&pcb ) -> void;

public:
    Muqss( void ) = default;
    ~Muqss( void ) = default;

    auto first_initialize( PCB & ) -> void;

    static inline bool interactive = false;
};
}     // namespace Task
