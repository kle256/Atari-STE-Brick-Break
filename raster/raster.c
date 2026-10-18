/* raster module. TODO: fill in each function and document assumptions. */
#include "raster.h"
#include <stdio.h>
#include <osbind.h>
#include <linea.h>

#define SCRNLNGS 8000
#define SCRWDTH 640
#define SCRHGHT 400
#define SCRROWBYTES 80
#define SCCROWWRDS 40
#define SCRROWLONGS 20
#define WRDMASK 0xFFFF
#define LONGMASK 0xFFFFFFFF
#define LINEWDTH 1
#define BMP8WDTH 8
#define BMP16WDTH 16

static int pxInBnds(int row, int col);
static UINT16 clipRegion(int *row, int *col, UINT16 *height, UINT16 width,
                         UINT16 *skipX, UINT16 *skipY);


void clear_screen(UINT32 *base) {
    int i;
    UINT32 *loc = base;

    for (i = 0; i < SCRNLNGS; i++) {
        *(loc++) = 0;
    }
}

void clear_region(UINT32 *base, int row, int col, UINT16 length, UINT16 width) {
    UINT8 *scrBase, *rowPtr; 
    UINT8 strtBit, endBit, mask;
    UINT16 visWdth, skipX, skipY, strtByte, endByte, r, b, height;
    int clipRow, clipCol;

    scrBase = (UINT8 *)base;
    clipRow = row;
    clipCol = col;
    height = length;

    /* Set visible region*/
    visWdth = clipRegion(&clipRow, &clipCol, &height, width, &skipX, &skipY);

    if (visWdth == 0 || height == 0) {
        return;
    }

    /* Calculate start and end byte position*/
    strtByte = clipCol >> 3;
    endByte = (clipCol + visWdth -1) >> 3;
    strtBit = clipCol & 7;
    endBit = (clipCol + visWdth -1) & 7;

    /* clear pixel in visible region*/
    for (r = 0; r < height; r++) {
        rowPtr = scrBase + (UINT32)(clipRow + r) * SCRROWBYTES;

        for (b = strtByte; b <= endByte; b++) {
            mask = 0xFF;

            if (b== strtByte) {
                mask &= (UINT8)(0xFF >> strtBit);
            }

            if (b== endByte) {
                mask &= (UINT8)(0xFF << (7 - endBit));
            }

            rowPtr[b] &= (UINT8)~mask;
        }
    }
}

/* Plot pixel within screen boundaries*/
void plot_pixel(UINT8 *base, int row, int col) {
    UINT32 byteOff;
    UINT8 bitMask;
    int bitPos, bitShift;
    
    if(pxInBnds(row, col)) {
        /* Calculate byte position*/
        byteOff = (UINT32)row * SCRROWBYTES;
        byteOff += col >> 3;

        /* Calculate bit mask*/
        bitPos = col & 7;
        bitShift = 7 - bitPos;
        bitMask = (UINT8)(1 << bitShift);

        /* Set pixel*/
        base[byteOff] |= bitMask;
    }

}

void plot_horizontal_line(UINT32 *base, int row, int col, UINT16 length) {
    UINT32 *strtPtr, *endPtr, *currPtr;
    UINT32 leftMask, rgtMask, rowOff;
    UINT16 visWdth, skipX, skipY, height;
    int clipRow, clipCol, endCol, strtBit, endBit, rgtShift, midLngs, i;

    clipRow = row;
    clipCol = col;
    height = 1;

    /* Determine visible portion of line*/
    visWdth = clipRegion(&clipRow, &clipCol, &height, length, &skipX, &skipY);

    if(visWdth == 0 || height ==0) {
        return;
    }

    endCol = clipCol + visWdth;
    endCol -= 1;

    /* Calculate row offset*/
    rowOff = (UINT32)clipRow * SCRROWLONGS;

    /* Locate starting and ending longs*/
    strtPtr = base + rowOff;
    strtPtr += clipCol >> 5;

    endPtr = base + rowOff;
    endPtr += endCol >> 5;

    /* Calculate bit positions */
    strtBit = clipCol & 31;
    endBit = endCol & 31;

    /* Calculate starting long mask*/
    leftMask = LONGMASK >> strtBit;

    /* Calculate ending long mask*/
    rgtShift = 31 - endBit;
    rgtMask = LONGMASK << rgtShift;

    /* Plot line with longs*/
    if (strtPtr == endPtr) {
        *strtPtr |= leftMask & rgtMask;

    } else { 
        /* Set start long*/
        *strtPtr |= leftMask;

        /* Fill longs between start and end*/
        currPtr = strtPtr + 1;
        midLngs = endPtr - currPtr;

        for (i = 0; i < midLngs; i++) {
            *currPtr = LONGMASK;
            currPtr++;
        }

        /* Set ending long*/
        *endPtr |= rgtMask;
    }
}

