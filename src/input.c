/** @file input.c
 * This is the high level module for RATR0 input.
 */
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>

#include <devices/keyboard.h>
#include <devices/input.h>
#include <devices/console.h>
#include <devices/conunit.h>

#include <exec/execbase.h>
#include <SDI/SDI_compiler.h>

#include <ratr0/data_types.h>
#include <ratr0/input.h>
#include <ratr0/debug_utils.h>


#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[31;1mINPUT\033[0m", __VA_ARGS__)

extern struct ExecBase *SysBase;
#define RAW_KEY_LSHIFT       (0x60)
#define RAW_KEY_ESCAPE       (0x45)
#define RAW_KEY_SPACE        (0x40)
#define RAW_KEY_CURSOR_UP    (0x4c)
#define RAW_KEY_CURSOR_DOWN  (0x4d)
#define RAW_KEY_CURSOR_RIGHT (0x4e)
#define RAW_KEY_CURSOR_LEFT  (0x4f)
#define RAW_KEY_Q            (0x10)
#define RAW_KEY_W            (0x11)
#define RAW_KEY_E            (0x12)
#define RAW_KEY_O            (0x18)
#define RAW_KEY_P            (0x19)
#define RAW_KEY_S            (0x21)
#define RAW_KEY_Z            (0x31)
#define RAW_KEY_X            (0x32)
#define RAW_KEY_C            (0x33)
#define RAW_KEY_M            (0x37)

void ratr0_input_shutdown(void);
static struct Ratr0InputSystem input_system;
static Ratr0Engine *engine;


/*
 * Amiga input system. Generally input should be polled rather than waiting for the
 * Intuition messaging system to have the smallest amount of lag. On the other hand
 * expecting correctly mapped keyboard input is also something that is desirable.
 */
// To handle input
static BYTE error;
static struct MsgPort *kb_mp, *console_mp;
static struct IOStdReq *kb_io, *console_io;

static UBYTE *kb_matrix;
#define MATRIX_SIZE (16L)

// Joystick / mouse registers
/*
static volatile UINT16 *custom_joy0dat = (volatile UINT16 *) 0xdff00a;
static volatile UINT16 *custom_joy1dat = (volatile UINT16 *) 0xdff00c;
*/
/*
  Example for reading the mouse
    UINT16 mousepos = *custom_joy0dat;
    UINT8 currx = mousepos & 0xff;
    UINT8 curry = (mousepos >> 8) & 0xff;
    UINT8 dirx = currx - lastx;
    UINT8 diry = curry - lasty;

    if (dirx > PLAYER_MAX_SPEED) dirx = PLAYER_MAX_SPEED;
    else if (dirx < -PLAYER_MAX_SPEED) dirx = -PLAYER_MAX_SPEED;
    if (diry > PLAYER_MAX_SPEED) diry = PLAYER_MAX_SPEED;
    else if (diry < -PLAYER_MAX_SPEED) diry = -PLAYER_MAX_SPEED;
    lastx = currx;
    lasty = curry;

 */

static BOOL init_console_device(void)
{
    console_mp = CreatePort(0, 0);
    console_io = (struct IOStdReq *) CreateExtIO(console_mp,
                                                 sizeof(struct IOStdReq));
    error = OpenDevice("console.device", CONU_LIBRARY,
                       (struct IORequest *) console_io, 0);
    return TRUE;
}

static void cleanup_console_device(void)
{
    if (console_io) {
        CloseDevice((struct IORequest *) console_io);
        DeleteExtIO((struct IORequest *) console_io);
    }
    if (console_mp) DeletePort(console_mp);
}

static BOOL init_keyboard_device(void)
{
    kb_mp = CreatePort(0, 0);
    kb_io = (struct IOStdReq *) CreateExtIO(kb_mp, sizeof(struct IOStdReq));
    error = OpenDevice("keyboard.device", 0L, (struct IORequest *) kb_io, 0);
    kb_matrix = AllocMem(MATRIX_SIZE, MEMF_PUBLIC|MEMF_CLEAR);
    return TRUE;
}

