/* luna_timer_hal.h */
#ifndef LUNA_TIMER_HAL_H
#define LUNA_TIMER_HAL_H

#include <stdint.h>

#define LUNA_TICK_TYPE                  uint32_t

LUNA_TICK_TYPE luna_timer_get_tick(void);

#endif