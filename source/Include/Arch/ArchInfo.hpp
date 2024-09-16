#pragma once
#if defined( __x86_64__ )
#define TARGET_ARCH QuantumNEC::Architecture::x86_64Architecture
#define SYSTEM_ARCH x86_64
#elif defined( __aarch64__ )
#define TARGET_ARCH QuantumNEC::Architecture::arm64Architecture
#define SYSTEM_ARCH arm64
#endif