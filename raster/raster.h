/* Generic monochrome 640x400 drawing primitives. */
#ifndef RASTER_H
#define RASTER_H
#include "game_types.h"

/* Each color argument is a monochrome pixel bit: 0 or 1.
   Exact filled/outline semantics still need to be decided. */
void clear_screen(UINT8 *base);
void clear_region(UINT8 *base, int x, int y, int width, int height);
void plot_pixel(UINT8 *base, int x, int y, int color);
void plot_horizontal_line(UINT8 *base, int x, int y, int length, int color);
void plot_vertical_line(UINT8 *base, int x, int y, int length, int color);
void plot_line(UINT8 *base, int x1, int y1, int x2, int y2, int color);
void plot_rectangle(UINT8 *base, int x, int y, int width, int height, int color);
void plot_square(UINT8 *base, int x, int y, int size, int color);
void plot_triangle(UINT8 *base, int x1, int y1, int x2, int y2,
                   int x3, int y3, int color);
void plot_8bit_bitmap(UINT8 *base, int x, int y, const UINT8 *bitmap,
                      int height);
void plot_16bit_bitmap(UINT8 *base, int x, int y, const UINT16 *bitmap,
                       int height);
void plot_32bit_bitmap(UINT8 *base, int x, int y, const UINT32 *bitmap,
                       int height);
/* Assumes glyphs are 8x16 with contiguous rows; adapt if using
   Atari TOS's row-strided font table directly. */
void plot_character(UINT8 *base, int x, int y, char ch,
                    const UINT8 *font);
void plot_string(UINT8 *base, int x, int y, const char *string,
                 const UINT8 *font);
#endif
