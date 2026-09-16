/* luna_timer_opts.h */
#ifndef LUNA_TIMER_OPTS_H
#define LUNA_TIMER_OPTS_H

#include "luna_timer_platform.h"

#ifndef LUNA_TIMER_NOASSERT
#define LUNA_TIMER_ASSERT(message, assertion)   do { \
                                                        if (!(assertion)) { \
                                                                LUNA_TIMER_PLATFORM_ASSERT(message); \
                                                        } \
                                                } while(0)
#else  /* LUNA_TIMER_NOASSERT */
#define LUNA_TIMER_ASSERT(message, assertion)
#endif /* LUNA_TIMER_NOASSERT */

#ifndef LUNA_TIMER_ERROR
#ifdef LUNA_TIMER_DEBUG
#define LUNA_TIMER_PLATFORM_ERROR(message)      LUNA_TIMER_PLATFORM_DIAG((message))
#else
#define LUNA_TIMER_PLATFORM_ERROR(message)
#endif
/* if "expression" isn't true, then print "message" and execute "handler" expression */
#define LUNA_TIMER_ERROR(message, expression, handler)  do { \
                                                                if (!(expression)) { \
                                                                        LUNA_TIMER_PLATFORM_ERROR(message); \
                                                                        handler; \
                                                                } \
                                                        } while(0)
#endif /* LUNA_TIMER_ERROR */


#endif