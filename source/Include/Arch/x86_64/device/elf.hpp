#pragma once
#include <Lib/Uefi.hpp>
#include <Arch/x86_64/platform/global.hpp>
PUBLIC namespace QuantumNEC::Architecture {
    class Elf
    {
    public:
        using Elf64_Address = uint64_t;
        using Elf64_Offset = uint64_t;
        using Elf64_Half = uint16_t;
        using Elf64_Word = uint32_t;
        using Elf64_Sword = int32_t;
        using Elf64_Xword = uint64_t;
        using Elf64_Sxword = int64_t;

    private:
        struct ElfHeader
        {
            Elf64_Word e_Magic;
            uint8_t e_Format;
            uint8_t e_Endianness;
            uint8_t e_Version;
            uint8_t e_OSAbi;
            uint8_t e_AbiVersion;
            uint8_t e_Rserved[ 7 ];
            Elf64_Half e_Type;
            Elf64_Half e_Machine;
            Elf64_Word e_ElfVersion;
            Elf64_Xword e_Entry;
            Elf64_Xword e_Phoff;
            Elf64_Xword e_Shoff;
            Elf64_Word e_Flags;
            Elf64_Half e_HeadSize;
            Elf64_Half e_PHeadSize;
            Elf64_Half e_PHeadCount;
            Elf64_Half e_SHeadSize;
            Elf64_Half e_SHeadCount;
            Elf64_Half e_SNameIndex;
        };

        struct ProgramHeaderTable
        {
            Elf64_Word p_type;
            Elf64_Word p_flags;
            Elf64_Offset p_offset;
            Elf64_Address p_vaddr;
            Elf64_Address p_paddr;
            Elf64_Xword p_filesz;
            Elf64_Xword p_memsz;
            Elf64_Xword p_align;
        } _packed;

        struct Relocation
        {
            Elf64_Address r_offset;
            Elf64_Xword r_info;
            Elf64_Sxword r_addend;
        } _packed;

        struct SectionHeaderEntries
        {
            Elf64_Word sh_name;
            Elf64_Word sh_type;
            Elf64_Xword sh_flags;
            Elf64_Address sh_addr;
            Elf64_Offset sh_offset;
            Elf64_Xword sh_size;
            Elf64_Word sh_link;
            Elf64_Word sh_info;
            Elf64_Xword sh_addralign;
            Elf64_Xword sh_entsize;
        } _packed;

        struct SymbolTable
        {
            Elf64_Word st_name;
            uint8_t st_info;
            uint8_t st_other;
            Elf64_Half st_shndx;
            Elf64_Address st_value;
            Elf64_Xword st_size;
        } _packed;

        struct DynamicTable
        {
            Elf64_Xword d_tag;
            union {
                Elf64_Xword d_val;
                Elf64_Address d_ptr;
            } d_un;
        } _packed;

        struct SymbolTableInformation
        {
            SectionHeaderEntries *shdr;
            size_t shdr_count;
            SymbolTable *sym_entries;
            size_t sym_count;
            char *sym_str_table;
            size_t sym_str_table_size;
            char *sect_str_table;
            size_t sect_str_table_size;
        };

    public:
        explicit Elf( VOID ) = default;

    public:
        STATIC auto load_elf_file( IN uint64_t address ) -> uint64_t;
        STATIC auto check_elf_magic( IN VOID *Ehdr ) -> BOOL;
    };
    // class FileHeaderService : Elf
    // {
    //     enum class ModuleFileType {
    //         BIN,
    //         ELF,
    //         PE
    //     };

    // public:
    //     explicit FileHeaderService( VOID ) noexcept;
    //     auto read_file( IN uint64_t index, IN ModuleFileType module_type ) -> uint64_t;

    // private:
    // };
}