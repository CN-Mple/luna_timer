/* app_timer.h */
#ifndef APP_TIMER_H
#define APP_TIMER_H

#include "luna_timer.h"

typedef void(*app_timer_callback_t)(void *data);

typedef enum {
        TIMER_ONE_SHOT = 0,
        TIMER_PERIODIC = 1,
} timer_mode_t;

struct app_timer {
        struct core_timer    core;
        uint32_t             msec :31;
        uint32_t             mode : 1;
        app_timer_callback_t user_callback;
        void                *user_data;
};

struct app_timer_ops {
    void* (*app_mem_malloc)(size_t size);
    void  (*app_mem_free)(void *p);
    struct core_timer_list* (*app_get_list)(void);
};

int app_timer_init(struct app_timer_ops *ops);

struct app_timer *app_timer_create(timer_mode_t mode, uint32_t msec, void (*user_callback)(void *user_data), void *user_data);
void app_timer_delete(struct app_timer *timer);

int app_timer_start(struct app_timer *timer);
int app_timer_stop(struct app_timer *timer);
int app_timer_restart(struct app_timer *timer);

bool app_timer_is_running(struct app_timer *timer);

#endif
