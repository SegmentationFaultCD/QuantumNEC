#include <modules/loader/loader.hpp>

using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
namespace QuantumNEC::Modules {
auto Modules::ModuleLoader::ModuleLoader::load( IN limine_file *file, IN ModuleFileType type ) -> std::expected< uint64_t, void > {
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
auto Modules::ModuleLoader::ModuleLoader::load( IN void *, IN ModuleFileType ) -> std::expected< uint64_t, void > {
    // TODO :
    // 动态挂载
    return { };
}
}     // namespace QuantumNEC::Modules
