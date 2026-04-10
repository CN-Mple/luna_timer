#ifndef LUNA_APP_TIMER_H
#define LUNA_APP_TIMER_H

#include "luna_timer.h"

#include <stdint.h>
#include <string.h>

#ifndef LUNA_ASSERT
#include <assert.h>
#define LUNA_ASSERT                     assert
#endif

#ifndef LUNA_MALLOC
#include <stdlib.h>
#define LUNA_MALLOC			malloc
#endif

#ifndef LUNA_FREE
#include <stdlib.h>
#define LUNA_FREE			free
#endif

struct apptimer;

typedef void (*app_timer_callback_t)(void *arg);

struct apptimer {
	struct core_timer timer;

	app_timer_callback_t callback;
	void *arg;

	uint32_t handle;
	bool scheduled;
};

struct core_timer **app_timer_get_head();

uint32_t app_timer_register(uint32_t ms, app_timer_callback_t callback, void *priv);
void app_timer_cancel(uint32_t handle);

#endif

#ifdef APP_TIMER_IMPLEMENTATION

static struct core_timer *timer_head = 0;

struct core_timer **app_timer_get_head()
{
	return &timer_head;
}

static void app_timer_callback(struct core_timer *timer)
{
        struct apptimer *app_timer = (struct apptimer *)timer;
    	app_timer->scheduled = 0;

        if(app_timer->callback){
                app_timer->callback(app_timer->arg);
        }
        LUNA_FREE(app_timer);
}

static uint32_t get_new_handle(void)
{
	static uint32_t handle = 0;
	++handle;
	return handle;
}

static struct apptimer *app_timer_get_by_id(uint32_t handle)
{
	struct core_timer *timer = timer_head;
	while (timer) {
		struct apptimer *apptimer = (struct apptimer *)timer;
		if (apptimer->handle == handle)
			return apptimer;
		timer = timer->next;
	}
	return 0;
}

uint32_t app_timer_register(uint32_t ms, app_timer_callback_t callback, void *arg)
{
	struct apptimer *timer = LUNA_MALLOC(sizeof(struct apptimer));
	LUNA_ASSERT(timer);
	
	memset(timer, 0, sizeof(struct apptimer));
	timer->timer.when     = LUNA_GET_TICK() + ms;
	timer->timer.callback = app_timer_callback;
	timer->callback       = callback;
	timer->arg            = arg;
	timer->scheduled      = true;
	timer->handle         = get_new_handle();

	luna_timer_append(&timer_head, &timer->timer);

	return timer->handle;
}

void app_timer_cancel(uint32_t handle)
{
	struct apptimer *timer = app_timer_get_by_id(handle);
	if (!timer) {
		return;
	}
	if (!(timer->scheduled)) {
		return;
	}
	luna_timer_remove(&timer_head, &timer->timer);
	LUNA_FREE(timer);
}

#endif
