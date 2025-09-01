#pragma once
#include <cstdint>
#include <limine.h>
namespace Driver {
auto initialize_smp( limine_smp_response *smp ) -> void;
[[noreturn]] auto smp_start( limine_smp_info *info ) -> void;
}     // namespace Driver