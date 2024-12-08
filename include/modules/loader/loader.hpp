#pragma once
#include <lib/Uefi.hpp>
#include <modules/loader/elf.hpp>
namespace QuantumNEC::Modules {
class ModuleLoader :
    Elf {
public:
    enum class ModuleFileType {
        BIN,
        ELF,
        PE
    };

public:
    explicit ModuleLoader( void ) = default;

public:
    auto load( IN limine_file *file, IN ModuleFileType type ) -> std::expected< uint64_t, void >;
    auto load( IN void *path, IN ModuleFileType type ) -> std::expected< uint64_t, void >;
};
}     // namespace QuantumNEC::Modules