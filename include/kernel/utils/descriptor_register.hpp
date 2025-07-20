#pragma once
#include <concepts>
#include <cstdint>
namespace Utils {
template < typename DTType, std::uint16_t descriptor_count >
class [[gnu::packed]] DescriptorRegister {
public:
    std::uint16_t size_;
    DTType       *offset;

public:
    DescriptorRegister( DTType *_offset ) :
        size_ { static_cast< std::uint16_t >( descriptor_count * sizeof( DTType ) - 1 ) }, offset { _offset } {
    }

    auto read( this auto &&self ) -> DTType * {
        return self.read_( );
    }
    auto write( this auto &&self ) {
        self.write_( );
    }

    auto size( ) {
        return this->size_ + 1;
    }
    auto &operator=( DTType *_offset ) {
        this->offset = _offset;
        return *this;
    }
};
}     // namespace Utils