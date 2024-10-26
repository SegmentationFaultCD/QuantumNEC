#pragma once
#include <lib/Uefi.hpp>
#include <modules/loader/elf.hpp>
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