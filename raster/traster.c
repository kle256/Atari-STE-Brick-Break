/* Tests monochrome framebuffer raster drawing routines */
#include <stdio.h>
#include <osbind.h>
#include "raster.h"

#define SCRNLNGS 8000
#define SCRROWBYTES 80

static UINT32 testBuf[SCRNLNGS];

/* Private test functions */
static void fillBlack(void);
static int pixelSet(int row, int col);
static int allBlack(void);
static int allClear(void);
static void testVisual(void);
static void testClearScreen(void);
static void testClearRegion(void);
static void testRegionBounds(void);
static void testRegionNoOp(void);
static void testRegionBits(void);
static void testPlotPixel(void);
static void testPixelCorners(void);
static void testPixelBounds(void);
static void testPixelBits(void);

int main() {
    /* Visual framebuffer tests */
    testVisual();

    /* Automated memory tests */
    testClearScreen();
    testClearRegion();
    testRegionBounds();
    testRegionNoOp();
    testRegionBits();
    testPlotPixel();
    testPixelCorners();
    testPixelBounds();
    testPixelBits();

    return 0;
}

/* Fills test framebuffer with black pixels */
static void fillBlack(void) {
    int i;

    for (i = 0; i < SCRNLNGS; i++) {
        testBuf[i] = 0xFFFFFFFFUL;
    }
}

/* Checks whether a specified pixel is set */
static int pixelSet(int row, int col) {
    UINT8 *scrBase;
    UINT32 byteOff;
    UINT8 mask;

    scrBase = (UINT8 *)testBuf;
    byteOff = (UINT32)row * SCRROWBYTES;
    byteOff += col >> 3;
    mask = (UINT8)(0x80 >> (col & 7));

    return (scrBase[byteOff] & mask) != 0;
}

/* Checks whether entire framebuffer is black */
static int allBlack(void) {
    int i;

    for (i = 0; i < SCRNLNGS; i++) {
        if (testBuf[i] != 0xFFFFFFFFUL) {
            return 0;
        }
    }

    return 1;
}

/* Checks whether entire framebuffer is cleared */
static int allClear(void) {
    int i;

    for (i = 0; i < SCRNLNGS; i++) {
        if (testBuf[i] != 0) {
            return 0;
        }
    }

    return 1;
}

/* Visually tests raster functions on physical screen */
static void testVisual(void) {
    UINT32 *base;
    UINT8 *base8;
    int i;

    base = (UINT32 *)Physbase();
    base8 = (UINT8 *)base;

    /* Stage 1: Fill screen black */
    for (i = 0; i < SCRNLNGS; i++) {
        base[i] = 0xFFFFFFFFUL;
    }
    Cnecin();

    /* Stage 2: Clear center rectangle */
    clear_region(base, 160, 260, 80, 120);
    Cnecin();

    /* Stage 3: Clip rectangle at top-left */
    clear_region(base, -20, -20, 50, 50);
    Cnecin();

    /* Stage 4: Clip rectangle at bottom-right */
    clear_region(base, 350, 600, 80, 100);
    Cnecin();

    /* Stage 5: Clear entire screen */
    clear_screen(base);
    Cnecin();

    /* Stage 6: Draw cross using individual pixels */
    for (i = 280; i < 360; i++) {
        plot_pixel(base8, 200, i);
    }

    for (i = 160; i < 240; i++) {
        plot_pixel(base8, i, 320);
    }

    Cnecin();

    /* Clear display before automated test results */
    clear_screen(base);
}

