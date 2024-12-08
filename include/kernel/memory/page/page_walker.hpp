#pragma once
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_collector.hpp>
#include <lib/Uefi.hpp>
namespace QuantumNEC::Kernel {
struct PageWalker : PageAllocater, PageCollector {};
}     // namespace QuantumNEC::Kernel