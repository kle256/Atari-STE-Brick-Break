/*
 * Raster test driver.
 * Draws basic raster shapes and bitmaps in STEEM.
 */

#include <osbind.h>
#include <linea.h>
#include "raster.h"

#define BMPHEIGHT 8

static const UINT8 bmp8[BMPHEIGHT] = {
    0x3C, 0x42, 0xA5, 0x81,
    0xA5, 0x99, 0x42, 0x3C
};

static const UINT16 bmp16[BMPHEIGHT] = {
    0xFFFF, 0x8001, 0x8001, 0x8001,
    0x8001, 0x8001, 0x8001, 0xFFFF
};

static const UINT32 bmp32[BMPHEIGHT] = {
    0xFFFFFFFFUL, 0x80000001UL, 0x80000001UL, 0x80000001UL,
    0x80000001UL, 0x80000001UL, 0x80000001UL, 0xFFFFFFFFUL
};

void testShapes(UINT8 *base8, UINT32 *base32);
void testBitmaps(UINT8 *base8, UINT16 *base16, UINT32 *base32);

int main(void) {
    UINT8 *base8;
    UINT16 *base16;
    UINT32 *base32;

    base8 = (UINT8 *)Physbase();
    base16 = (UINT16 *)base8;
    base32 = (UINT32 *)base8;
    linea0();

    clear_screen(base32);
    testShapes(base8, base32);
    testBitmaps(base8, base16, base32);
    plot_character(base8, 16, 16, 'A');
    plot_string(base8, 16, 40, "Raster test");
    Cnecin();

    clear_region(base32, 100, 100, 80, 80);
    Cnecin();
    clear_screen(base32);

    return 0;
}

/* Draw basic shapes and a clipped square. */
void testShapes(UINT8 *base8, UINT32 *base32) {
    plot_pixel(base8, 60, 60);
    plot_horizontal_line(base32, 100, 60, 60);
    plot_vertical_line(base32, 100, 60, 60);
    plot_line(base32, 100, 100, 150, 150);
    plot_rectangle(base32, 100, 200, 40, 80);
    plot_square(base32, 180, 100, 40);
    plot_triangle(base32, 230, 100, 40, 30, 0);
    plot_square(base32, 385, 625, 30);
}

/* Draw each supported bitmap size. */
void testBitmaps(UINT8 *base8, UINT16 *base16, UINT32 *base32) {
    plot_8bit_bitmap(base8, 170, 300, bmp8, BMPHEIGHT);
    plot_16bit_bitmap(base16, 200, 300, bmp16, BMPHEIGHT);
    plot_32bit_bitmap(base32, 230, 300, bmp32, BMPHEIGHT);
}
