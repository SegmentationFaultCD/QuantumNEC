#include <lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Kernel {
    PUBLIC using namespace QuantumNEC::Lib;
    /**
     * Copy 16 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov16( OUT uint8_t * dst, IN CONST uint8_t * src ) -> VOID {
#if defined( __x86_64__ )
        ASM( "movdqu (%[src]), %%xmm0\n\t"
             "movdqu %%xmm0, (%[dst])\n\r"
             :
             : [src] "r"( src ), [dst] "r"( dst )
             : "xmm0", "memory" );
#elif defined( __aarch64__ ) || defined( __riscv )
#elif defined( __loongarch64 )
#endif
    }

    /**
     * Copy 32 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov32( OUT uint8_t * dst, IN CONST uint8_t * src ) -> VOID {
#if defined( __x86_64__ )
        ASM( "movdqu (%[src]), %%xmm0\n\t"
             "movdqu 16(%[src]), %%xmm1\n\t"
             "movdqu %%xmm0, (%[dst])\n\t"
             "movdqu %%xmm1, 16(%[dst])\n\r"
             :
             : [src] "r"( src ), [dst] "r"( dst )
             : "xmm0", "xmm1", "memory" );
#elif defined( __aarch64__ ) || defined( __riscv )
#elif defined( __loongarch64 )
#endif
    }

    /**
     * Copy 48 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov48( OUT uint8_t * dst, IN CONST uint8_t * src ) -> VOID {
#if defined( __x86_64__ )
        ASM( "movdqu (%[src]), %%xmm0\n\t"
             "movdqu 16(%[src]), %%xmm1\n\t"
             "movdqu 32(%[src]), %%xmm2\n\t"
             "movdqu %%xmm0, (%[dst])\n\t"
             "movdqu %%xmm1, 16(%[dst])\n\t"
             "movdqu %%xmm2, 32(%[dst])\n\r"
             :
             : [src] "r"( src ), [dst] "r"( dst )
             : "xmm0", "xmm1", "memory" );
#elif defined( __aarch64__ ) || defined( __riscv )
#elif defined( __loongarch64 )
#endif
    }

    /**
     * Copy 64 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov64( OUT uint8_t * dst, IN CONST uint8_t * src ) -> VOID {
#if defined( __x86_64__ )
        ASM( "movdqu (%[src]), %%xmm0\n\t"
             "movdqu 16(%[src]), %%xmm1\n\t"
             "movdqu 32(%[src]), %%xmm2\n\t"
             "movdqu 48(%[src]), %%xmm3\n\t"
             "movdqu %%xmm0, (%[dst])\n\t"
             "movdqu %%xmm1, 16(%[dst])\n\t"
             "movdqu %%xmm2, 32(%[dst])\n\t"
             "movdqu %%xmm3, 48(%[dst])\n\r"
             :
             : [src] "r"( src ), [dst] "r"( dst )
             : "xmm0", "xmm1", "xmm2", "xmm3", "memory" );
#elif defined( __aarch64__ ) || defined( __riscv )
#elif defined( __loongarch64 )
#endif
    }

    /**
     * Copy 128 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov128( OUT uint8_t * dst, IN CONST uint8_t * src ) -> VOID {
#if defined( __x86_64__ )
        ASM( "movdqu (%[src]), %%xmm0\n\t"
             "movdqu 16(%[src]), %%xmm1\n\t"
             "movdqu 32(%[src]), %%xmm2\n\t"
             "movdqu 48(%[src]), %%xmm3\n\t"
             "movdqu 64(%[src]), %%xmm4\n\t"
             "movdqu 80(%[src]), %%xmm5\n\t"
             "movdqu 96(%[src]), %%xmm6\n\t"
             "movdqu 112(%[src]), %%xmm7\n\t"
             "movdqu %%xmm0, (%[dst])\n\t"
             "movdqu %%xmm1, 16(%[dst])\n\t"
             "movdqu %%xmm2, 32(%[dst])\n\t"
             "movdqu %%xmm3, 48(%[dst])\n\t"
             "movdqu %%xmm4, 64(%[dst])\n\t"
             "movdqu %%xmm5, 80(%[dst])\n\t"
             "movdqu %%xmm6, 96(%[dst])\n\t"
             "movdqu %%xmm7, 112(%[dst])\n\r"
             :
             : [src] "r"( src ), [dst] "r"( dst )
             : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",
               "memory" );

#elif defined( __aarch64__ ) || defined( __riscv )
#elif defined( __loongarch64 )
#endif
    }

    /**
     * Copy 256 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov256( OUT uint8_t * dst, IN CONST uint8_t * src ) -> VOID {
#if defined( __x86_64__ )
        /*
         * There are 16XMM registers, but this function does not use
         * them all so that it can still be compiled as 32bit
         * code. The performance increase was neglible if all 16
         * registers were used.
         */
        mov128( dst, src );
        mov128( dst + 128, src + 128 );

#elif defined( __aarch64__ ) || defined( __riscv )
#elif defined( __loongarch64 )
#endif
    }
}
