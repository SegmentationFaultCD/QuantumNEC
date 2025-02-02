#pragma once
#include <expected>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Modules {
class Elf {
public:
    constexpr static uint32_t ELF_MAGIC = 0x464C457F;     // 0x7f454c46; // ELF Magic as one unit

    constexpr static uint16_t ELF_ET_NONE       = 0x0000;
    constexpr static uint16_t ELF_ET_REL        = 0x0001;
    constexpr static uint16_t ELF_ET_EXEC       = 0x0002;
    constexpr static uint16_t ELF_ET_DYN        = 0x0003;
    constexpr static uint16_t ELF_ET_CORE       = 0x0004;
    constexpr static uint16_t ELF_ET_LOOS       = 0xFE00;
    constexpr static uint16_t ELF_ET_HIOS       = 0xFEFF;
    constexpr static uint16_t ELF_ET_LOPROC     = 0xFF00;
    constexpr static uint16_t ELF_ET_HIPROC     = 0xFFFF;
    constexpr static uint8_t  ELF_EI_MAG0       = 0;
    constexpr static uint8_t  ELF_EI_MAG1       = 1;
    constexpr static uint8_t  ELF_EI_MAG2       = 2;
    constexpr static uint8_t  ELF_EI_MAG3       = 3;
    constexpr static uint8_t  ELF_EI_CLASS      = 4;
    constexpr static uint8_t  ELF_EI_DATA       = 5;
    constexpr static uint8_t  ELF_EI_VERSION    = 6;
    constexpr static uint8_t  ELF_EI_OSABI      = 7;
    constexpr static uint8_t  ELF_EI_ABIVERSION = 8;
    constexpr static uint8_t  ELF_EI_PAD        = 9;
    constexpr static uint8_t  ELF_EI_NIDENT     = 0x10;

    constexpr static uint32_t ELF_PT_NULL    = 0;
    constexpr static uint32_t ELF_PT_LOAD    = 1;
    constexpr static uint32_t ELF_PT_DYNAMIC = 2;
    constexpr static uint32_t ELF_PT_INTERP  = 3;
    constexpr static uint32_t ELF_PT_NOTE    = 4;
    constexpr static uint32_t ELF_PT_SHLIB   = 5;
    constexpr static uint32_t ELF_PT_PHDR    = 6;
    constexpr static uint32_t ELF_PT_LOOS    = 0x60000000;
    constexpr static uint32_t ELF_PT_HIOS    = 0x6fffffff;
    constexpr static uint32_t ELF_PT_LOPROC  = 0x70000000;
    constexpr static uint32_t ELF_PT_HIPROC  = 0x7fffffff;

    constexpr static uint32_t ELF_SHT_NULL     = 0;
    constexpr static uint32_t ELF_SHT_PROGBITS = 1;
    constexpr static uint32_t ELF_SHT_SYMTAB   = 2;
    constexpr static uint32_t ELF_SHT_STRTAB   = 3;
    constexpr static uint32_t ELF_SHT_RELA     = 4;
    constexpr static uint32_t ELF_SHT_HASH     = 5;
    constexpr static uint32_t ELF_SHT_DYNAMIC  = 6;
    constexpr static uint32_t ELF_SHT_NOTE     = 7;
    constexpr static uint32_t ELF_SHT_NOTBITS  = 8;
    constexpr static uint32_t ELF_SHT_REL      = 9;
    constexpr static uint32_t ELF_SHT_SHLIB    = 10;
    constexpr static uint32_t ELF_SHT_DYNSYM   = 11;
    constexpr static uint32_t ELF_SHT_LOPROC   = 0x70000000;
    constexpr static uint32_t ELF_SHT_HIPROC   = 0x7fffffff;
    constexpr static uint32_t ELF_SHT_LOUSER   = 0x80000000;
    constexpr static uint32_t ELF_SHT_HIUSER   = 0xffffffff;

    constexpr static uint32_t ELF_PF_X        = 0x1;
    constexpr static uint32_t ELF_PF_W        = 0x2;
    constexpr static uint32_t ELF_PF_R        = 0x4;
    constexpr static uint32_t ELF_PF_MASKOS   = 0x00ff0000;
    constexpr static uint32_t ELF_PF_MASKPROC = 0xff000000;

    constexpr static uint8_t ELF_ELFCLASS32 = 1;
    constexpr static uint8_t ELF_ELFCLASS64 = 2;

