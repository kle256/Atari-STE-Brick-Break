/* raster module. TODO: fill in each function and document assumptions. */
#include "raster.h"
#include <stdio.h>
#include <osbind.h>
#include <linea.h>

#define SCRNLNGS 8000
#define SCRWDTH 640
#define SCRHGHT 400
#define SCRROWBYTES 80
#define SCRROWLONGS 20
#define LONGMASK 0xFFFFFFFF

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