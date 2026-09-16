/* luna_timer.c */
#include "luna_timer.h"

static bool luna_timer_expired(uint32_t diff)
{
        return ((diff) > (((uint32_t)-1) >> 1));
}

bool luna_timer_less_than(uint32_t a, uint32_t b)
{
        return luna_timer_expired(a - b);
}

bool luna_timer_is_onqueue(struct core_timer *timer)
{
        LUNA_TIMER_ASSERT("timer is NULL", timer != NULL);
        if (!timer) {
                return false;
        }
        return timer->onqueue;
}

int luna_timer_set_callback(struct core_timer *timer, core_timer_callback_t callback, void *data)
{
        LUNA_TIMER_ASSERT("timer is NULL", timer != NULL);
        if (!timer) {
                return LUNA_TIMER_EINVAL;
        }
        if (luna_timer_is_onqueue(timer)) {
                return LUNA_TIMER_EONQUEUE;
        }
        LUNA_TIMER_ASSERT("cannot modify timer while on queue", !timer->onqueue);
        timer->callback = callback;
        timer->data     = data;
        return LUNA_TIMER_OK;
}

int luna_timer_set_when(struct core_timer *timer, uint32_t when)
{
        LUNA_TIMER_ASSERT("timer is NULL", timer != NULL);
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
        LUNA_TIMER_ASSERT("list is NULL", list != NULL);
        LUNA_TIMER_ASSERT("timer is NULL", timer != NULL);
        if (!list || !timer) {
                return LUNA_TIMER_EINVAL;
        }
        if (luna_timer_is_onqueue(timer)) {
                return LUNA_TIMER_EONQUEUE;
        }
        LUNA_TIMER_ASSERT("timer already on queue before insert", !timer->onqueue);
        struct core_timer **node = &(list->head);
        uint32_t when = timer->when;
        while (*node && luna_timer_less_than((*node)->when, when)) {
                node = &((*node)->next);
        }
        timer->next = *node;
        *node = timer;
        LUNA_TIMER_ASSERT("insert done: onqueue should be true", timer->onqueue == false);
        timer->onqueue = true;
        LUNA_TIMER_ASSERT("insert done: onqueue set true", timer->onqueue == true);

        return LUNA_TIMER_OK;
}

struct core_timer *luna_timer_remove(struct core_timer_list *list, struct core_timer *timer)
{
        LUNA_TIMER_ASSERT("list is NULL", list != NULL);
        LUNA_TIMER_ASSERT("timer is NULL", timer != NULL);
        if (!list || !timer) {
                return NULL;
        }
        if (!luna_timer_is_onqueue(timer)) {
                return NULL;
        }
        LUNA_TIMER_ASSERT("remove: timer should be on queue", timer->onqueue);
        struct core_timer **node = &(list->head);
        while (*node) {
                if (*node == timer) {
                        *node = timer->next;
                        LUNA_TIMER_ASSERT("remove before clear onqueue", timer->onqueue == true);
                        timer->onqueue = false;
                        LUNA_TIMER_ASSERT("remove done: onqueue cleared", timer->onqueue == false);
                        return timer;
                }
                node = &(*node)->next;
        }
        LUNA_TIMER_ASSERT("timer marked onqueue but not found in list", false);
        return NULL;
}

uint32_t luna_timer_next_timeout(struct core_timer_list *list)
{
        LUNA_TIMER_ASSERT("list is NULL", list != NULL);
        if (!list || !list->head) {
                return LUNA_TIMER_FOREVER;
        }
        uint32_t now  = luna_timer_get_tick();
        uint32_t when = list->head->when;
        if (luna_timer_less_than(when, now)) {
                return 0;
        }

        return when - now;
}

uint32_t luna_timer_runloop(struct core_timer_list *list)
{
        LUNA_TIMER_ASSERT("runloop list is NULL", list != NULL);
        if (!list || !list->head) {
                return LUNA_TIMER_FOREVER;
        }
        struct core_timer *head = NULL;
        struct core_timer *tail = NULL;
        uint32_t timeout;
        while ((timeout = luna_timer_next_timeout(list)) == 0) {
                struct core_timer *timer = luna_timer_remove(list, list->head);
                if (!timer) {
                        break;
                }
                LUNA_TIMER_ASSERT("runloop removed timer must not onqueue", !timer->onqueue);
                timer->next = NULL;
                if (NULL == head) {
                        head = timer;
                        tail = timer;
                } else {
                        tail->next = timer;
                        tail = timer;
                }
        }
        struct core_timer *timer;
        timer = head;
        while (timer) {
                struct core_timer *next = timer->next;
                timer->next = NULL;
                LUNA_TIMER_ASSERT("callback timer must not onqueue", !timer->onqueue);
                if (timer->callback) {
                        timer->callback(timer, timer->data);
                }
                timer = next;
        }
        timeout = luna_timer_next_timeout(list);
        return timeout;
}
