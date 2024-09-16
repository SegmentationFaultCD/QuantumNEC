#include <Lib/Uefi.hpp>
PUBLIC namespace QuantumNEC::Architecture::ABI {
    /**
     * Copy 16 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov16( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID;

    /**
     * Copy 32 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov32( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID;

    /**
     * Copy 48 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov48( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID;

    /**
     * Copy 64 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov64( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID;

    /**
     * Copy 128 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov128( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID;

    /**
     * Copy 256 bytes from one location to another using optimised SSE
     * instructions. The locations should not overlap.
     *
     * @param s1
     *   Pointer to the destination of the data.
     * @param s2
     *   Pointer to the source data.
     */
    PUBLIC _C_LINK auto mov256( OUT Ptr< uint8_t > dst, IN Ptr< CONST uint8_t > src )->VOID;

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
    PUBLIC _C_LINK auto __builtin_memcpy( IN OUT Ptr< VOID > s1, IN Ptr< CONST VOID > s2,
                                          IN size_t n )
        ->Ptr< VOID >;

    PUBLIC _C_LINK auto __builtin_memcmp( IN Ptr< CONST VOID > buf1,
                                          IN Ptr< CONST VOID > buf2, IN uint64_t size )
        ->int32_t;

    PUBLIC _C_LINK auto __builtin_memset( IN Ptr< VOID > p, IN int32_t data,
                                          IN size_t size )
        ->Ptr< VOID >;
}
