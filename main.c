#include <stdio.h>
#include <ratr0/timers.h>

void notify_timeout()
{
    printf("timer_timeout !!!\n");
}

int main(int argc, char **argv)
{
    Ratr0Timer timer;
    ratr0_init_timer(&timer, 5, 0, notify_timeout);
    for (int i = 0; i < 15; i++) ratr0_update_timer(&timer);
    return 0;
}
