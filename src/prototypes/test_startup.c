#include <workbench/startup.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Determine the full path of our execution directory.
 * Turns out the lock of the current dir only holds the name of the dir
 * we need to call ParentDir() to find the entire chain upwards until
 * the lock is 0
 */
int ConstructDosPath(BPTR curdir_lock, char *buf, int bufpos, int bufsize)
{
    struct FileInfoBlock fib;
    BPTR parent = ParentDir(curdir_lock);

    if (parent == 0) {
        // root, we are at the start
        Examine(curdir_lock, &fib);
        int slen = strlen(fib.fib_FileName);
        strncpy(buf, fib.fib_FileName, bufsize - bufpos + 1);
        bufpos += slen;
        buf[bufpos++] = ':';
        buf[bufpos] = '\0';
    } else {
        bufpos = ConstructDosPath(parent, buf, bufpos, bufsize);
        Examine(curdir_lock, &fib);
        int slen = strlen(fib.fib_FileName);
        strncpy(&buf[bufpos], fib.fib_FileName, bufsize - bufpos + 1);
        bufpos += slen;
        buf[bufpos++] = '/';
        buf[bufpos] = '\0';
    }
    return bufpos;
}

int main(int argc, char **argv)
{
    FILE *debug_fp;
    char filename[256], fullpath[256];
    int wb_numargs = 0;
    struct MsgPort *proc_msgport;
    struct Process *process;
    BPTR olddir;
    BOOL is_cli = FALSE;
    struct FileInfoBlock fib;
    if (argc == 0) {
        // WB start
        struct WBStartup *argmsg;
        struct WBArg *wb_arg;
        argmsg = (struct WBStartup *) argv;
        // This is the MsgPort member of the Process structure, so we need
        // to convert this into the Process structure. What a pain
        proc_msgport = argmsg->sm_Process;
        process = (struct Process *)
            (((int) proc_msgport) - sizeof(struct Message));
        //process = (struct Process *) argmsg->sm_Process;

        wb_arg = argmsg->sm_ArgList;         /* head of the arg list */
        wb_numargs = argmsg->sm_NumArgs;

        // the first olddir is the program directory lock !!!!
        for (int i = 0; i < argmsg->sm_NumArgs; i++, wb_arg++) {
            olddir = CurrentDir(wb_arg->wa_Lock);
            strncpy(filename, wb_arg->wa_Name, 256);
            CurrentDir(olddir);
        }
    } else {
        strncpy(filename, argv[0], 256);
        // CLI start, means we can call Cli() and access pr_Cli
        struct Task *task = FindTask(NULL);
        process = (struct Process *) task;
        olddir = process->pr_CurrentDir;
        is_cli = TRUE;
    }
    // Make the debug file handle available from the start so
    // we can write to it during startup
#ifdef DEBUG
    debug_fp = fopen("src:ratr0.debug", "a");
    fprintf(debug_fp, "FILENAME: %s\n", filename);
    if (is_cli) {
        fprintf(debug_fp, "CLI START\n");
    } else {
        fprintf(debug_fp, "WORKBENCH START\n");
        fprintf(debug_fp, "WB #args: %d\n", wb_numargs);
    }
    fprintf(debug_fp, "STACK: %d\n", process->pr_StackSize);
    fprintf(debug_fp, "PROC ADDR: %d: MSGPORT ADDR: %d\n",
            process, proc_msgport);
    fprintf(debug_fp, "curdir: %d olddir: %d\n",
            process->pr_CurrentDir, olddir);
    Examine(olddir, &fib);
    fprintf(debug_fp, "curdir name: %s\n", fib.fib_FileName);

    ConstructDosPath(olddir, fullpath, 0, 256);
    fprintf(debug_fp, "full path: %s\n", fullpath);

    FILE *indir_fp = fopen("assets/title_screen.ts", "rb");
    if (!indir_fp) {
        fprintf(debug_fp, "CANT open title screen\n");
    } else {
        fprintf(debug_fp, "CAN open title screen\n");
        fclose(indir_fp);
    }
    fflush(debug_fp);
#endif

    fclose(debug_fp);

    return 0;
}
