#include <kernel/syscall/services/filesystem.hpp>

namespace Kernel {
auto FilesystemServcie::handle( Interrupt::IDT::Frame *frame ) -> Interrupt::IDT::Frame * {
    
    // 进程间通信
    return frame;
}


}     // namespace Kernel