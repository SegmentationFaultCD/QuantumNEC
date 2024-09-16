#pragma once
#include <Lib/Types/Uefi.hpp>
#include <Lib/Base/bitmap.hpp>
namespace QuantumNEC::Kernel {
PUBLIC constexpr CONST auto PID_COUNT { 1024 };
class PidPool
{
public:
    STATIC auto allocate( VOID ) -> Lib::Types::uint64_t {
        bitmap_.set_length( PID_COUNT );
        bitmap_.set_bits( bits );
        auto pid_index { bitmap_.allocate( 1 ) };
        bitmap_.set( pid_index, 1 );
        return pid_index + pid_head;
    }

private:
    inline STATIC Lib::Types::uint64_t pid_head { };
    inline STATIC Lib::Types::byte_t bits[ PID_COUNT ] { };
    inline STATIC Lib::Base::Bitmap bitmap_ { };
};

}     // namespace QuantumNEC::Kernel