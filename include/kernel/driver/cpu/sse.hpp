#pragma once
#include <cstdint>
namespace Driver {
class SSE {
private:
    std::uint8_t fxsave_region[ 512 ];

public:
    auto check_fpu( void ) -> bool;
    auto save( void ) -> void;
    auto write( void ) -> void;
};
auto initialize_sse( void ) -> void;
}     // namespace Driver