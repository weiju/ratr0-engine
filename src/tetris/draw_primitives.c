#include "draw_primitives.h"

void draw_1x3(struct Ratr0Surface *backbuffer_surface,
              struct Ratr0Surface *tiles_surface,
              int color, int row, int col)
{
    int x = col * 8 + BOARD_X0;
    int y = row * 8 + BOARD_Y0;
    int blit_width_words = 2;
    int shift = x % 16;
    x -= shift;

    UINT16 afwm, alwm;
    if (shift == 8) {
        afwm = 0x00ff;
        alwm = 0xffff;
        blit_width_words++;
    } else {
        afwm = 0xffff;
        alwm = 0xff00;
    }

    // 3x1 block
    ratr0_blit_ad_d(backbuffer_surface, tiles_surface,
                    0, color * 32,
                    x, y,
                    0xfc, 0,
                    afwm, alwm,
                    blit_width_words, 8);
}

void draw_1x2(struct Ratr0Surface *backbuffer_surface,
              struct Ratr0Surface *tiles_surface,
              int color, int row, int col)
{
    int x = col * 8 + BOARD_X0;
    int y = row * 8 + BOARD_Y0;
    int blit_width_words = 1;
    int shift = x % 16;
    x -= shift;

    UINT16 afwm, alwm;
    if (shift == 8) {
        afwm = 0x00ff;
        alwm = 0xff00;
        blit_width_words++;
    } else {
        afwm = 0xffff;
        alwm = 0xffff;
    }

    // 2x1 block
    ratr0_blit_ad_d(backbuffer_surface, tiles_surface,
                    0, color * 32,
                    x, y, 0xfc, 0,
                    afwm, alwm,
                    blit_width_words, 8);
}

void draw_2x2(struct Ratr0Surface *backbuffer_surface,
              struct Ratr0Surface *tiles_surface,
              int color, int row, int col)
{
    int x = col * 8 + BOARD_X0;
    int y = row * 8 + BOARD_Y0;
    int blit_width_words = 1;
    int shift = x % 16;
    x -= shift;

    UINT16 afwm, alwm;
    if (shift == 8) {
        afwm = 0x00ff;
        alwm = 0xff00;
        blit_width_words++;
    } else {
        afwm = 0xffff;
        alwm = 0xffff;
    }

    // 2x1 block
    ratr0_blit_ad_d(backbuffer_surface, tiles_surface,
                    0, color * 32,
                    x, y, 0xfc, 0,
                    afwm, alwm,
                    blit_width_words, 16);
}

/**
 * Blits a 1x4 block
 */
void draw_1x4(struct Ratr0Surface *backbuffer_surface,
              struct Ratr0Surface *tiles_surface,
              int color, int row, int col)
{
    int x = col * 8 + BOARD_X0;
    int y = row * 8 + BOARD_Y0;
    int blit_width_words = 2;
    int shift = x % 16;
    x -= shift;

    UINT16 afwm = 0xffff, alwm = 0xffff;
    if (shift == 8) {
        blit_width_words++;
    }
    ratr0_blit_ad_d(backbuffer_surface, tiles_surface,
                    0, color * 32,
                    x, y,
                    // we actually perform that shift !!!
                    0xfc, shift,
                    afwm, alwm,
                    blit_width_words, 8);
}

void draw_nx1(struct Ratr0Surface *backbuffer_surface,
              struct Ratr0Surface *tiles_surface,
              int color, int row, int col, int num_rows)
{
    int x = col * 8 + BOARD_X0;
    int y = row * 8 + BOARD_Y0;
    int shift = x % 16;
    x -= shift;

    UINT16 afwm, alwm;
    if (shift == 8) {
        afwm = 0x00ff;
        alwm = 0xffff;
    } else {
        afwm = 0xff00;
        alwm = 0xffff;
    }
    // we don't actually need a shift. We just mask either
    // the first or the second block
    ratr0_blit_ad_d(backbuffer_surface, tiles_surface,
                    0, color * 32,
                    x, y, 0xfc, 0,
                    afwm, alwm,
                    1, num_rows * 8);
}

void draw_1x1(struct Ratr0Surface *backbuffer_surface,
              struct Ratr0Surface *tiles_surface,
              int color, int row, int col)
{
    draw_nx1(backbuffer_surface, tiles_surface, color, row, col, 1);
}
void draw_2x1(struct Ratr0Surface *backbuffer_surface,
              struct Ratr0Surface *tiles_surface,
              int color, int row, int col)
{
    draw_nx1(backbuffer_surface, tiles_surface, color, row, col, 2);
}
void draw_3x1(struct Ratr0Surface *backbuffer_surface,
              struct Ratr0Surface *tiles_surface,
              int color, int row, int col)
{
    draw_nx1(backbuffer_surface, tiles_surface, color, row, col, 3);
}
void draw_4x1(struct Ratr0Surface *backbuffer_surface,
              struct Ratr0Surface *tiles_surface,
              int color, int row, int col)
{
    draw_nx1(backbuffer_surface, tiles_surface, color, row, col, 4);
}

