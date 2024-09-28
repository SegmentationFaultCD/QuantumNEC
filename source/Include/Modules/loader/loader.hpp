#pragma once
#include "Libcxx/expected.hpp"
#include <Lib/Uefi.hpp>
#include <Modules/loader/elf.hpp>
PUBLIC namespace QuantumNEC::Modules {
    PUBLIC class ModuleLoader :
        Elf
    {
    public:
        enum class ModuleFileType {
            BIN,
            ELF,
            PE
        };

    public:
        explicit ModuleLoader( VOID ) = default;

    public:
        auto load( IN limine_file *file, IN ModuleFileType type ) -> std::expected< uint64_t, void >;
        auto load( IN VOID *path, IN ModuleFileType type ) -> std::expected< uint64_t, void >;
    };
}