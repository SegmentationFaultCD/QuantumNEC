#include <kernel/memory/allocator/page.hpp>
#include <kernel/memory/paging/hhdm.hpp>
#include <kernel/syscall/module_loader/elf.hpp>
namespace Kernel {
auto Elf::load_elf_file( uint64_t module_address ) -> FileInformation {
    auto elf_header { reinterpret_cast< ElfHeader * >( module_address ) };
    if ( !check_elf_magic( elf_header ) )
        return { };

    auto P_header = (ProgramHeaderTable *)( module_address + elf_header->e_Phoff );

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

    using enum Memory::Page::Type;
    using namespace Memory;
    Page::allocator< P4Kib > allocater;
    // allocate memory for relocating
    auto relocate_base { (uint64_t)physical_to_virtual( allocater.allocate( page_count ) ) };
    auto relocate_offset = relocate_base - low_address;
    auto zero_start = reinterpret_cast< uint64_t * >( relocate_base );
    for ( uint64_t i { }; i < ( page_count << 9 ); i++ ) {
        *zero_start = 0x000000000000;
        zero_start++;
    }
    for ( uint64_t i { }; i < elf_header->e_PHeadCount; i++ ) {
        if ( P_header[ i ].p_type == ELF_PT_LOAD ) {
            std::memcpy( (void *)( P_header[ i ].p_vaddr + relocate_offset ), (void *)( module_address + P_header[ i ].p_offset ), P_header[ i ].p_filesz );
        }
    }

    file.loadsegment_end = (uint64_t)virtual_to_physical( high_address + relocate_offset );
    file.loadsegment_start = (uint64_t)virtual_to_physical( low_address + relocate_offset );
    file.entry_offset = (uint64_t)virtual_to_physical( elf_header->e_Entry + relocate_offset - file.loadsegment_start );
    return file;
}

auto Elf::check_elf_magic( ElfHeader *Ehdr ) -> bool {
    if ( Ehdr->e_Magic != ELF_MAGIC )
        return false;
    return true;
}
}     // namespace Kernel