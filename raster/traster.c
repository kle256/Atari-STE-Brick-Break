/* Tests monochrome framebuffer raster drawing routines */
#include <stdio.h>
#include <osbind.h>
#include "raster.h"

#define SCRNLNGS 8000
#define SCRROWBYTES 80
#define SCRROWLONGS 20
#define SCRWDTH 640
#define SCRHGHT 400

static UINT32 testBuf[SCRNLNGS];
static UINT32 expectedBuf[SCRNLNGS];

/* Private test helpers */
static void fillBlack(void);
static int pixelSet(int row, int col);
static int allBlack(void);
static int allClear(void);
static void markExpected(int row, int col);
static int matchExpected(void);
static int checkHLine(int row, int col, UINT16 length);
static int checkVLine(int row, int col, UINT16 length);
static int checkOctant(int row, int col, int sy, int sx,
                       int steep, int reverse);
static int checkAxis(int startRow, int startCol,
                     int endRow, int endCol);
static int checkDiag45(int startRow, int startCol,
                       int endRow, int endCol);
static int checkRect(int row, int col, UINT16 length, UINT16 width);
static int checkTriangle(int row, int col, UINT16 triangle_base,
                         UINT16 height, UINT8 direction);

/* Test functions */
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
static void testHLine(void);
static void testHLinePreserve(void);
static void testVLine(void);
static void testVLinePreserve(void);
static void testLineOctants(void);
static void testLineReverse(void);
static void testLineAxes(void);
static void testLineDiag(void);
static void testLineSingle(void);
static void testLinePreserve(void);
static void testRectangle(void);
static void testRectanglePreserve(void);
static void testTriangle(void);
static void testTrianglePreserve(void);

int main() {
    testVisual();

    testClearScreen();
    testClearRegion();
    testRegionBounds();
    testRegionNoOp();
    testRegionBits();

    testPlotPixel();
    testPixelCorners();
    testPixelBounds();
    testPixelBits();

    testHLine();
    testHLinePreserve();

    testVLine();
    testVLinePreserve();

    testLineOctants();
    testLineReverse();
    testLineAxes();
    testLineDiag();
    testLineSingle();
    testLinePreserve();

    testRectangle();
    testRectanglePreserve();

    testTriangle();
    testTrianglePreserve();

    return 0;
}

/* Fill test framebuffer with black pixels */
static void fillBlack(void) {
    int i;

    for (i = 0; i < SCRNLNGS; i++) {
        testBuf[i] = 0xFFFFFFFF;
    }
}

/* Check whether specified pixel is set */
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

/* Check whether entire framebuffer is black */
static int allBlack(void) {
    int i;

    for (i = 0; i < SCRNLNGS; i++) {
        if (testBuf[i] != 0xFFFFFFFF) {
            return 0;
        }
    }

    return 1;
}

/* Check whether entire framebuffer is clear */
static int allClear(void) {
    int i;

    for (i = 0; i < SCRNLNGS; i++) {
        if (testBuf[i] != 0) {
            return 0;
        }
    }

    return 1;
}

/* Set one expected pixel using 32-bit longwords */
static void markExpected(int row, int col) {
    UINT32 lngOff, bitMask;
    int bitShift;

    lngOff = (UINT32)row * SCRROWLONGS;
    lngOff += col >> 5;

    bitShift = 31 - (col & 31);
    bitMask = (UINT32)1 << bitShift;

    expectedBuf[lngOff] |= bitMask;
}

/* Compare actual and expected framebuffers */
static int matchExpected(void) {
    int i;

    for (i = 0; i < SCRNLNGS; i++) {
        if (testBuf[i] != expectedBuf[i]) {
            return 0;
        }
    }

    return 1;
}

