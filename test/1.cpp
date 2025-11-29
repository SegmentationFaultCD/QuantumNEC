#include <atomic>
#include <future>
#include <mutex>
#include <print>
#include <thread>
template < typename T >
struct alignas( 32 ) Message {
    std::uint64_t node;
    T *package_address;
    std::uint64_t length;
    std::uint64_t lock;
};
// 4K / 2^5 = 2^12 / 2^5 = 2^7 = 128

struct IPC {
    // classic productor-consumer problem
    // use shared memory
    // shared_page is a 4K memory page, and there is a message queue in it, it is composed of a array, structure be like:
    /* |      head     | => number of messages,
     * |   message1    | => address of package(physical address), length, lock, node(for the receiver's mailbox)
     * |   message2    |
     * - - - -
     * |   messageMax  |
     */
    // receiver process provides physical address of shared_page
    // any sender should map this address to thier page table
    // and write its request in the queue in order
    // when a sender tries to write request, it should check the message element's lock, if it is 0, lock and write.(atomic operation)
    // if not, search for the next element, do it like before. if all elements are locked, choose one randomly and block itself.
    // search algorithm is ???
    // in the end, add the request( = node = physical address of the message) into the process's mailbox
    // receiver will convert the address in its mailbox into special virtual address space and visit the message that it points.
    // then map the pack's physical address, and get the data, handle message.
    // after handling, free the lock, remove the request from the mailbox
    /*

        receiver --- mailbox |   phy_addr1   |   phy_addr2   |   phy_addr3   |
                     convert |   vir_addr1   |   vir_addr2   |   vir_addr3   |
                                    |               |               |
                                   \/              \/              \/
                             |   Message1    |   Message2    |   Message3    |
                                    |              |                |
                                   \/             \/               \/
                             | pack1_phy_addr| pack2_phy_addr| pack3_phy_addr|
                 map convert | pack1_vrt_addr| pack2_vrt_addr| pack3_vrt_addr|
                             | handle data   | handle data   | handle data   |

    */

    template < typename... Args >
    auto receive( std::uint64_t source ) {
        // check self mailbox
        // null, hang | not null, handle, wake up sender, continue handling or sleeping
        // sync
        // mailbox = get_current()->mailbox
    }

    auto send( std::uint64_t destination, auto &&...args ) {
    }
};

#include <concepts>
#include <queue>
#include <ranges>
#include <utility>
#include <vector>
template < typename D >
struct root_mode {
    root_mode( ) = default;

    auto operator( )( std::strong_ordering ord ) const {
        return D { }.compare( ord );
    }
};

class Rmax : public root_mode< Rmax > {
    friend root_mode;

public:
    Rmax( ) = default;

private:
    auto compare( std::strong_ordering ord ) {
        return ord == std::strong_ordering::greater;
    }

} rmax;
class Rmin : public root_mode< Rmin > {
    friend root_mode;

public:
    Rmin( ) = default;

private:
    auto compare( std::strong_ordering ord ) {
        return ord == std::strong_ordering::less;
    }
} rmin;

template < typename T, root_mode r = rmin >
    requires std::totally_ordered< T > && std::movable< T >
struct priority_queue {
    auto push( T &&x ) {
        heap.push_back( x );
        size++;
        up( size );
    }
    auto pop( void ) -> T {
        down( 1 );
        auto element = *heap.end( );
        heap.pop_back( );
        return element;
    }

    auto up( std::uint64_t x ) {
        while ( x > 1 && r( heap[ x ] <=> heap[ x / 2 ] ) ) {
            std::swap( heap[ x ], heap[ x / 2 ] );
            x /= 2;
        }
    }
    auto down( std::uint64_t x ) {
        std::uint64_t t;
        while ( x * 2 <= size ) {
            t = x * 2;
            if ( t + 1 <= size && !r( heap[ t + 1 ] <=> heap[ t ] ) )
                t++;
            if ( heap[ t ] <= heap[ x ] )
                break;
            std::swap( heap[ x ], heap[ t ] );
            x = t;
        }
    }

public:
    std::vector< T > heap { T {} };     // 第一个留空
    std::uint64_t size = 0;
};
struct ScheduleData {
    std::uint64_t time_slice;
    std::uint64_t priority;
    std::uint64_t nice;
    std::uint64_t virtual_deadline;
    std::uint64_t cpu;
};
struct Task {
public:
    std::thread self;
    ScheduleData data;
    Task( auto &&lambda, auto &&...args ) :
        self { lambda, std::forward< decltype( auto { args } ) >( args )... } {
    }

    Task( Task && ) = default;

    Task( ) = default;

    auto operator<=>( const Task &oper ) const {
        return this->data.virtual_deadline <=> oper.data.virtual_deadline;
    }
    auto operator==( const Task &oper ) const {
        return this->data.virtual_deadline == oper.data.virtual_deadline;
    }
    template < typename U >
        requires std::convertible_to< U, Task >
    auto operator=( this auto &&self, U &&o ) -> Task & {
        std::thread s { std::move( self.self ) };
        self.self = std::move( o.self );
        o.self = std::move( s );
        std::swap( self.data, o.data );
        return *self;
    }
} task[ 4 ];
auto operator<=>( Task &A, Task &oper ) -> std::strong_ordering {
    return A.data.virtual_deadline <=> oper.data.virtual_deadline;
}
// namespace std
class Muqss {
    constexpr static double min_prio_ratio = 1.0;     // 静态优先级在时间片计算的权重
    constexpr static auto rr_interval = 6ul;          // 6ms,这个一般作为时间片填充
    // nice默认为0，如要更改使用系统调用, 更改优先级，重新计算VD
    constexpr static auto default_nice = 0;
    // nice有40个
    constexpr static auto min_nice = -20;
    constexpr static auto max_nice = 40;
    auto get_prio_ratio( std::uint64_t nice ) {
        auto prio_ratio = this->min_prio_ratio;
        for ( auto i = 1; i <= nice - this->min_nice; ++i ) {
            prio_ratio *= 1.1;
        }
        return prio_ratio;
    }

    // VD(Virtual Deadline) 计算公式为 niffies(纳秒级最小时间间隔计数) + (prio_ratio * rr_interval)

    auto get_virtual_deadline( std::uint64_t now_time, double prio_ratio ) {
        return now_time + prio_ratio * this->rr_interval;
    }

    std::queue< Task > RT_task;
    std::priority_queue< int, std::vector< int >, std::greater< int > > queue;
};

auto main( void ) -> int {
    int i = 0;
    for ( auto thd : std::ranges::views::repeat( []( int id ) {
                         std::println( "{}", id );
                     } ) | std::ranges::views::take( 4 ) ) {
        std::construct_at( &task[ i ], thd, i );
        ++i;
    }
}
