#pragma once

#include <kernel/task/general/scheduler/scheduler.hpp>
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
#include <lib/spin_lock.hpp>

PUBLIC namespace QuantumNEC::Kernel {
    // 该调度器思想由Con Kolivas发明
    // 面向桌面端设备，适用与较少的CPU
    class BrainFuckScheduler : public Scheduler {
        /**
         * 这个是CPU分配给每个任务的时间片，一个常数
         * 该值以毫秒为单位，默认值为 6ms。有效值为 1 至 1000。减小该值将减少延迟，但代价是降低吞吐量；增大
         * 该值将提高吞吐量，但代价是恶化延迟。rr 间隔的精度受限于内核配置的 HZ 分辨率。因此，最坏情况下的延
         * 迟通常略高于实际值。BFS 使用 "抖动 "来尽量减少 Hz 限制的影响。默认值 6 并不是一个任意的值。它基于这
         * 样一个事实，即人类可以在大约 7 毫秒的时间内检测到抖动，因此在大多数情况下，追求更低的延迟是毫无意
         * 义的。在比较 BFS 与其他调度器的延迟性能时，值得注意这一事实。实验表明，将 rr 间隔增加到 300 可以提高
         * 吞吐量，但超过这个间隔后，来自其他地方的调度噪声会阻碍吞吐量的进一步提高。
         */
        constexpr static auto rr_interval    = 6;
        constexpr static auto total_priority = 103;
        // 每个优先级对应的prio_ratios
        constexpr static uint64_t prio_ratios[ total_priority ] {
            128,
            141,
            155,
            171,
            188,
            207,
            228,
            251,
            276,
            304,
            334,
            367,
            404,
            444,
            488,
            537,
            591,
            650,
            715,
            787,
            866,
            953,
            1048,
            1153,
            1268,
            1395,
            1535,
            1689,
            1858,
            2044,
            2248,
            2473,
            2720,
            2992,
            3291,
            3620,
            3982,
            4380,
            4818,
            5300,
            5830,
            6413,
            7054,
            7759,
            8535,
            9389,
            10328,
            11361,
            12497,
            13747,
            15122,
            16634,
            18297,
            20127,
            22140,
            24354,
            26789,
            29468,
            32415,
            35657,
            39223,
            43145,
            47460,
            52206,
            57427,
            63170,
            69487,
            76436,
            84080,
            92488,
            101737,
            111911,
            123102,
            135412,
            148953,
            163848,
            180233,
            198256,
            218082,
            239890,
            263879,
            290267,
            319294,
            351223,
            386345,
            424980,
            467478,
            514226,
            565649,
            622214,
            684435,
            752879,
            828167,
            910984,
            1002082,
            1102290,
            1212519,
            1333771,
            1467148,
            1613863,
            1775249,
            1952774,
            2148051,
        };     // 每个优先级对应的偏移量

        /**
         * virtual deadline计算方法 global_jiffies + prio_ratios[priority] * rr_interval
         */

    public:
        enum Priority {
            // 0 ~ 99 为实时任务
            ISO          = 100,     // 等时任务
            NORMAL_BATCH = 101,     // 分时任务，普通任务
            IDLEPRIO     = 102,     // CPU 即将处于 IDLE 状态时才被调度的进程，低优先级任务
        };

    public:
        explicit BrainFuckScheduler( VOID ) = default;

        ~BrainFuckScheduler( VOID ) = default;

    public:
        friend Scheduler;

    private:
        auto __pick_next__( VOID ) -> std::expected< PCB *, ErrorCode >;
        // 任务插入
        auto __insert__( PCB *pcb ) -> std::expected< PCB *, ErrorCode >;
        // 任务唤醒
        auto __wake_up__( PCB *pcb ) -> std::expected< PCB *, ErrorCode >;
        // 任务睡眠
        auto __sleep__( VOID ) -> std::expected< PCB *, ErrorCode >;
        // 任务调度
        auto __schedule__( VOID ) -> std::expected< PCB *, ErrorCode >;

    private:
        // 存放除了running状态下的其他所有任务
        Lib::ListTable task_queue[ total_priority ] { };
        // 每个队列对应的位图
        BOOL bitmap[ total_priority ] { };
        // 全局锁
        Lib::SpinLock global_lock { };
        // 仅仅存放运行任务，数量为CPU个数
        Lib::ListTable running_queue { };
    };
}
