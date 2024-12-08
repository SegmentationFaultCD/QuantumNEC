#pragma once

#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/heap/kheap/kheap_collector.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
struct KHeapWalker : KHeapAllocater, KHeapCollector {};
}     // namespace QuantumNEC::Kernel