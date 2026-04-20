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
# luna_timer

``` c
/* main.c */
#include <stdio.h>
#define LUNA_TIMER_IMPLEMENTATION
#include "luna_timer.h"
#undef LUNA_TIMER_IMPLEMENTATION
#include <stdio.h>
#include <unistd.h>

static struct core_timer *timer_head = NULL;

void timer_periodic_cb(void *arg)
{
        printf("[Periodic] Triggered every 1 second\n");
}

void timer_oneshot_cb(void *arg)
{
        printf("[One-shot] Triggered once after 2.5 seconds\n");
}

int main(void)
{
        struct auto_timer timer1;
        struct auto_timer timer2;

        printf("Autotimer example started\n");

        luna_timer_init(&timer1,
			&timer_head,
			1000,
			TIMER_PERIODIC,
			timer_periodic_cb,
			NULL);

        luna_timer_init(&timer2,
			&timer_head,
			2500,
			TIMER_ONE_SHOT,
			timer_oneshot_cb,
			NULL);

        luna_timer_start(&timer1);
        luna_timer_start(&timer2);

        while (1)
        {
                luna_timer_run(&timer_head);
                usleep(1000);
        }

        return 0;
}

```
