#pragma once
#ifdef __cplusplus
constexpr auto SELECTOR_CODE64_KERNEL { ( 1ull << 3 ) | TI_GDT | RPL0 }; /* 64位内核代码段 */
constexpr auto SELECTOR_DATA64_KERNEL { ( 2ull << 3 ) | TI_GDT | RPL0 }; /* 64位内核数据段 */
constexpr auto SELECTOR_CODE32_USER { ( 3ull << 3 ) | TI_GDT | RPL0 };   /* 32位用户代码段 */
constexpr auto SELECTOR_DATA32_USER { ( 4ull << 3 ) | TI_GDT | RPL0 };   /* 32位用户数据段 */
constexpr auto SELECTOR_DATA64_USER { ( 5ull << 3 ) | TI_GDT | RPL3 };   /* 64位用户数据段 */
constexpr auto SELECTOR_CODE64_USER { ( 6ull << 3 ) | TI_GDT | RPL3 };   /* 64位用户代码段 */
constexpr auto SELECTOR_CODE32_KERNEL { ( 7ull << 3 ) | TI_GDT | RPL0 }; /* 32位内核代码段 */
constexpr auto SELECTOR_DATA32_KERNEL { ( 8ull << 3 ) | TI_GDT | RPL0 }; /* 32位内核数据段 */
constexpr auto SELECTOR_TSS { ( 10ull << 3 ) | TI_GDT | RPL0 };          /* TSS段 */
#else
#define SELECTOR_CODE64_KERNEL 0x08
#define SELECTOR_DATA64_KERNEL 0x10
#define SELECTOR_CODE64_USER 0x33
#define SELECTOR_DATA64_USER 0x2b
#define SELECTOR_TSS 0x50
#endif