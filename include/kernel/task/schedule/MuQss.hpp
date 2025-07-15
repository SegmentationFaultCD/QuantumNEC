#pragma once
#include <cstdint>
#include <lib/list.hpp>
#include <lib/skiplist.hpp>
namespace Task {
class MuQss {
    // MuQSS - The Multiple Queue Skiplist Scheduler by Con Kolivas.
    // MuQSS is a per-cpu runqueue variant of the original BFS scheduler with
    // one 8 level skiplist per runqueue, and fine grained locking for much more
    // scalability.

public:
    constexpr static std::uint64_t rr_interval = 6;     // 6ms,
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

    // 由于MuQss没有动态优先级的概念，我们默认nice就是priority，与CFS，O1不同
    // nice默认为0，如要更改使用系统调用, 更改优先级，重新计算VD

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
    constexpr static std::int64_t prio_ratio[ 40 ] {
        /* -20 */ 88761, 71755, 56483, 46273, 36291,
        /* -15 */ 29154, 23254, 18705, 14949, 11916,
        /* -10 */ 9548, 7620, 6100, 4904, 3906,
        /*  -5 */ 3121, 2501, 1991, 1586, 1277,
        /*   0 */ 1024, 820, 655, 526, 423,
        /*   5 */ 335, 272, 215, 172, 137,
        /*  10 */ 110, 87, 70, 56, 45,
        /*  15 */ 36, 29, 23, 18, 15
        // 优先级数值越大优先级越低, vd越大
    };     // 其实这里和CFS是一样的

    // VD(Virtual Deadline) 计算公式为 niffies(纳秒级最小时间间隔计数) + (prio_ratio[nice] * rr_interval)
    /*
     * Niffies are a monotonic forward moving timer not unlike the "jiffies" but are
     * of nanosecond resolution. Niffies are calculated per-runqueue from the high
     * resolution TSC timers, and in order to maintain fairness are synchronised
     * between CPUs whenever both runqueues are locked concurrently.
     */
    // PS: niffies可以放hpet里面算

private:
    // 任务运行队列
    Library::List< std::int32_t > running_queue;
    // 任务调度队列
    Library::List< Library::Skiplist< std::int32_t, 8ul >[ 103 ] > scheduler_queue;
    // CPU链表
    // 1                                                            2                       3                       4
    // [0 ,     1,  ···,  100,        101,       102]
    // 0无就看1，1无就看2以此类推
    // 插入任务，哪个队列少就插哪个

public:
};
}     // namespace Task