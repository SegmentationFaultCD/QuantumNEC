#include <kernel/print.hpp>
#include <kernel/task/process/process.hpp>
#include <kernel/task/task.hpp>
#include <modules/modules.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
namespace QuantumNEC::Modules {
Modules::Module::Module( void ) noexcept {
    // 首先将所有limine读入的文件进行解析
    // 使用limine读入的模块文件一律视为servicer
    ModuleLoader loader { };

    for ( auto i { 0ul }; i < Kernel::__config.modules.module_count; ++i ) {
        auto file_entry = loader.load( Kernel::__config.modules.modules[ i ], ModuleLoader::ModuleFileType::ELF );
        if ( file_entry.has_value( ) ) {
            println< print_level::SYSTEM >( "Service {} ready!", Kernel::__config.modules.modules[ i ]->path );
            Kernel::Process servicer { file_entry.value( ), 1, Kernel::PCB::__flags__::__type__::USER_PROCESS };
            servicer.join( );
        }
        else {
            // TODO fault handler
        }
    }

    // TODO 挂载动态模块文件
}
}     // namespace QuantumNEC::Modules