/* luna_timer.h */
#ifndef LUNA_TIMER_H
#define LUNA_TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "luna_timer_platform.h"

#define LUNA_TIMER_OK            0
#define LUNA_TIMER_EINVAL       -1
#define LUNA_TIMER_EONQUEUE     -2
#define LUNA_TIMER_ENOTFOUND    -3

struct core_timer;
struct core_timer_list;

typedef void (*core_timer_callback_t)(struct core_timer *timer, void *data);

struct core_timer {
        struct core_timer      *next;
        uint32_t                when;            //tick less than half of type.
        core_timer_callback_t   callback;
	void                   *data;
	bool                    onqueue;
};

struct core_timer_list {
	struct core_timer    *head;
};

bool luna_timer_is_onqueue(struct core_timer *timer);

int luna_timer_set_callback(struct core_timer *timer, core_timer_callback_t callback, void *data);
int luna_timer_set_when(struct core_timer *timer, uint32_t when);

int luna_timer_insert(struct core_timer_list *list, struct core_timer *timer);
struct core_timer *luna_timer_remove(struct core_timer_list *list, struct core_timer *timer);

uint32_t luna_timer_next_timeout(struct core_timer_list *list);
uint32_t luna_timer_runloop(struct core_timer_list *list);

#endif
