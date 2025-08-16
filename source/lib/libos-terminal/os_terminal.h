#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  Success,
  MallocIsNull,
  FreeIsNull,
  FontBufferIsNull,
} TerminalInitResult;

typedef struct {
  size_t width;
  size_t height;
  uint32_t *buffer;
  size_t pitch;
  uint8_t red_mask_size;
  uint8_t red_mask_shift;
  uint8_t green_mask_size;
  uint8_t green_mask_shift;
  uint8_t blue_mask_size;
  uint8_t blue_mask_shift;
} TerminalDisplay;

typedef struct {
  uint32_t foreground;
  uint32_t background;
  uint32_t ansi_colors[16];
} TerminalPalette;

typedef struct {
  const char *(*get)(void);
  void (*set)(const char*);
} TerminalClipboard;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if defined(TERMINAL_EMBEDDED_FONT)
TerminalInitResult terminal_init(const TerminalDisplay *display,
                                 float font_size,
                                 void *(*malloc)(size_t),
                                 void (*free)(void*));
#endif

#if !defined(TERMINAL_EMBEDDED_FONT)
TerminalInitResult terminal_init(const TerminalDisplay *display,
                                 const uint8_t *font_buffer,
                                 size_t font_buffer_size,
                                 float font_size,
                                 void *(*malloc)(size_t),
                                 void (*free)(void*));
#endif

void terminal_destroy(void);

void terminal_flush(void);

void terminal_process(const char *s);

void terminal_process_byte(uint8_t c);

void terminal_handle_keyboard(uint8_t scancode);

void terminal_handle_mouse_scroll(ptrdiff_t delta);

void terminal_set_history_size(size_t size);

void terminal_set_scroll_speed(size_t speed);

void terminal_set_auto_flush(bool auto_flush);

void terminal_set_crnl_mapping(bool auto_crnl);

void terminal_set_bell_handler(void (*handler)(void));

void terminal_set_color_scheme(size_t palette_index);

void terminal_set_custom_color_scheme(const TerminalPalette *palette);

void terminal_set_clipboard(TerminalClipboard clipboard);

void terminal_set_pty_writer(void (*writer)(const uint8_t*));

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus
