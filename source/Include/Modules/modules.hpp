#pragma once
#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Modules {
    class ModuleService
    {
        enum class ModuleFileType {
            BIN,
            ELF,
            PE
        };

    public:
        explicit ModuleService( VOID ) noexcept;
        auto read_limine_module_file( IN uint64_t index, IN ModuleFileType module_type ) -> uint64_t;

    private:
    };
}