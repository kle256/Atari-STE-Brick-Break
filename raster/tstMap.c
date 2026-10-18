/* Bitmap raster tests for the Atari ST monochrome screen */
#include <stdio.h>
#include <osbind.h>
#include "raster.h"

#define SCRWDTH 640
#define SCRHGHT 400
#define SCRROWBYTES 80
#define SCRNLNGS 8000
#define BMP8WDTH 8
#define BMP16WDTH 16
#define BMP32WDTH 32

static UINT32 testBuf[SCRNLNGS];
static UINT32 expectedBuf[SCRNLNGS];
static UINT8 tallBmp[SCRHGHT];
static UINT16 tallBmp16[SCRHGHT];
static UINT32 tallBmp32[SCRHGHT];
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

/* A 16x16 sprite with distinct details for alignment testing */
static const UINT16 sprite16[16] = {
    0x0180, 0x03C0, 0x07E0, 0x0FF0,
    0x1FF8, 0x3FFC, 0x7FFE, 0xFFFF,
    0xE007, 0xE007, 0x7FFE, 0x3FFC,
    0x1FF8, 0x0FF0, 0x03C0, 0x0180
};

static const UINT16 edges16[8] = {
    0x8000, 0x0001, 0x8000, 0x0001,
    0x8000, 0x0001, 0x8000, 0x0001
};

static const UINT16 solid16[8] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};

static const UINT16 empty16[8] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

/* A 32x32 diamond with distinct rows for alignment testing */
static const UINT32 sprite32[32] = {
    0x00018000UL, 0x0003C000UL, 0x0007E000UL, 0x000FF000UL,
    0x001FF800UL, 0x003FFC00UL, 0x007FFE00UL, 0x00FFFF00UL,
    0x01FFFF80UL, 0x03FFFFC0UL, 0x07FFFFE0UL, 0x0FFFFFF0UL,
    0x1FFFFFF8UL, 0x3FFFFFFCUL, 0x7FFFFFFEUL, 0xFFFFFFFFUL,
    0xFFFFFFFFUL, 0x7FFFFFFEUL, 0x3FFFFFFCUL, 0x1FFFFFF8UL,
    0x0FFFFFF0UL, 0x07FFFFE0UL, 0x03FFFFC0UL, 0x01FFFF80UL,
    0x00FFFF00UL, 0x007FFE00UL, 0x003FFC00UL, 0x001FF800UL,
    0x000FF000UL, 0x0007E000UL, 0x0003C000UL, 0x00018000UL
};

static const UINT32 edges32[8] = {
    0x80000000UL, 0x00000001UL, 0x80000000UL, 0x00000001UL,
    0x80000000UL, 0x00000001UL, 0x80000000UL, 0x00000001UL
};

static const UINT32 solid32[8] = {
    0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFFUL,
    0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFFUL, 0xFFFFFFFFUL
};

static const UINT32 empty32[8] = {
    0, 0, 0, 0, 0, 0, 0, 0
};

static void clearBuffers(void);
static void markExpected(int row, int col);
static void buildExpected(int row, int col,
                          const UINT8 *bitmap, UINT16 height);
static int matchBuffers(void);
static void buildExpected16(int row, int col,
                            const UINT16 *bitmap, UINT16 height);
static void checkBitmap16(const char *label, int row, int col,
                          const UINT16 *bitmap, UINT16 height);
static void testPreserve16(void);
static void buildExpected32(int row, int col,
                            const UINT32 *bitmap, UINT16 height);
static void checkBitmap32(const char *label, int row, int col,
                          const UINT32 *bitmap, UINT16 height);
static void testPreserve32(void);
static void checkBitmap(const char *label, int row, int col,
                        const UINT8 *bitmap, UINT16 height);
static void testPreserve(void);
static void testVisual(void);

