#include <cstdint>
#include <memory>
auto operator new( std::size_t ) -> void * {
}
auto operator new[]( std::size_t ) -> void * {
}
auto operator new[]( unsigned long, std::align_val_t ) -> void * {
}
auto operator delete( void * ) noexcept -> void {
    return;
}
auto operator delete[]( void * ) noexcept -> void {
    return;
}
auto operator delete( void *, unsigned long ) noexcept -> void {
    return;
}
auto operator delete[]( void *, unsigned long ) noexcept -> void {
    return;
}
extern "C" auto __cxa_atexit( void ) -> void {
}
extern "C" auto __dso_handle( void ) -> void {
}
extern "C" auto __cxa_pure_virtual( void ) -> void {
}