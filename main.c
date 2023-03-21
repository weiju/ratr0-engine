#include <stdio.h>
#include "timers.h"

void notify_timeout()
{
    printf("timer_timeout !!!\n");
}

int main(int argc, char **argv)
{
    Timer timer;
    init_timer(&timer, 5, 0, notify_timeout);
    for (int i = 0; i < 15; i++) update_timer(&timer);
    return 0;
}
