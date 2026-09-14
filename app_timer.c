/* app_timer.c */
#include "app_timer.h"
#include <stdlib.h>
#include <string.h>

struct app_timer {
        struct core_timer    core;
        uint32_t             msec :31;
        uint32_t             mode : 1;
        app_timer_callback_t user_callback;
        void                *user_data;
};

static void _callback(struct core_timer *core, void *user_data)
{
        (void)user_data;
        struct app_timer *timer = (struct app_timer *)core;

        if (timer->user_callback) {
                timer->user_callback(timer->user_data);
        }
        if (timer->mode == TIMER_PERIODIC) {
                uint32_t when = timer->core.when + luna_timer_platform_msec_to_tick(timer->msec);
                luna_timer_set_when(&timer->core, when);
                luna_timer_insert(app_timer_get_list(), &timer->core);
        }
}

struct app_timer *app_timer_create(void)
{
        struct app_timer *timer = malloc(sizeof(*timer));
        if (!timer) {
                return NULL;
        }
        memset(timer, 0, sizeof(*timer));
        luna_timer_set_callback(&timer->core, _callback, NULL);
        return timer;
}

void app_timer_delete(struct app_timer *timer)
{
        if (!timer) {
                return;
        }
        app_timer_stop(timer);
        free(timer);
}

void app_timer_init(struct app_timer *mem)
{
        struct app_timer *timer = mem;
        memset(timer, 0, sizeof(*timer));
        luna_timer_set_callback(&timer->core, _callback, NULL);
}

void app_timer_detach(struct app_timer *timer)
{
        if (!timer) {
                return;
        } 
        app_timer_stop(timer);
}

int app_timer_set(struct app_timer *timer, timer_mode_t mode, uint32_t msec, void (*user_callback)(void *user_data), void *user_data)
{
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        if (luna_timer_is_onqueue(&timer->core)) {
                return LUNA_TIMER_EONQUEUE;
        }
        timer->mode          = mode;
        timer->msec          = msec;
        timer->user_callback = user_callback;
        timer->user_data     = user_data;

        return LUNA_TIMER_OK;
}

int app_timer_start(struct app_timer *timer)
{
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        uint32_t now = luna_timer_platform_get_tick();
        luna_timer_set_when(&timer->core, now + luna_timer_platform_msec_to_tick(timer->msec));
        return luna_timer_insert(app_timer_get_list(), &timer->core);
}

int app_timer_stop(struct app_timer *timer)
{
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        luna_timer_remove(app_timer_get_list(), &timer->core);
        return LUNA_TIMER_OK;
}

bool app_timer_is_running(struct app_timer *timer)
{
        if (!timer) {
                return false;
        }
        return luna_timer_is_onqueue(&timer->core);
}
