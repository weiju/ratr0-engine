#include <ratr0/amiga/world.h>
#include <ratr0/amiga/display.h>
#include <ratr0/amiga/sprites.h>
#include <ratr0/amiga/blitter.h>


// JUST PERFORMANCE TESTING
//#define BOBS_PATH ("test_assets/fox-idle_16x16x3.ts")
//#define BOBS_PATH_IL ("test_assets/fox-idle_16x16x3i.ts")
#define BOBS_PATH ("test_assets/fox_jump_23x21x3_ni.ts")
#define BOBS_PATH_IL ("test_assets/fox_jump_23x21x3.ts")
static struct Ratr0TileSheet bobs, bobs_il;

static Ratr0Engine *engine;

// Sprite and bob tables can be in Fastmem
struct Ratr0AnimatedAmigaSprite hw_sprite_table[20];
UINT16 next_hw_sprite = 0;
struct Ratr0AnimatedAmigaBob bob_table[20];
UINT16 next_bob = 0;

void ratr0_amiga_world_update(void);

void ratr0_amiga_world_startup(Ratr0Engine *eng, struct Ratr0WorldSystem *world_system)
{
    engine = eng;
    world_system->update = &ratr0_amiga_world_update;
    next_hw_sprite = next_bob = 0;
}
struct Ratr0AnimatedAmigaSprite *ratr0_create_amiga_sprite(struct Ratr0TileSheet *tilesheet,
                                                           UINT8 *frame_indexes, UINT8 num_frames)
{
    // Note we can only work within the Amiga hardware sprite limitations
    // 1. Reserve memory from engine
    // 2. Convert into sprite data structure and store into allocated memory
    // 3. Return the initialized object
    UINT16 *sprite_data = ratr0_amiga_make_sprite_data(tilesheet, frame_indexes, num_frames);
    struct Ratr0AnimatedAmigaSprite *result = &hw_sprite_table[next_hw_sprite++];
    result->base_obj.node.class_id = AMIGA_SPRITE;
    result->sprite_data = sprite_data;
    // store sprite information
    return result;
}

struct Ratr0AnimatedAmigaBob *ratr0_create_amiga_bob(struct Ratr0TileSheet *tilesheet,
                                                     UINT8 *frame_indexes, UINT8 num_frames)
{
    struct Ratr0AnimatedAmigaBob *result = &bob_table[next_bob++];
    result->base_obj.node.class_id = AMIGA_BOB;
    result->tilesheet = tilesheet;
    return result;
}

/**
 * This is the main function to simulate the game world. All our game logic happpens here.
 */
UINT8 iteration = 0;
extern struct Ratr0Backdrop *backdrop;

void ratr0_amiga_world_update(void)
{
    struct Ratr0AmigaSurface *disp_surf = ratr0_amiga_get_back_buffer();
    // TODO: add dirty rectangle restore
    //ratr0_amiga_blit_rect(disp_surf, &backdrop->surface, 0, 0, 0, 0, 320, 256);
    //ratr0_amiga_blit_rect(disp_surf, &backdrop->surface, 32, 16, 32, 16, 16, 16);

#ifdef QUEUED_RENDERING
    if (current_scene) {
        ratr0_world_update_node(current_scene);

        for (int i = 0; i < 12; i++) {
            int frame = ((iteration < 10 ? 1 : 0) + i) % 2;
#ifdef INTERLEAVED_BOBS
            ratr0_amiga_enqueue_blit_object_il(disp_surf, &bobs_il, 0, frame, 16 + i * 2, 16 * i);
#else
            ratr0_amiga_enqueue_blit_object(disp_surf, &bobs, 0, frame, 16 + i * 2, 16 * i);
#endif
        }
    } else {
        PRINT_DEBUG("ERROR: *no main scene set !*");
        engine->exit();
    }
#else // non queued rendering
    ratr0_amiga_blit_rect(disp_surf, &backdrop->surface, 16, 0, 16, 0, 16, 16);
    ratr0_amiga_blit_rect(disp_surf, &backdrop->surface, 32, 0, 32, 0, 16, 16);
    ratr0_amiga_blit_rect(disp_surf, &backdrop->surface, 16, 16, 16, 0, 16, 16);
    ratr0_amiga_blit_rect(disp_surf, &backdrop->surface, 32, 16, 32, 0, 16, 16);
    for (int i = 0; i < 14; i++) {
        int frame = ((iteration < 10 ? 1 : 0) + i) % 2;
#ifdef INTERLEAVED_BOBS
        ratr0_amiga_blit_object_il(disp_surf, &bobs_il, 0, frame, 16 + i * 2, 16 * i);
#else
        ratr0_amiga_blit_object(disp_surf, &bobs, 0, frame, 16 + i * 2, 16 * i);
#endif
    }
#endif // QUEUED_RENDERING
    // frame counter
    iteration++;
    if (iteration == 20) iteration = 0;
}
