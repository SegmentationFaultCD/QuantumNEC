#pragma once
#include <lib/Uefi.hpp>
#include <modules/loader/elf.hpp>
namespace QuantumNEC::Modules {
class ModuleLoader :
    Elf {
public:
    using Elf::FileInformation;

public:
    enum class ModuleFileType {
        BIN,
        ELF,
        PE
    };
    enum class ErrorCode {
        FORMAT_ERROR,
        FORMAT_DOESNT_SUPPORT
            
    };

public:
    explicit ModuleLoader( void ) = default;

public:
    auto load( IN limine_file *file, IN ModuleFileType type ) -> std::expected< FileInformation, ErrorCode >;
    auto load( IN void *path, IN ModuleFileType type ) -> std::expected< FileInformation, ErrorCode >;
};
}     // namespace QuantumNEC::Modules