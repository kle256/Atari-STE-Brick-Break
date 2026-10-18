/* Bitmap raster tests for the Atari ST monochrome screen */
#include <stdio.h>
#include <osbind.h>
#include "raster.h"

#define SCRWDTH 640
#define SCRHGHT 400
#define SCRROWBYTES 80
#define SCRNLNGS 8000
#define BMP8WDTH 8

static UINT32 testBuf[SCRNLNGS];
static UINT32 expectedBuf[SCRNLNGS];
static UINT8 tallBmp[SCRHGHT];
static int passCnt = 0;
static int failCnt = 0;

static const UINT8 sprite[8] = {
    0x3C, 0x42, 0xA5, 0x81,
    0xA5, 0x99, 0x42, 0x3C
};

static const UINT8 edges[8] = {
    0x80, 0x01, 0x80, 0x01,
    0x80, 0x01, 0x80, 0x01
};

static const UINT8 solid[8] = {
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};

static const UINT8 empty[8] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

static void clearBuffers(void);
static void markExpected(int row, int col);
static void buildExpected(int row, int col,
                          const UINT8 *bitmap, UINT16 height);
static int matchBuffers(void);
static void checkBitmap(const char *label, int row, int col,
                        const UINT8 *bitmap, UINT16 height);
static void testPreserve(void);
static void testVisual(void);

int main(void) {
    int i;

    testVisual();

    checkBitmap("aligned byte", 100, 80, sprite, 8);
    checkBitmap("unaligned col 11", 100, 11, sprite, 8);
    checkBitmap("cross at col 7", 100, 7, sprite, 8);
    checkBitmap("cross at col 31", 100, 31, sprite, 8);
    checkBitmap("single-bit edges", 150, 53, edges, 8);
    checkBitmap("all bits on", 180, 103, solid, 8);
    checkBitmap("all bits off", 180, 103, empty, 8);

    checkBitmap("left clip -1", 200, -1, sprite, 8);
    checkBitmap("left clip -3", 200, -3, sprite, 8);
    checkBitmap("left clip -7", 200, -7, edges, 8);
    checkBitmap("right clip 632", 200, 632, sprite, 8);
    checkBitmap("right clip 633", 200, 633, sprite, 8);
    checkBitmap("right clip 636", 200, 636, sprite, 8);
    checkBitmap("right clip 639", 200, 639, solid, 8);

    checkBitmap("top clip", -3, 220, sprite, 8);
    checkBitmap("bottom clip", 396, 220, sprite, 8);
    checkBitmap("top-left clip", -3, -3, sprite, 8);
    checkBitmap("bottom-right clip", 397, 636, sprite, 8);

    checkBitmap("zero height", 100, 100, sprite, 0);
    checkBitmap("outside left", 100, -8, sprite, 8);
    checkBitmap("outside right", 100, 640, sprite, 8);
    checkBitmap("outside top", -8, 100, sprite, 8);
    checkBitmap("outside bottom", 400, 100, sprite, 8);

    for (i = 0; i < SCRHGHT; i++) {
        if ((i & 1) == 0) {
            tallBmp[i] = 0x81;
        } else {
            tallBmp[i] = 0xFF;
        }
    }

    checkBitmap("height above 255", 50, 290, tallBmp, 300);
    checkBitmap("full screen height", 0, 320, tallBmp, 400);
    testPreserve();

    printf("\nBitmap tests: %d passed, %d failed\n", passCnt, failCnt);
    return failCnt != 0;
}

/* Clear actual and expected screen buffers */
static void clearBuffers(void) {
    clear_screen(testBuf);
    clear_screen(expectedBuf);
}

/* Set one expected pixel without calling raster plot functions */
static void markExpected(int row, int col) {
    UINT8 *bytes;
    UINT32 byteOff;
    UINT8 bitMask;
    int bitShft;

    bytes = (UINT8 *)expectedBuf;
    byteOff = (UINT32)row * SCRROWBYTES;
    byteOff += col >> 3;
    bitShft = 7 - (col & 7);
    bitMask = (UINT8)(1 << bitShft);
    bytes[byteOff] |= bitMask;
}

