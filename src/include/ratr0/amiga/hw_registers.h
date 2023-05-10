/** @file hw_registers.h */
#pragma once
#ifndef __RATR0_AMIGA_HW_REGISTERS_H__
#define __RATR0_AMIGA_HW_REGISTERS_H__

// Custom Chip Registers
/** \brief Blitter data register DMA channel D */
#define BLTDDAT       0x000
/** \brief DMA control (read only) */
#define DMACONR       0x002
/** \brief raster beam vertical position (read only) */
#define VPOSR         0x004
/** \brief raster beam horizontal position (read only) */
#define VHPOSR        0x006

/** \brief display window start  */
#define DIWSTRT       0x08e
/** \brief display window stop  */
#define DIWSTOP       0x090
/** \brief DMA data fetch start  */
#define DDFSTRT       0x092
/** \brief DMA data fetch stop  */
#define DDFSTOP       0x094
/** \brief DMA control */
#define DMACON        0x096
/** \brief Bitplane 1 data pointer (hi-word) */
#define BPL1PTH       0x0e0
/** \brief Bitplane 1 data pointer (lo-word) */
#define BPL1PTL       0x0e2
/** \brief Bitplane 2 data pointer (hi-word) */
#define BPL2PTH       0x0e4
/** \brief Bitplane 2 data pointer (lo-word) */
#define BPL2PTL       0x0e6
/** \brief Bitplane 3 data pointer (hi-word) */
#define BPL3PTH       0x0e8
/** \brief Bitplane 3 data pointer (lo-word) */
#define BPL3PTL       0x0ea
/** \brief Bitplane 4 data pointer (hi-word) */
#define BPL4PTH       0x0ec
/** \brief Bitplane 4 data pointer (lo-word) */
#define BPL4PTL       0x0ee
/** \brief Bitplane 5 data pointer (hi-word) */
#define BPL5PTH       0x0f0
/** \brief Bitplane 5 data pointer (lo-word) */
#define BPL5PTL       0x0f2
/** \brief Bitplane 6 data pointer (hi-word) */
#define BPL6PTH       0x0f4
/** \brief Bitplane 6 data pointer (lo-word) */
#define BPL6PTL       0x0f6

/** \brief Bitplane control register 0 */
#define BPLCON0       0x100
/** \brief Bitplane control register 1 */
#define BPLCON1       0x102
/** \brief Bitplane control register 2 */
#define BPLCON2       0x104
/** \brief Bitplane control register 3 */
#define BPLCON3       0x106
/** \brief Bitplane modulo register odd bitplanes */
#define BPL1MOD       0x108
/** \brief Bitplane modulo register even bitplanes */
#define BPL2MOD       0x10a
/** \brief Sprite 0 data pointer (hi-word) */
#define SPR0PTH       0x120
/** \brief Sprite 0 data pointer (lo-word) */
#define SPR0PTL       0x122
/** \brief Sprite 1 data pointer (hi-word) */
#define SPR1PTH       0x124
/** \brief Sprite 1 data pointer (lo-word) */
#define SPR1PTL       0x126
/** \brief Sprite 2 data pointer (hi-word) */
#define SPR2PTH       0x128
/** \brief Sprite 2 data pointer (lo-word) */
#define SPR2PTL       0x12a
/** \brief Sprite 3 data pointer (hi-word) */
#define SPR3PTH       0x12c
/** \brief Sprite 3 data pointer (lo-word) */
#define SPR3PTL       0x12e
/** \brief Sprite 4 data pointer (hi-word) */
#define SPR4PTH       0x130
/** \brief Sprite 4 data pointer (lo-word) */
#define SPR4PTL       0x132
/** \brief Sprite 5 data pointer (hi-word) */
#define SPR5PTH       0x134
/** \brief Sprite 5 data pointer (lo-word) */
#define SPR5PTL       0x136
/** \brief Sprite 6 data pointer (hi-word) */
#define SPR6PTH       0x138
/** \brief Sprite 6 data pointer (lo-word) */
#define SPR6PTL       0x13a
/** \brief Sprite 7 data pointer (hi-word) */
#define SPR7PTH       0x13c
/** \brief Sprite 7 data pointer (lo-word) */
#define SPR7PTL       0x13e

/** \brief color register 0 */
#define COLOR00       0x180
/** \brief color register 1 */
#define COLOR01       0x182
/** \brief color register 2 */
#define COLOR02       0x184
/** \brief color register 3 */
#define COLOR03       0x186
/** \brief color register 4 */
#define COLOR04       0x188
/** \brief color register 5 */
#define COLOR05       0x18a
/** \brief color register 6 */
#define COLOR06       0x18c
/** \brief color register 7 */
#define COLOR07       0x18e
/** \brief color register 8 */
#define COLOR08       0x190
/** \brief color register 9 */
#define COLOR09       0x192
/** \brief color register 10 */
#define COLOR10       0x194
/** \brief color register 11 */
#define COLOR11       0x196
/** \brief color register 12 */
#define COLOR12       0x198
/** \brief color register 13 */
#define COLOR13       0x19a
/** \brief color register 14 */
#define COLOR14       0x19c
/** \brief color register 15 */
#define COLOR15       0x19e
/** \brief color register 16 */
#define COLOR16       0x1a0
/** \brief color register 17 */
#define COLOR17       0x1a2
/** \brief color register 18 */
#define COLOR18       0x1a4
/** \brief color register 19 */
#define COLOR19       0x1a6
/** \brief color register 20 */
#define COLOR20       0x1a8
/** \brief color register 21 */
#define COLOR21       0x1aa
/** \brief color register 22 */
#define COLOR22       0x1ac
/** \brief color register 23 */
#define COLOR23       0x1ae
/** \brief color register 24 */
#define COLOR24       0x1b0
/** \brief color register 25 */
#define COLOR25       0x1b2
/** \brief color register 26 */
#define COLOR26       0x1b4
/** \brief color register 27 */
#define COLOR27       0x1b6
/** \brief color register 28 */
#define COLOR28       0x1b8
/** \brief color register 29 */
#define COLOR29       0x1ba
/** \brief color register 30 */
#define COLOR30       0x1bc
/** \brief color register 31 */
#define COLOR31       0x1be

/** \brief DMA fetch mode */
#define FMODE         0x1fc

#endif /* __RATR0_AMIGA_HW_REGISTERS_H__ */
