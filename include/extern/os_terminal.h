#pragma once

typedef struct TerminalPalette {
  uint32_t foreground;
  uint32_t background;
  uint32_t ansi_colors[16];
} TerminalPalette;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if defined(TERMINAL_EMBEDDED_FONT)
bool terminal_init(size_t width,
                   size_t height,
                   uint32_t *screen,
                   float font_size,
                   void *(*malloc)(size_t),
                   void (*free)(void*),
                   void (*serial_print)(const char*));
#endif

#if !defined(TERMINAL_EMBEDDED_FONT)
bool terminal_init(size_t width,
                   size_t height,
                   uint32_t *screen,
                   const uint8_t *font_buffer,
                   size_t font_buffer_size,
                   float font_size,
                   void *(*malloc)(size_t),
                   void (*free)(void*),
                   void (*serial_print)(const char*));
#endif

void terminal_destroy(void);

void terminal_flush(void);

void terminal_set_auto_flush(size_t auto_flush);

void terminal_advance_state(const char *s);

void terminal_advance_state_single(char c);

void terminal_set_color_scheme(size_t palette_index);

void terminal_set_custom_color_scheme(struct TerminalPalette palette);

bool terminal_handle_keyboard(uint8_t scancode, char *buffer);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
