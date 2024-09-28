#include <Modules/modules.hpp>
#include <Arch/Arch.hpp>
#include <Kernel/print.hpp>
#include <Kernel/task/task.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
PUBLIC namespace QuantumNEC::Modules {
    Modules::Module::Module( VOID ) noexcept {
        char_t name[ Kernel::TASK_NAME_SIZE ] { };
        // 首先将所有limine读入的文件进行解析
        // 使用limine读入的模块文件一律视为servicer
        ModuleLoader loader { };
        for ( auto i { 0 }; i < Architecture::__config.modules.module_count; ++i ) {
            auto file_entry = loader.load( Architecture::__config.modules.modules[ i ], ModuleLoader::ModuleFileType::ELF );
            auto j = strlen( Architecture::__config.modules.modules[ i ]->path ), k = 0ul;
            memset( name, 0, Kernel::TASK_NAME_SIZE );
            // 解析类型名
            for ( ; Architecture::__config.modules.modules[ i ]->path[ j ] != '/'; --j );
            j++;
            for ( ; Architecture::__config.modules.modules[ i ]->path[ j ] != '\0'; k++, j++ ) {
                name[ k ] = Architecture::__config.modules.modules[ i ]->path[ j ];
            }
            name[ k + 1 ] = '\0';
            println< ostream::HeadLevel::SYSTEM >( "Service {} ready!", name );
            // Kernel::Task::create< Kernel::Process >( name, 31, Kernel::TASK_FLAG_USER_PROCESS, (Kernel::TaskFunction)file_entry, 0 );
        }
        // TODO 挂载动态模块文件
    }
}