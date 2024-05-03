/** @file input.c */
#include <ratr0/debug_utils.h>
#include <ratr0/input.h>

#ifdef AMIGA
#include <ratr0/amiga/input.h>
#endif /* AMIGA */

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[31;1mINPUT\033[0m", __VA_ARGS__)

void ratr0_input_shutdown(void);
void ratr0_input_update(void);
static struct Ratr0InputSystem input_system;
static Ratr0Engine *engine;

// The map lookup needs to find
// the action id for an input class and + input id and vice versa, each in O(1).
// An array of action mappings, indexed by action ids. Each entry is a singly
// linked list of InputEvents.
/** \brief maximum entries in action map */
#define RATR0_MAX_ACTIONS (20)
/** \brief default value in input2action, means the input is unmapped */
#define RATR0_INPUT_UNMAPPED (-1)
#define RATR0_MAX_INPUT_DEFS (100)
int next_input_def = 0;

struct Ratr0InputEvent *action2input[RATR0_MAX_ACTIONS];
static int next_input_action;
struct Ratr0InputEvent input_def_pool[RATR0_MAX_INPUT_DEFS];

// The other direction is a tree-like structure: it's a 2-level array
// with the first level being the input class and the second being the input ids.
// The values are action ids
RATR0_ACTION_ID input2action[RATR0_NUM_INPUT_CLASSES][RATR0_NUM_INPUT_IDS];
/** \brief map that indicates the occurence of an action */
BOOL action_occurred[RATR0_MAX_ACTIONS];

/**
 * NOTE: no limit check !!!
 */
RATR0_ACTION_ID ratr0_alloc_action(void) { return next_input_action++; }

void ratr0_map_input_to_action(RATR0_ACTION_ID action_id, UINT16 input_class, UINT16 input_id)
{
    struct Ratr0InputEvent *list_item = action2input[action_id];
    struct Ratr0InputEvent *evt = &input_def_pool[next_input_def++];
    evt->input_class = input_class;
    evt->input_id = input_id;
    evt->action_id = action_id;
    evt->next = NULL;

    // Add the event to the list of action mappings
    if (list_item == NULL) {
        action2input[action_id] = evt;
    } else {
        while (list_item->next != NULL) {
            list_item = list_item->next;
        }
        list_item->next = evt;
    }

    // And the other direction
    input2action[input_class][input_id] = action_id;
}

BOOL ratr0_was_action_pressed(RATR0_ACTION_ID action_id)
{
    return action_occurred[action_id];
}

struct Ratr0InputSystem *ratr0_input_startup(Ratr0Engine *eng)
{
    engine = eng;

    // initialize action map
    next_input_action = 0;
    next_input_def = 0;
    for (int i = 0; i < RATR0_MAX_ACTIONS; i++) action2input[i] = NULL;
    for (int i = 0; i < RATR0_NUM_INPUT_CLASSES; i++) {
        for (int j = 0; j < RATR0_NUM_INPUT_IDS; j++) {
            input2action[i][j] = RATR0_INPUT_UNMAPPED;
        }
    }

    input_system.shutdown = &ratr0_input_shutdown;
    input_system.update = &ratr0_input_update;
    input_system.alloc_action = &ratr0_alloc_action;
    input_system.map_input_to_action = &ratr0_map_input_to_action;
    input_system.was_action_pressed = &ratr0_was_action_pressed;
    ratr0_amiga_input_startup();
    PRINT_DEBUG("Startup finished.");
    return &input_system;
}

void ratr0_input_shutdown(void)
{
    ratr0_amiga_input_shutdown();
    PRINT_DEBUG("Shutdown finished.");
}

void clear_action_occurrences(void)
{
    for (int i = 0; i < next_input_action; i++) action_occurred[i] = FALSE;
}

#define REGISTER_ACTION(class, id) { \
    RATR0_ACTION_ID action_id = input2action[class][id]; \
    if (action_id != RATR0_INPUT_UNMAPPED) { \
      action_occurred[action_id] = TRUE; \
    } \
  }

/**
 * Polls the Amiga joystick with the specified number (0 or 1).
 * @param device_num device number, 0 is mouse port, 1 is joystick port
 */
void poll_amiga_joystick(UINT8 device_num)
{
    UINT16 joystate = ratr0_amiga_get_joystick_state(device_num);
    UINT8 inp_class = device_num == 0 ? RATR0_IC_JS0 : RATR0_IC_JS1;
    if (joystate != 0) {
        if ((joystate & JOY_FIRE0) == JOY_FIRE0) {
            REGISTER_ACTION(inp_class, RATR0_INPUT_JS_BUTTON0);
        }
        if ((joystate & JOY_D_LEFT) == JOY_D_LEFT) {
            REGISTER_ACTION(inp_class, RATR0_INPUT_JS_LEFT);
        } else if ((joystate & JOY_D_RIGHT) == JOY_D_RIGHT) {
            REGISTER_ACTION(inp_class, RATR0_INPUT_JS_RIGHT);
        }
        if ((joystate & JOY_D_UP) == JOY_D_UP) {
            REGISTER_ACTION(inp_class, RATR0_INPUT_JS_UP);
        } else if ((joystate & JOY_D_DOWN) == JOY_D_DOWN) {
            REGISTER_ACTION(inp_class, RATR0_INPUT_JS_DOWN);
        }
    }
}

/**
 * Poll the keyboard and map keys to engine input actions.
 */
void poll_amiga_keyboard(void)
{
    UINT16 keystate = ratr0_amiga_get_keyboard_state();
    if (keystate == RATR0_KEY_ESCAPE) {
        REGISTER_ACTION(RATR0_IC_KB, RATR0_KEY_ESCAPE);
    }
}

/**
 * Poll the input systems. Called in every frame.
 */
void ratr0_input_update(void)
{
    clear_action_occurrences();
    poll_amiga_joystick(1);
    poll_amiga_keyboard();
}
