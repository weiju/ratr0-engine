/** @file scenes.c */
#include <stdio.h>
#include <ratr0/debug_utils.h>
#include <ratr0/engine.h>
#include <ratr0/scenes.h>

#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <clib/graphics_protos.h>
#include <ratr0/display.h>
#include <ratr0/sprites.h>
#include <ratr0/blitter.h>
#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[33;1mSCENES\033[0m", __VA_ARGS__)

extern struct Custom custom;

/*
 * The scenes system is where games are implemented on a high level and tied together.
 * It builds on all the subsystems that deal with low-level aspects and integrates
 * with the scripting system.
 */
static struct Ratr0ScenesSystem scenes_system;
static struct Ratr0NodeFactory node_factory;
static Ratr0Engine *engine;
static struct Ratr0Scene *current_scene;
static struct Ratr0Backdrop *backdrop;

static void ratr0_scenes_shutdown(void);
static void ratr0_scenes_update(struct Ratr0DisplayBuffer *, UINT8);
static void ratr0_scenes_set_current_scene(struct Ratr0Scene *);

/**
 * Node factory
 */
// *do not* make the arrays static !!!! Otherwise bad things happen when you
// try accessing them outside of the module
struct Ratr0Scene _scenes[10];
static UINT16 next_scene = 0;

struct Ratr0Backdrop _backdrops[2]; // we don't have many of those
static UINT16 next_backdrop = 0;

static struct Ratr0NodeFactory *ratr0_scenes_get_node_factory(void) { return &node_factory; }

static struct Ratr0Scene *ratr0_scenes_create_scene(void)
{
    struct Ratr0Scene *result = &_scenes[next_scene++];
    result->engine = engine;
    result->num_bobs = 0;
    result->num_sprites = 0;

    // TODO: we might want to accept a custom copper list and store it here
    return result;
}

static struct Ratr0Sprite *ratr0_nf_create_sprite(struct Ratr0TileSheet *,
                                                  UINT8[], UINT8, UINT8, BOOL);

struct Ratr0Backdrop *ratr0_nf_create_backdrop(struct Ratr0TileSheet *tilesheet);

struct Ratr0ScenesSystem *ratr0_scenes_startup(Ratr0Engine *eng)
{
    engine = eng;
    current_scene = NULL;

    scenes_system.update = &ratr0_scenes_update;
    scenes_system.shutdown = &ratr0_scenes_shutdown;
    scenes_system.set_current_scene = &ratr0_scenes_set_current_scene;

    // Node factory
    node_factory.create_scene = &ratr0_scenes_create_scene;
    node_factory.create_sprite = &ratr0_nf_create_sprite;
    node_factory.create_backdrop = &ratr0_nf_create_backdrop;
    scenes_system.get_node_factory = &ratr0_scenes_get_node_factory;

    PRINT_DEBUG("Startup finished.");
    return &scenes_system;
}

static void ratr0_scenes_shutdown(void)
{
    PRINT_DEBUG("Shutdown finished.");
}

static void ratr0_scenes_set_current_scene(struct Ratr0Scene *scene)
{
    // Leave previous scene if existing
    if (current_scene && current_scene->on_exit) {
        current_scene->on_exit(scene);
    }

    current_scene = scene;

    // Enter new scene
    if (current_scene && current_scene->backdrop) {
        // Make this the new backdrop for efficiency this is module global
        backdrop = current_scene->backdrop;
        // Blit the backdrop once if it exists
        struct Ratr0Surface *back_buffer, *front_buffer;
        front_buffer = &ratr0_display_get_front_buffer()->surface;
        back_buffer = &ratr0_display_get_back_buffer()->surface;
        OwnBlitter();
        ratr0_blit_rect_simple(front_buffer, &backdrop->surface, 0, 0, 0, 0,
                               backdrop->surface.width, backdrop->surface.height);
        ratr0_blit_rect_simple(back_buffer, &backdrop->surface, 0, 0, 0, 0,
                               backdrop->surface.width, backdrop->surface.height);
        DisownBlitter();
    }
    if (current_scene && current_scene->on_enter) {
        current_scene->on_enter(scene);
    }
}

static struct Ratr0Sprite *ratr0_nf_create_sprite(struct Ratr0TileSheet *tilesheet,
                                                  UINT8 frames[], UINT8 num_frames,
                                                  UINT8 speed, BOOL is_hw)
{
    if (is_hw) {
        return (struct Ratr0Sprite *) ratr0_create_sprite(tilesheet, frames, num_frames, speed);
    } else {
        return (struct Ratr0Sprite *) ratr0_create_bob(tilesheet, frames, num_frames, speed);
    }
}

