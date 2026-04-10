/* luna_timer.h */
#ifndef LUNA_TIMER_H
#define LUNA_TIMER_H

#include <stdbool.h>

struct core_timer;

typedef size_t                          tick_type_t;
typedef void (*luna_timer_callback_t)(struct core_timer *timer);

#ifndef LUNA_GET_TICK
#include <time.h>
#define LUNA_GET_TICK()    (tick_type_t)(clock() * 1000 / CLOCKS_PER_SEC)
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
        tick_type_t when;
        luna_timer_callback_t callback;
};

void luna_timer_append(struct core_timer **head, struct core_timer *timer);
void luna_timer_remove(struct core_timer **head, struct core_timer *timer);

tick_type_t luna_timer_get_next_expiry(struct core_timer **head);
tick_type_t luna_timer_run(struct core_timer **head);

#endif

#ifdef LUNA_TIMER_IMPLEMENTATION

void luna_timer_append(struct core_timer **head, struct core_timer *timer)
{
        LUNA_ASSERT(head);
        LUNA_ASSERT(timer);

        struct core_timer **next = head;
        tick_type_t when = timer->when;

        while (*next && LUNA_LESS_THAN(tick_type_t, (*next)->when, when)) {
                next = &((*next)->next);
        }
        timer->next = *next;
        *next = timer;
}

void luna_timer_remove(struct core_timer **head, struct core_timer *timer)
{
        LUNA_ASSERT(head);
        LUNA_ASSERT(timer);

        struct core_timer **next = head;

        while (*next) {
                if (*next == timer) {
                        *next = timer->next;
                        return;
                }
                next = &(*next)->next;
        }
}

tick_type_t luna_timer_get_next_expiry(struct core_timer **head)
{
        if (!head) {
                return (tick_type_t)-1;
        }
        tick_type_t now  = LUNA_GET_TICK();
        tick_type_t when = (*head)->when;
        if (LUNA_LESS_THAN(tick_type_t, when, now)) {
                return 0;
        }

        return when - now;
}

tick_type_t luna_timer_run(struct core_timer **head)
{
        LUNA_ASSERT(head);
        tick_type_t next_expiry;

        next_expiry = luna_timer_get_next_expiry(head);
        if (0 == next_expiry) {
               struct core_timer *timer = *head;
                *head = timer->next;
                if (timer->callback) {
                        timer->callback(timer);
                }
        }
        return next_expiry;
}

#endif
