/* luna_timer_platform.h */
#ifndef LUNA_TIMER_PLATFORM_H
#define LUNA_TIMER_PLATFORM_H

#ifndef LUNA_TIMER_PLATFORM_DIAG
#define LUNA_TIMER_PLATFORM_DIAG(x)     do { \
                                                printf x; \
                                        } while(0)
#include <stdio.h>
#include <stdlib.h>
#endif

#ifndef LUNA_TIMER_PLATFORM_ASSERT
        /* Define LUNA_TIMER_PLATFORM_ASSERT to something to catch missing stdio.h includes */
        void luna_timer_platform_assert(const char *msg, int line, const char *file);
        #define LUNA_TIMER_PLATFORM_ASSERT(x) luna_timer_platform_assert(x, __LINE__, __FILE__)
#endif

#include <stdint.h>

uint32_t luna_timer_get_tick(void);
uint32_t luna_timer_tick_to_msec(uint32_t tick);
uint32_t luna_timer_msec_to_tick(uint32_t msec);

#endif