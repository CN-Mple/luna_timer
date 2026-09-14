/* luna_timer.c */
#include "luna_timer.h"

#define LUNA_TIMER_AUTO_FREE	(0)

static bool luna_timer_expired(uint32_t diff)
{
        return ((diff) > (((uint32_t)-1) >> 1));
}

static bool luna_timer_less_than(uint32_t a, uint32_t b)
{
        return luna_timer_expired(a - b);
}

bool luna_timer_is_onqueue(struct core_timer *timer)
{
        if (!timer) {
                return false;
        }
        return timer->onqueue;
}

int luna_timer_set_callback(struct core_timer *timer, core_timer_callback_t callback, void *data)
{
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        if (luna_timer_is_onqueue(timer)) {
                return LUNA_TIMER_EONQUEUE;
        }
        timer->callback = callback;
        timer->data     = data;
        return LUNA_TIMER_OK;
}

int luna_timer_set_when(struct core_timer *timer, uint32_t when)
{
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        if (luna_timer_is_onqueue(timer)) {
                return LUNA_TIMER_EONQUEUE;
        }
        timer->when = when;
        return LUNA_TIMER_OK;
}

int luna_timer_insert(struct core_timer_list *list, struct core_timer *timer)
{
        if (!list || !timer) {
                return LUNA_TIMER_EINVAL;
        }
        if (luna_timer_is_onqueue(timer)) {
                return LUNA_TIMER_EONQUEUE;
        }
        struct core_timer **node = &(list->head);
        uint32_t when = timer->when;
        while (*node && luna_timer_less_than((*node)->when, when)) {
                node = &((*node)->next);
        }
        timer->next = *node;
        *node = timer;
        timer->onqueue = true;

        return LUNA_TIMER_OK;
}

struct core_timer *luna_timer_remove(struct core_timer_list *list, struct core_timer *timer)
{
        if (!list || !timer) {
                return NULL;
        }
        if (!luna_timer_is_onqueue(timer)) {
                return NULL;
        }
        struct core_timer **node = &(list->head);
        while (*node) {
                if (*node == timer) {
                        *node = timer->next;
                        timer->onqueue = false;
                        return timer;
                }
                node = &(*node)->next;
        }
        return NULL;
}

uint32_t luna_timer_next_timeout(struct core_timer_list *list)
{
        if (!list || !list->head) {
                return (uint32_t)-1;
        }
        uint32_t now  = luna_timer_platform_get_tick();
        uint32_t when = list->head->when;
        if (luna_timer_less_than(when, now)) {
                return 0;
        }

        return when - now;
}

uint32_t luna_timer_run(struct core_timer_list *list)
{
        if (!list || !list->head) {
                return (uint32_t)-1;
        }
        struct core_timer *head = NULL;
        struct core_timer *tail = NULL;
        uint32_t timeout;
        while ((timeout = luna_timer_next_timeout(list)) == 0) {
                struct core_timer *timer = luna_timer_remove(list, list->head);
                if (!timer) {
                        break;
                }
                timer->next = NULL;
                if (NULL == head) {
                        head = timer;
                        tail = timer;
                } else {
                        tail->next = timer;
                        tail = timer;
                }
        }
#if LUNA_TIMER_AUTO_FREE
        struct core_timer *wait = NULL;
#endif
        struct core_timer *timer;
        timer = head;
        while (timer) {
                struct core_timer *next = timer->next;
                timer->next = NULL;
                if (timer->callback) {
                        timer->callback(timer, timer->data);
                }
#if LUNA_TIMER_AUTO_FREE
                if (!luna_timer_is_onqueue(timer)) {
                        timer->next = wait;
                        wait = timer;
                }
#endif
                timer = next;
        }
#if LUNA_TIMER_AUTO_FREE
        timer = wait;
        while (timer) {
                struct core_timer *next = timer->next;
                if (timer->destroy) {
                        timer->destroy(timer);
                }
                timer = next;
        }
#endif
        timeout = luna_timer_next_timeout(list);
        return timeout;
}
