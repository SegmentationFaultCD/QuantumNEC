#pragma once

#include <kernel/memory/heap/slab/slab_allocater.hpp>
#include <kernel/memory/heap/slab/slab_collector.hpp>
#include <kernel/memory/heap/slab/slab_creater.hpp>
#include <kernel/memory/heap/slab/slab_destoryer.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
struct SlabWalker : SlabAllocater, SlabCollector, SlabCreater, SlabDestroyer {};
}     // namespace QuantumNEC::Kernel