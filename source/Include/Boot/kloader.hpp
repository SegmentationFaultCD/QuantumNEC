#pragma once
#include <Boot/base.hpp>

namespace QuantumNEC::Boot {

class KernelLoaderService
{
private:
#pragma pack( 1 )
    struct ElfHeader32
    {
        uint32_t Magic;           // 0x00
        uint8_t Format;           // 0x04,32 or 64 bits format 1=32, 2=64
        uint8_t Endianness;       // 0x05
        uint8_t Version;          // 0x06
        uint8_t OSAbi;            // 0x07
        uint8_t AbiVersion;       // 0x08
        uint8_t Rserved[ 7 ];     // 0x09
        uint16_t Type;            // 0x10
        uint16_t Machine;         // 0x12
        uint32_t ElfVersion;      // 0x14
        uint32_t Entry;           // 0x18
        uint32_t Phoff;           // 0x1C
        uint32_t Shoff;           // 0x20
        uint32_t Flags;           // 0x24
        uint16_t HeadSize;        // 0x28,size of elf head
        uint16_t PHeadSize;       // 0x2A, size of a program header table entry
        uint16_t PHeadCount;      // 0x2C, count of entries in the program header table
        uint16_t SHeadSize;       // 0x2E, size of a section header table entry
        uint16_t SHeadCount;      // 0x30, count of entries in the section header table
        uint16_t SNameIndex;      // 0x32, index of entry that contains the section names
    };
    struct ElfHeader64
    {
        uint32_t Magic;           // 0x00
        uint8_t Format;           // 0x04,32 or 64 bits format
        uint8_t Endianness;       // 0x05
        uint8_t Version;          // 0x06
        uint8_t OSAbi;            // 0x07
        uint8_t AbiVersion;       // 0x08
        uint8_t Rserved[ 7 ];     // 0x09
        uint16_t Type;            // 0x10
        uint16_t Machine;         // 0x12
        uint32_t ElfVersion;      // 0x14
        uint64_t Entry;           // 0x18
        uint64_t Phoff;           // 0x20
        uint64_t Shoff;           // 0x28
        uint32_t Flags;           // 0x30
        uint16_t HeadSize;        // 0x34,size of elf head
        uint16_t PHeadSize;       // 0x36, size of a program header table entry
        uint16_t PHeadCount;      // 0x38, count of entries in the program header table
        uint16_t SHeadSize;       // 0x3A, size of a section header table entry
        uint16_t SHeadCount;      // 0x3C, count of entries in the section header table
        uint16_t SNameIndex;      // 0x3E, index of entry that contains the section names
    };

    struct ProgramHeader32
    {
        uint32_t Type;             // 0x00, type of segment
        uint32_t Offset;           // 0x04, Offset of the segment in the file image
        uint32_t VAddress;         // 0x08, Virtual address of the segment in memory
        uint32_t PAddress;         // 0x0C, reserved for segment`s physical address
        uint32_t SizeInFile;       // 0x10, size in bytes of the segment in the file image
        uint32_t SizeInMemory;     // 0x14, size in bytes of the segment in memory
        uint32_t Flags;            // 0x18
        uint32_t Align;            // 0x1C, 0 and 1
    };

    struct ProgramHeader64
    {
        uint32_t Type;             // 0x00, type of segment
        uint32_t Flags;            // 0x04 Segment-dependent flags
        uint64_t Offset;           // 0x08
        uint64_t VAddress;         // 0x10
        uint64_t PAddress;         // 0x18
        uint64_t SizeInFile;       // 0x20
        uint64_t SizeInMemory;     // 0x28
        uint64_t Align;            // 0x30
    };
#pragma pack( )
/* Mutiple arch but not used now */
#if defined( MDE_CPU_X64 ) || defined( MDE_CPU_AARCH64 )
    using ElfProgramHeader = ProgramHeader64;
#elif defined( MDE_CPU_IA32 ) || defined( MDE_CPU_ARM )
    using ElfProgramHeader = ProgramHeader32;
#endif
public:
    explicit KernelLoaderService( IN uchar_t *path ) noexcept;
    ~KernelLoaderService( VOID ) noexcept {
    }

private:
    template < typename... T >
    struct Data : T...
    {
        explicit Data( IN T &...args ) :
            T { args }... {
        }
    };

public:
    constexpr auto call_kernel_entry( IN auto... args ) {
        Data< decltype( args )... > data_list { args... };
        return reinterpret_cast< EFI_STATUS ( * )( decltype( data_list ) * ) >( this->kernel_entry )( &data_list );
    }

public:
    uintptr_t kernel_entry;
};

}     // namespace QuantumNEC::Boot