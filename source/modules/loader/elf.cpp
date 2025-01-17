#include <kernel/kernel.hpp>
#include <kernel/memory/memory.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <modules/loader/elf.hpp>
using namespace QuantumNEC::Kernel;
namespace QuantumNEC::Modules {
auto Elf::load_elf_file( IN uint64_t address ) -> std::expected< FileInformation, ElfErrorCode > {
    auto elf_header { reinterpret_cast< ElfHeader * >( address ) };
    if ( !check_elf_magic( elf_header ) )
        return std::unexpected { ElfErrorCode::MAGIC_IS_NOT_STANDARD };

    auto P_header = (ProgramHeaderTable *)( address + elf_header->e_Phoff );

    FileInformation file { };

    auto low_address { 0xFFFFFFFFFFFFFFFFul };
    auto high_address { 0ul };
    for ( uint64_t i { }; i < elf_header->e_PHeadCount; ++i ) {
        if ( P_header[ i ].p_type == ELF_PT_LOAD ) {
            if ( low_address > P_header[ i ].p_paddr ) {
                low_address = P_header[ i ].p_paddr;
            }
            if ( high_address < ( P_header[ i ].p_paddr + P_header[ i ].p_memsz ) ) {
                high_address = P_header[ i ].p_paddr + P_header[ i ].p_memsz;
            }
        }
    }
    auto page_count { ( ( high_address - low_address ) >> 12 ) + 1 };

    Kernel::PageAllocator< MemoryPageType::PAGE_4K > allocater;
    // allocate memory for relocating
    auto relocate_base { (uint64_t)physical_to_virtual( allocator_traits< decltype( allocater ) >::allocate( allocater, page_count ) ) };
    auto relocate_offset = relocate_base - low_address;
    auto zero_start      = reinterpret_cast< uint64_t      *>( relocate_base );
    for ( uint64_t i { }; i < ( page_count << 9 ); i++ ) {
        *zero_start = 0x000000000000;
        zero_start++;
    }
    auto load_segment_size = 0ul;
    for ( uint64_t i { }; i < elf_header->e_PHeadCount; i++ ) {
        if ( P_header[ i ].p_type == ELF_PT_LOAD ) {
            auto source_start { reinterpret_cast< uint8_t * >( address + P_header[ i ].p_offset ) };
            auto dest_start { reinterpret_cast< uint8_t * >( P_header[ i ].p_vaddr + relocate_offset ) };
            load_segment_size += P_header[ i ].p_filesz;
            for ( uint64_t j { }; j < P_header[ i ].p_filesz; j++ ) {
                *dest_start = *source_start;
                ++source_start;
                ++dest_start;
            }
        }
    }
    file.executable_start = (uint64_t)virtual_to_physical( elf_header->e_Entry + relocate_offset );
    file.executable_end   = file.executable_start + load_segment_size;
    return file;
}
auto Elf::check_elf_magic( IN void *Ehdr ) -> bool {
    auto header = (ElfHeader *)Ehdr;
    if ( header->e_Magic != ELF_MAGIC )
        return false;
    // if ( header->e_Type != ELF_ET_DYN ) {
    //     return false;
    // }
    return true;
}
}     // namespace QuantumNEC::Modules