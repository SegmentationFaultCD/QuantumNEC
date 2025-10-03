#include <kernel/syscall/module_loader/elf.hpp>
#include <kernel/syscall/module_loader/loader.hpp>
#include <kernel/syscall/services/filesystem.hpp>
#include <kernel/syscall/syscall.hpp>
#include <kernel/task/task.hpp>
namespace Kernel {
auto Loader::initialize( limine_module_response *modules ) -> Loader * {
    static Loader loader;
    Elf elf;
    std::uint64_t index { };

    // filesystem

    auto info = elf.load_elf_file( (std::uint64_t)modules->modules[ index++ ]->address );

    auto fs_interface = new FilesystemServcie {
        new Task::PCB {
            "filesystem",
            info.entry_offset,
            info.loadsegment_start,
            info.loadsegment_end - info.loadsegment_start }
    };
    syscall->register_syscall( 1, fs_interface );
    fs_interface->enable( );
    // 严格意义来说不算是模块

    return &loader;
}
}     // namespace Kernel