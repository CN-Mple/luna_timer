# low-level
``` C
/* luna_timer_platform.c */
#include <windows.h>
uint32_t luna_timer_get_tick(void)
{
	return GetTickCount();
}

uint32_t luna_timer_tick_to_msec(uint32_t tick)
{
	return tick;
}

uint32_t luna_timer_msec_to_tick(uint32_t msec)
{
	return msec;
}

static struct core_timer_list list = {0};

struct core_timer_list *app_timer_get_list(void)
{
        return &list;
}


void luna_timer_platform_assert(const char *msg, int line, const char *file)
{
	printf("%s %d %s\r\n", msg, line, file);
	abort();
}

```

``` c

int main(void)
{
	for (;;) {
		struct core_timer_list *list = app_timer_get_list();
		uint32_t next_wait = luna_timer_get_next_timeou(list);
		if (next_wait == 0) {
			luna_timer_dispatch(list);
		} else {
			if (next_wait == 0xFFFFFFFFU) {
				Sleep(next_wait);
			} else {
				if(next_wait > 50)
					next_wait = 50;
				Sleep(next_wait);
			}
		}
	}
}

```