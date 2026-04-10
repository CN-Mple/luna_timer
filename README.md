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
``` c
/* main.c */
#include <stdio.h>
#define LUNA_TIMER_IMPLEMENTATION
#include "luna_timer.h"
#undef LUNA_TIMER_IMPLEMENTATION

#define APP_TIMER_IMPLEMENTATION
#include "app_timer.h"
#undef APP_TIMER_IMPLEMENTATION

uint32_t handle = 0;

void _app_timer_callback(void *arg)
{
        int *number = (int *)arg;
        printf("number is %d.\n", *number);
        (*number)++;
        handle = app_timer_register(500, _app_timer_callback, arg);
}

int main()
{
        int number = 0;
        handle = app_timer_register(500, _app_timer_callback, &number);
        while (1) {
                luna_timer_run(app_timer_get_head());
        }
        return 0;
}
```
