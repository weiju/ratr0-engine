#include <stdio.h>
#include <ratr0/timers.h>
#include <ratr0/engine.h>

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
    ratr0_engine_shutdown();

    return 0;
}
