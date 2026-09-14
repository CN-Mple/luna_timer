/* app_timer.h */
#ifndef APP_TIMER_H
#define APP_TIMER_H

#include "luna_timer.h"

typedef void(*app_timer_callback_t)(void *data);

typedef enum {
        TIMER_ONE_SHOT = 0,
        TIMER_PERIODIC = 1,
} timer_mode_t;

struct core_timer_list *app_timer_get_list(void);

struct app_timer *app_timer_create(void);
void app_timer_delete(struct app_timer *timer);

void app_timer_init(struct app_timer *mem);
void app_timer_detach(struct app_timer *timer);

int app_timer_set(struct app_timer *timer, timer_mode_t mode, uint32_t interval_tick, void (*user_callback)(void *user_data), void *user_data);

int app_timer_start(struct app_timer *timer);
int app_timer_stop(struct app_timer *timer);

bool app_timer_is_running(struct app_timer *timer);

#endif