void plot_vertical_line(UINT32 *base, int row, int col, UINT16 length) {
    UINT32 *currPtr; 
    UINT32 bitMask, rowOff;
    UINT16 visWdth, skipX, skipY, height;
    int clipRow, clipCol, bitPos, bitShift, i;

    clipRow = row;
    clipCol = col;
    height = length;

    /* Determine visible portion of line*/
    visWdth = clipRegion(&clipRow, &clipCol, &height, LINEWDTH, &skipX, &skipY);

    if (visWdth == 0 || height == 0) {
        return;
    }

    /* Calculate starting long*/
    rowOff = (UINT32)clipRow * SCRROWLONGS;

    currPtr = base + rowOff;
    currPtr += clipCol >> 5;

    /* Calculate bit position*/
    bitPos = clipCol & 31;
    bitShift = 31 - bitPos;

    /* Shift single set bit into position*/
    bitMask = (UINT32)1 << bitShift;

    /* Plot vertical line*/
    for (i = 0; i < height; i++) {
        *currPtr |= bitMask;
        currPtr += SCRROWLONGS;
    }
}

void plot_line(UINT32 *base, int start_row, int start_col, int end_row, int end_col) {
    int dx, dy, err, e2, sx, sy;

    /* Calculate horizontal distance*/
    dx = end_col - start_col;

    /* flip dx if negative*/
    if (dx < 0) {
        dx = dx * -1;
    }

    /* Calculate negative vertical distance*/
    dy = end_row - start_row;

    /* flip dy if negative*/
    if (dy < 0) {
        dy = dy * -1;
    }

    dy = dy * -1;

    /* Determine horizontal direction*/
    if (start_col < end_col) {
        sx = 1;
    } else {
        sx = -1;
    }

    /* Determine vertical direction*/
    if (start_row < end_row) {
        sy = 1;
    } else {
        sy = -1;
    }

    err = dx + dy;

    /* Plot starting pixel*/
    plot_pixel((UINT8 *)base, start_row, start_col);

    /* Plot line using Bresenham's algorithm*/
    while(start_row != end_row || start_col != end_col) {
        e2 = 2 * err;

        /* Move horizontally*/
        if (e2 >= dy) {
            err += dy;
            start_col += sx;
        }

        /* move vertically */
        if (e2 <= dx) {
            err += dx;
            start_row += sy;
        }
        plot_pixel((UINT8 *)base, start_row, start_col);
    }
}

void plot_rectangle(UINT32 *base, int row, int col, UINT16 length, UINT16 width) {
    int endRow, endCol;

    if (length == 0 || width == 0) {
        return;
    }

    /* Calculate ending positions*/
    endRow = row + length;
    endRow -= 1;
    endCol = col + width;
    endCol -= 1;

    /* Plot horizontal edges*/
    plot_horizontal_line(base, row, col, width);
    plot_horizontal_line(base, endRow, col, width);

    /* Plot vertical edges*/
    plot_vertical_line(base, row, col, length);
    plot_vertical_line(base, row, endCol, length);
}

void plot_square(UINT32 *base, int row, int col, UINT16 side) {
    plot_rectangle(base, row, col, side, side);
}

void plot_triangle(UINT32 *base, int row, int col, UINT16 triangle_base, 
                   UINT16 height, UINT8 direction) {
    int topRow, btmRow, lftCol, rgtCol, heightOff, baseOff;

    if (triangle_base == 0 || height == 0) {
        return;
    }

    if (direction > 3) {
        return;
    }

    /* Calculate offset*/
    heightOff = height - 1;
    baseOff = triangle_base - 1;

    /* Calculate ending position*/
    topRow = row - heightOff;
    btmRow = row + heightOff;
    lftCol = col - baseOff;
    rgtCol = col + baseOff;

    /* Down and right*/
    if (direction == 0) {
        plot_horizontal_line(base, row, col, triangle_base);
        plot_vertical_line(base, row, col, height);
        plot_line(base, btmRow, col, row, rgtCol);
    
    /* Down and left*/
    } else if (direction == 1) {
        plot_horizontal_line(base, row, lftCol, triangle_base);
        plot_vertical_line(base, row, col, height);
        plot_line(base, btmRow, col, row, lftCol);

    /* Up and right*/
    } else if (direction == 2) {
        plot_horizontal_line(base, row, col, triangle_base);
        plot_vertical_line(base, topRow, col, height);
        plot_line(base, topRow, col, row, rgtCol);

    /* Up and left*/
    } else if (direction == 3) {
        plot_horizontal_line(base, row, lftCol, triangle_base);
        plot_vertical_line(base, topRow, col, height);
        plot_line(base, topRow, col, row, lftCol);

    }
}

