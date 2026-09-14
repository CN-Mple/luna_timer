# low-level
``` C
/* luna_timer_platform.c */
#include <windows.h>
uint32_t luna_timer_platform_get_tick(void)
{
    return GetTickCount();
}

static struct core_timer_list list = {0};

struct core_timer_list *app_timer_get_list(void)
{
        return &list;
}

```

``` c

int main(void)
{
	for (;;) {
		struct core_timer_list *list = app_timer_get_list();
		uint32_t next_wait = luna_timer_run(list);
		if (next_wait > 0) {
			Sleep(next_wait);
                }
	}
}

```