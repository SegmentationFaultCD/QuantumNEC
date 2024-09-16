#pragma once
#include "Lib/Base/attribute.hpp"
#include "type_base.hpp"
#include "type_char.hpp"
#include "type_int.hpp"
#include "type_ptr.hpp"
#include "type_bool.hpp"
#define OUT
#define IN
namespace QuantumNEC::Lib::Types {
using UINTN = uint64_t;
using EfiPhysicalAddress = uint64_t;
using EfiVirtualAddress = uint64_t;
using PhysicalAddress = uint64_t;
using VirtualAddress = uint64_t;
// typedef struct
// {
//   UINTN Status;
// } SystemStatus;

using SystemStatus = UINTN;
static constexpr auto SYSTEM_SUCCESS { (SystemStatus)0 };
typedef enum class EfiMemoryType : Lib::Types::uint32_t {
    /// Not used.
    EfiReservedMemoryType,
    /// The code portions of a loaded application.
    /// (Note that UEFI OS loaders are UEFI applications.)
    EfiLoaderCode,
    /// The data portions of a loaded application and the default data allocation
    /// type used by an application to allocate pool memory.
    EfiLoaderData,
    /// The code portions of a loaded Boot Services Driver.
    EfiBootServicesCode,
    /// The data portions of a loaded Boot Serves Driver, and the default data
    /// allocation type used by a Boot Services Driver to allocate pool memory.
    EfiBootServicesData,
    /// The code portions of a loaded Runtime Services Driver.
    EfiRuntimeServicesCode,
    /// The data portions of a loaded Runtime Services Driver and the default
    /// data allocation type used by a Runtime Services Driver to allocate pool memory.
    EfiRuntimeServicesData,
    /// Free (unallocated) memory.
    EfiConventionalMemory,
    /// Memory in which errors have been detected.
    EfiUnusableMemory,
    /// Memory that holds the ACPI tables.
    EfiACPIReclaimMemory,
    /// Address space reserved for use by the firmware.
    EfiACPIMemoryNVS,
    /// Used by system firmware to request that a memory-mapped IO region
    /// be mapped by the OS to a virtual address so it can be accessed by EFI runtime services.
    EfiMemoryMappedIO,
    /// System memory-mapped IO region that is used to translate memory
    /// cycles to IO cycles by the processor.
    EfiMemoryMappedIOPortSpace,
    /// Address space reserved by the firmware for code that is part of the processor.
    EfiPalCode,
    /// A memory region that operates as EfiConventionalMemory,
    /// however it happens to also support byte-addressable non-volatility.
    EfiPersistentMemory,
    EfiMaxMemoryType
} EfiMemoryType;

// Root System Description Pointer
typedef struct
{
    CHAR8 signature[ 8 ];
    UINT8 checkSum;
    CHAR8 OEMID[ 6 ];
    UINT8 revision;
    UINT32 rsdtAddress;
    UINT32 length;
    UINT64 xsdtAddress;
    UINT8 extendedChecksum;
    CHAR8 reserved[ 3 ];
} _packed Rsdp;

typedef struct
{
    EfiPhysicalAddress FrameBufferBase;
    UINTN FrameBufferSize;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
    UINT32 PixelsPerScanLine;
} GraphicsConfig;
typedef struct
{
    ///
    /// Type of the memory region.
    /// Type EFI_MEMORY_TYPE is defined in the
    /// AllocatePages() function description.
    ///
    EfiMemoryType Type;
    ///
    /// Physical address of the first byte in the memory region. PhysicalStart must be
    /// aligned on a 4 KiB boundary, and must not be above 0xfffffffffffff000. Type
    /// EFI_PHYSICAL_ADDRESS is defined in the AllocatePages() function description
    ///
    EfiPhysicalAddress PhysicalStart;
    ///
    /// Virtual address of the first byte in the memory region.
    /// VirtualStart must be aligned on a 4 KiB boundary,
    /// and must not be above 0xfffffffffffff000.
    ///
    EfiVirtualAddress VirtualStart;
    ///
    /// NumberOfPagesNumber of 4 KiB pages in the memory region.
    /// NumberOfPages must not be 0, and must not be any value
    /// that would represent a memory page with a start address,
    /// either physical or virtual, above 0xfffffffffffff000.
    ///
    UINT64 NumberOfPages;
    ///
    /// Attributes of the memory region that describe the bit mask of capabilities
    /// for that memory region, and not necessarily the current settings for that
    /// memory region.
    ///
    UINT64 Attribute;
} _aligned( 16 ) EfiMemoryDescriptor;

typedef struct
{
    Lib::Types::Ptr< EfiMemoryDescriptor > Buffer;
    UINTN MemorySize;
    UINTN MemoryCount;
    UINTN MemoryKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
} MemoryConfig;

typedef struct BootConfig
{
    GraphicsConfig graphics_data;
    MemoryConfig memory_map;
    Lib::Types::Ptr< Rsdp > acpi_table;
    UINTN font_frame;
} BootConfig;
}     // namespace QuantumNEC::Lib::Types
using QuantumNEC::Lib::Types::SYSTEM_SUCCESS;