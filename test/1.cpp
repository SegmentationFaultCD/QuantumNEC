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
#include <queue>
#include <vector>
auto main( void ) -> int;

class task_cmp;

struct task {
    friend task_cmp;
    friend auto main( void ) -> int;

public:
    task( task &&m ) = default;
    task( ) = default;
    task( const task &m ) :
        dat { m.dat },
        core { std::move( const_cast< std::jthread & >( m.core ) ) } {
    }

    auto operator=( task && ) -> task & = default;

private:
    std::jthread core { [ & ] { std::println( "{}", dat.vd ); } };
    struct data {
        std::uint64_t vd = 0;
        std::uint64_t prio = 0;
        std::uint64_t id = 0;
    } dat;
};
class task_cmp {
public:
    bool operator( )( const task &__x, const task &__y ) const {
        if ( auto result = __x.dat.prio <=> __y.dat.prio; result != 0 ) {
            return result > 0;
        }
        return __x.dat.vd > __y.dat.vd;
    }
};

std::priority_queue< task, std::vector< task >, task_cmp > scheduler_queue;
std::vector< task > running_queue;

auto insert_task( task &&t ) -> void {
    scheduler_queue.push( std::move( t ) );
}
auto schedule( ) -> task {
    
}

auto main( void ) -> int {
    task t[ 4 ];
    t[ 1 ].dat.prio = 0;
    t[ 1 ].dat.vd = 114;
    t[ 1 ].dat.id = 1;
    t[ 0 ].dat.prio = 0;
    t[ 0 ].dat.vd = 115;
    t[ 0 ].dat.id = 0;
    t[ 3 ].dat.prio = 4;
    t[ 3 ].dat.vd = 116;
    t[ 3 ].dat.id = 3;
    t[ 2 ].dat.prio = 4;
    t[ 2 ].dat.vd = 117;
    t[ 2 ].dat.id = 2;
    scheduler_queue.push( std::move( t[ 0 ] ) );
    scheduler_queue.push( std::move( t[ 1 ] ) );
    scheduler_queue.push( std::move( t[ 2 ] ) );
    scheduler_queue.push( std::move( t[ 3 ] ) );

    for ( int i = 1; i <= 4; ++i ) {
        auto &&m = ( scheduler_queue.top( ) );
        std::println( "---{} {}---", m.dat.vd, m.dat.id );
        scheduler_queue.pop( );
        running_queue.push_back( std::move( m ) );
    }
}