    constexpr static uint64_t ELF_DT_NULL    = 0x00000000;
    constexpr static uint64_t ELF_DT_NEEDED  = 0x00000001;
    constexpr static uint64_t ELF_DT_RELA    = 0x00000007;
    constexpr static uint64_t ELF_DT_RELASZ  = 0x00000008;
    constexpr static uint64_t ELF_DT_RELAENT = 0x00000009;
    constexpr auto            ELF64_R_SYM( auto i ) {
        return ( ( i ) >> 32 );
    }
    constexpr auto ELF64_R_TYPE( auto i ) {
        return ( ( i ) & 0xFFFFFFFFL );
    }
    constexpr auto ELF64_R_INFO( auto s, auto t ) {
        return ( ( ( s ) << 32 ) + ( ( t ) & 0xFFFFFFFFL ) );
    }
    constexpr static uint64_t R_X86_64_64 { 1 };
    constexpr static uint64_t R_X86_64_32 { 10 };
    constexpr static uint64_t R_X86_64_32S { 11 };
    constexpr static uint64_t R_X86_64_PC32 { 2 };
    constexpr static uint64_t R_X86_64_PLT32 { 4 };
    constexpr static uint64_t R_X86_64_RELATIVE { 8 };

public:
    using Elf64_Address = uint64_t;
    using Elf64_Offset  = uint64_t;
    using Elf64_Half    = uint16_t;
    using Elf64_Word    = uint32_t;
    using Elf64_Sword   = int32_t;
    using Elf64_Xword   = uint64_t;
    using Elf64_Sxword  = int64_t;

private:
    struct _packed ElfHeader {
        Elf64_Word  e_Magic;
        uint8_t     e_Format;
        uint8_t     e_Endianness;
        uint8_t     e_Version;
        uint8_t     e_OSAbi;
        uint8_t     e_AbiVersion;
        uint8_t     e_Rserved[ 7 ];
        Elf64_Half  e_Type;
        Elf64_Half  e_Machine;
        Elf64_Word  e_ElfVersion;
        Elf64_Xword e_Entry;
        Elf64_Xword e_Phoff;
        Elf64_Xword e_Shoff;
        Elf64_Word  e_Flags;
        Elf64_Half  e_HeadSize;
        Elf64_Half  e_PHeadSize;
        Elf64_Half  e_PHeadCount;
        Elf64_Half  e_SHeadSize;
        Elf64_Half  e_SHeadCount;
        Elf64_Half  e_SNameIndex;
    };

    struct _packed ProgramHeaderTable {
        Elf64_Word    p_type;
        Elf64_Word    p_flags;
        Elf64_Offset  p_offset;
        Elf64_Address p_vaddr;
        Elf64_Address p_paddr;
        Elf64_Xword   p_filesz;
        Elf64_Xword   p_memsz;
        Elf64_Xword   p_align;
    };

    struct _packed Relocation {
        Elf64_Address r_offset;
        Elf64_Xword   r_info;
        Elf64_Sxword  r_addend;
    };

    struct _packed SectionHeaderEntries {
        Elf64_Word    sh_name;
        Elf64_Word    sh_type;
        Elf64_Xword   sh_flags;
        Elf64_Address sh_addr;
        Elf64_Offset  sh_offset;
        Elf64_Xword   sh_size;
        Elf64_Word    sh_link;
        Elf64_Word    sh_info;
        Elf64_Xword   sh_addralign;
        Elf64_Xword   sh_entsize;
    };

    struct _packed SymbolTable {
        Elf64_Word    st_name;
        uint8_t       st_info;
        uint8_t       st_other;
        Elf64_Half    st_shndx;
        Elf64_Address st_value;
        Elf64_Xword   st_size;
    };

    struct _packed DynamicTable {
        Elf64_Xword d_tag;
        union {
            Elf64_Xword   d_val;
            Elf64_Address d_ptr;
        } d_un;
    };

    struct _packed SymbolTableInformation {
        SectionHeaderEntries *shdr;
        size_t                shdr_count;
        SymbolTable          *sym_entries;
        size_t                sym_count;
        char                 *sym_str_table;
        size_t                sym_str_table_size;
        char                 *sect_str_table;
        size_t                sect_str_table_size;
    };

public:
    struct FileInformation {
        const char *module_name;
        uint64_t    size;
        uint64_t    loadsegment_start;
        uint64_t    loadsegment_end;
        uint64_t    entry_offset;     // entry = loadsegment_start + entry_offset
    };

public:
    enum class ElfErrorCode {
        MAGIC_IS_NOT_STANDARD,
    };

public:
    explicit Elf( void ) = default;

public:
    static auto load_elf_file( IN uint64_t address ) -> std::expected< FileInformation, ElfErrorCode >;
    static auto check_elf_magic( IN void *Ehdr ) -> bool;
};
}     // namespace QuantumNEC::Modules