/* Visually test raster drawing on physical screen */
static void testVisual(void) {
    UINT32 *base;
    UINT8 *base8;
    int i;

    base = (UINT32 *)Physbase();
    base8 = (UINT8 *)base;

    /* Stage 1: Fill screen black */
    for (i = 0; i < SCRNLNGS; i++) {
        base[i] = 0xFFFFFFFF;
    }
    Cnecin();

    /* Stage 2: Clear center rectangle */
    clear_region(base, 160, 260, 80, 120);
    Cnecin();

    /* Stage 3: Top-left clipping */
    clear_region(base, -20, -20, 50, 50);
    Cnecin();

    /* Stage 4: Bottom-right clipping */
    clear_region(base, 350, 600, 80, 100);
    Cnecin();

    /* Stage 5: Clear entire screen */
    clear_screen(base);
    Cnecin();

    /* Stage 6: Draw cross using pixels */
    for (i = 280; i < 360; i++) {
        plot_pixel(base8, 200, i);
    }

    for (i = 160; i < 240; i++) {
        plot_pixel(base8, i, 320);
    }
    Cnecin();

    /* Stage 7: Draw horizontal lines */
    clear_screen(base);

    plot_horizontal_line(base, 60, 100, 20);
    plot_horizontal_line(base, 110, 100, 140);
    plot_horizontal_line(base, 160, 200, 250);
    plot_horizontal_line(base, 210, -30, 180);
    plot_horizontal_line(base, 260, 550, 150);
    plot_horizontal_line(base, 310, 32, 576);
    Cnecin();

    /* Stage 8: Draw vertical lines */
    clear_screen(base);

    plot_vertical_line(base, 40, 80, 80);
    plot_vertical_line(base, 70, 160, 150);
    plot_vertical_line(base, -30, 260, 110);
    plot_vertical_line(base, 320, 380, 120);
    plot_vertical_line(base, 40, 639, 300);
    plot_vertical_line(base, 20, 500, 350);
    Cnecin();

    /* Stage 9: Draw diagonal lines */
    clear_screen(base);

    plot_line(base, 30, 30, 120, 160);
    plot_line(base, 30, 160, 120, 30);
    plot_line(base, 130, 30, 170, 220);
    plot_line(base, 130, 220, 170, 30);
    plot_line(base, 200, 50, 330, 100);
    plot_line(base, 200, 100, 330, 50);
    plot_line(base, 200, 300, 360, 460);
    plot_line(base, 360, 300, 200, 460);
    Cnecin();

    /* Stage 10: Draw rectangle outlines */
    clear_screen(base);

    plot_rectangle(base, 40, 60, 50, 100);
    plot_rectangle(base, 100, 200, 120, 180);
    plot_rectangle(base, -20, -20, 70, 100);
    plot_rectangle(base, 330, 560, 100, 120);
    plot_rectangle(base, 260, 300, 1, 120);
    plot_rectangle(base, 100, 500, 150, 1);
    Cnecin();

    /* Stage 11: Draw all four triangle orientations */
    clear_screen(base);

    plot_triangle(base, 40, 60, 100, 80, 0);
    plot_triangle(base, 40, 300, 100, 80, 1);
    plot_triangle(base, 340, 60, 100, 80, 2);
    plot_triangle(base, 340, 300, 100, 80, 3);

    /* Unequal base and height */
    plot_triangle(base, 150, 390, 170, 45, 0);
    plot_triangle(base, 310, 600, 50, 120, 3);
    Cnecin();

    /* Clear display before automated tests */
    clear_screen(base);
}