void draw_piece(struct Ratr0Surface *backbuffer_surface,
                struct Ratr0Surface *tiles_surface,
                struct DrawSpec *spec, int color, int row, int col)
{
    for (int i = 0; i < spec->num_rects; i++) {
        switch (spec->draw_rects[i].shape) {
        case RS_1x1:
            draw_1x1(backbuffer_surface, tiles_surface,
                     color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_1x2:
            draw_1x2(backbuffer_surface, tiles_surface,
                     color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_1x3:
            draw_1x3(backbuffer_surface, tiles_surface,
                     color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_1x4:
            draw_1x4(backbuffer_surface, tiles_surface,
                     color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_2x1:
            draw_2x1(backbuffer_surface, tiles_surface,
                     color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_3x1:
            draw_3x1(backbuffer_surface, tiles_surface,
                     color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_4x1:
            draw_4x1(backbuffer_surface, tiles_surface,
                     color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        case RS_2x2:
            draw_2x2(backbuffer_surface, tiles_surface,
                     color, spec->draw_rects[i].row + row,
                     spec->draw_rects[i].col + col);
            break;
        default:
            break;
        }
    }
}

void clear_rect(struct Ratr0Surface *backbuffer_surface,
                int row, int col, int num_rows, int num_cols)
{
    int x = BOARD_X0 + col * 8;
    int y = BOARD_Y0 + row * 8;
    ratr0_blit_clear8(backbuffer_surface, x, y, num_cols * 8,
                      num_rows * 8);
}
void clear_piece(struct Ratr0Surface *backbuffer_surface,
                 struct DrawSpec *spec, int row, int col)
{
    for (int i = 0; i < spec->num_rects; i++) {
        switch (spec->draw_rects[i].shape) {
        case RS_1x1:
            clear_rect(backbuffer_surface,
                       row + spec->draw_rects[i].row,
                       col + spec->draw_rects[i].col, 1, 1);
            break;
        case RS_1x2:
            clear_rect(backbuffer_surface,
                       row + spec->draw_rects[i].row,
                       col + spec->draw_rects[i].col, 1, 2);
            break;
        case RS_1x3:
            clear_rect(backbuffer_surface,
                       row + spec->draw_rects[i].row,
                       col + spec->draw_rects[i].col, 1, 3);
            break;
        case RS_1x4:
            clear_rect(backbuffer_surface,
                       row + spec->draw_rects[i].row,
                       col + spec->draw_rects[i].col, 1, 4);
            break;
        case RS_2x1:
            clear_rect(backbuffer_surface,
                       row + spec->draw_rects[i].row,
                       col + spec->draw_rects[i].col, 2, 1);
            break;
        case RS_3x1:
            clear_rect(backbuffer_surface,
                       row + spec->draw_rects[i].row,
                       col + spec->draw_rects[i].col, 3, 1);
            break;
        case RS_4x1:
            clear_rect(backbuffer_surface,
                       row + spec->draw_rects[i].row,
                       col + spec->draw_rects[i].col, 4, 1);
            break;
        case RS_2x2:
            clear_rect(backbuffer_surface,
                       row + spec->draw_rects[i].row,
                       col + spec->draw_rects[i].col, 2, 2);
            break;
        default:
            break;
        }
    }
}

extern FILE *debug_fp;
BOOL digit_drawn = FALSE;
/**
 * the digit is '0'-'9' or '.' or ':'
 */
void draw_digit(struct Ratr0Surface *surface,
                struct Ratr0Surface *digits_surface,
                UINT8 digit,
                UINT16 x, UINT16 y)
{
    UINT16 afwm = 0xff00; //, alwm = 0xffff;

    // the position within the 16 pixel 0 means 0, 1 means 8
    int tiley = (digit - '0')  * 8; // this is the row in the tile set
    if ((x % 16) == 8) {
        afwm = 0x00ff; // use the second 8 pixel block
        x -= 8; // align x to 16 pixel boundary
    }
    ratr0_blit_ad_d(surface, digits_surface,
                    0, tiley, // src
                    x, y, // dest
                    0xfc,  // D <- A + D
                    0, // never shift
                    afwm, 0xffff,
                    1, 8); // always 1 word, always 8 pixels

    if (!digit_drawn) {
        fflush(debug_fp);
    }
    digit_drawn = TRUE;
}

void draw_preview_piece(struct Ratr0Surface *surface,
                        struct Ratr0Surface *preview_surface,
                        UINT8 piece,
                        UINT16 x, UINT16 y)
{
    ratr0_blit_ad_d(surface, preview_surface,
                    0, piece * 16, // src
                    x, y, // dest
                    0xf0,  // D <- A
                    0, // never shift
                    0xffff, 0xffff,
                    2, 16); // always 2 words, always 16 pixels
}