static void cleanup_keyboard_device(void)
{
    if (kb_matrix) FreeMem(kb_matrix, MATRIX_SIZE);
    if (kb_io) {
        CloseDevice((struct IORequest *) kb_io);
        DeleteExtIO((struct IORequest *) kb_io);
    }
    if (kb_mp) DeletePort(kb_mp);
}

static void read_keyboard(void)
{
    kb_io->io_Command = KBD_READMATRIX;
    kb_io->io_Data = (APTR) kb_matrix;
    kb_io->io_Length = SysBase->LibNode.lib_Version >= 36 ? MATRIX_SIZE : 13;
    DoIO((struct IORequest *) kb_io);
}

static int is_keydown(BYTE keycode) {
    return kb_matrix[keycode / 8] & (1 << (keycode % 8));
}

/**
 * This might look as if we are stuck, but it is actually
 * the CLI that has echoed the characters and is waiting for
 * input. While we are debugging in the CLI, just hit the backspace key
 * a few times to remove the echoed characters to proceed
 */
UINT16 ratr0_get_keyboard_state(void)
{
    read_keyboard();
    /**
     * TODO:
     * Another inefficiency: This mapping is from
     * Amiga RAW keys to RATR0 logical keys. We should
     * actually just either have a translation table or
     * directly use the raw key
     */
    if (is_keydown(RAW_KEY_ESCAPE)) {
        return RATR0_KEY_ESCAPE;
    } else if (is_keydown(RAW_KEY_CURSOR_LEFT)) {
        return RATR0_KEY_LEFT;
    } else if (is_keydown(RAW_KEY_CURSOR_RIGHT)) {
        return RATR0_KEY_RIGHT;
    } else if (is_keydown(RAW_KEY_CURSOR_UP)) {
        return RATR0_KEY_UP;
    } else if (is_keydown(RAW_KEY_CURSOR_DOWN)) {
        return RATR0_KEY_DOWN;
    } else if (is_keydown(RAW_KEY_SPACE)) {
        return RATR0_KEY_SPACE;
    } else if (is_keydown(RAW_KEY_Z)) {
        return RATR0_KEY_RAW_Z;
    } else if (is_keydown(RAW_KEY_M)) {
        return RATR0_KEY_RAW_M;
    }
    return RATR0_KEY_NONE;
}

/**
 * Joystick system
 */
static volatile UINT8 *ciaa_pra = (volatile UINT8 *) 0xbfe001;
#define  PRA_FIR0_BIT (1 << 6)
#define  PRA_FIR1_BIT (1 << 7)
#define JOY0FIR0_PRESSED (!(*ciaa_pra & PRA_FIR0_BIT))
#define JOY1FIR0_PRESSED (!(*ciaa_pra & PRA_FIR1_BIT))

// in amiga.lib, don't use !!! I don't think they are volatile
//extern UINT16 *joy0dat, *joy1dat;

// Make sure we are reading from a volatile place
/*
static volatile UINT16 *custom_joy0dat = (volatile UINT16 *) 0xdff00a;
static volatile UINT16 *custom_joy1dat = (volatile UINT16 *) 0xdff00c;
UINT16 ratr0_get_joystick_state(UINT8 device_num)
{
    UINT16 result = 0;
    UINT16 tmp = (device_num == 0) ? *custom_joy0dat : *custom_joy1dat;
    BOOL fire_button = device_num == 0 ? JOY0FIR0_PRESSED : JOY1FIR0_PRESSED;

    // This code is not really efficient, but it works, optimize if
    // it becomes an issue
    UINT16 bit0  =  tmp & 0x01;
    UINT16 right = (tmp >> 1) & 0x01;
    UINT16 bit8  = (tmp >> 8) & 0x01;
    UINT16 left  = (tmp >> 9) & 0x01;
    UINT16 down  = right ^ bit0;
    UINT16 up = left ^ bit8;

    // convert to an easy interpretable bit mask, we could skip this
    // translation
    if (left) result |= JOY_D_LEFT;
    if (right) result |= JOY_D_RIGHT;
    if (up) result |= JOY_D_UP;
    if (down) result |= JOY_D_DOWN;
    if (fire_button) result |= JOY_FIRE0;
    return result;
}
*/

