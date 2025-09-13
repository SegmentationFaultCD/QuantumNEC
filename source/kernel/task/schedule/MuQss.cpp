#include <cmath>
#include <kernel/interrupt/apic.hpp>
#include <kernel/interrupt/hpet.hpp>
#include <kernel/task/schedule/MuQss.hpp>
namespace Task {
MuQss::MuQss( ) {
}
auto MuQss::schedule( void ) -> Interrupt::IDT::Frame * {
}
auto MuQss::sleep( PCB * ) -> void {
}
auto MuQss::wake_up( PCB * ) -> void {
}
auto MuQss::insert( PCB *pcb ) -> void {
    auto sched = pcb->schedule;
    // 时间片默认就是rr_interval值,但是可以改
    sched->time_slice = this->rr_interval;
    sched->cpu = Interrupt::apic.apic_id( );

    sched->nice = this->default_nice;
    sched->virtual_deadline = Interrupt::hpet->nano_time( ) + this->get_prio_ratio( this->default_prio_ratio ) * this->rr_interval;

    // 0 ~ 99 实时任务
    // 100 等时任务
    // 101 分时任务
    // 102 IDLE任务
    sched->priority = 101;

    // prio_ratio的计算要参考nice, nice有40个, nice每次增加1,

    // nice可以在系统调用里面改
}

}     // namespace Task