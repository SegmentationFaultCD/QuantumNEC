#include <modules/loader/elf.hpp>
#include <kernel/kernel.hpp>
#include <kernel/memory/memory.hpp>
using namespace QuantumNEC::Kernel;
PUBLIC namespace QuantumNEC::Modules {
    auto Elf::load_elf_file( IN uint64_t address ) -> std::expected< uint64_t, ElfErrorCode > {
        auto Elf_header { reinterpret_cast< ElfHeader * >( address ) };
        if ( !check_elf_magic( Elf_header ) )
            return std::unexpected { ElfErrorCode::MAGIC_IS_NOT_STANDARD };
        auto P_header = (ProgramHeaderTable *)( address + Elf_header->e_Phoff );
        auto low_address { 0xFFFFFFFFFFFFFFFFull };
        auto high_address { 0ull };
        for ( uint64_t i { }; i < Elf_header->e_PHeadCount; ++i ) {
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
        auto relocate_base { (uint64_t)physical_to_virtual( Kernel::PageAllocater { }.allocate< MemoryPageType::PAGE_4K >( page_count ) ) };
        Kernel::PageTableWalker { }.map( (uint64_t)Kernel::virtual_to_physical( relocate_base ), relocate_base, 1, Kernel::PAGE_PRESENT | Kernel::PAGE_RW_W | Kernel::PAGE_US_U, MemoryPageType::PAGE_2M );

        auto relocate_offset = relocate_base - low_address;
        auto zero_start = reinterpret_cast< uint64_t * >( relocate_base );
        for ( uint64_t i { }; i < ( page_count << 9 ); i++ ) {
            *zero_start = 0x000000000000;
            zero_start++;
        }
        for ( uint64_t i { }; i < Elf_header->e_PHeadCount; i++ ) {
            if ( P_header[ i ].p_type == ELF_PT_LOAD ) {
                auto source_start { reinterpret_cast< uint8_t * >( address + P_header[ i ].p_offset ) };
                auto dest_start { reinterpret_cast< uint8_t * >( P_header[ i ].p_vaddr + relocate_offset ) };
                for ( uint64_t j { }; j < P_header[ i ].p_filesz; j++ ) {
                    *dest_start = *source_start;
                    ++source_start;
                    ++dest_start;
                }
            }
        }
        return Elf_header->e_Entry + relocate_offset;
    }
    auto Elf::check_elf_magic( IN VOID * Ehdr ) -> BOOL {
        auto header = (ElfHeader *)Ehdr;
        if ( header->e_Magic != ELF_MAGIC )
            return FALSE;
        if ( header->e_Type != ELF_ET_DYN ) {
            return FALSE;
        }
        return TRUE;
    }
}