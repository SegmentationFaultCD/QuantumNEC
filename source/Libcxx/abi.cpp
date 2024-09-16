auto operator delete( void *, unsigned long int ) noexcept -> void {
}
auto operator delete[]( void *, unsigned long int ) noexcept -> void {
}
extern "C" {
void *__dso_handle = 0;
void *__cxa_atexit = 0;
}