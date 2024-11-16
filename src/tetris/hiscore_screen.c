/** @file hiscore_screen.c */
#include <stdio.h>
#include <string.h>

#include <ratr0/ratr0.h>
#include "hiscore_screen.h"
#include "title_screen.h"
#include "game_data.h"
#include "utils.h"
#include "../default_copper.h"

static Ratr0Engine *engine = NULL;
extern struct Ratr0Stage *main_stage, *title_screen;

extern RATR0_ACTION_ID action_quit, action_drop;

#ifdef DEBUG
#define TITLE_PATH_PAL ("tetris/assets/hiscores_title.ts")
#define FONT_PATH_PAL ("tetris/assets/hiscores_font.ts")
#else
#define TITLE_PATH_PAL ("assets/hiscores_title.ts")
#define FONT_PATH_PAL ("assets/hiscores_font.ts")
#endif

struct Ratr0TileSheet title_ts, font_ts;
struct Ratr0Surface title_surf, font_surf;

void draw_char16(struct Ratr0Surface *surface,
                 struct Ratr0Surface *font_surface,
                 UINT8 c, UINT16 x, UINT16 y)
{
    // map the ASCII character c to a position within the tile set
    // The font set is 512x72 and 32 horizontal chars and 3 vertical chars
    // so each character is 16 pixels wide and 24 pixels high
    // the first character is SPACE (ASCII 0x20) and the last is DEL (0x7F)
    int char_index = c - ' ';
    int row = char_index / 32;
    int col = char_index % 32;
    int tiley = row  * 24; // this is the row in the tile set
    int tilex = col * 16;
    ratr0_blit_ad_d(surface, font_surface,
                    tilex, tiley, // src
                    x, y, // dest, x must be multiple of 16
                    0xf0,  // D <- A
                    0, // never shift
                    0xffff, 0xffff,
                    1, 24); // always 1 word, always 24 pixels
}

#define DIGITBUFFER_SIZE (10)
UINT8 digit_buffer[DIGITBUFFER_SIZE];

void draw_row(struct Ratr0Surface *surface,
              UINT8 pos,
              UINT8 initials[MAX_HIGHSCORE_INITIALS_CHARS],
              UINT32 score)
{
    int num_digits = extract_digits(digit_buffer, DIGITBUFFER_SIZE, score);
    int x = 0;
    int y = 48 + ((pos - 1) * 26);
    draw_char16(surface, &font_surf, '0', x, y);
    x += 16;
    draw_char16(surface, &font_surf, '0' + pos, x, y);
    x += 16;
    draw_char16(surface, &font_surf, '.', x, y);
    x += 16;

    // initials
    for (int i = 0; i < 3; i++) {
        draw_char16(surface, &font_surf, initials[i], x, y);
        x += 16;
    }
    // spacing
    for (int i = 0; i < (14 - num_digits); i++) {
        draw_char16(surface, &font_surf, '.', x, y);
        x += 16;
    }
    for (int i  = num_digits - 1; i >= 0; i--) {
        draw_char16(surface, &font_surf, digit_buffer[i], x, y);
        x += 16;
    }
}

static BOOL hiscore_screen_first_update = FALSE;
static UINT16 hiscore_screen_timeout = 0;
static UINT16 hiscore_blitcount = 0;
#define HISCORE_SCREEN_TIMEOUT (300)
void hiscore_screen_update(struct Ratr0Stage *this_stage,
                           struct Ratr0DisplayBuffer *backbuffer,
                           UINT8 frame_elapsed) {
    if (!hiscore_screen_first_update) {
#ifdef DEBUG
        fprintf(debug_fp, "HISCORE SCREEN DISPLAY, SETTING TIMEOUT\n");
        fflush(debug_fp);
#endif
        hiscore_screen_timeout = HISCORE_SCREEN_TIMEOUT;
        hiscore_screen_first_update = TRUE;
    }
    if (hiscore_blitcount < 2) {
        // blit the high score list from the font
        for (int i = 0; i < num_hiscore_entries; i++) {
            draw_row(&backbuffer->surface, i + 1,
                     hiscore_list[i].initials, hiscore_list[i].points);
        }
        hiscore_blitcount++;  // blit twice, once for each buffer
    }
    hiscore_screen_timeout--;
    if (hiscore_screen_timeout <= 0) {
#ifdef DEBUG
        fprintf(debug_fp, "HISCORE SCREEN TIMEOUT\n");
        fflush(debug_fp);
#endif
        ratr0_stages_set_current_stage(title_screen);
    }
}

static void _load_resources(void)
{
    // Load background
    BOOL ts_read = ratr0_resources_read_tilesheet(TITLE_PATH_PAL, &title_ts);
    ratr0_resources_init_surface_from_tilesheet(&title_surf, &title_ts);
    ratr0_display_set_palette(title_ts.palette, 32, 0);

    // from here we don't need to the memory for the background
    // anymore, we can free the surface and tilesheet
    //ratr0_resources_free_tilesheet_data(&title_ts);
    ts_read = ratr0_resources_read_tilesheet(FONT_PATH_PAL, &font_ts);
    ratr0_resources_init_surface_from_tilesheet(&font_surf, &font_ts);

    // just to make sure we have a valid hiscore list
    load_hiscore_list();
}

void hiscore_screen_on_enter(struct Ratr0Stage *this_stage)
{
#ifdef DEBUG
    fprintf(debug_fp, "HISCORESCREEN_ON_ENTER()\n");
    fflush(debug_fp);
#endif

    hiscore_screen_first_update = FALSE;
    hiscore_blitcount = 0;

    // set new copper list
    ratr0_display_init_copper_list(default_copper, DEFAULT_COPPER_SIZE_WORDS,
                                   &DEFAULT_COPPER_INFO);
    ratr0_display_set_copperlist(default_copper, DEFAULT_COPPER_SIZE_WORDS,
                                 &DEFAULT_COPPER_INFO);
    _load_resources();

#ifdef TEST
    int display_height = 256;
#else
    int display_height = display_info.is_pal ? 256 : 200;
#endif
    ratr0_blit_clear16(&ratr0_display_get_front_buffer()->surface,
                       0, 0, 320, display_height);
    ratr0_blit_clear16(&ratr0_display_get_back_buffer()->surface,
                       0, 0, 320, display_height);

    ratr0_display_blit_surface_to_buffers(&title_surf, 48, 0);
}

void hiscore_screen_on_exit(struct Ratr0Stage *this_stage)
{
#ifdef DEBUG
    fprintf(debug_fp, "HISCORESCREEN_ON_EXIT()\n");
    fflush(debug_fp);
#endif
}

struct Ratr0Stage *setup_hiscorescreen_stage(Ratr0Engine *eng)
{
    engine = eng;
    // Use the stages module to create a stage and run that
    struct Ratr0NodeFactory *node_factory = ratr0_stages_get_node_factory();
    struct Ratr0Stage *this_stage = node_factory->create_stage();
    this_stage->update = hiscore_screen_update;
    this_stage->on_enter = hiscore_screen_on_enter;
    this_stage->on_exit = hiscore_screen_on_exit;

    return this_stage;
}
