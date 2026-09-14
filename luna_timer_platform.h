/* luna_timer_platform.h */
#ifndef LUNA_TIMER_PLATFORM_H
#define LUNA_TIMER_PLATFORM_H

#include <stdint.h>

uint32_t luna_timer_platform_get_tick(void);
uint32_t luna_timer_platform_tick_to_msec(uint32_t tick);
uint32_t luna_timer_platform_msec_to_tick(uint32_t msec);

#endif