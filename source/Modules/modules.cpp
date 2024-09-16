#include <Modules/modules.hpp>
#include <Arch/Arch.hpp>
#include <Kernel/print.hpp>
#include <Kernel/task/task.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
Modules::ModuleService::ModuleService( VOID ) {
    uint64_t file_entry { };
    char_t name[ Kernel::TASK_NAME_SIZE ] { };
    // 首先将所有limine读入的文件进行解析
    // 使用limine读入的模块文件一律视为servicer
    Architecture::ArchitectureManager< TARGET_ARCH >::switch_to = (VOID *)read_limine_module_file( 0, ModuleFileType::BIN );
    Architecture::ArchitectureManager< TARGET_ARCH >::to_process = (VOID *)read_limine_module_file( 1, ModuleFileType::BIN );

    for ( auto i { 2ul }; i < Architecture::__config.modules.module_count; ++i ) {
        file_entry = read_limine_module_file( i, ModuleFileType::ELF );

        auto j = Lib::strlen( Architecture::__config.modules.modules[ i ]->path ), k = 0ul;
        memset( name, 0, Kernel::TASK_NAME_SIZE );
        for ( ; Architecture::__config.modules.modules[ i ]->path[ j ] != '/'; --j )
            ;
        j++;
        for ( ; Architecture::__config.modules.modules[ i ]->path[ j ] != '\0'; k++, j++ ) {
            name[ k ] = Architecture::__config.modules.modules[ i ]->path[ j ];
        }
        name[ k + 1 ] = '\0';
        println< ostream::HeadLevel::SYSTEM >( "Service {} ready!", name );
        Kernel::Task::create< Kernel::Process >( name, 31, Kernel::TASK_FLAG_USER_PROCESS, (Kernel::TaskFunction)file_entry, 0 );
    }
}
auto Modules::ModuleService::read_limine_module_file( IN uint64_t index, IN ModuleFileType module_type ) -> uint64_t {
    if ( index < Architecture::__config.modules.module_count ) {
        switch ( module_type ) {
        case ModuleFileType::ELF:
            if ( Architecture::ArchitectureManager< TARGET_ARCH >::check_elf_magic( (uint64_t)Architecture::__config.modules.modules[ index ]->address ) ) {
                auto entry = Architecture::ArchitectureManager< TARGET_ARCH >::relocate_elf( (uint64_t)Architecture::__config.modules.modules[ index ]->address );
                Kernel::Memory::memory_paging_map->map( (uint64_t)Kernel::virtual_to_physical( entry ), entry, 1, Kernel::PAGE_PRESENT | Kernel::PAGE_RW_W | Kernel::PAGE_US_U, Kernel::MemoryPageType::PAGE_2M );
                return entry;
            }
            else
                return 0;
        case ModuleFileType::BIN:
            // bin文件，不需要解析，读取出来的即是它入口函数地址
            return (uint64_t)Architecture::__config.modules.modules[ index ]->address;
        case ModuleFileType::PE: return 0;
        }
    }
    return 0;
}