/** Mapping system */
// The map lookup needs to find
// the action id for an input class and + input id and vice versa, each in O(1).
// An array of action mappings, indexed by action ids. Each entry is a singly
// linked list of InputEvents.
/** \brief maximum entries in action map */
#define RATR0_MAX_ACTIONS (30)
/** \brief default value in input2action, means the input is unmapped */
#define RATR0_INPUT_UNMAPPED (-1)
#define RATR0_MAX_INPUT_DEFS (100)
int next_input_def = 0;

struct Ratr0InputEvent *action2input[RATR0_MAX_ACTIONS];
static UINT8 next_input_action;
struct Ratr0InputEvent input_def_pool[RATR0_MAX_INPUT_DEFS];

// The other direction is a tree-like structure: it's a 2-level array
// with the first level being the input class and the second being the input ids.
// The values are action ids
RATR0_ACTION_ID input2action[RATR0_NUM_INPUT_CLASSES][RATR0_NUM_INPUT_IDS];
/** \brief map that indicates the occurence of an action */
BOOL action_occurred[RATR0_MAX_ACTIONS];

// a list of registered RATR0 keys to make sure we only check
// the keys we need
#define NUM_KEYS (32)
static UINT16 registered_keys[NUM_KEYS];
static UINT16 num_registered_keys = 0;

/**
 * NOTE: no limit check !!!
 */
RATR0_ACTION_ID ratr0_input_alloc_action(void) { return next_input_action++; }

void ratr0_input_map_input_to_action(RATR0_ACTION_ID action_id,
                                     UINT16 input_class, UINT16 input_id)
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

    // if this is a keyboard key, add this to the list of
    // allowed keys
    registered_keys[num_registered_keys++] = input_id;
}

BOOL ratr0_input_was_action_pressed(RATR0_ACTION_ID action_id)
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
    init_keyboard_device();

    PRINT_DEBUG("Startup finished.");
    return &input_system;
}

void ratr0_input_shutdown(void)
{
    cleanup_keyboard_device();
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
static volatile UINT16 *custom_joy0dat = (volatile UINT16 *) 0xdff00a;
static volatile UINT16 *custom_joy1dat = (volatile UINT16 *) 0xdff00c;
void poll_joystick(UINT8 device_num)
{
    UINT16 tmp = (device_num == 0) ? *custom_joy0dat : *custom_joy1dat;
    BOOL fire_button = device_num == 0 ? JOY0FIR0_PRESSED : JOY1FIR0_PRESSED;

    // This code is not super efficient, but it works, optimize if
    // it becomes an issue
    UINT16 bit0  =  tmp & 0x01;
    UINT16 right = (tmp >> 1) & 0x01;
    UINT16 bit8  = (tmp >> 8) & 0x01;
    UINT16 left  = (tmp >> 9) & 0x01;
    UINT16 down  = right ^ bit0;
    UINT16 up = left ^ bit8;

    UINT8 inp_class = device_num == 0 ? RATR0_IC_JS0 : RATR0_IC_JS1;
    if (fire_button) {
        REGISTER_ACTION(inp_class, RATR0_INPUT_JS_BUTTON0);
    }
    if (left) {
        REGISTER_ACTION(inp_class, RATR0_INPUT_JS_LEFT);
    } else if (right) {
        REGISTER_ACTION(inp_class, RATR0_INPUT_JS_RIGHT);
    }
    if (up) {
        REGISTER_ACTION(inp_class, RATR0_INPUT_JS_UP);
    } else if (down) {
        REGISTER_ACTION(inp_class, RATR0_INPUT_JS_DOWN);
    }
}

/**
 * Poll the keyboard and map keys to engine input actions.
 * Only look for the keys that were actually registered
 * TODO:
 *   * we can't handle multiple simultaneous keypresses
 *   * We need a "catchall" for any key
 */
void poll_keyboard(void)
{
    UINT16 keystate = ratr0_get_keyboard_state();
    for (int i = 0; i < num_registered_keys; i++) {
        if (keystate == registered_keys[i]) {
            REGISTER_ACTION(RATR0_IC_KB, registered_keys[i]);
            break;
        }
    }
}

/**
 * Poll the input systems. Called in every frame.
 */
void ratr0_input_update(void)
{
    clear_action_occurrences();
    poll_joystick(1);
    poll_keyboard();
}
