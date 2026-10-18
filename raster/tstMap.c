/* Bitmap raster tests for the Atari ST monochrome screen */
#include <stdio.h>
#include <osbind.h>
#include <linea.h>
#include "raster.h"

#define SCRWDTH 640
#define SCRHGHT 400
#define SCRROWBYTES 80
#define SCRNLNGS 8000
#define BMP8WDTH 8
#define BMP16WDTH 16
#define BMP32WDTH 32
#define CHARWDTH 8
#define CHARHGHT 16
#define FNTRWBYTES 256

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
static void buildExpectedChar(int row, int col, char ch);
static void checkCharacter(const char *label, int row, int col, char ch);
static void testPreserveChar(void);
static void buildExpectedString(int row, int col, char *str);
static void checkString(const char *label, int row, int col, char *str);
static void testPreserveString(void);
static void checkBitmap(const char *label, int row, int col,
                        const UINT8 *bitmap, UINT16 height);
static void testPreserve(void);
static void testVisual(void);

int main(void) {
    int i, pass8, fail8, pass16, fail16, pass32, fail32;
    int passBeforeString, failBeforeString;

    /* V_FNT_AD becomes available after Line-A initialization */
    linea0();
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

    pass32 = passCnt - pass8 - pass16;
    fail32 = failCnt - fail8 - fail16;
    printf("\n32-bit bitmap tests: %d passed, %d failed\n", pass32, fail32);
    printf("Bitmap tests total: %d passed, %d failed\n", passCnt, failCnt);

    /* Test individual system-font characters at different alignments */
    checkCharacter("capital A aligned", 100, 80, 'A');
    checkCharacter("capital B col 81", 100, 81, 'B');
    checkCharacter("lowercase g", 100, 103, 'g');
    checkCharacter("number 5", 100, 120, '5');
    checkCharacter("question mark", 100, 127, '?');
    checkCharacter("space", 100, 144, ' ');
    checkCharacter("exclamation mark", 100, 159, '!');
    checkCharacter("column 0", 100, 0, 'W');
    checkCharacter("column 7", 100, 7, 'M');
    checkCharacter("column 8", 100, 8, 'X');
    checkCharacter("column 15", 100, 15, 'Z');
    checkCharacter("column 31", 100, 31, 'N');

    /* Character boundaries are 8 pixels wide and 16 rows high */
    checkCharacter("left clip -1", 160, -1, 'W');
    checkCharacter("left clip -4", 160, -4, 'W');
    checkCharacter("left clip -7", 160, -7, 'W');
    checkCharacter("outside left", 160, -8, 'W');
    checkCharacter("right edge col 632", 160, 632, 'M');
    checkCharacter("right clip 633", 160, 633, 'M');
    checkCharacter("right clip 636", 160, 636, 'M');
    checkCharacter("right clip 639", 160, 639, 'M');
    checkCharacter("outside right", 160, 640, 'M');

    checkCharacter("top clip -1", -1, 240, 'A');
    checkCharacter("top clip -7", -7, 240, 'A');
    checkCharacter("top clip -15", -15, 240, 'A');
    checkCharacter("outside top", -16, 240, 'A');
    checkCharacter("bottom edge row 384", 384, 240, 'g');
    checkCharacter("bottom clip 390", 390, 240, 'g');
    checkCharacter("bottom clip 399", 399, 240, 'g');
    checkCharacter("outside bottom", 400, 240, 'g');
    checkCharacter("top-left clip", -4, -3, 'K');
    checkCharacter("bottom-right clip", 394, 636, 'R');
    testPreserveChar();

    printf("\nCharacter tests: %d passed, %d failed\n",
           passCnt - pass8 - pass16 - pass32,
           failCnt - fail8 - fail16 - fail32);
    passBeforeString = passCnt;
    failBeforeString = failCnt;

    /* Test complete strings, character spacing, and clipping */
    checkString("empty aligned", 100, 80, "");
    checkString("empty outside", 400, 640, "");
    checkString("single aligned", 100, 80, "A");
    checkString("single unaligned", 100, 83, "A");
    checkString("word aligned", 100, 80, "HELLO");
    checkString("word unaligned", 100, 83, "HELLO");
    checkString("words and spaces", 100, 81, "BRICK BREAKER");
    checkString("mixed case", 100, 82, "Hello World");
    checkString("numbers", 100, 83, "0123456789");
    checkString("punctuation", 100, 84, "!?.,+-:;()");
    checkString("repeated letters", 100, 85, "MMMMMMMM");
    checkString("single space", 100, 80, " ");
    checkString("multiple spaces", 100, 80, "A   B");
    checkString("column 0", 100, 0, "ABC");
    checkString("column 7", 100, 7, "ABC");
    checkString("column 8", 100, 8, "ABC");
    checkString("column 31", 100, 31, "ABC");
    checkString("right full col 632", 100, 632, "A");
    checkString("right clip col 636", 100, 636, "A");
    checkString("right clip col 639", 100, 639, "W");
    checkString("right stop aligned", 100, 608, "ABCDEFGH");
    checkString("right stop unaligned", 100, 609, "ABCDEFGH");
    checkString("outside right 640", 100, 640, "ABC");
    checkString("outside right 700", 100, 700, "ABC");
    checkString("left clip -3", 100, -3, "ABC");
    checkString("left clip -8", 100, -8, "ABC");
    checkString("left crosses screen", 100, -16, "ABCDE");
    checkString("outside left", 100, -40, "AB");
    checkString("top clip -1", -1, 100, "ABC");
    checkString("top clip -15", -15, 100, "ABC");
    checkString("bottom clip 390", 390, 100, "ABC");
    checkString("bottom clip 399", 399, 100, "ABC");
    checkString("outside top", -16, 100, "ABC");
    checkString("outside bottom", 400, 100, "ABC");
    checkString("top-left clip", -5, -5, "ABC");
    checkString("bottom-right clip", 395, 637, "ABC");
    testPreserveString();

    printf("\nString tests: %d passed, %d failed\n",
           passCnt - passBeforeString, failCnt - failBeforeString);
    printf("All tests total: %d passed, %d failed\n", passCnt, failCnt);
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

/* Build expected glyph one pixel at a time from Atari font data */
static void buildExpectedChar(int row, int col, char ch) {
    UINT8 *font;
    UINT8 srcBits, bitMask;
    int r, c, fontOff;
    long scrRow, scrCol;

    font = (UINT8 *)V_FNT_AD;
    for (r = 0; r < CHARHGHT; r++) {
        fontOff = r * FNTRWBYTES;
        fontOff += (UINT8)ch;
        srcBits = font[fontOff];

        for (c = 0; c < CHARWDTH; c++) {
            scrRow = (long)row + r;
            scrCol = (long)col + c;
            if (scrRow < 0 || scrRow >= SCRHGHT ||
                scrCol < 0 || scrCol >= SCRWDTH) {
                continue;
            }

            bitMask = (UINT8)(0x80 >> c);
            if (srcBits & bitMask) {
                markExpected((int)scrRow, (int)scrCol);
            }
        }
    }
}

/* Build expected string using individual font glyph pixels */
static void buildExpectedString(int row, int col, char *str) {
    long textCol;
    int i;

    textCol = col;
    i = 0;
    while (str[i] != '\0') {
        if (textCol < SCRWDTH) {
            buildExpectedChar(row, (int)textCol, str[i]);
        }
        textCol += CHARWDTH;
        i++;
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

/* Test character rendering against independent per-pixel placement */
static void checkCharacter(const char *label, int row, int col, char ch) {
    int passed;

    clearBuffers();
    buildExpectedChar(row, col, ch);
    plot_character((UINT8 *)testBuf, row, col, ch);
    passed = matchBuffers();

    printf("character %-24s %s\n", label, passed ? "PASS" : "FAIL");
    if (passed) {
        passCnt++;
    } else {
        failCnt++;
    }
}

/* Test complete string output against independent expected pixels */
static void checkString(const char *label, int row, int col, char *str) {
    int passed;

    clearBuffers();
    buildExpectedString(row, col, str);
    plot_string((UINT8 *)testBuf, row, col, str);
    passed = matchBuffers();

    printf("string %-24s %s\n", label, passed ? "PASS" : "FAIL");
    if (passed) {
        passCnt++;
    } else {
        failCnt++;
    }
}

/* String rendering must not erase pixels already on screen */
static void testPreserveString(void) {
    UINT8 *bytes;
    int passed;

    clearBuffers();
    bytes = (UINT8 *)testBuf;

    plot_pixel(bytes, 100, 99);
    plot_pixel(bytes, 100, 100);
    plot_pixel(bytes, 110, 108);
    plot_pixel(bytes, 117, 117);
    plot_pixel(bytes, 120, 125);

    markExpected(100, 99);
    markExpected(100, 100);
    markExpected(110, 108);
    markExpected(117, 117);
    markExpected(120, 125);

    buildExpectedString(100, 100, "TEST");
    plot_string(bytes, 100, 100, "TEST");

    passed = matchBuffers();
    printf("string %-24s %s\n", "preserve pixels",
           passed ? "PASS" : "FAIL");
    if (passed) {
        passCnt++;
    } else {
        failCnt++;
    }
}

/* Character rendering should preserve other pixels */
static void testPreserveChar(void) {
    int passed;
    UINT8 *bytes;

    clearBuffers();
    bytes = (UINT8 *)testBuf;

    plot_pixel(bytes, 100, 99);
    plot_pixel(bytes, 100, 100);
    plot_pixel(bytes, 110, 107);
    plot_pixel(bytes, 116, 103);

    markExpected(100, 99);
    markExpected(100, 100);
    markExpected(110, 107);
    markExpected(116, 103);

    buildExpectedChar(100, 100, 'A');
    plot_character(bytes, 100, 100, 'A');

    passed = matchBuffers();
    printf("character %-24s %s\n", "preserve pixels",
           passed ? "PASS" : "FAIL");
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

    /* Stage 4: Atari 8x16 system-font characters */
    for (c = 0; c < 8; c++) {
        plot_character(bytes, 80, 80 + c * 12, (char)('A' + c));
        plot_character(bytes, 110, 81 + c * 12, (char)('a' + c));
        plot_character(bytes, 140, 82 + c * 12, (char)('0' + c));
    }

    /* Larger character samples, with different byte alignments */
    plot_character(bytes, 195, 80, 'W');
    plot_character(bytes, 195, 93, 'M');
    plot_character(bytes, 195, 106, '?');
    plot_character(bytes, 195, 119, '!');

    /* Characters clipped at the four screen edges */
    plot_character(bytes, 250, -3, 'W');
    plot_character(bytes, -5, 360, 'A');
    plot_character(bytes, 394, 360, 'g');
    plot_character(bytes, 250, 636, 'M');

    Cnecin();
    clear_screen(scrBase);

    /* Stage 5: complete strings with normal and clipped positions */
    plot_string(bytes, 70, 80, "BRICK BREAKER");
    plot_string(bytes, 102, 81, "RASTER LIBRARY COMPLETE");
    plot_string(bytes, 134, 82, "0123456789 !?.,+-");
    plot_string(bytes, 180, 80, "Aligned text");
    plot_string(bytes, 205, 83, "Unaligned text");

    /* Screen-edge strings should be cropped without stray pixels */
    plot_string(bytes, 260, -12, "LEFT CLIPPED");
    plot_string(bytes, -8, 320, "TOP CLIPPED");
    plot_string(bytes, 390, 340, "BOTTOM CLIPPED");
    plot_string(bytes, 290, 600, "RIGHT EDGE CLIPPED");

    Cnecin();
    clear_screen(scrBase);
}
