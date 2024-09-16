#pragma once
#include <Kernel/Memory/map.hpp>
#include <Lib/Base/bitmap.hpp>
#include <Lib/Types/Uefi.hpp>
#include <Lib/Types/type_bool.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC inline constexpr Lib::Types::size_t operator""_KB( IN CONST Lib::Types::size_t kib ) {
        return kib * 1024;
    }

    PUBLIC inline constexpr Lib::Types::size_t operator""_MB( IN CONST Lib::Types::size_t mib ) {
        return mib * 1024_KB;
    }

    PUBLIC inline constexpr Lib::Types::size_t operator""_GB( IN CONST Lib::Types::size_t gib ) {
        return gib * 1024_MB;
    }
    PUBLIC constexpr CONST auto PAGE_SIZE { 0x200000 };
    PUBLIC constexpr CONST auto KERNEL_BASE_ADDRESS { 0xffff800000000000ULL };

    PUBLIC constexpr CONST auto PAGE_GDT_SHIFT { 39 };
    PUBLIC constexpr CONST auto PAGE_1G_SHIFT { 30 };
    PUBLIC constexpr CONST auto PAGE_2M_SHIFT { 21 };
    PUBLIC constexpr CONST auto PAGE_4K_SHIFT { 12 };
    PUBLIC constexpr CONST auto PAGE_2M_SIZE { ( 1UL << PAGE_2M_SHIFT ) };
    PUBLIC constexpr CONST auto PAGE_4K_SIZE { ( 1UL << PAGE_4K_SHIFT ) };
    PUBLIC constexpr CONST auto PAGE_1G_SIZE { ( 1UL << PAGE_1G_SHIFT ) };
    PUBLIC constexpr CONST auto PAGE_2M_MASK { ( ~( PAGE_2M_SIZE - 1 ) ) };
    PUBLIC constexpr CONST auto PAGE_4K_MASK { ( ~( PAGE_4K_SIZE - 1 ) ) };
    PUBLIC constexpr CONST auto PAGE_1G_MASK { ( ~( PAGE_1G_SIZE - 1 ) ) };
    PUBLIC inline constexpr auto Page2MAligned( auto addr ) {
        return ( ( (unsigned long)( addr ) + PAGE_2M_SIZE - 1 ) & PAGE_2M_MASK );
    };
    PUBLIC inline constexpr auto Page4KAligned( auto addr ) {
        return ( ( (unsigned long)( addr ) + PAGE_4K_SIZE - 1 ) & PAGE_4K_MASK );
    };
    PUBLIC constexpr CONST auto MEMORY_PAGE_DESCRIPTOR { 2048 };

    /**
     * @brief 将物理地址转换为线性地址
     */
    PUBLIC inline constexpr auto physical_to_virtual( auto address )->Lib::Types::Ptr< VOID > {
        return reinterpret_cast< Lib::Types::Ptr< VOID > >( ( QuantumNEC::Lib::Types::uint64_t )( address ) + KERNEL_BASE_ADDRESS );
    }
    /**
     * @brief 将线性地址转换为物理地址
     */
    PUBLIC inline constexpr auto virtual_to_physical( auto address )->Lib::Types::Ptr< VOID > {
        return reinterpret_cast< Lib::Types::Ptr< VOID > >( ( QuantumNEC::Lib::Types::uint64_t )(address)-KERNEL_BASE_ADDRESS );
    }

    PUBLIC class PageMemory
    {
        friend MemoryMap;

    private:
        enum class MemoryPageAttribute {
            FREE_MEMORY = 1,          // 可用内存
            RESERVED_MEMORY = 2,      // 保留(不可用)
            ACPI_MEMORY = 3,          // ACPI可回收内存
            ACPI_MEMORY_NVS = 4,      // ACPI NVS内存
            UNUSEABLE_MEMORY = 5,     // 不可用的内存
            UNDEF_MEMORY = 6,         // 未定义
        };
        using BitMapEntry = Lib::Types::uint8_t;

    public:
        enum class MemoryPageType {
            PAGE_4K,
            PAGE_2M,
            PAGE_1G,
        };

    public:
        explicit PageMemory( IN Lib::Types::Ptr< Lib::Types::BootConfig > _config ) noexcept;

    public:
        virtual ~PageMemory( VOID ) noexcept = default;

    public:
        auto malloc( IN Lib::Types::size_t size, IN MemoryPageType type ) -> Lib::Types::Ptr< VOID >;
        auto free( IN Lib::Types::Ptr< VOID > address, IN Lib::Types::size_t size, IN MemoryPageType type ) -> VOID;

    private:
        auto malloc_2M_page( IN Lib::Types::size_t size ) -> Lib::Types::Ptr< VOID >;
        auto free_2M_page( IN Lib::Types::Ptr< VOID > address, IN Lib::Types::size_t size ) -> VOID;
        auto malloc_4K_page( IN Lib::Types::size_t size ) -> Lib::Types::Ptr< VOID >;
        auto free_4K_page( IN Lib::Types::Ptr< VOID > address, IN Lib::Types::size_t size ) -> VOID;
        auto malloc_1G_page( IN Lib::Types::size_t size ) -> Lib::Types::Ptr< VOID >;
        auto free_1G_page( IN Lib::Types::Ptr< VOID > address, IN Lib::Types::size_t size ) -> VOID;

    public:
        Lib::Types::size_t memory_total { };
        Lib::Types::size_t general_memory_total { };

    private:
        BitMapEntry page_descriptor_entry[ MEMORY_PAGE_DESCRIPTOR ] { };
        Lib::Base::Bitmap bitmap_ { };
    };
 
}