struct Ratr0Backdrop *ratr0_nf_create_backdrop(struct Ratr0TileSheet *tilesheet)
{
    struct Ratr0Backdrop *result = &_backdrops[next_backdrop++];

    // Initialize the backdrop
    result->surface.width = tilesheet->header.width;
    result->surface.height = tilesheet->header.height;
    result->surface.depth = tilesheet->header.bmdepth;
    result->surface.is_interleaved = tilesheet->header.flags & TSFLAGS_NON_INTERLEAVED == 0;
    result->surface.buffer = engine->memory_system->block_address(tilesheet->h_imgdata);
    result->was_drawn = FALSE;

    return result;
}


/**
 * Restore dirty rectangles from backdrop image
 */
static UINT16 dirty_bltsize = 0;
struct Ratr0Surface *back_buffer;
void process_dirty_rect(UINT16 x, UINT16 y)
{
    if (!dirty_bltsize) {
        dirty_bltsize = ratr0_blit_rect_simple(back_buffer, &backdrop->surface,
                                               x, y, x, y, 16, 16);
    } else {
        ratr0_blit_rect_simple2(back_buffer, &backdrop->surface, x, y, x, y, dirty_bltsize);
    }
}


void add_restore_tiles_for_bob(struct Ratr0Bob *bob)
{
    // Compute dirty rectangles for the BOB
    // determine first and last horizontal tile positions horizontal and vertical
    int tx0 = bob->base_obj.bounds.x >> 4;
    int ty0 = bob->base_obj.bounds.y >> 4;
    int txn = (bob->base_obj.bounds.x + bob->base_obj.bounds.width) >> 4;
    int tyn = (bob->base_obj.bounds.y + bob->base_obj.bounds.height) >> 4;
    int x, y;
    for (y = ty0; y <= tyn; y++) {
        for (x = tx0; x <= txn; x++) {
            add_dirty_rectangle(x, y);
        }
    }
}

/**
 * just fake animation for now until we know it works
 */
static int anim_frames = 0;
BOOL update_bob(struct Ratr0Bob *bob)
{
    BOOL result = FALSE;
    // if there is translation, it means, we need to change the BOB position, but not
    // in this step, because it would mess up our dirty rectangle
    if (bob->base_obj.translate.x || bob->base_obj.translate.y) {
        result = TRUE;
    }
    // switching a BOB frame means it is updated
    bob->base_obj.anim_frames.current_tick++;
    if (bob->base_obj.anim_frames.current_tick >= bob->base_obj.anim_frames.speed) {
        // Add an actual frame switcher
        bob->base_obj.anim_frames.current_frame_idx = (bob->base_obj.anim_frames.current_frame_idx + 1) % bob->base_obj.anim_frames.num_frames;
        bob->base_obj.anim_frames.current_tick = 0;
        result = TRUE;
    }
    return result;
}

void move_bob(struct Ratr0Bob *bob)
{
    bob->base_obj.bounds.x += bob->base_obj.translate.x;
    bob->base_obj.bounds.y += bob->base_obj.translate.y;
    bob->base_obj.translate.x = 0;
    bob->base_obj.translate.y = 0;
}

