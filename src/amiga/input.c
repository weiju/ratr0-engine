#include <clib/alib_protos.h>
#include <clib/exec_protos.h>

#include <devices/keyboard.h>
#include <devices/input.h>
#include <exec/execbase.h>
#include <SDI/SDI_compiler.h>

#include <ratr0/data_types.h>
#include <ratr0/input.h>

extern struct ExecBase *SysBase;
#define RAW_KEY_LSHIFT       (0x60)
#define RAW_KEY_ESCAPE       (0x45)
#define RAW_KEY_SPACE        (0x40)
#define RAW_KEY_CURSOR_UP    (0x4c)
#define RAW_KEY_CURSOR_DOWN  (0x4d)
#define RAW_KEY_CURSOR_RIGHT (0x4e)
#define RAW_KEY_CURSOR_LEFT  (0x4f)
#define RAW_KEY_W            (0x11)
#define RAW_KEY_S            (0x21)

#include <ratr0/debug_utils.h>
#include <ratr0/amiga/input.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mINPUT\033[0m", __VA_ARGS__)

/*
 * Amiga input system. Generally input should be polled rather than waiting for the
 * Intuition messaging system to have the smallest amount of lag. On the other hand
 * expecting correctly mapped keyboard input is also something that is desirable.
 */
// To handle input
static BYTE error;
static struct MsgPort *kb_mp;
static struct IOStdReq *kb_io;
static UBYTE *kb_matrix;
#define MATRIX_SIZE (16L)

// Joystick / mouse registers
static volatile UWORD *custom_joy0dat = (volatile UWORD *) 0xdff00a;
static volatile UWORD *custom_joy1dat = (volatile UWORD *) 0xdff00c;

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

static int init_keyboard_device(void)
{
    kb_mp = CreatePort(0, 0);
    kb_io = (struct IOStdReq *) CreateExtIO(kb_mp, sizeof(struct IOStdReq));
    error = OpenDevice("keyboard.device", 0L, (struct IORequest *) kb_io, 0);
    kb_matrix = AllocMem(MATRIX_SIZE, MEMF_PUBLIC|MEMF_CLEAR);
    return 1;
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

BOOL handle_input(void)
{
    // now check the keyboard status
    read_keyboard();
    if (is_keydown(RAW_KEY_ESCAPE)) return TRUE;
    return FALSE;
}

void ratr0_amiga_input_startup(void)
{
    init_keyboard_device();
}

/**
 * Shut down the input subsystem.
 */
void ratr0_amiga_input_shutdown(void)
{
    cleanup_keyboard_device();
    PRINT_DEBUG("Shutdown finished.");
}

static volatile UINT8 *ciaa_pra = (volatile UINT8 *) 0xbfe001;
#define  PRA_FIR0_BIT (1 << 6)
static BOOL was_joy0fir0_pressed(void) { return (*ciaa_pra & PRA_FIR0_BIT) == 0; }

UINT32 _ratr0_amiga_get_joystick_state(UINT16 device_num)
{
    // currently only the mouse port is supported
    UINT32 result = 0;
    if (was_joy0fir0_pressed()) result |= JOY_FIRE0;
    return result;
}

static UINT32 last_joy0_state;
void ratr0_amiga_input_update(void)
{
    last_joy0_state = _ratr0_amiga_get_joystick_state(0);
}

UINT32 ratr0_amiga_get_joystick_state(UINT16 device_num) { return last_joy0_state; }
