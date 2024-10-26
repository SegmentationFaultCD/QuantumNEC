#include <modules/loader/loader.hpp>

using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
PUBLIC namespace QuantumNEC::Modules {
    auto Modules::ModuleLoader::ModuleLoader::load( IN limine_file * file, IN ModuleFileType type ) -> std::expected< uint64_t, void > {
        using enum ModuleFileType;
        switch ( type ) {
        case ELF: {     // Elf文件解析
            auto resule = this->load_elf_file( (uint64_t)file->address );
            if ( resule.has_value( ) ) {
                return resule.value( );
            }
            switch ( resule.error( ) ) {
            case ElfErrorCode::MAGIC_IS_NOT_STANDARD:     // TODO : 错误处理
                break;
            }
            return { };
        }
        case BIN: {
            // bin文件，不需要解析，读取出来的即是它入口函数地址
            return (uint64_t)file->address;
        }
        case PE: {
            return { };
        }
        default:
            break;
        }
        return { };
    }
    auto Modules::ModuleLoader::ModuleLoader::load( IN VOID *, IN ModuleFileType ) -> std::expected< uint64_t, void > {
        // TODO :
        // 动态挂载
        return { };
    }
}
// auto Modules::ModuleService::read_limine_module_file( IN uint64_t index, IN ModuleFileType module_type ) -> uint64_t {
//     if ( index < Architecture::__config.modules.module_count ) {
//         switch ( module_type ) {
//         case ModuleFileType::ELF:
//             using arch = Architecture::ArchitectureManager< TARGET_ARCH >;
//             if ( arch::check_elf_magic( Architecture::__config.modules.modules[ index ]->address ) ) {
//                 auto entry = arch::load_elf_file( (uint64_t)Architecture::__config.modules.modules[ index ]->address );
//                 return entry;
//             }
//             else
//                 return 0;
//         case ModuleFileType::BIN:
//             // bin文件，不需要解析，读取出来的即是它入口函数地址
//             return (uint64_t)Architecture::__config.modules.modules[ index ]->address;
//         case ModuleFileType::PE: return 0;
//         }
//     }
//     return 0;
// }
