/* luna_timer.c */
#include "luna_timer.h"

void luna_timer_append(struct core_timer **head, struct core_timer *timer)
{
        LUNA_ASSERT(head);
        LUNA_ASSERT(timer);

        struct core_timer **next = head;
        LUNA_TICK_TYPE when = timer->when;

        while (*next && LUNA_LESS_THAN(LUNA_TICK_TYPE, (*next)->when, when)) {
                next = &((*next)->next);
        }
        timer->next = *next;
        *next = timer;
}

void luna_timer_remove(struct core_timer **head, struct core_timer *timer)
{
        LUNA_ASSERT(head);
        LUNA_ASSERT(timer);
        if(!(*head)) {
                return;
        }
        struct core_timer **next = head;
        while (*next) {
                if (*next == timer) {
                        *next = timer->next;
                        return;
                }
                next = &(*next)->next;
        }
}

LUNA_TICK_TYPE luna_timer_get_next_expiry(struct core_timer **head)
{
        LUNA_ASSERT(head);
        if (!(*head)) {
                return (LUNA_TICK_TYPE)-1;
        }
        LUNA_TICK_TYPE now  = LUNA_GET_TICK();
        LUNA_TICK_TYPE when = (*head)->when;
        if (LUNA_LESS_THAN(LUNA_TICK_TYPE, when, now)) {
                return 0;
        }

        return when - now;
}

LUNA_TICK_TYPE luna_timer_run(struct core_timer **head)
{
        LUNA_ASSERT(head);
        if(!(*head)) {
                return (LUNA_TICK_TYPE)-1;
        }
        LUNA_TICK_TYPE next_expiry;
        next_expiry = luna_timer_get_next_expiry(head);
        if (0 == next_expiry) {
               struct core_timer *timer = *head;
                *head                   = timer->next;
                timer->next             = 0;
                if (timer->callback) {
                        timer->callback(timer);
                }
        }
        return next_expiry;
}

static void _core_timer_callback(struct core_timer *super)
{
	struct auto_timer *timer = (struct auto_timer *)super;
	timer->running = 0;

	if (timer->mode == TIMER_ONE_SHOT) {
                
	} else {
		timer->super.when = timer->super.when + timer->interval;
		luna_timer_append(timer->header, &timer->super);
		timer->running    = 1;
	}
	if (timer->callback) {
		timer->callback(timer->arg);
	}
}

void luna_timer_init(struct auto_timer *timer, struct core_timer **header, LUNA_TICK_TYPE interval, auto_timer_mode_t mode, auto_timer_callback_t callback, void *arg)
{
	LUNA_ASSERT(timer);
	LUNA_ASSERT(interval <= (((LUNA_TICK_TYPE)-1) >> 1));

	timer->super.next     = NULL;
	timer->super.callback = _core_timer_callback;
	timer->header         = header;
	timer->interval       = interval;
	timer->mode           = mode;
	timer->running        = 0;
	timer->callback       = callback;
	timer->arg            = arg;
}

void luna_timer_start(struct auto_timer *timer)
{
	LUNA_ASSERT(timer);

	if (timer->running) {
		return;
	}
	timer->super.when = LUNA_GET_TICK() + timer->interval;
	luna_timer_append(timer->header, &timer->super);
	timer->running    = 1;
}

void luna_timer_stop(struct auto_timer *timer)
{
	LUNA_ASSERT(timer);

	if (!timer->running) {
		return;
	}
	luna_timer_remove(timer->header, &timer->super);
	timer->running = 0;
}

void luna_timer_restart(struct auto_timer *timer)
{
	LUNA_ASSERT(timer);

	luna_timer_stop(timer);
	luna_timer_start(timer);
}

void luna_timer_set_interval(struct auto_timer *timer, LUNA_TICK_TYPE interval)
{
	LUNA_ASSERT(timer);
        LUNA_ASSERT(interval > 0);
	LUNA_ASSERT(interval <= (((LUNA_TICK_TYPE)-1) >> 1));
        if (timer->running) {
            luna_timer_stop(timer);
        }
	timer->interval = interval;
	luna_timer_start(timer);
}
