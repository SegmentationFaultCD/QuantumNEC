#pragma once
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
#include <libcxx/bitset.hpp>

PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC constexpr auto operator""_KB( IN CONST unsigned long long kib )->size_t {
        return kib * 1024;
    }

    PUBLIC constexpr auto operator""_MB( IN CONST unsigned long long mib )->size_t {
        return mib * 1024_KB;
    }

    PUBLIC constexpr auto operator""_GB( IN CONST unsigned long long gib )->size_t {
        return gib * 1024_MB;
    }
    PUBLIC enum MemoryPageType : uint64_t {
        NONE = 0,
        PAGE_4K = 1,     // 01
        PAGE_2M = 2,     // 10
        PAGE_1G = 3,     // 11
    };
    PUBLIC class PageManager
    {
    public:
        explicit PageManager( VOID ) noexcept;
        virtual ~PageManager( VOID ) noexcept = default;

    private:
        size_t all_memory_total { };
        size_t free_memory_total { };
    };
}