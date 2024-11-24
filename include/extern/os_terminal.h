#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum PixelFormat {
    Rgb,
    Bgr,
} PixelFormat;

typedef enum TerminalInitResult {
    Success,
    MallocIsNull,
    FreeIsNull,
    FontBufferIsNull,
} TerminalInitResult;

struct [[gnu::packed]] TerminalDisplay {
    size_t   width;
    size_t   height;
    uint8_t *address;
};

struct [[gnu::packed]] TerminalPalette {
    uint32_t foreground;
    uint32_t background;
    uint32_t ansi_colors[ 16 ];
};

#ifdef __cplusplus
extern "C" {
#endif     // __cplusplus

enum TerminalInitResult terminal_init( const struct TerminalDisplay *display,
                                       float                         font_size,
                                       void *( *malloc )( size_t ),
                                       void ( *free )( void * ),
                                       void ( *serial_print )( const char * ) );

void terminal_destroy( void );

void terminal_flush( void );

void terminal_set_auto_flush( size_t auto_flush );

void terminal_advance_state( const char *s );

void terminal_advance_state_single( char c );

void terminal_set_color_scheme( size_t palette_index );

void terminal_set_custom_color_scheme( struct TerminalPalette palette );

bool terminal_handle_keyboard( uint8_t scancode, char *buffer );

#ifdef __cplusplus
}     // extern "C"
#endif     // __cplusplus