/* Test clearing entire framebuffer */
static void testClearScreen(void) {
    int passed;

    fillBlack();
    clear_screen(testBuf);

    passed = allClear();

    printf("clear_screen: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Test normal region clearing and top-left clipping */
static void testClearRegion(void) {
    int passed;

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

    fillBlack();
    clear_region(testBuf, -20, -20, 50, 50);

    passed = !pixelSet(0, 0);
    passed &= !pixelSet(29, 29);
    passed &= pixelSet(0, 30);
    passed &= pixelSet(30, 0);

    printf("clear_region top-left: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Test region clipping at right and bottom edges */
static void testRegionBounds(void) {
    int passed;

    fillBlack();
    clear_region(testBuf, 100, 620, 20, 50);

    passed = !pixelSet(100, 620);
    passed &= !pixelSet(119, 639);
    passed &= pixelSet(100, 619);
    passed &= pixelSet(120, 620);

    printf("clear_region right: %s\n",
           passed ? "PASS" : "FAIL");

    fillBlack();
    clear_region(testBuf, 390, 100, 30, 20);

    passed = !pixelSet(390, 100);
    passed &= !pixelSet(399, 119);
    passed &= pixelSet(389, 100);
    passed &= pixelSet(399, 120);

    printf("clear_region bottom: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Test regions that should make no changes */
static void testRegionNoOp(void) {
    int passed;

    fillBlack();
    clear_region(testBuf, 450, 700, 20, 20);
    passed = allBlack();

    printf("clear_region outside: %s\n",
           passed ? "PASS" : "FAIL");

    fillBlack();
    clear_region(testBuf, 100, 100, 50, 0);
    passed = allBlack();

    printf("clear_region zero width: %s\n",
           passed ? "PASS" : "FAIL");

    fillBlack();
    clear_region(testBuf, 100, 100, 0, 50);
    passed = allBlack();

    printf("clear_region zero height: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Test clearing individual bits within a byte */
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

/* Test plotting a single pixel */
static void testPlotPixel(void) {
    UINT8 *scrBase;
    int passed;

    scrBase = (UINT8 *)testBuf;
    clear_screen(testBuf);

    plot_pixel(scrBase, 200, 320);

    passed = pixelSet(200, 320);
    passed &= !pixelSet(200, 319);
    passed &= !pixelSet(200, 321);
    passed &= !pixelSet(199, 320);
    passed &= !pixelSet(201, 320);

    printf("plot_pixel normal: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Test pixel plotting at screen corners */
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

/* Test invalid pixel coordinates */
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

/* Test pixel plotting across byte boundaries */
static void testPixelBits(void) {
    UINT8 *scrBase;
    int passed;

    scrBase = (UINT8 *)testBuf;
    clear_screen(testBuf);

    plot_pixel(scrBase, 50, 7);
    plot_pixel(scrBase, 50, 8);

    passed = pixelSet(50, 7);
    passed &= pixelSet(50, 8);
    passed &= !pixelSet(50, 6);
    passed &= !pixelSet(50, 9);

    printf("plot_pixel bit positions: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Check horizontal line against expected framebuffer */
static int checkHLine(int row, int col, UINT16 length) {
    int r, c, i, expected;
    UINT32 rowOff;
    long endCol;

    clear_screen(testBuf);
    plot_horizontal_line(testBuf, row, col, length);

    endCol = (long)col + length;

    for (r = 0; r < SCRHGHT; r++) {
        if (r == row) {
            for (c = 0; c < SCRWDTH; c++) {
                expected = length > 0 && c >= col;
                expected &= (long)c < endCol;

                if (pixelSet(r, c) != expected) {
                    return 0;
                }
            }
        } else {
            rowOff = (UINT32)r * SCRROWLONGS;

            for (i = 0; i < SCRROWLONGS; i++) {
                if (testBuf[rowOff + i] != 0) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

/* Test horizontal line positions and clipping */
static void testHLine(void) {
    int passed;

    passed = checkHLine(100, 5, 10);
    printf("hline single long: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkHLine(100, 28, 10);
    printf("hline cross long: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkHLine(200, 12, 160);
    printf("hline multiple longs: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkHLine(200, 32, 64);
    printf("hline aligned: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkHLine(120, -20, 100);
    printf("hline left clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkHLine(150, 620, 100);
    printf("hline right clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkHLine(0, 0, 640);
    printf("hline top row: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkHLine(399, 0, 640);
    printf("hline bottom row: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkHLine(200, 100, 0);
    printf("hline zero length: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkHLine(100, -100, 40);
    passed &= checkHLine(100, 700, 40);
    passed &= checkHLine(-1, 100, 40);
    passed &= checkHLine(400, 100, 40);

    printf("hline outside screen: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Verify existing pixels are preserved by horizontal line */
static void testHLinePreserve(void) {
    UINT8 *scrBase;
    int passed;

    scrBase = (UINT8 *)testBuf;
    clear_screen(testBuf);

    plot_pixel(scrBase, 200, 199);
    plot_pixel(scrBase, 200, 320);
    plot_pixel(scrBase, 199, 250);

    plot_horizontal_line(testBuf, 200, 200, 120);

    passed = pixelSet(200, 199);
    passed &= pixelSet(200, 320);
    passed &= pixelSet(199, 250);
    passed &= pixelSet(200, 200);
    passed &= pixelSet(200, 319);
    passed &= !pixelSet(201, 250);

    printf("hline preserve pixels: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Check vertical line against expected framebuffer */
static int checkVLine(int row, int col, UINT16 length) {
    UINT32 expected, bitMask, rowOff;
    long endRow;
    int r, i, inCol, drawRow;

    clear_screen(testBuf);
    plot_vertical_line(testBuf, row, col, length);

    endRow = (long)row + length;
    inCol = col >= 0 && col < SCRWDTH;
    bitMask = 0;

    if (inCol) {
        bitMask = (UINT32)0x80000000 >> (col & 31);
    }

    for (r = 0; r < SCRHGHT; r++) {
        rowOff = (UINT32)r * SCRROWLONGS;

        drawRow = length > 0 && inCol;
        drawRow &= r >= row && (long)r < endRow;

        for (i = 0; i < SCRROWLONGS; i++) {
            expected = 0;

            if (drawRow && i == (col >> 5)) {
                expected = bitMask;
            }

            if (testBuf[rowOff + i] != expected) {
                return 0;
            }
        }
    }

    return 1;
}

/* Test vertical line positions and clipping */
static void testVLine(void) {
    int passed;

    passed = checkVLine(100, 200, 80);
    printf("vline normal: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkVLine(50, 0, 100);
    printf("vline left edge: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkVLine(50, 639, 100);
    printf("vline right edge: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkVLine(50, 31, 100);
    passed &= checkVLine(50, 32, 100);
    printf("vline long boundary: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkVLine(-20, 100, 80);
    printf("vline top clipping: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkVLine(380, 100, 50);
    printf("vline bottom clipping: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkVLine(0, 320, 400);
    printf("vline full height: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkVLine(200, 320, 1);
    printf("vline single pixel: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkVLine(200, 320, 0);
    printf("vline zero length: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkVLine(100, -1, 50);
    passed &= checkVLine(100, 640, 50);
    passed &= checkVLine(-100, 100, 50);
    passed &= checkVLine(400, 100, 50);

    printf("vline outside screen: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Verify existing pixels are preserved by vertical line */
static void testVLinePreserve(void) {
    UINT8 *scrBase;
    int passed;

    scrBase = (UINT8 *)testBuf;
    clear_screen(testBuf);

    plot_pixel(scrBase, 100, 99);
    plot_pixel(scrBase, 100, 101);
    plot_pixel(scrBase, 99, 100);
    plot_pixel(scrBase, 180, 100);

    plot_vertical_line(testBuf, 100, 100, 80);

    passed = pixelSet(100, 99);
    passed &= pixelSet(100, 101);
    passed &= pixelSet(99, 100);
    passed &= pixelSet(180, 100);
    passed &= pixelSet(100, 100);
    passed &= pixelSet(179, 100);
    passed &= !pixelSet(181, 100);

    printf("vline preserve pixels: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Check eight-pixel shallow or steep Bresenham pattern */
static int checkOctant(int row, int col, int sy, int sx,
                       int steep, int reverse) {
    static int minorOff[8] = {0, 0, 1, 1, 2, 2, 3, 3};
    int endRow, endCol, r, c, i;

    clear_screen(testBuf);
    clear_screen(expectedBuf);

    if (steep) {
        endRow = row + sy * 7;
        endCol = col + sx * 3;
    } else {
        endRow = row + sy * 3;
        endCol = col + sx * 7;
    }

    if (reverse) {
        plot_line(testBuf, endRow, endCol, row, col);
    } else {
        plot_line(testBuf, row, col, endRow, endCol);
    }

    for (i = 0; i < 8; i++) {
        if (steep) {
            r = i;
            c = minorOff[i];
        } else {
            r = minorOff[i];
            c = i;
        }

        markExpected(row + sy * r, col + sx * c);
    }

    return matchExpected();
}

/* Check horizontal or vertical Bresenham line */
static int checkAxis(int startRow, int startCol,
                     int endRow, int endCol) {
    int first, last, i;

    clear_screen(testBuf);
    clear_screen(expectedBuf);

    plot_line(testBuf, startRow, startCol, endRow, endCol);

    if (startRow == endRow) {
        first = startCol;
        last = endCol;

        if (first > last) {
            first = endCol;
            last = startCol;
        }

        for (i = first; i <= last; i++) {
            markExpected(startRow, i);
        }
    } else {
        first = startRow;
        last = endRow;

        if (first > last) {
            first = endRow;
            last = startRow;
        }

        for (i = first; i <= last; i++) {
            markExpected(i, startCol);
        }
    }

    return matchExpected();
}

/* Check 45-degree line against expected pixels */
static int checkDiag45(int startRow, int startCol,
                       int endRow, int endCol) {
    int rowStep, colStep, steps, i;

    clear_screen(testBuf);
    clear_screen(expectedBuf);

    plot_line(testBuf, startRow, startCol, endRow, endCol);

    rowStep = 1;
    colStep = 1;

    if (endRow < startRow) {
        rowStep = -1;
    }

    if (endCol < startCol) {
        colStep = -1;
    }

    steps = endRow - startRow;

    if (steps < 0) {
        steps = steps * -1;
    }

    for (i = 0; i <= steps; i++) {
        markExpected(startRow + i * rowStep,
                     startCol + i * colStep);
    }

    return matchExpected();
}

/* Test all eight Bresenham octants */
static void testLineOctants(void) {
    int passed;

    passed = checkOctant(200, 320, 1, 1, 0, 0);
    printf("line shallow down-right: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkOctant(200, 320, 1, -1, 0, 0);
    printf("line shallow down-left: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkOctant(200, 320, -1, 1, 0, 0);
    printf("line shallow up-right: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkOctant(200, 320, -1, -1, 0, 0);
    printf("line shallow up-left: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkOctant(200, 320, 1, 1, 1, 0);
    printf("line steep down-right: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkOctant(200, 320, 1, -1, 1, 0);
    printf("line steep down-left: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkOctant(200, 320, -1, 1, 1, 0);
    printf("line steep up-right: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkOctant(200, 320, -1, -1, 1, 0);
    printf("line steep up-left: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Test reversed starting and ending points */
static void testLineReverse(void) {
    int passed;

    passed = checkOctant(200, 320, 1, 1, 0, 1);
    printf("line reverse shallow: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkOctant(200, 320, -1, 1, 1, 1);
    printf("line reverse steep: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Test horizontal and vertical lines using Bresenham */
static void testLineAxes(void) {
    int passed;

    passed = checkAxis(100, 50, 100, 250);
    printf("line horizontal forward: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkAxis(100, 250, 100, 50);
    printf("line horizontal reverse: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkAxis(50, 320, 300, 320);
    printf("line vertical forward: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkAxis(300, 320, 50, 320);
    printf("line vertical reverse: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Test long 45-degree diagonals */
static void testLineDiag(void) {
    int passed;

    passed = checkDiag45(0, 0, 399, 399);
    printf("line 45-degree down-right: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkDiag45(0, 639, 399, 240);
    printf("line 45-degree down-left: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Test drawing a single pixel using Bresenham */
static void testLineSingle(void) {
    int passed;

    clear_screen(testBuf);
    clear_screen(expectedBuf);

    plot_line(testBuf, 200, 320, 200, 320);
    markExpected(200, 320);

    passed = matchExpected();

    printf("line single pixel: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Verify existing pixels are preserved by diagonal line */
static void testLinePreserve(void) {
    UINT8 *scrBase;
    int passed;

    scrBase = (UINT8 *)testBuf;
    clear_screen(testBuf);

    plot_pixel(scrBase, 100, 99);
    plot_pixel(scrBase, 100, 108);
    plot_pixel(scrBase, 99, 104);
    plot_pixel(scrBase, 104, 103);

    plot_line(testBuf, 100, 100, 103, 107);

    passed = pixelSet(100, 99);
    passed &= pixelSet(100, 108);
    passed &= pixelSet(99, 104);
    passed &= pixelSet(104, 103);

    passed &= pixelSet(100, 100);
    passed &= pixelSet(100, 101);
    passed &= pixelSet(101, 102);
    passed &= pixelSet(101, 103);
    passed &= pixelSet(102, 104);
    passed &= pixelSet(102, 105);
    passed &= pixelSet(103, 106);
    passed &= pixelSet(103, 107);

    passed &= !pixelSet(101, 101);
    passed &= !pixelSet(103, 108);

    printf("line preserve pixels: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Build expected rectangle outline and compare framebuffers */
static int checkRect(int row, int col, UINT16 length, UINT16 width) {
    long endRow, endCol;
    int r, c;

    clear_screen(testBuf);
    clear_screen(expectedBuf);

    plot_rectangle(testBuf, row, col, length, width);

    if (length == 0 || width == 0) {
        return matchExpected();
    }

    endRow = (long)row + length - 1;
    endCol = (long)col + width - 1;

    /* Expected top horizontal edge */
    if (row >= 0 && row < SCRHGHT) {
        for (c = 0; c < SCRWDTH; c++) {
            if (c >= col && (long)c <= endCol) {
                markExpected(row, c);
            }
        }
    }

    /* Expected bottom horizontal edge */
    if (endRow >= 0 && endRow < SCRHGHT) {
        for (c = 0; c < SCRWDTH; c++) {
            if (c >= col && (long)c <= endCol) {
                markExpected((int)endRow, c);
            }
        }
    }

    /* Expected left vertical edge */
    if (col >= 0 && col < SCRWDTH) {
        for (r = 0; r < SCRHGHT; r++) {
            if (r >= row && (long)r <= endRow) {
                markExpected(r, col);
            }
        }
    }

    /* Expected right vertical edge */
    if (endCol >= 0 && endCol < SCRWDTH) {
        for (r = 0; r < SCRHGHT; r++) {
            if (r >= row && (long)r <= endRow) {
                markExpected(r, (int)endCol);
            }
        }
    }

    return matchExpected();
}

/* Test rectangle outlines, clipping and dimensions */
static void testRectangle(void) {
    int passed;

    passed = checkRect(100, 200, 50, 100);
    printf("rectangle normal: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(0, 0, 400, 640);
    printf("rectangle screen border: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(-20, -20, 70, 100);
    printf("rectangle top-left clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(370, 600, 80, 100);
    printf("rectangle bottom-right clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(-20, 100, 80, 120);
    printf("rectangle top clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(100, -20, 80, 120);
    printf("rectangle left clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(100, 600, 80, 100);
    printf("rectangle right clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(370, 100, 80, 120);
    printf("rectangle bottom clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(100, 200, 1, 100);
    printf("rectangle one row: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(100, 200, 50, 1);
    printf("rectangle one column: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(100, 200, 1, 1);
    printf("rectangle single pixel: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(100, 200, 0, 100);
    printf("rectangle zero height: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(100, 200, 50, 0);
    printf("rectangle zero width: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkRect(450, 100, 50, 100);
    passed &= checkRect(100, 700, 50, 100);
    passed &= checkRect(-100, -100, 20, 20);

    printf("rectangle outside screen: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Verify outline does not fill or overwrite existing pixels */
static void testRectanglePreserve(void) {
    UINT8 *scrBase;
    int passed;

    scrBase = (UINT8 *)testBuf;
    clear_screen(testBuf);

    /* Existing pixels inside and outside rectangle */
    plot_pixel(scrBase, 115, 215);
    plot_pixel(scrBase, 99, 240);
    plot_pixel(scrBase, 110, 199);

    plot_rectangle(testBuf, 100, 200, 40, 80);

    passed = pixelSet(115, 215);
    passed &= pixelSet(99, 240);
    passed &= pixelSet(110, 199);

    /* Rectangle corners */
    passed &= pixelSet(100, 200);
    passed &= pixelSet(100, 279);
    passed &= pixelSet(139, 200);
    passed &= pixelSet(139, 279);

    /* Middle of each edge */
    passed &= pixelSet(100, 240);
    passed &= pixelSet(139, 240);
    passed &= pixelSet(120, 200);
    passed &= pixelSet(120, 279);

    /* Interior must remain unfilled */
    passed &= !pixelSet(110, 220);
    passed &= !pixelSet(120, 240);

    /* Pixels outside rectangle must remain clear */
    passed &= !pixelSet(99, 200);
    passed &= !pixelSet(140, 200);
    passed &= !pixelSet(100, 199);
    passed &= !pixelSet(100, 280);

    printf("rectangle preserve pixels: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Compare triangle against three independently drawn edges */
static int checkTriangle(int row, int col, UINT16 triangle_base,
                         UINT16 height, UINT8 direction) {
    int vRow, hCol, rowDir, colDir;

    clear_screen(testBuf);
    clear_screen(expectedBuf);

    plot_triangle(testBuf, row, col, triangle_base, height, direction);

    if (triangle_base == 0 || height == 0 || direction > 3) {
        return matchExpected();
    }

    /* Calculate direction of horizontal and vertical legs */
    rowDir = 1;
    colDir = 1;

    if (direction == 2 || direction == 3) {
        rowDir = -1;
    }

    if (direction == 1 || direction == 3) {
        colDir = -1;
    }

    vRow = row + rowDir * (height - 1);
    hCol = col + colDir * (triangle_base - 1);

    /* Construct expected triangle from its three vertices */
    plot_line(expectedBuf, row, col, row, hCol);
    plot_line(expectedBuf, row, col, vRow, col);
    plot_line(expectedBuf, vRow, col, row, hCol);

    return matchExpected();
}

/* Test all triangle directions and boundary conditions */
static void testTriangle(void) {
    int passed;

    passed = checkTriangle(100, 100, 50, 50, 0);
    printf("triangle down-right: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(100, 300, 50, 50, 1);
    printf("triangle down-left: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(300, 100, 50, 50, 2);
    printf("triangle up-right: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(300, 300, 50, 50, 3);
    printf("triangle up-left: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(120, 120, 80, 25, 0);
    printf("triangle wide down-right: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(120, 320, 25, 80, 1);
    printf("triangle tall down-left: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(300, 120, 80, 25, 2);
    printf("triangle wide up-right: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(300, 320, 25, 80, 3);
    printf("triangle tall up-left: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(-20, -20, 70, 70, 0);
    printf("triangle top-left clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(-20, 659, 70, 70, 1);
    printf("triangle top-right clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(420, -20, 70, 70, 2);
    printf("triangle bottom-left clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(420, 659, 70, 70, 3);
    printf("triangle bottom-right clip: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(200, 320, 1, 1, 0);
    printf("triangle single pixel: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(200, 320, 1, 1, 3);
    printf("triangle single pixel reverse: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(200, 320, 1, 40, 1);
    printf("triangle one-column width: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(200, 320, 40, 1, 2);
    printf("triangle one-row height: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(100, 100, 0, 50, 0);
    printf("triangle zero base: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(100, 100, 50, 0, 1);
    printf("triangle zero height: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(100, 100, 50, 50, 4);
    printf("triangle invalid direction: %s\n",
           passed ? "PASS" : "FAIL");

    passed = checkTriangle(-100, -100, 20, 20, 0);
    passed &= checkTriangle(500, 700, 30, 30, 0);
    passed &= checkTriangle(-100, 700, 20, 20, 1);
    passed &= checkTriangle(500, -100, 20, 20, 2);
    printf("triangle outside screen: %s\n",
           passed ? "PASS" : "FAIL");
}

/* Check existing pixels and unfilled interior */
static void testTrianglePreserve(void) {
    UINT8 *scrBase;
    int passed;

    scrBase = (UINT8 *)testBuf;
    clear_screen(testBuf);

    /* Existing interior and exterior pixels */
    plot_pixel(scrBase, 115, 120);
    plot_pixel(scrBase, 99, 110);
    plot_pixel(scrBase, 125, 99);

    plot_triangle(testBuf, 100, 100, 60, 40, 0);

    passed = pixelSet(115, 120);
    passed &= pixelSet(99, 110);
    passed &= pixelSet(125, 99);

    /* Check all three vertices */
    passed &= pixelSet(100, 100);
    passed &= pixelSet(100, 159);
    passed &= pixelSet(139, 100);

    /* Other pixels should remain clear */
    passed &= !pixelSet(120, 110);
    passed &= !pixelSet(140, 100);
    passed &= !pixelSet(100, 160);

    printf("triangle preserve pixels: %s\n",
           passed ? "PASS" : "FAIL");
}
