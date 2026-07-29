/* luna_timer.h */
#ifndef LUNA_TIMER_H
#define LUNA_TIMER_H

#include <stdint.h>

struct core_timer;

typedef void (*core_timer_callback_t)(struct core_timer *timer);

#ifndef LUNA_TICK_TYPE
#define LUNA_TICK_TYPE                  uint32_t
#endif

#ifndef LUNA_GET_TICK
#include <time.h>
static inline LUNA_TICK_TYPE luna_get_tick(void)
{
        return (LUNA_TICK_TYPE)(clock() * 1000 / CLOCKS_PER_SEC);
}
#define LUNA_GET_TICK                   luna_get_tick
#endif

#ifndef LUNA_ASSERT
#include <assert.h>
#define LUNA_ASSERT                     assert
#endif

#ifndef LUNA_EXPIRED
#define luna_expired(type, diff)        ((diff) > (((type)-1) >> 1))
#define LUNA_EXPIRED                    luna_expired
#endif

#ifndef LUNA_LESS_THAN
#define LUNA_LESS_THAN(type, a, b)      LUNA_EXPIRED(type, (a) - (b))
#endif

struct core_timer {
        struct core_timer *next;
        LUNA_TICK_TYPE when;            //tick less than half of type.
        core_timer_callback_t callback;
};

void luna_timer_append(struct core_timer **head, struct core_timer *timer);
void luna_timer_remove(struct core_timer **head, struct core_timer *timer);

LUNA_TICK_TYPE luna_timer_get_next_expiry(struct core_timer **head);
LUNA_TICK_TYPE luna_timer_run(struct core_timer **head);

typedef void(*auto_timer_callback_t)(void *arg);

typedef enum {
	TIMER_ONE_SHOT = 0,
	TIMER_PERIODIC = 1,
} auto_timer_mode_t;

struct auto_timer {
	struct core_timer     super;

	struct core_timer   **header;
	uint32_t              running;
	LUNA_TICK_TYPE        interval;
	auto_timer_mode_t     mode;

	auto_timer_callback_t callback;
	void                 *arg;
};

void luna_timer_init(struct auto_timer *timer, struct core_timer **header, LUNA_TICK_TYPE interval, auto_timer_mode_t mode, auto_timer_callback_t callback, void *user_data);
void luna_timer_start(struct auto_timer *timer);
void luna_timer_stop(struct auto_timer *timer);
void luna_timer_restart(struct auto_timer *timer);
void luna_timer_set_interval(struct auto_timer *timer, LUNA_TICK_TYPE interval);

#endif
