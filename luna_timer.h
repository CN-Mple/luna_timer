/* luna_timer.h */
#ifndef LUNA_TIMER_H
#define LUNA_TIMER_H

#include <stdint.h>
#include <stdbool.h>

#include "luna_timer_hal.h"

struct core_timer;

typedef void (*core_timer_callback_t)(struct core_timer *timer);

struct core_timer {
        struct core_timer    *next;
        uint32_t              when;            //tick less than half of type.
        core_timer_callback_t callback;
};

void luna_timer_append(struct core_timer **head, struct core_timer *timer);
void luna_timer_remove(struct core_timer **head, struct core_timer *timer);

uint32_t luna_timer_get_next_expiry(struct core_timer **head);
uint32_t luna_timer_run(struct core_timer **head);

typedef void(*auto_timer_callback_t)(void *arg);

typedef enum {
	TIMER_ONE_SHOT = 0,
	TIMER_PERIODIC = 1,
} auto_timer_mode_t;

struct auto_timer {
	struct core_timer     super;

	struct core_timer   **header;
	uint32_t              running;
	uint32_t        interval;
	auto_timer_mode_t     mode;

	auto_timer_callback_t callback;
	void                 *arg;
};

void luna_timer_init(struct auto_timer *timer, struct core_timer **header, uint32_t interval, auto_timer_mode_t mode, auto_timer_callback_t callback, void *user_data);
void luna_timer_start(struct auto_timer *timer);
void luna_timer_stop(struct auto_timer *timer);
void luna_timer_restart(struct auto_timer *timer);
void luna_timer_set_interval(struct auto_timer *timer, uint32_t interval);

#endif
