#include <kernel/print.hpp>
#include <kernel/task/process/process_creater.hpp>
#include <kernel/task/task.hpp>
#include <modules/modules.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
namespace QuantumNEC::Modules {
Modules::Module::Module( void ) noexcept {
    char_t name[ Kernel::TASK_NAME_SIZE ] { };
    // 首先将所有limine读入的文件进行解析
    // 使用limine读入的模块文件一律视为servicer
    ModuleLoader           loader { };
    Kernel::ProcessCreater creater { };

    for ( auto i { 0ul }; i < Kernel::__config.modules.module_count; ++i ) {
        auto file_entry = loader.load( Kernel::__config.modules.modules[ i ], ModuleLoader::ModuleFileType::ELF );
        if ( file_entry.has_value( ) ) {
            auto j = strlen( Kernel::__config.modules.modules[ i ]->path ), k = 0ul;
            memset( name, 0, Kernel::TASK_NAME_SIZE );
            // 解析类型名
            for ( ; Kernel::__config.modules.modules[ i ]->path[ j ] != '/'; --j );
            j++;
            for ( ; Kernel::__config.modules.modules[ i ]->path[ j ] != '\0'; k++, j++ ) {
                name[ k ] = Kernel::__config.modules.modules[ i ]->path[ j ];
            }
            name[ k + 1 ] = '\0';
            println< print_level::SYSTEM >( "Service {} ready!", name );

            auto service = creater.create( name, 1, (void *)file_entry.value( ), Kernel::PCB::Flags::Type::USER_PROCESS );
            service.value( )->memory_manager.page_table->map(
                (uint64_t)Kernel::x86_64::virtual_to_physical( service.value( )->context.pcontext->rip ),
                (uint64_t)service.value( )->context.pcontext->rip,
                10,
                service.value( )->memory_manager.page_table->PAGE_PRESENT | service.value( )->memory_manager.page_table->PAGE_RW_W | service.value( )->memory_manager.page_table->PAGE_US_U,
                Kernel::MemoryPageType::PAGE_2M );
        }
        else {
            // TODO fault handler
        }
    }
    // TODO 挂载动态模块文件
}
}     // namespace QuantumNEC::Modules