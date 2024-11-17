#pragma once

#include <kernel/memory/heap/kheap/kheap_allocater.hpp>
#include <kernel/memory/heap/kheap/kheap_collector.hpp>
#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    struct KHeapWalker : KHeapAllocater, KHeapCollector {};
}