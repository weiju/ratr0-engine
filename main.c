#include <stdio.h>
#include <ratr0/timers.h>
#include <ratr0/engine.h>
#include <ratr0/memory.h>

void notify_timeout()
{
    printf("timer_timeout !!!\n");
}

int main(int argc, char **argv)
{
    Ratr0Timer timer;
    ratr0_engine_startup();

    ratr0_init_timer(&timer, 5, 0, notify_timeout);
    for (int i = 0; i < 5; i++) ratr0_update_timer(&timer);

    Ratr0MemHandle memblock1 = ratr0_memory_allocate_block(RATR0_MEM_CHIP, 1024);

    ratr0_engine_shutdown();

    return 0;
}
