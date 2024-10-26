#include <modules/modules.hpp>
#include <kernel/print.hpp>
#include <kernel/task/task.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
PUBLIC namespace QuantumNEC::Modules {
    Modules::Module::Module( VOID ) noexcept {
        char_t name[ Kernel::TASK_NAME_SIZE ] { };
        // 首先将所有limine读入的文件进行解析
        // 使用limine读入的模块文件一律视为servicer
        ModuleLoader loader { };
        for ( auto i { 0ul }; i < Kernel::__config.modules.module_count; ++i ) {
            auto file_entry = loader.load( Kernel::__config.modules.modules[ i ], ModuleLoader::ModuleFileType::ELF );
            auto j = strlen( Kernel::__config.modules.modules[ i ]->path ), k = 0ul;
            memset( name, 0, Kernel::TASK_NAME_SIZE );
            // 解析类型名
            for ( ; Kernel::__config.modules.modules[ i ]->path[ j ] != '/'; --j );
            j++;
            for ( ; Kernel::__config.modules.modules[ i ]->path[ j ] != '\0'; k++, j++ ) {
                name[ k ] = Kernel::__config.modules.modules[ i ]->path[ j ];
            }
            name[ k + 1 ] = '\0';
            println< ostream::HeadLevel::SYSTEM >( "Service {} ready!", name );
            // Kernel::Task::create< Kernel::Process >( name, 31, Kernel::TASK_FLAG_USER_PROCESS, (Kernel::TaskFunction)file_entry, 0 );
        }
        // TODO 挂载动态模块文件
    }
}