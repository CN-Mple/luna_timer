/* app_timer.c */
#include "app_timer.h"
#include <string.h>

static struct app_timer_ops _app_timer_ops;

int app_timer_init(struct app_timer_ops *ops)
{
        LUNA_TIMER_ASSERT("ops is NULL", ops != NULL);
        LUNA_TIMER_ASSERT("ops->app_mem_malloc is NULL", ops->app_mem_malloc != NULL);
        LUNA_TIMER_ASSERT("ops->app_mem_free is NULL", ops->app_mem_free != NULL);
        LUNA_TIMER_ASSERT("ops->app_get_core_timer_list is NULL", ops->app_get_core_timer_list != NULL);
        if (!ops || !ops->app_mem_malloc || !ops->app_mem_free || !ops->app_get_core_timer_list) {
                return LUNA_TIMER_EINVAL;
        }
        _app_timer_ops = *ops;
        return 0;
}

static struct core_timer_list *app_get_core_timer_list(void)
{
        LUNA_TIMER_ASSERT("app_get_core_timer_list ops is null", _app_timer_ops.app_get_core_timer_list != NULL);
        if (!_app_timer_ops.app_get_core_timer_list) {
                return NULL;
        }
        return _app_timer_ops.app_get_core_timer_list();
}

static void *app_mem_malloc(size_t size)
{
        LUNA_TIMER_ASSERT("app_mem_malloc ops is null", _app_timer_ops.app_mem_malloc != NULL);
        if (!_app_timer_ops.app_mem_malloc) {
                return NULL;
        }
        return _app_timer_ops.app_mem_malloc(size);
}

static void app_mem_free(void *p)
{
        LUNA_TIMER_ASSERT("app_mem_free ops is null", _app_timer_ops.app_mem_free != NULL);
        if (!_app_timer_ops.app_mem_free) {
                return;
        }
        _app_timer_ops.app_mem_free(p);
}

static void _callback(struct core_timer *core, void *user_data)
{
        (void)user_data;
        LUNA_TIMER_ASSERT("timer is NULL", core != NULL);
        struct app_timer *timer = (struct app_timer *)core;
        LUNA_TIMER_ASSERT("core timer should not onqueue in callback", !luna_timer_is_onqueue(core));

        struct core_timer_list *list = app_get_core_timer_list();
        LUNA_TIMER_ASSERT("core timer list NULL in callback", list != NULL);

        if (timer->mode == TIMER_PERIODIC) {
                uint32_t now = luna_timer_get_tick();
                uint32_t when = timer->core.when + luna_timer_msec_to_tick(timer->msec);
                
                if (luna_timer_less_than(when, now)) {
                        LUNA_TIMER_ERROR("next time is out need offset now\r\n", 0, (void)0);
                        while (luna_timer_less_than(when, now)) {
                                when += luna_timer_msec_to_tick(timer->msec);
                        }
                }
                int ret;
                ret = luna_timer_set_when(&timer->core, when);
                LUNA_TIMER_ASSERT("luna_timer_set_when fail in periodic callback", ret == LUNA_TIMER_OK);
                ret = luna_timer_insert(list, &timer->core);
                LUNA_TIMER_ASSERT("luna_timer_insert fail in periodic callback", ret == LUNA_TIMER_OK);
        }
        app_timer_callback_t callback = timer->user_callback;
        void *data = timer->user_data;
        if (callback) {
                callback(data);
        }
}

struct app_timer *app_timer_create(timer_mode_t mode, uint32_t msec, void (*user_callback)(void *user_data), void *user_data)
{
        LUNA_TIMER_ASSERT("msec set 0!!!", msec != 0);
        if (msec == 0) {
                return NULL;
        }
        struct app_timer *timer = app_mem_malloc(sizeof(struct app_timer));
        if (!timer) {
                return NULL;
        }
        memset(timer, 0, sizeof(struct app_timer));
        int ret = luna_timer_set_callback(&timer->core, _callback, NULL);
        LUNA_TIMER_ASSERT("set callback failed in create", ret == LUNA_TIMER_OK);

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

int app_timer_static(struct app_timer *timer, timer_mode_t mode, uint32_t msec, void (*user_callback)(void *user_data), void *user_data)
{
        LUNA_TIMER_ASSERT("msec set 0!!!", msec != 0);
        if (msec == 0) {
                return LUNA_TIMER_EINVAL;
        }
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        memset(timer, 0, sizeof(struct app_timer));
        int ret = luna_timer_set_callback(&timer->core, _callback, NULL);
        LUNA_TIMER_ASSERT("set callback failed in create", ret == LUNA_TIMER_OK);
        if (ret != LUNA_TIMER_OK) {
                return ret;
        }
        timer->mode          = mode;
        timer->msec          = msec;
        timer->user_callback = user_callback;
        timer->user_data     = user_data;

        return LUNA_TIMER_OK;
}

int app_timer_start(struct app_timer *timer)
{
        LUNA_TIMER_ASSERT("timer is NULL", timer != NULL);
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        if (luna_timer_is_onqueue(&timer->core)) {
                return LUNA_TIMER_EONQUEUE;
        }
        
        struct core_timer_list *list = app_get_core_timer_list();
        LUNA_TIMER_ASSERT("core timer list NULL in callback", list != NULL);

        uint32_t now = luna_timer_get_tick();
        luna_timer_set_when(&timer->core, now + luna_timer_msec_to_tick(timer->msec));
        return luna_timer_insert(list, &timer->core);
}

int app_timer_stop(struct app_timer *timer)
{
        LUNA_TIMER_ASSERT("timer is NULL", timer != NULL);
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        struct core_timer *removed = luna_timer_remove(app_get_core_timer_list(), &timer->core);
        return removed ? LUNA_TIMER_OK : LUNA_TIMER_ENOTFOUND;
}

int app_timer_restart(struct app_timer *timer)
{
        LUNA_TIMER_ASSERT("timer is NULL", timer != NULL);
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        app_timer_stop(timer);
        return app_timer_start(timer);
}

bool app_timer_is_running(struct app_timer *timer)
{
        LUNA_TIMER_ASSERT("timer is NULL", timer != NULL);
        if (!timer) {
                return false;
        }
        return luna_timer_is_onqueue(&timer->core);
}
