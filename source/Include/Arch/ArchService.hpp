#pragma once
#include <Arch/ArchInfo.hpp>
#if SYSTEM_ARCH == x86_64
#include <Arch/x86_64/syscall/service.hpp>
#elif SYSTEM_ARCH == risc_v
#include <Arch/risc-v/syscall/service.hpp>
#elif SYSTEM_ARCH == arm64
#include <Arch/aarch64/syscall/service.hpp>
#endif