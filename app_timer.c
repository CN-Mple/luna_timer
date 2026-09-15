/* app_timer.c */
#include "app_timer.h"
#include <stdlib.h>
#include <string.h>

static struct app_timer_ops _app_timer_ops;

int app_timer_init(struct app_timer_ops *ops)
{
        if (!ops || !ops->app_mem_malloc || !ops->app_mem_free || !ops->app_get_core_timer_list) {
                return LUNA_TIMER_EINVAL;
        }
        _app_timer_ops = *ops;
        return 0;
}

static struct core_timer_list *app_get_core_timer_list(void)
{
        if (!_app_timer_ops.app_get_core_timer_list) {
                return NULL;
        }
        return _app_timer_ops.app_get_core_timer_list();
}

static void *app_mem_malloc(size_t size)
{
        if (!_app_timer_ops.app_mem_malloc) {
                return NULL;
        }
        return _app_timer_ops.app_mem_malloc(size);
}

static void app_mem_free(void *p)
{
        if (!_app_timer_ops.app_mem_malloc) {
                return;
        }
        _app_timer_ops.app_mem_free(p);
}

static void _callback(struct core_timer *core, void *user_data)
{
        (void)user_data;
        struct app_timer *timer = (struct app_timer *)core;

        if (timer->mode == TIMER_PERIODIC) {
                uint32_t now = luna_timer_get_tick();
                uint32_t when = timer->core.when + luna_timer_msec_to_tick(timer->msec);
                if (luna_timer_less_than(when, now)) {
                        when = now + luna_timer_msec_to_tick(timer->msec);
                }
                luna_timer_set_when(&timer->core, when);
                luna_timer_insert(app_get_core_timer_list(), &timer->core);
        }
        app_timer_callback_t callback = timer->user_callback;
        void *data = timer->user_data;
        if (callback) {
                callback(data);
        }
}

struct app_timer *app_timer_create(timer_mode_t mode, uint32_t msec, void (*user_callback)(void *user_data), void *user_data)
{
        struct app_timer *timer = app_mem_malloc(sizeof(*timer));
        if (!timer) {
                return NULL;
        }
        memset(timer, 0, sizeof(*timer));
        luna_timer_set_callback(&timer->core, _callback, NULL);

        timer->mode          = mode;
        timer->msec          = msec;
        timer->user_callback = user_callback;
        timer->user_data     = user_data;
        return timer;
}

void app_timer_delete(struct app_timer *timer)
{
        if (!timer) {
                return;
        }
        app_timer_stop(timer);
        app_mem_free(timer);
}

int app_timer_start(struct app_timer *timer)
{
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        if (luna_timer_is_onqueue(&timer->core)) {
                return LUNA_TIMER_EONQUEUE;
        }
        uint32_t now = luna_timer_get_tick();
        luna_timer_set_when(&timer->core, now + luna_timer_msec_to_tick(timer->msec));
        return luna_timer_insert(app_get_core_timer_list(), &timer->core);
}

int app_timer_stop(struct app_timer *timer)
{
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        struct core_timer *removed = luna_timer_remove(app_get_core_timer_list(), &timer->core);
        return removed ? LUNA_TIMER_OK : LUNA_TIMER_ENOTFOUND;
}

int app_timer_restart(struct app_timer *timer)
{
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        app_timer_stop(timer);
        return app_timer_start(timer);
}

bool app_timer_is_running(struct app_timer *timer)
{
        if (!timer) {
                return false;
        }
        return luna_timer_is_onqueue(&timer->core);
}
