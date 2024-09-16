#pragma once
#define attribute( x, ... ) __attribute__( ( x, ##__VA_ARGS__ ) )
/**
 * @brief 强制变量在本地分配
 */
#define LOCALVAR __attribute__( ( section( ".data" ) ) )
#define USED __attribute__( ( used ) )
#define ALIGN( x ) __attribute__( ( aligned( #x ) ) )
#define VOLATILE __attribute__( ( volatile ) )
#define ASMCALL __attribute__( ( sysv_abi ) )
#define ____cacheline_aligned __attribute__( ( __aligned__( 32 ) ) )
#define ALIGNED( x ) __attribute__( ( __aligned__( x ) ) )
#define REGPARMS( x ) __attribute__( regparms( x ) )
#define _packed __attribute__( ( packed ) )     // 定义特殊结构
#define _weak __attribute__( ( weak ) )
#define _aligned( n ) __attribute__( ( __aligned__( ( n ) ) ) )
#define _used __attribute__( ( used ) )
#define _unused __attribute__( ( unused ) )