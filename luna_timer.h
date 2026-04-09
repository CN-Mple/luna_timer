/* luna_timer.h */
#ifndef LUNA_TIMER_H
#define LUNA_TIMER_H

#include <stdbool.h>

struct core_timer;

typedef unsigned long   tick_type_t;
typedef void (*luna_timer_callback_t)(struct core_timer *timer);

#ifndef LUNA_GET_TICK
#include <windows.h>
#define LUNA_GET_TICK                   GetTickCount
#endif

#ifndef LUNA_ASSERT
#include <assert.h>
#define LUNA_ASSERT                     assert
#endif

#ifndef LUNA_EXPIRED
#define luna_expired(type, diff)        ((diff) > (((type)-1) >> 1))
#define LUNA_EXPIRED                    luna_expired
#endif

struct core_timer {
    struct core_timer *next;
    tick_type_t when;
    luna_timer_callback_t callback;
};

void luna_timer_append(struct core_timer **head, struct core_timer *timer);
void luna_timer_remove(struct core_timer **head, struct core_timer *timer);

tick_type_t luna_timer_get_next_expiry(const struct core_timer *head);
tick_type_t luna_timer_expired(struct core_timer **head);

#endif

#ifdef LUNA_TIMER_IMPLEMENTATION

static bool less_than(const struct core_timer *a, const struct core_timer *b)
{
        if (a->when <= b->when) {
                return true;
        }
        return false;
}

void luna_timer_append(struct core_timer **head, struct core_timer *timer)
{
        LUNA_ASSERT(head);
        LUNA_ASSERT(timer);

        struct core_timer **next = head;
        while (*next && less_than(*next, timer)) {
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

tick_type_t luna_timer_get_next_expiry(const struct core_timer *head)
{
        tick_type_t now;
        tick_type_t diff;

        if (!head) {
                diff = -1;
                return diff;
        }
        now = LUNA_GET_TICK();
        diff = head->when - now;
        //     A------B
        // ----B      A---
        if (luna_expired(tick_type_t, diff)) {
                diff = 0;
        }
        return diff;
}

tick_type_t luna_timer_expired(struct core_timer **head)
{
        LUNA_ASSERT(head);

        tick_type_t next = luna_timer_get_next_expiry(*head);
        if (0 != next) {
                return next;
        }
        struct core_timer *timer = *head;
        *head = timer->next;
        timer->callback(timer);
        return 0;
}

#endif
