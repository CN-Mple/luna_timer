# luna_timer

``` c
/* main.c */
#include <stdio.h>
#define LUNA_TIMER_IMPLEMENTATION
#include "luna_timer.h"

struct core_timer *head = 0;

void timer_callback(struct core_timer *timer)
{
        printf("hello world.\n");
        
        timer->when = LUNA_GET_TICK() + 1000;
        luna_timer_append(&head, timer);
}

int main()
{
        struct core_timer timer = {0};
        timer.when              = LUNA_GET_TICK() + 1000;
        timer.callback          = timer_callback;
        luna_timer_append(&head, &timer);

        while (1) {
                luna_timer_run(&head);
        }
        return 0;
}
```
