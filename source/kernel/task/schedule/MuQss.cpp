#include <kernel/task/schedule/MuQss.hpp>

namespace Task {
MuQss::MuQss( ) {
}
auto MuQss::schedule( void ) -> Interrupt::IDT::Frame * {}
auto MuQss::sleep( PCB * ) -> void {}
auto MuQss::wake_up( PCB * ) -> void {}

}     // namespace Task