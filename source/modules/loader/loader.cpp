#include <kernel/print.hpp>
#include <modules/loader/loader.hpp>
using namespace QuantumNEC;
using namespace QuantumNEC::Lib;
using namespace std;
namespace QuantumNEC::Modules {
auto Modules::ModuleLoader::ModuleLoader::load( IN limine_file *file, IN ModuleFileType type ) -> std::expected< FileInformation, ErrorCode > {
    using enum ModuleFileType;
    switch ( type ) {
    case ELF: {
        // Elf formatted file load.
        auto resule = this->load_elf_file( (uint64_t)file->address );
        if ( resule.has_value( ) ) {
            resule.value( ).size        = file->size;
            resule.value( ).module_name = file->path;
            return resule.value( );
        }
        switch ( resule.error( ) ) {
        case ElfErrorCode::MAGIC_IS_NOT_STANDARD:
            std::println< std::print_level::ERROR >( "File {} doesn't conform to elf formatted, cannot load.", file->path );
            break;
        }
        return std::unexpected { ErrorCode::FORMAT_ERROR };
    }
    case BIN: {
        // Entire binary file
        // The start address is the main function entry.
        return FileInformation {
            .module_name      = file->path,
            .size             = file->size,
            .executable_start = (uint64_t)file->address,
            .executable_end   = (uint64_t)file->address + file->size
        };
    }
    case PE: {
        return std::unexpected { ErrorCode::FORMAT_ERROR };
    }
    default:
        break;
    }
    return std::unexpected { ErrorCode::FORMAT_DOESNT_SUPPORT };
}
auto Modules::ModuleLoader::ModuleLoader::load( IN void *, IN ModuleFileType ) -> std::expected< FileInformation, ErrorCode > {
    // TODO :
    // 动态挂载
    return { };
}
}     // namespace QuantumNEC::Modules
