#include <clib/alib_protos.h>
#include <clib/exec_protos.h>

#include <devices/keyboard.h>
#include <devices/input.h>
#include <exec/execbase.h>
#include <SDI/SDI_compiler.h>

extern struct ExecBase *SysBase;

#include <ratr0/debug_utils.h>
#include <ratr0/amiga/input.h>

#define PRINT_DEBUG(...) PRINT_DEBUG_TAG("\033[32mINPUT\033[0m", __VA_ARGS__)


// To handle input
static BYTE error;
static struct MsgPort *kb_mp;
static struct IOStdReq *kb_io;
static UBYTE *kb_matrix;
#define MATRIX_SIZE (16L)

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
}
