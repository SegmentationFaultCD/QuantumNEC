#pragma once

#define PRINT_LOGGER

#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
#define PLATFORM x86_64
#else
#endif