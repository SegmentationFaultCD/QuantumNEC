#include <Arch/x86_64/platform/platform.hpp>
PUBLIC namespace QuantumNEC::Architecture::ABI {
    PUBLIC using namespace QuantumNEC::Lib::Types;
    /**
     * Copy 16 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC auto mov16( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID {
        ASM( "movdqu (%[src]), %%xmm0\n\t"
             "movdqu %%xmm0, (%[dst])\n\r"
             :
             : [src] "r"( src ), [dst] "r"( dst )
             : "xmm0", "memory" );
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
    PUBLIC auto mov32( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID {
        ASM( "movdqu (%[src]), %%xmm0\n\t"
             "movdqu 16(%[src]), %%xmm1\n\t"
             "movdqu %%xmm0, (%[dst])\n\t"
             "movdqu %%xmm1, 16(%[dst])\n\r"
             :
             : [src] "r"( src ), [dst] "r"( dst )
             : "xmm0", "xmm1", "memory" );
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
    PUBLIC auto mov48( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID {
        ASM( "movdqu (%[src]), %%xmm0\n\t"
             "movdqu 16(%[src]), %%xmm1\n\t"
             "movdqu 32(%[src]), %%xmm2\n\t"
             "movdqu %%xmm0, (%[dst])\n\t"
             "movdqu %%xmm1, 16(%[dst])\n\t"
             "movdqu %%xmm2, 32(%[dst])\n\r"
             :
             : [src] "r"( src ), [dst] "r"( dst )
             : "xmm0", "xmm1", "memory" );
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
    PUBLIC auto mov64( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID {
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
    PUBLIC auto mov128( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID {
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
    PUBLIC auto mov256( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID {
        /*
         * There are 16XMM registers, but this function does not use
         * them all so that it can still be compiled as 32bit
         * code. The performance increase was neglible if all 16
         * registers were used.
         */
        mov128( dst, src );
        mov128( dst + 128, src + 128 );
    }

    /**
     * Copy bytes from one location to another. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     * @param n
     *   Number of bytes to copy.
     * @return
     *   s1
     */
    PUBLIC auto __builtin_memcpy( IN OUT Ptr< VOID > s1, IN Ptr< CONST VOID > s2,
                                  IN size_t n )
        ->Ptr< VOID > {
        Ptr< uint8_t > dst { reinterpret_cast< Ptr< uint8_t > >( s1 ) };
        Ptr< CONST uint8_t > src { reinterpret_cast< Ptr< CONST uint8_t > >( s2 ) };

        /* We can't copy < 16 bytes using XMM registers so do it manually. */
        if ( n < 16 ) {
            if ( n & 0x01 ) {
                *dst = *src;
                dst += 1;
                src += 1;
            }
            if ( n & 0x02 ) {
                *reinterpret_cast< Ptr< uint16_t > >( dst ) = *reinterpret_cast< Ptr< CONST uint16_t > >( src );
                dst += 2;
                src += 2;
            }
            if ( n & 0x04 ) {
                *reinterpret_cast< Ptr< uint32_t > >( dst ) = *reinterpret_cast< Ptr< CONST uint32_t > >( src );
                dst += 4;
                src += 4;
            }
            if ( n & 0x08 ) {
                *reinterpret_cast< Ptr< uint64_t > >( dst ) = *reinterpret_cast< Ptr< CONST uint64_t > >( src );
            }
            return dst;
        }

        /* Special fast cases for <= 128 bytes */
        if ( n <= 32 ) {
            mov16( dst, src );
            mov16( dst - 16 + n, src - 16 + n );
            return s1;
        }
        if ( n <= 64 ) {
            mov32( dst, src );
            mov32( dst - 32 + n, src - 32 + n );
            return s1;
        }
        if ( n <= 128 ) {
            mov64( dst, src );
            mov64( dst - 64 + n, src - 64 + n );
            return s1;
        }

        /*
         * For large copies > 128 bytes. This combination of 256, 64 and 16 byte
         * copies was found to be faster than doing 128 and 32 byte copies as
         * well.
         */
        for ( ; n >= 256; n -= 256, dst += 256, src += 256 ) {
            mov256( dst, src );
        }

        /*
         * We split the remaining bytes (which will be less than 256) into
         * 64byte (2^6) chunks.
         * Using incrementing integers in the case labels of a switch statement
         * enourages the compiler to use a jump table. To get incrementing
         * integers, we shift the 2 relevant bits to the LSB position to first
         * get decrementing integers, and then subtract.
         */
        switch ( 3 - ( n >> 6 ) ) {
        case 0x00:
            mov64( dst, src );
            n -= 64;
            dst += 64;
            src += 64; /* fallthrough */
        case 0x01:
            mov64( dst, src );
            n -= 64;
            dst += 64;
            src += 64; /* fallthrough */
        case 0x02:
            mov64( dst, src );
            n -= 64;
            dst += 64;
            src += 64; /* fallthrough */
        default:;
        }

        /*
         * We split the remaining bytes (which will be less than 64) into
         * 16byte (2^4) chunks, using the same switch structure as above.
         */
        switch ( 3 - ( n >> 4 ) ) {
        case 0x00:
            mov16( dst, src );
            n -= 16;
            dst += 16;
            src += 16; /* fallthrough */
        case 0x01:
            mov16( dst, src );
            n -= 16;
            dst += 16;
            src += 16; /* fallthrough */
        case 0x02:
            mov16( dst, src );
            n -= 16;
            dst += 16;
            src += 16; /* fallthrough */
        default:;
        }

        /* Copy any remaining bytes, without going beyond end of buffers */
        if ( n != 0 ) {
            mov16( dst - 16 + n, src - 16 + n );
        }
        return s1;
    }

    PUBLIC auto __builtin_memcmp( IN Ptr< CONST VOID > buf1,
                                  IN Ptr< CONST VOID > buf2, IN uint64_t size )
        ->int32_t {
        size_t i { };
        Ptr< uint8_t > p1 { reinterpret_cast< Ptr< uint8_t > >(
            const_cast< Ptr< VOID > >( buf1 ) ) };
        Ptr< uint8_t > p2 { reinterpret_cast< Ptr< uint8_t > >(
            const_cast< Ptr< VOID > >( buf2 ) ) };
        while ( *p1++ == *p2++ )
            ++i;
        if ( i != size )
            return -1;
        return 0;
    }

    PUBLIC auto __builtin_memset( IN Ptr< VOID > p, IN int32_t data,
                                  IN size_t size )
        ->Ptr< VOID > {
        Ptr< uint8_t > p1 { reinterpret_cast< Ptr< uint8_t > >( p ) };
        for ( int64_t i = 0; i < static_cast< int64_t >( size ); ++i ) {
            p1[ i ] = static_cast< uint8_t >( data );
        }
        return p;
    }
}
