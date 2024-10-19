auto main( int argv, char *argc[], char *envp[] ) -> int;

extern "C" auto _start( void ) -> void {
    main( 0, nullptr, nullptr );
}
