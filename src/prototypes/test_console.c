#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <devices/console.h>
#include <devices/conunit.h>
#include <exec/execbase.h>
#include <SDI/SDI_compiler.h>

#include <stdio.h>

static BYTE error;
static struct MsgPort *console_mp;
static struct IOStdReq *console_io;

int main(int argc, char *argv[])
{
    console_mp = CreatePort(0, 0);
    console_io = (struct IOStdReq *) CreateExtIO(console_mp,
                                                 sizeof(struct IOStdReq));
    error = OpenDevice("console.device", CONU_LIBRARY,
                       (struct IORequest *) console_io, 0);
    if (!error) {
        printf("yead WE CAN OPEN CONU_LIBRARY !!!!\n");
    } else {
        printf("Could not open device with CONU_LIBRARY !!!!\n");
        error = OpenDevice("console.device", CONU_STANDARD,
                           (struct IORequest *) console_io, 0);
        if (!error) {
            printf("YES WE CAN OPEN CONU_STANDARD !!!!\n");
        } else {
            printf("WE CAN'T CONU_STANDARD !!!!\n");
        }
    }

    if (console_io) {
        CloseDevice((struct IORequest *) console_io);
        DeleteExtIO((struct IORequest *) console_io);
    }
    if (console_mp) DeletePort(console_mp);

    return 0;
}