/* Build reference image one pixel at a time */
static void buildExpected(int row, int col,
                          const UINT8 *bitmap, UINT16 height) {
    UINT16 r;
    int c;
    long scrRow, scrCol;
    UINT8 bitMask;

    for (r = 0; r < height; r++) {
        scrRow = (long)row + r;
        if (scrRow < 0 || scrRow >= SCRHGHT) {
            continue;
        }

        for (c = 0; c < BMP8WDTH; c++) {
            scrCol = (long)col + c;
            if (scrCol < 0 || scrCol >= SCRWDTH) {
                continue;
            }

            bitMask = (UINT8)(0x80 >> c);
            if (bitmap[r] & bitMask) {
                markExpected((int)scrRow, (int)scrCol);
            }
        }
    }
}

/* Compare every framebuffer longword */
static int matchBuffers(void) {
    int i;

    for (i = 0; i < SCRNLNGS; i++) {
        if (testBuf[i] != expectedBuf[i]) {
            return 0;
        }
    }
    return 1;
}

/* Test one bitmap position against independent reference image */
static void checkBitmap(const char *label, int row, int col,
                        const UINT8 *bitmap, UINT16 height) {
    int passed;

    clearBuffers();
    buildExpected(row, col, bitmap, height);
    plot_8bit_bitmap((UINT8 *)testBuf, row, col, bitmap, height);
    passed = matchBuffers();

    printf("bitmap8 %-24s %s\n", label, passed ? "PASS" : "FAIL");
    if (passed) {
        passCnt++;
    } else {
        failCnt++;
    }
}

/* Ensure drawing does not erase previously set pixels */
static void testPreserve(void) {
    int passed;
    UINT8 *bytes;

    clearBuffers();
    bytes = (UINT8 *)testBuf;

    /* Existing pixels beside and inside the target bitmap */
    plot_pixel(bytes, 100, 99);
    plot_pixel(bytes, 100, 100);
    plot_pixel(bytes, 103, 107);
    plot_pixel(bytes, 108, 105);

    markExpected(100, 99);
    markExpected(100, 100);
    markExpected(103, 107);
    markExpected(108, 105);

    buildExpected(100, 100, sprite, 8);
    plot_8bit_bitmap(bytes, 100, 100, sprite, 8);

    passed = matchBuffers();
    printf("bitmap8 %-24s %s\n", "preserve pixels",
           passed ? "PASS" : "FAIL");
    if (passed) {
        passCnt++;
    } else {
        failCnt++;
    }
}

/* Display tiled sprites and clipped sprites in STEEM */
static void testVisual(void) {
    UINT32 *scrBase;
    UINT8 *bytes;
    int r, c, tileRow, tileCol;

    scrBase = (UINT32 *)Physbase();
    bytes = (UINT8 *)scrBase;
    clear_screen(scrBase);

    /* Larger tiled image aligned to a byte boundary */
    for (r = 0; r < 8; r++) {
        for (c = 0; c < 8; c++) {
            tileRow = 80 + r * 8;
            tileCol = 80 + c * 8;
            plot_8bit_bitmap(bytes, tileRow, tileCol, sprite, 8);
        }
    }

    /* Same tiled image beginning at an unaligned column */
    for (r = 0; r < 8; r++) {
        for (c = 0; c < 8; c++) {
            tileRow = 80 + r * 8;
            tileCol = 201 + c * 8;
            plot_8bit_bitmap(bytes, tileRow, tileCol, sprite, 8);
        }
    }

    /* Individual sprites and screen-edge clipping */
    plot_8bit_bitmap(bytes, 200, 100, sprite, 8);
    plot_8bit_bitmap(bytes, 200, 111, sprite, 8);
    plot_8bit_bitmap(bytes, 200, 122, sprite, 8);
    plot_8bit_bitmap(bytes, 250, -3, sprite, 8);
    plot_8bit_bitmap(bytes, -3, 400, sprite, 8);
    plot_8bit_bitmap(bytes, 396, 400, sprite, 8);
    plot_8bit_bitmap(bytes, 250, 636, sprite, 8);

    Cnecin();
    clear_screen(scrBase);
}