void plot_8bit_bitmap(UINT8 *base, int row, int col, const UINT8 *bitmap, UINT16 height) {
    UINT16 visWdth, skipX, skipY, visHght, i, bmpRow;
    UINT32 byteOff;
    UINT8 srcBits, visMask;
    int clipRow, clipCol, bitShft, maskShft, endBit, nextShft;

    clipRow = row;
    clipCol = col;
    visHght = height;

    /* Determine visible portion of bitmap*/
    visWdth = clipRegion(&clipRow, &clipCol, &visHght, BMP8WDTH, &skipX, &skipY);

    if (visWdth == 0 || visHght == 0) {
        return;
    }

    /*Calculate starting byte position*/
    byteOff = (UINT32)clipRow * SCRROWBYTES;
    byteOff += clipCol >> 3;

    /* Calculate bit shift in byte*/
    bitShft = clipCol & 7;

    /* Calculate visible bitmap mask*/
    maskShft = BMP8WDTH - visWdth;

    /*Shift mask to visible width*/
    visMask = 0xFF;
    visMask = (UINT8)(visMask << maskShft);

    /* Calculate byte boundary*/
    endBit = bitShft + visWdth;
    nextShft = BMP8WDTH - bitShft;

    /* Read visible bitmap rows*/
    for (i = 0; i < visHght; i++) {
        bmpRow = i + skipY;
        srcBits = bitmap[bmpRow];

        /* Skip clipped source columns*/
        srcBits <<= skipX;

        /* Mask pixels outside visible width*/
        srcBits &= visMask;

        /* Plot pixels in first byte*/
        base[byteOff] |= (UINT8)(srcBits >> bitShft);

        /* Plot remaining pixels in next byte*/
        if (endBit > BMP8WDTH) {
            base[byteOff + 1] |= (UINT8)(srcBits << nextShft);
        }

        /* Advance to next screen row*/
        byteOff += SCRROWBYTES;
    }

}

void plot_16bit_bitmap(UINT16 *base, int row, int col, const UINT16 *bitmap, UINT16 height) {
    UINT16 visWdth, skipX, skipY, visHght, srcBits, visMask;
    UINT32 wordOff;
    int clipRow, clipCol, bitShft, maskShft, endBit, nextShft, i, bmpRow;

    clipRow = row;
    clipCol = col;
    visHght = height;

    /* Determine visible portion of bitmap*/
    visWdth = clipRegion(&clipRow, &clipCol, &visHght, BMP16WDTH, &skipX, &skipY);

    if (visWdth == 0 || visHght == 0) {
        return;
    }
    wordOff = (UINT32)clipRow * SCCROWWRDS;
    wordOff += clipCol >> 4;

    /* Calculate bit shift in word*/
    bitShft = clipCol & 15;

    /* Calculate visible bitmap mask*/
    maskShft = BMP16WDTH - visWdth;

    /* Shift mask to visible width*/
    visMask = WRDMASK;
    visMask = (UINT16)(visMask << maskShft);

    /* Calculate word boundary*/
    endBit = bitShft + visWdth;
    nextShft = BMP16WDTH - bitShft;

    /* Plot visible bitmap rows*/
    for (i = 0; i < visHght; i++) {
        bmpRow = i + skipY;
        srcBits = bitmap[bmpRow];

        /* Skip clipped source columns*/
        srcBits <<= skipX;

        /* Mask pixels outside visible width*/
        srcBits &= visMask;

        /* Plot pixels in first word*/
        base[wordOff] |= (UINT16)(srcBits >> bitShft);

        /* Plot remaining pixels in next word*/

        if (endBit > BMP16WDTH) {
            base[wordOff + 1] |= (UINT16)(srcBits << nextShft);
        }
        wordOff += SCCROWWRDS;
    }

    
}


/* Clip rectangular region to screen boundary*/
static UINT16 clipRegion(int *row, int *col, UINT16 *height, UINT16 width,
                         UINT16 *skipX, UINT16 *skipY) {
    long off;
    UINT16 visWdth;

    *skipX = 0;
    *skipY = 0;

    if (*height == 0 || width == 0) {
        return 0;
    }

    /* Clip top boundary*/
    if (*row < 0) {
        off = -(long)*row;

        if ( off >= *height) {
            return 0;
        }

        *skipY = (UINT16)off;
        *height -= *skipY;
        *row = 0;
    }

    /* Clip bottom boundary*/
    if (*row >= SCRHGHT) {
        return 0;
    }

    if ((long)*row + *height > SCRHGHT) {
        *height = SCRHGHT - *row;
    }

    /* Clip left Boundary*/
    if (*col < 0) {
        off = -(long)*col;

        if (off >= width) {
            return 0;
        }

        *skipX = (UINT16)off;
        *col = 0;
    }

    /* Clip right boundary*/
    if (*col >= SCRWDTH) {
        return 0;
    }

    visWdth = width -*skipX;

    if (visWdth > SCRWDTH - *col) {
        visWdth = SCRWDTH - *col;
    }

    return visWdth;
                    
}

static int pxInBnds(int row, int col) {
    return (unsigned int)row < SCRHGHT && (unsigned int)col < SCRWDTH;
}