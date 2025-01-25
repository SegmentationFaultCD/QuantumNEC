#pragma once
#include <lib/Uefi.hpp>
#include <lib/list.hpp>
#include <lib/rbtree.hpp>
#include <libcxx/bitset.hpp>
namespace QuantumNEC::Kernel {
consteval auto operator""_KB( IN const unsigned long long kib ) {
    return (uint64_t)kib * 1024;
}
consteval auto operator""_MB( IN const unsigned long long mib ) {
    return (uint64_t)mib * 1024_KB;
}
consteval auto operator""_GB( IN const unsigned long long gib ) {
    return (uint64_t)gib * 1024_MB;
}
enum MemoryPageType : uint64_t {
    NONE    = 0,
    PAGE_4K = 1,     // 01
    PAGE_2M = 2,     // 10
    PAGE_1G = 3,     // 11
};
class PageManager {
public:
    explicit PageManager( void ) noexcept;
    ~PageManager( void ) noexcept = default;

private:
    inline static size_t all_memory_total { };
    inline static size_t free_memory_total { };
};
}     // namespace QuantumNEC::Kernel