/* Tests clearing the entire framebuffer */
static void testClearScreen(void) {
    int passed;

    fillBlack();
    clear_screen(testBuf);

    passed = allClear();

    printf("clear_screen: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Tests normal region clearing and top-left clipping */
static void testClearRegion(void) {
    int passed;

    /* Normal rectangular region */
    fillBlack();
    clear_region(testBuf, 100, 200, 80, 120);

    passed = !pixelSet(100, 200);
    passed &= !pixelSet(179, 319);
    passed &= pixelSet(99, 200);
    passed &= pixelSet(180, 200);
    passed &= pixelSet(100, 199);
    passed &= pixelSet(100, 320);

    printf("clear_region normal: %s\n",
           passed ? "PASS" : "FAIL");

    /* Top-left boundary clipping */
    fillBlack();
    clear_region(testBuf, -20, -20, 50, 50);

    passed = !pixelSet(0, 0);
    passed &= !pixelSet(29, 29);
    passed &= pixelSet(0, 30);
    passed &= pixelSet(30, 0);

    printf("clear_region top-left: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Tests clipping at right and bottom boundaries */
static void testRegionBounds(void) {
    int passed;

    /* Right boundary */
    fillBlack();
    clear_region(testBuf, 100, 620, 20, 50);

    passed = !pixelSet(100, 620);
    passed &= !pixelSet(119, 639);
    passed &= pixelSet(100, 619);
    passed &= pixelSet(120, 620);

    printf("clear_region right: %s\n",
           passed ? "PASS" : "FAIL");

    /* Bottom boundary */
    fillBlack();
    clear_region(testBuf, 390, 100, 30, 20);

    passed = !pixelSet(390, 100);
    passed &= !pixelSet(399, 119);
    passed &= pixelSet(389, 100);
    passed &= pixelSet(399, 120);

    printf("clear_region bottom: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Tests regions that should not modify the framebuffer */
static void testRegionNoOp(void) {
    int passed;

    /* Region completely outside screen */
    fillBlack();
    clear_region(testBuf, 450, 700, 20, 20);
    passed = allBlack();

    printf("clear_region outside: %s\n",
           passed ? "PASS" : "FAIL");

    /* Zero width */
    fillBlack();
    clear_region(testBuf, 100, 100, 50, 0);
    passed = allBlack();

    printf("clear_region zero width: %s\n",
           passed ? "PASS" : "FAIL");

    /* Zero height */
    fillBlack();
    clear_region(testBuf, 100, 100, 0, 50);
    passed = allBlack();

    printf("clear_region zero height: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Tests clearing selected bits within one byte */
static void testRegionBits(void) {
    int passed;

    fillBlack();
    clear_region(testBuf, 50, 10, 1, 3);

    passed = !pixelSet(50, 10);
    passed &= !pixelSet(50, 11);
    passed &= !pixelSet(50, 12);
    passed &= pixelSet(50, 9);
    passed &= pixelSet(50, 13);

    printf("clear_region bit mask: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Tests plotting a single pixel */
static void testPlotPixel(void) {
    UINT8 *scrBase;
    int passed;

    scrBase = (UINT8 *)testBuf;
    clear_screen(testBuf);

    /* Plot pixel near screen center */
    plot_pixel(scrBase, 200, 320);

    passed = pixelSet(200, 320);
    passed &= !pixelSet(200, 319);
    passed &= !pixelSet(200, 321);
    passed &= !pixelSet(199, 320);
    passed &= !pixelSet(201, 320);

    printf("plot_pixel normal: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Tests plotting pixels at screen corners */
static void testPixelCorners(void) {
    UINT8 *scrBase;
    int passed;

    scrBase = (UINT8 *)testBuf;
    clear_screen(testBuf);

    plot_pixel(scrBase, 0, 0);
    plot_pixel(scrBase, 0, 639);
    plot_pixel(scrBase, 399, 0);
    plot_pixel(scrBase, 399, 639);

    passed = pixelSet(0, 0);
    passed &= pixelSet(0, 639);
    passed &= pixelSet(399, 0);
    passed &= pixelSet(399, 639);

    printf("plot_pixel corners: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Tests invalid pixel coordinates */
static void testPixelBounds(void) {
    UINT8 *scrBase;
    int passed;

    scrBase = (UINT8 *)testBuf;
    clear_screen(testBuf);

    plot_pixel(scrBase, -1, 100);
    plot_pixel(scrBase, 400, 100);
    plot_pixel(scrBase, 100, -1);
    plot_pixel(scrBase, 100, 640);

    passed = allClear();

    printf("plot_pixel bounds: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Tests plotting individual bits across byte boundaries */
static void testPixelBits(void) {
    UINT8 *scrBase;
    int passed;

    scrBase = (UINT8 *)testBuf;
    clear_screen(testBuf);

    /* Pixels in adjacent bytes */
    plot_pixel(scrBase, 50, 7);
    plot_pixel(scrBase, 50, 8);

    passed = pixelSet(50, 7);
    passed &= pixelSet(50, 8);
    passed &= !pixelSet(50, 6);
    passed &= !pixelSet(50, 9);

    printf("plot_pixel bit positions: %s\n",
           passed ? "PASS" : "FAIL");
}