static current_frame_sprites = 0;
static void _update_sprite(struct Ratr0HWSprite *sprite)
{
    UINT16 hstart, vstart, vstop;
    sprite->base_obj.anim_frames.current_tick++;
    // update frame animation
    if (sprite->base_obj.anim_frames.current_tick >= sprite->base_obj.anim_frames.speed) {
        if (sprite->base_obj.anim_frames.loop_type == RATR0_ANIM_LOOP_TYPE_LOOP) {
            sprite->base_obj.anim_frames.current_frame_idx = (sprite->base_obj.anim_frames.current_frame_idx + 1) % sprite->base_obj.anim_frames.num_frames;
        } else if (sprite->base_obj.anim_frames.loop_type == RATR0_ANIM_LOOP_TYPE_PINGPONG) {
            if (sprite->base_obj.anim_frames.loop_dir > 0) {
                sprite->base_obj.anim_frames.current_frame_idx++;
                if (sprite->base_obj.anim_frames.current_frame_idx == (sprite->base_obj.anim_frames.num_frames - 1)) {
                    sprite->base_obj.anim_frames.loop_dir = -1;
                }
            } else {
                sprite->base_obj.anim_frames.current_frame_idx--;
                if (sprite->base_obj.anim_frames.current_frame_idx == 0) {
                    sprite->base_obj.anim_frames.loop_dir = 1;
                }
            }
        } else {
            // NONE DEBUG
            sprite->base_obj.anim_frames.current_frame_idx = 0;
        }
        sprite->base_obj.anim_frames.current_tick = 0;
    }

    // Set the sprite data to the sprite channel. If it is a an attached
    // sprite, set 2 channels that are at the same position
    UINT16 *sprite_data0 = sprite->sprite_data;
    UINT16 *sprite_data1;

    // set pointer to the frame
    // number of frame words is the height + 2 control words +
    // 2 zero stop words
    int frame_words = (sprite->base_obj.bounds.height + 2) * 2;
    int frames_per_sprite = sprite->is_attached ? 2 : 1;

    // The actual frame index within the source sprite data
    int frame_index = sprite->base_obj.anim_frames.frames[sprite->base_obj.anim_frames.current_frame_idx];
    sprite_data0 += frame_index * frames_per_sprite * frame_words;
    // The second half of the attached sprite is always on the next frame
    sprite_data1 = sprite_data0 + frame_words;

    // update and set sprite position
    // bounds are dependent on window start !!!!

    // TODO: this is actually not completely correct, because this
    // assumes a fixed DIWSTRT
    hstart = sprite->base_obj.bounds.x + DISP_SPRITE_X0_320;
    vstart = sprite->base_obj.bounds.y + DISP_SPRITE_Y0;
    vstop = vstart + sprite->base_obj.bounds.height;

    if (sprite->is_attached) {
        // TODO: currently, this does not look up if the
        // sprites are in the same group, but this needs to be
        // if we want them to display in 16 colors
        ratr0_sprites_set_pos(sprite_data1,
                              hstart, vstart, vstop);
        ratr0_display_set_sprite(current_coplist, current_coplist_size,
                                 current_copper_info,
                                 current_frame_sprites + 1, sprite_data1);
        ratr0_sprites_set_pos(sprite_data0,
                              hstart, vstart, vstop);
        ratr0_display_set_sprite(current_coplist, current_coplist_size,
                                 current_copper_info,
                                 current_frame_sprites, sprite_data0);
        current_frame_sprites += 2;
    } else {
        ratr0_sprites_set_pos(sprite_data0,
                              hstart, vstart, vstop);
        ratr0_display_set_sprite(current_coplist, current_coplist_size,
                                 current_copper_info,
                                 current_frame_sprites, sprite_data0);
        current_frame_sprites++;
    }
}

static void _update_sprites(void)
{
    // update sprites
    for (int i = 0; i < current_scene->num_sprites; i++) {
        _update_sprite(current_scene->sprites[i]);
    }
}

static void ratr0_scenes_update(struct Ratr0DisplayBuffer *backbuffer,
                                UINT8 frames_elapsed)
{
    if (current_scene) {
        // update the scene
        if (current_scene->update) {
            current_scene->update(current_scene, backbuffer, frames_elapsed);
        }
        // process all the BOBS
        back_buffer = &ratr0_display_get_back_buffer()->surface;

        struct Ratr0Bob *bob;
        for (int i = 0; i < current_scene->num_bobs; i++) {
            bob = current_scene->bobs[i];
            if (update_bob(bob)) {
                // enqueue dirties
                add_restore_tiles_for_bob(bob);
                move_bob(bob);

                // TODO: check/handle collisions
            }
        }

        OwnBlitter();
        // Enable blitter nasty
        custom.dmacon = DMAF_SETCLR | DMAF_BLITHOG;

        process_dirty_rectangles(process_dirty_rect);
        // reset bltsize since it's used to determine the first blit of the chain
        dirty_bltsize = 0;

        // 2. Blit updated objects
        for (int i = 0; i < current_scene->num_bobs; i++) {
            bob = current_scene->bobs[i];
            ratr0_blit_object_il(back_buffer, bob->tilesheet,
                                 0,
                                 bob->base_obj.anim_frames.frames[bob->base_obj.anim_frames.current_frame_idx],
                                 bob->base_obj.bounds.x,
                                 bob->base_obj.bounds.y);
        }
        // Disable blitter nasty
        custom.dmacon = DMAF_BLITHOG;
        DisownBlitter();

        current_frame_sprites = 0;
        _update_sprites();
    }
}
