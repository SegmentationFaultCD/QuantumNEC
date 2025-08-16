
namespace std {

extern "C" void __cxa_pure_virtual( ) {
    while ( 1 ) __asm__( "hlt" );
}
extern "C" auto __cxa_atexit( void ) -> void {
}
extern "C" auto __dso_handle( void ) -> void {
}
auto __throw_bad_array_new_length( ) -> void {
}
auto __throw_bad_alloc( ) -> void {
}
auto __throw_length_error( char const * ) -> void {
}

auto __throw_logic_error( char const * ) -> void {
}

auto __throw_out_of_range_fmt( char const *, ... ) -> void {
}
auto __glibcxx_assert_fail( char const *, int, char const *, char const * ) noexcept -> void {}
auto __throw_format_error( char const * ) -> void {}

void __throw_invalid_argument( char const * ) {
}
extern "C" void __errno_location( void ) {
}
void __throw_out_of_range( char const * ) {
}
}     // namespace std