int main(void) {
    int i, pass8, fail8, pass16, fail16;

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

    pass8 = passCnt;
    fail8 = failCnt;
    printf("\n8-bit bitmap tests: %d passed, %d failed\n\n", pass8, fail8);

    checkBitmap16("aligned word", 100, 80, sprite16, 16);
    checkBitmap16("unaligned col 19", 100, 19, sprite16, 16);
    checkBitmap16("cross at col 15", 100, 15, sprite16, 16);
    checkBitmap16("cross at col 31", 100, 31, sprite16, 16);
    checkBitmap16("cross at col 47", 100, 47, sprite16, 16);
    checkBitmap16("single-bit edges", 150, 53, edges16, 8);
    checkBitmap16("all bits on", 180, 103, solid16, 8);
    checkBitmap16("all bits off", 180, 103, empty16, 8);

    checkBitmap16("left clip -1", 200, -1, sprite16, 16);
    checkBitmap16("left clip -3", 200, -3, sprite16, 16);
    checkBitmap16("left clip -15", 200, -15, edges16, 8);
    checkBitmap16("right clip 624", 200, 624, sprite16, 16);
    checkBitmap16("right clip 625", 200, 625, sprite16, 16);
    checkBitmap16("right clip 630", 200, 630, sprite16, 16);
    checkBitmap16("right clip 639", 200, 639, solid16, 8);

    checkBitmap16("top clip", -3, 220, sprite16, 16);
    checkBitmap16("bottom clip", 396, 220, sprite16, 16);
    checkBitmap16("top-left clip", -3, -3, sprite16, 16);
    checkBitmap16("bottom-right clip", 397, 630, sprite16, 16);

    checkBitmap16("zero height", 100, 100, sprite16, 0);
    checkBitmap16("outside left", 100, -16, sprite16, 16);
    checkBitmap16("outside right", 100, 640, sprite16, 16);
    checkBitmap16("outside top", -16, 100, sprite16, 16);
    checkBitmap16("outside bottom", 400, 100, sprite16, 16);

    for (i = 0; i < SCRHGHT; i++) {
        if ((i & 1) == 0) {
            tallBmp16[i] = 0x8001;
        } else {
            tallBmp16[i] = 0xFFFF;
        }
    }

    checkBitmap16("height above 255", 50, 290, tallBmp16, 300);
    checkBitmap16("full screen height", 0, 320, tallBmp16, 400);
    testPreserve16();

    pass16 = passCnt - pass8;
    fail16 = failCnt - fail8;
    printf("\n16-bit bitmap tests: %d passed, %d failed\n\n", pass16, fail16);

    checkBitmap32("aligned long", 100, 64, sprite32, 32);
    checkBitmap32("aligned col 0", 100, 0, sprite32, 32);
    checkBitmap32("unaligned col 35", 100, 35, sprite32, 32);
    checkBitmap32("cross at col 1", 100, 1, sprite32, 32);
    checkBitmap32("cross at col 31", 100, 31, sprite32, 32);
    checkBitmap32("cross at col 63", 100, 63, sprite32, 32);
    checkBitmap32("cross at col 95", 100, 95, sprite32, 32);
    checkBitmap32("single-bit edges", 150, 53, edges32, 8);
    checkBitmap32("all bits on", 180, 103, solid32, 8);
    checkBitmap32("all bits off", 180, 103, empty32, 8);

    checkBitmap32("left clip -1", 200, -1, sprite32, 32);
    checkBitmap32("left clip -3", 200, -3, sprite32, 32);
    checkBitmap32("left clip -17", 200, -17, sprite32, 32);
    checkBitmap32("left clip -31", 200, -31, edges32, 8);
    checkBitmap32("right edge col 608", 200, 608, sprite32, 32);
    checkBitmap32("right clip 609", 200, 609, sprite32, 32);
    checkBitmap32("right clip 630", 200, 630, sprite32, 32);
    checkBitmap32("right clip 639", 200, 639, solid32, 8);

    checkBitmap32("top clip", -3, 220, sprite32, 32);
    checkBitmap32("bottom clip", 396, 220, sprite32, 32);
    checkBitmap32("top-left clip", -3, -3, sprite32, 32);
    checkBitmap32("bottom-right clip", 397, 630, sprite32, 32);
    checkBitmap32("last screen row", 399, 607, sprite32, 32);

    checkBitmap32("zero height", 100, 100, sprite32, 0);
    checkBitmap32("outside left", 100, -32, sprite32, 32);
    checkBitmap32("outside right", 100, 640, sprite32, 32);
    checkBitmap32("outside top", -32, 100, sprite32, 32);
    checkBitmap32("outside bottom", 400, 100, sprite32, 32);

    for (i = 0; i < SCRHGHT; i++) {
        if ((i & 1) == 0) {
            tallBmp32[i] = 0x80000001UL;
        } else {
            tallBmp32[i] = 0xFFFFFFFFUL;
        }
    }

    checkBitmap32("height above 255", 50, 290, tallBmp32, 300);
    checkBitmap32("full screen height", 0, 320, tallBmp32, 400);
    testPreserve32();

    printf("\n32-bit bitmap tests: %d passed, %d failed\n",
           passCnt - pass8 - pass16, failCnt - fail8 - fail16);
    printf("Bitmap tests total: %d passed, %d failed\n", passCnt, failCnt);
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

/* Build reference image from individual 16-bit source pixels */
static void buildExpected16(int row, int col,
                            const UINT16 *bitmap, UINT16 height) {
    UINT16 r, bitMask;
    int c;
    long scrRow, scrCol;

    for (r = 0; r < height; r++) {
        scrRow = (long)row + r;
        if (scrRow < 0 || scrRow >= SCRHGHT) {
            continue;
        }

        for (c = 0; c < BMP16WDTH; c++) {
            scrCol = (long)col + c;
            if (scrCol < 0 || scrCol >= SCRWDTH) {
                continue;
            }

            bitMask = (UINT16)(0x8000 >> c);
            if (bitmap[r] & bitMask) {
                markExpected((int)scrRow, (int)scrCol);
            }
        }
    }
}

/* Build reference image from individual 32-bit source pixels */
static void buildExpected32(int row, int col,
                            const UINT32 *bitmap, UINT16 height) {
    UINT16 r;
    UINT32 bitMask;
    int c;
    long scrRow, scrCol;

    for (r = 0; r < height; r++) {
        scrRow = (long)row + r;
        if (scrRow < 0 || scrRow >= SCRHGHT) {
            continue;
        }

        for (c = 0; c < BMP32WDTH; c++) {
            scrCol = (long)col + c;
            if (scrCol < 0 || scrCol >= SCRWDTH) {
                continue;
            }

            bitMask = 0x80000000UL >> c;
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

/* Test 16-bit bitmap against independently calculated pixels */
static void checkBitmap16(const char *label, int row, int col,
                          const UINT16 *bitmap, UINT16 height) {
    int passed;

    clearBuffers();
    buildExpected16(row, col, bitmap, height);
    plot_16bit_bitmap((UINT16 *)testBuf, row, col, bitmap, height);
    passed = matchBuffers();

    printf("bitmap16 %-24s %s\n", label, passed ? "PASS" : "FAIL");
    if (passed) {
        passCnt++;
    } else {
        failCnt++;
    }
}

/* Test 32-bit bitmap against independently calculated pixels */
static void checkBitmap32(const char *label, int row, int col,
                          const UINT32 *bitmap, UINT16 height) {
    int passed;

    clearBuffers();
    buildExpected32(row, col, bitmap, height);
    plot_32bit_bitmap(testBuf, row, col, bitmap, height);
    passed = matchBuffers();

    printf("bitmap32 %-24s %s\n", label, passed ? "PASS" : "FAIL");
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

/* Verify a 16-bit bitmap preserves existing framebuffer pixels */
static void testPreserve16(void) {
    UINT8 *bytes;
    int passed;

    clearBuffers();
    bytes = (UINT8 *)testBuf;

    /* Existing pixels inside and outside the 16x16 sprite */
    plot_pixel(bytes, 100, 99);
    plot_pixel(bytes, 100, 100);
    plot_pixel(bytes, 110, 115);
    plot_pixel(bytes, 116, 113);

    markExpected(100, 99);
    markExpected(100, 100);
    markExpected(110, 115);
    markExpected(116, 113);

    buildExpected16(100, 100, sprite16, 16);
    plot_16bit_bitmap((UINT16 *)testBuf, 100, 100, sprite16, 16);

    passed = matchBuffers();
    printf("bitmap16 %-24s %s\n", "preserve pixels",
           passed ? "PASS" : "FAIL");
    if (passed) {
        passCnt++;
    } else {
        failCnt++;
    }
}

/* Verify 32-bit bitmap preserves existing framebuffer pixels */
static void testPreserve32(void) {
    UINT8 *bytes;
    int passed;

    clearBuffers();
    bytes = (UINT8 *)testBuf;

    /* Existing pixels inside and outside the 32x32 sprite */
    plot_pixel(bytes, 100, 99);
    plot_pixel(bytes, 100, 100);
    plot_pixel(bytes, 120, 131);
    plot_pixel(bytes, 132, 113);

    markExpected(100, 99);
    markExpected(100, 100);
    markExpected(120, 131);
    markExpected(132, 113);

    buildExpected32(100, 100, sprite32, 32);
    plot_32bit_bitmap(testBuf, 100, 100, sprite32, 32);

    passed = matchBuffers();
    printf("bitmap32 %-24s %s\n", "preserve pixels",
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

    /* Stage 1: 8-bit bitmap patterns */
    Cnecin();
    clear_screen(scrBase);

    /* Stage 2: 16-bit aligned and unaligned sprites */
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            tileRow = 70 + r * 16;
            tileCol = 80 + c * 16;
            plot_16bit_bitmap((UINT16 *)scrBase, tileRow,
                              tileCol, sprite16, 16);
        }
    }

    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            tileRow = 70 + r * 16;
            tileCol = 241 + c * 16;
            plot_16bit_bitmap((UINT16 *)scrBase, tileRow,
                              tileCol, sprite16, 16);
        }
    }

    /* Individual 16-bit sprites and clipping at screen edges */
    plot_16bit_bitmap((UINT16 *)scrBase, 200, 100, sprite16, 16);
    plot_16bit_bitmap((UINT16 *)scrBase, 200, 119, sprite16, 16);
    plot_16bit_bitmap((UINT16 *)scrBase, 200, 138, sprite16, 16);
    plot_16bit_bitmap((UINT16 *)scrBase, 250, -5, sprite16, 16);
    plot_16bit_bitmap((UINT16 *)scrBase, -5, 400, sprite16, 16);
    plot_16bit_bitmap((UINT16 *)scrBase, 394, 400, sprite16, 16);
    plot_16bit_bitmap((UINT16 *)scrBase, 250, 630, sprite16, 16);

    Cnecin();
    clear_screen(scrBase);

    /* Stage 3: 32-bit aligned and unaligned sprites */
    for (r = 0; r < 3; r++) {
        for (c = 0; c < 3; c++) {
            tileRow = 70 + r * 32;
            tileCol = 80 + c * 32;
            plot_32bit_bitmap(scrBase, tileRow, tileCol, sprite32, 32);
        }
    }

    for (r = 0; r < 3; r++) {
        for (c = 0; c < 3; c++) {
            tileRow = 70 + r * 32;
            tileCol = 241 + c * 32;
            plot_32bit_bitmap(scrBase, tileRow, tileCol, sprite32, 32);
        }
    }

    /* Individual 32-bit sprites and clipping at screen edges */
    plot_32bit_bitmap(scrBase, 220, 100, sprite32, 32);
    plot_32bit_bitmap(scrBase, 220, 135, sprite32, 32);
    plot_32bit_bitmap(scrBase, 220, 170, sprite32, 32);
    plot_32bit_bitmap(scrBase, 250, -9, sprite32, 32);
    plot_32bit_bitmap(scrBase, -9, 400, sprite32, 32);
    plot_32bit_bitmap(scrBase, 390, 400, sprite32, 32);
    plot_32bit_bitmap(scrBase, 250, 627, sprite32, 32);

    Cnecin();
    clear_screen(scrBase);
}
