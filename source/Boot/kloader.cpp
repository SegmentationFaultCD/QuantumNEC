#include <Boot/kloader.hpp>
#include <Boot/file.hpp>
#include <Library/MemoryAllocationLib.h>
#include <Boot/output.hpp>
namespace QuantumNEC::Boot {
constexpr CONST auto ELF_64 { 2 };
constexpr CONST auto NOT_64_BIT { -2 };
constexpr CONST auto NOT_ELF { -1 };
constexpr CONST auto ELF_32 { 1 };
constexpr CONST auto PT_LOAD { 1 };
// ELF头魔术字节
constexpr CONST auto ELF_MAGIC { 0x464c457F };
constexpr CONST auto ELFCLASSNONE { 0 };
constexpr CONST auto ELFCLASS32 { 1 };
constexpr CONST auto ELFCLASS64 { 2 };
constexpr CONST auto ELFDATANONE { 0 };
constexpr CONST auto ELFDATA_LSB { 1 };
constexpr CONST auto ELFDATA_MSB { 2 };
constexpr CONST auto EV_NONE { 0 };
constexpr CONST auto EV_CURRENT { 1 };
constexpr CONST auto ELFOSABI_NONE { 0 };
constexpr CONST auto ELFOSABI_LINUX { 3 };
constexpr CONST auto ELFRESERVED { 0 };
constexpr CONST auto EM_386 { 3 };
constexpr CONST auto EM_X86_64 { 62 };
constexpr CONST auto EM_ARM { 40 };
constexpr CONST auto EM_AARCH64 { 183 };
constexpr CONST auto ET_NONE { 0 };
constexpr CONST auto ET_REL { 1 };
constexpr CONST auto ET_EXEC { 2 };
constexpr CONST auto ET_DYN { 3 };
KernelLoaderService::KernelLoaderService( IN uchar_t *path ) noexcept {
    // 读取内核
    auto kernel = FileService { path }.read( );

    // 解析内核是否为ELF格式
    uint32_t elf_magic { };
    elf_magic += *( reinterpret_cast< uint8_t * >( kernel ) + 0 ) << 0 * 8;
    elf_magic += *( reinterpret_cast< uint8_t * >( kernel ) + 1 ) << 1 * 8;
    elf_magic += *( reinterpret_cast< uint8_t * >( kernel ) + 2 ) << 2 * 8;
    elf_magic += *( reinterpret_cast< uint8_t * >( kernel ) + 3 ) << 3 * 8;
    if ( elf_magic != ELF_MAGIC ) {
#ifdef DEBUG
        print( u"[INFO] The format of the file is elf32\n" );

#endif
        while ( true )
            ;
    }
#ifdef DEBUG
    print( u"[INFO] The format of the file is elf64\n" );
#endif

    // 装载内核

    auto Elf_header { reinterpret_cast< ElfHeader64 * >( kernel ) };
    auto P_header { reinterpret_cast< ProgramHeader64 * >( kernel + Elf_header->Phoff ) };
    auto low_address { 0xFFFFFFFFFFFFFFFFull };
    auto high_address { 0ull };

    for ( uint64_t i { }; i < Elf_header->PHeadCount; ++i ) {
        if ( P_header[ i ].Type == PT_LOAD ) {
            if ( low_address > P_header[ i ].PAddress ) {
                low_address = P_header[ i ].PAddress;
            }
            if ( high_address < ( P_header[ i ].PAddress + P_header[ i ].SizeInMemory ) ) {
                high_address = P_header[ i ].PAddress + P_header[ i ].SizeInMemory;
            }
        }
    }
    auto page_count { ( ( high_address - low_address ) >> 12 ) + 1 };
    EFI_PHYSICAL_ADDRESS kernel_relocate_base { };

    if ( EFI_ERROR( gBS->AllocatePages( AllocateAnyPages, EfiLoaderCode, page_count, &kernel_relocate_base ) ) ) {
#ifdef DEBUG
        print( u"[ERROR] Allocate pages for kernelrelocatebuffer error.\n" );
#endif
        while ( true )
            ;
    }

    auto relocate_offset = kernel_relocate_base - low_address;
    auto zero_start = reinterpret_cast< uint64_t * >( kernel_relocate_base );
    for ( uint64_t i { }; i < ( page_count << 9 ); i++ ) {
        *zero_start = 0x000000000000;
        zero_start++;
    }
    for ( uint64_t i { }; i < Elf_header->PHeadCount; i++ ) {
        if ( P_header[ i ].Type == PT_LOAD ) {
            auto source_start { reinterpret_cast< uint8_t * >( kernel + P_header[ i ].Offset ) };
            auto dest_start { reinterpret_cast< uint8_t * >( P_header[ i ].VAddress + relocate_offset ) };
            for ( uint64_t j { }; j < P_header[ i ].SizeInFile; j++ ) {
                *dest_start = *source_start;
                ++source_start;
                ++dest_start;
            }
            // 获取内核页
        }
    }
    this->kernel_entry = Elf_header->Entry + relocate_offset;
#ifdef DEBUG
    print( u"[SUCCESS] The kernel has been loaded.\n" );
    print( u"[INFO] Kernel Entry : {}\n[INFO] Kernel base address : {}\n", (void *)this->kernel_entry, (void *)Elf_header->Entry );
#endif
}

}     // namespace QuantumNEC::Boot