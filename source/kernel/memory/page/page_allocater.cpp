#include <kernel/memory/memory.hpp>
#include <kernel/memory/page/page_allocater.hpp>
#include <kernel/memory/page/page_header.hpp>
#include <kernel/print.hpp>
#include <libcxx/cstring.hpp>
namespace QuantumNEC::Kernel {
// I must tell you a thing, there is something wrong with my rbtree iterator, I have to use the ugly function 'traverse' to replace the range-for.
// If you have some good advice on how to fix the bug of my rbtree iterator, please send a email to C2100China@outlook.com

template <>
auto PageAllocater::allocate< MemoryPageType::PAGE_4K >( IN uint64_t __size__ ) -> void * {
    using PH  = __page_header__< PAGE_4K, PAGE_2M >;
    using PHI = PH::__helper__::__page_information__;
    auto &group { PH::__helper__::get_group( ) };

    auto index                   = 0ul;
    auto bitmap_index            = 0ul;
    auto first_header_bits_count = 0ul;
    // The variable is not always used.
    // If size is less than page descriptor count of a header, the variable will not be used.
    auto needed_header_count = Lib::DIV_ROUND_UP( __size__, PH::__helper__::page_descriptor_count );

    PHI *node { };
    if ( __size__ < PH::__helper__::page_descriptor_count ) {
        if ( !group.empty( ) ) {
            this->lock.acquire( );
            group.traverse( [ & ]( PHI &zone ) {
                if ( zone.owner ) {
                    return false;
                }
                // zone.owner is null, that means it is the head of the zone.
                auto header  = &zone;
                auto success = false;
                for ( auto i = 0ul; i < zone.header_count; ++i ) {
                    // If the header's state is ALL_FULL, that means there is no need to search the header's bitmap.
                    // Because the bitmap's the all of bits are filled, it also means there is no memory can be allocated from the memory space managed by the header.
                    if ( header[ i ].flags.state != PHI::__page_flags__::__page_state__::ALL_FULL ) {
                        // If the zone's state isn't ALL_FULL, that means we can allocate memory from the memory space managed by the header.
                        // Than, we should find a free bit from bitmap.
                        if ( auto result = header[ i ].bitmap.find< false >( __size__ ); result.has_value( ) ) {
                            index                   = i;
                            bitmap_index            = result.value( );
                            node                    = &zone;
                            success                 = true;
                            first_header_bits_count = __size__;
                            // Find the suitable zone to allocate, jump.
                            break;
                        }
                    }
                    else {
                        // switch to the next header.
                        continue;
                    }
                }
                if ( success ) {
                    return true;
                }
                return false;
            } );
            this->lock.release( );
        }
    }
    else {
        if ( !group.empty( ) ) {
            this->lock.acquire( );
            group.traverse( [ & ]( PHI &zone ) {
                if ( zone.owner ) {
                    return false;
                }
                // Pass, it means the header is the first header in the zone.
                if ( zone.header_count < needed_header_count ) {
                    return false;
                }
                // Pass, it means the number of headers in the zone satisfies needed headers.
                // Then, translate the zone so that searching suitable and uninterrupted headers group.
                auto header  = &zone;
                auto success = true;
                auto i       = 0ul;
                for ( ; i < zone.header_count; ++i ) {
                    // The basicest condition, the range from i to (i + needed_header_count - 1) must be between 0 and zone.header_count - 1.
                    if ( i + needed_header_count > zone.header_count ) {
                        success = false;
                        break;
                    }
                    // The second condition, the state in the first header isn't ALL_FULL.
                    if ( header[ i ].flags.state == PHI::__page_flags__::__page_state__::ALL_FULL ) {
                        // the header has no bit can filled;
                        continue;
                    }
                    // The third confition, the number of uninterrupted bits from the highest to meet the first filled bit in the first header must be greater than 0
                    first_header_bits_count = header[ i ].bitmap.count_from_high< false >( );
                    if ( !first_header_bits_count ) {
                        // the header has no uninterrupted and free bit
                        continue;
                    }
                    // The fourth confition, all status in the headers from i + 1 to (i + middle_header_count - 1) are ALL_FREE
                    auto new_size            = __size__ - first_header_bits_count;
                    auto middle_header_count = new_size / PH::__helper__::page_descriptor_count;
                    for ( auto j = i + 1; j < i + middle_header_count; ++j ) {
                        if ( header[ j ].flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                            // Fail, Someone's state isn't ALL_FREE
                            success = false;
                            break;
                        }
                    }
                    if ( !success ) {
                        continue;
                    }
                    // The fifth condition, the state in the final header isn't ALL_FULL.
                    if ( header[ i + middle_header_count ].flags.state == PHI::__page_flags__::__page_state__::ALL_FULL ) {
                        // the header has no bit can filled;
                        continue;
                    }
                    // The final confition, the number of uninterrupted bits from the lowest to meet the first filled bit in final header must be greater than remainder_size
                    auto remainder_size          = new_size % PH::__helper__::page_descriptor_count;
                    auto final_header_bits_count = header[ i + middle_header_count ].bitmap.count_from_low< false >( );
                    if ( final_header_bits_count < remainder_size ) {
                        // the header has no uninterrupted and free bit
                        continue;
                    }
                    success = true;
                    break;
                }
                if ( !success ) {
                    return false;
                }
                // Successfully find suitable headers group.
                node         = &zone;
                index        = i;
                bitmap_index = header[ i ].bitmap.find_from_high< false >( );
                return true;
            } );

            this->lock.release( );
        }
    }

    if ( node ) {
        // There are some suitable page headers searched.
        this->lock.acquire( );

        // head
        node[ index ].bitmap.set( bitmap_index, first_header_bits_count );
        node[ index ].free_memory_page_count -= first_header_bits_count;
        if ( !node[ index ].free_memory_page_count ) {
            node[ index ].flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
        }
        else {
            node[ index ].flags.state = PHI::__page_flags__::__page_state__::NORMAL;
        }
        if ( __size__ >= PH::__helper__::page_descriptor_count ) {
            // middle
            for ( auto i = index + 1; i < index + needed_header_count - 1; ++i ) {
                node[ i ].bitmap.set( 0, PH::__helper__::page_descriptor_count );
                node[ i ].free_memory_page_count = 0;
                node[ i ].flags.state            = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            // end
            node[ index + needed_header_count - 1 ].free_memory_page_count -= __size__ % PH::__helper__::page_descriptor_count;
            node[ index + needed_header_count - 1 ].bitmap.set( 0, __size__ % PH::__helper__::page_descriptor_count );
            if ( !node[ index + needed_header_count - 1 ].free_memory_page_count ) {
                node[ index + needed_header_count - 1 ].flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            else {
                node[ index + needed_header_count - 1 ].flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }
        }

        auto address = node[ index ].base_address + bitmap_index * this->__page_size__< PAGE_4K >;
        std::memset( (void *)physical_to_virtual( address ), 0, __size__ * this->__page_size__< PAGE_4K > );
        this->lock.release( );

        return (void *)address;
    }
    // No any zone is suitable
    // Create a new zone to allocate.
    auto address = (void *)( PH { needed_header_count, { }, {} }.__allocate_headers__( __size__ )[ 0 ].base_address );
    this->lock.acquire( );
    std::memset( (void *)physical_to_virtual( address ), 0, __size__ * this->__page_size__< PAGE_4K > );
    this->lock.release( );
    return address;
}
template <>
auto PageAllocater::allocate< MemoryPageType::PAGE_2M >( IN uint64_t __size__ ) -> void * {
    using PH  = __page_header__< PAGE_2M, PAGE_1G >;
    using PHI = PH::__helper__::__page_information__;
    auto &group { PH::__helper__::get_group( ) };

    auto index                   = 0ul;
    auto bitmap_index            = 0ul;
    auto first_header_bits_count = 0ul;
    // The variable is not always used.
    // If size is less than page descriptor count of a header, the variable will not be used.
    auto needed_header_count = Lib::DIV_ROUND_UP( __size__, PH::__helper__::page_descriptor_count );

    PHI *node { };
    if ( __size__ < PH::__helper__::page_descriptor_count ) {
        if ( !group.empty( ) ) {
            this->lock.acquire( );
            group.traverse( [ & ]( PHI &zone ) {
                if ( zone.owner ) {
                    return false;
                }
                // zone.owner is null, that means it is the head of the zone.
                auto header  = &zone;
                auto success = false;
                for ( auto i = 0ul; i < zone.header_count; ++i ) {
                    // If the header's state is ALL_FULL, that means there is no need to search the header's bitmap.
                    // Because the bitmap's the all of bits are filled, it also means there is no memory can be allocated from the memory space managed by the header.
                    if ( header[ i ].flags.state != PHI::__page_flags__::__page_state__::ALL_FULL ) {
                        // If the zone's state isn't ALL_FULL, that means we can allocate memory from the memory space managed by the header.
                        // Than, we should find a free bit from bitmap.
                        if ( auto result = header[ i ].bitmap.find< false >( __size__ ); result.has_value( ) ) {
                            index                   = i;
                            bitmap_index            = result.value( );
                            node                    = &zone;
                            success                 = true;
                            first_header_bits_count = __size__;
                            // Find the suitable zone to allocate, jump.
                            break;
                        }
                    }
                    else {
                        // switch to the next header.
                        continue;
                    }
                }
                if ( success ) {
                    return true;
                }
                return false;
            } );
            this->lock.release( );
        }
    }
    else {
        if ( !group.empty( ) ) {
            this->lock.acquire( );
            group.traverse( [ & ]( PHI &zone ) {
                if ( zone.owner ) {
                    return false;
                }
                // Pass, it means the header is the first header in the zone.
                if ( zone.header_count < needed_header_count ) {
                    return false;
                }
                // Pass, it means the number of headers in the zone satisfies needed headers.
                // Then, translate the zone so that searching suitable and uninterrupted headers group.
                auto header  = &zone;
                auto success = true;
                auto i       = 0ul;
                for ( ; i < zone.header_count; ++i ) {
                    // The basicest condition, the range from i to (i + needed_header_count - 1) must be between 0 and zone.header_count - 1.
                    if ( i + needed_header_count > zone.header_count ) {
                        success = false;
                        break;
                    }
                    // The second condition, the state in the first header isn't ALL_FULL.
                    if ( header[ i ].flags.state == PHI::__page_flags__::__page_state__::ALL_FULL ) {
                        // the header has no bit can filled;
                        continue;
                    }
                    // The third confition, the number of uninterrupted bits from the highest to meet the first filled bit in the first header must be greater than 0
                    first_header_bits_count = header[ i ].bitmap.count_from_high< false >( );
                    if ( !first_header_bits_count ) {
                        // the header has no uninterrupted and free bit
                        continue;
                    }
                    // The fourth confition, all status in the headers from i + 1 to (i + middle_header_count - 1) are ALL_FREE
                    auto new_size            = __size__ - first_header_bits_count;
                    auto middle_header_count = new_size / PH::__helper__::page_descriptor_count;
                    for ( auto j = i + 1; j < i + middle_header_count; ++j ) {
                        if ( header[ j ].flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                            // Fail, Someone's state isn't ALL_FREE
                            success = false;
                            break;
                        }
                    }
                    if ( !success ) {
                        continue;
                    }
                    // The fifth condition, the state in the final header isn't ALL_FULL.
                    if ( header[ i + middle_header_count ].flags.state == PHI::__page_flags__::__page_state__::ALL_FULL ) {
                        // the header has no bit can filled;
                        continue;
                    }
                    // The final confition, the number of uninterrupted bits from the lowest to meet the first filled bit in final header must be greater than remainder_size
                    auto remainder_size          = new_size % PH::__helper__::page_descriptor_count;
                    auto final_header_bits_count = header[ i + middle_header_count ].bitmap.count_from_low< false >( );
                    if ( final_header_bits_count < remainder_size ) {
                        // the header has no uninterrupted and free bit
                        continue;
                    }
                    success = true;
                    break;
                }
                if ( !success ) {
                    return false;
                }
                // Successfully find suitable headers group.
                node         = &zone;
                index        = i;
                bitmap_index = header[ i ].bitmap.find_from_high< false >( );
                return true;
            } );

            this->lock.release( );
        }
    }
    if ( node ) {
        // There are some suitable page headers searched.
        this->lock.acquire( );
        // head
        node[ index ].bitmap.set( bitmap_index, first_header_bits_count );
        node[ index ].free_memory_page_count -= first_header_bits_count;
        if ( !node[ index ].free_memory_page_count ) {
            node[ index ].flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
        }
        else {
            node[ index ].flags.state = PHI::__page_flags__::__page_state__::NORMAL;
        }
        if ( __size__ >= PH::__helper__::page_descriptor_count ) {
            // middle
            for ( auto i = index + 1; i < index + needed_header_count - 1; ++i ) {
                node[ i ].bitmap.set( 0, PH::__helper__::page_descriptor_count );
                node[ i ].free_memory_page_count = 0;
                node[ i ].flags.state            = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            // end
            node[ index + needed_header_count - 1 ].free_memory_page_count -= __size__ % PH::__helper__::page_descriptor_count;
            node[ index + needed_header_count - 1 ].bitmap.set( 0, __size__ % PH::__helper__::page_descriptor_count );
            if ( !node[ index + needed_header_count - 1 ].free_memory_page_count ) {
                node[ index + needed_header_count - 1 ].flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            else {
                node[ index + needed_header_count - 1 ].flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }
        }

        auto address = node[ index ].base_address + bitmap_index * this->__page_size__< PAGE_2M >;
        std::memset( (void *)physical_to_virtual( address ), 0, __size__ * this->__page_size__< PAGE_2M > );
        this->lock.release( );

        return (void *)address;
    }
    // No any zone is suitable
    // Create a new zone to allocate.
    auto address = (void *)( PH { needed_header_count, { }, {} }.__allocate_headers__( __size__ )[ 0 ].base_address );
    this->lock.acquire( );
    std::memset( (void *)physical_to_virtual( address ), 0, __size__ * this->__page_size__< PAGE_2M > );
    this->lock.release( );
    return address;
}

template <>
auto PageAllocater::allocate< MemoryPageType::PAGE_1G >( IN uint64_t __size__ ) -> void * {
    using PH  = __page_header__< PAGE_1G, NONE >;
    using PHI = PH::__helper__::__page_information__;
    auto &group { PH::__helper__::get_group( ) };

    auto index                   = 0ul;
    auto bitmap_index            = 0ul;
    auto first_header_bits_count = 0ul;
    // The variable is not always used.
    // If size is less than page descriptor count of a header, the variable will not be used.
    auto needed_header_count = Lib::DIV_ROUND_UP( __size__, PH::__helper__::page_descriptor_count );

    PHI *node { };
    if ( __size__ < PH::__helper__::page_descriptor_count ) {
        if ( !group.empty( ) ) {
            this->lock.acquire( );
            group.traverse( [ & ]( PHI &zone ) {
                if ( zone.owner ) {
                    return false;
                }
                // zone.owner is null, that means it is the head of the zone.
                auto header  = &zone;
                auto success = false;
                for ( auto i = 0ul; i < zone.header_count; ++i ) {
                    // If the header's state is ALL_FULL, that means there is no need to search the header's bitmap.
                    // Because the bitmap's the all of bits are filled, it also means there is no memory can be allocated from the memory space managed by the header.
                    if ( header[ i ].flags.state != PHI::__page_flags__::__page_state__::ALL_FULL ) {
                        // If the zone's state isn't ALL_FULL, that means we can allocate memory from the memory space managed by the header.
                        // Than, we should find a free bit from bitmap.
                        if ( auto result = header[ i ].bitmap.find< false >( __size__ ); result.has_value( ) ) {
                            index                   = i;
                            bitmap_index            = result.value( );
                            node                    = &zone;
                            success                 = true;
                            first_header_bits_count = __size__;
                            // Find the suitable zone to allocate, jump.
                            break;
                        }
                    }
                    else {
                        // switch to the next header.
                        continue;
                    }
                }
                if ( success ) {
                    return true;
                }
                return false;
            } );
            this->lock.release( );
        }
    }
    else {
        if ( !group.empty( ) ) {
            this->lock.acquire( );
            group.traverse( [ & ]( PHI &zone ) {
                if ( zone.owner ) {
                    return false;
                }
                // Pass, it means the header is the first header in the zone.
                if ( zone.header_count < needed_header_count ) {
                    return false;
                }
                // Pass, it means the number of headers in the zone satisfies needed headers.
                // Then, translate the zone so that searching suitable and uninterrupted headers group.
                auto header  = &zone;
                auto success = true;
                auto i       = 0ul;
                for ( ; i < zone.header_count; ++i ) {
                    // The basicest condition, the range from i to (i + needed_header_count - 1) must be between 0 and zone.header_count - 1.
                    if ( i + needed_header_count > zone.header_count ) {
                        success = false;
                        break;
                    }
                    // The second condition, the state in the first header isn't ALL_FULL.
                    if ( header[ i ].flags.state == PHI::__page_flags__::__page_state__::ALL_FULL ) {
                        // the header has no bit can filled;
                        continue;
                    }
                    // The third confition, the number of uninterrupted bits from the highest to meet the first filled bit in the first header must be greater than 0
                    first_header_bits_count = header[ i ].bitmap.count_from_high< false >( );
                    if ( !first_header_bits_count ) {
                        // the header has no uninterrupted and free bit
                        continue;
                    }
                    // The fourth confition, all status in the headers from i + 1 to (i + middle_header_count - 1) are ALL_FREE
                    auto new_size            = __size__ - first_header_bits_count;
                    auto middle_header_count = new_size / PH::__helper__::page_descriptor_count;
                    for ( auto j = i + 1; j < i + middle_header_count; ++j ) {
                        if ( header[ j ].flags.state != PHI::__page_flags__::__page_state__::ALL_FREE ) {
                            // Fail, Someone's state isn't ALL_FREE
                            success = false;
                            break;
                        }
                    }
                    if ( !success ) {
                        continue;
                    }
                    // The fifth condition, the state in the final header isn't ALL_FULL.
                    if ( header[ i + middle_header_count ].flags.state == PHI::__page_flags__::__page_state__::ALL_FULL ) {
                        // the header has no bit can filled;
                        continue;
                    }
                    // The final confition, the number of uninterrupted bits from the lowest to meet the first filled bit in final header must be greater than remainder_size
                    auto remainder_size          = new_size % PH::__helper__::page_descriptor_count;
                    auto final_header_bits_count = header[ i + middle_header_count ].bitmap.count_from_low< false >( );
                    if ( final_header_bits_count < remainder_size ) {
                        // the header has no uninterrupted and free bit
                        continue;
                    }
                    success = true;
                    break;
                }
                if ( !success ) {
                    return false;
                }
                // Successfully find suitable headers group.
                node         = &zone;
                index        = i;
                bitmap_index = header[ i ].bitmap.find_from_high< false >( );
                return true;
            } );

            this->lock.release( );
        }
    }
    if ( node ) {
        // There are some suitable page headers searched.
        this->lock.acquire( );
        // head
        node[ index ].bitmap.set( bitmap_index, first_header_bits_count );
        node[ index ].free_memory_page_count -= first_header_bits_count;
        if ( !node[ index ].free_memory_page_count ) {
            node[ index ].flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
        }
        else {
            node[ index ].flags.state = PHI::__page_flags__::__page_state__::NORMAL;
        }
        if ( __size__ >= PH::__helper__::page_descriptor_count ) {
            // middle
            for ( auto i = index + 1; i < index + needed_header_count - 1; ++i ) {
                node[ i ].bitmap.set( 0, PH::__helper__::page_descriptor_count );
                node[ i ].free_memory_page_count = 0;
                node[ i ].flags.state            = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            // end
            node[ index + needed_header_count - 1 ].free_memory_page_count -= __size__ % PH::__helper__::page_descriptor_count;
            node[ index + needed_header_count - 1 ].bitmap.set( 0, __size__ % PH::__helper__::page_descriptor_count );
            if ( !node[ index + needed_header_count - 1 ].free_memory_page_count ) {
                node[ index + needed_header_count - 1 ].flags.state = PHI::__page_flags__::__page_state__::ALL_FULL;
            }
            else {
                node[ index + needed_header_count - 1 ].flags.state = PHI::__page_flags__::__page_state__::NORMAL;
            }
        }
        auto address = node[ index ].base_address + bitmap_index * this->__page_size__< PAGE_1G >;
        std::memset( (void *)physical_to_virtual( address ), 0, __size__ * this->__page_size__< PAGE_1G > );
        this->lock.release( );
        return (void *)address;
    }

    // No any zone is suitable
    // Create a new zone to allocate.
    auto address = (void *)( PH { needed_header_count, { }, global_memory_address }.__allocate_headers__( __size__ )[ 0 ].base_address );
    this->lock.acquire( );
    // 1G allocate need control Global Memory Address
    // Beacuse it doesn't have any mmap allocater.
    // So it has to control memory address(memory map) by itself.
    global_memory_address += needed_header_count * PH::__helper__::__zone_min_memory__;
    std::memset( (void *)physical_to_virtual( address ), 0, __size__ * this->__page_size__< PAGE_1G > );
    this->lock.release( );
    return address;
}
}     // namespace QuantumNEC::Kernel