#pragma once

#include "common.h"
#include "Utility.h"

#undef LOG_FORMAT
#undef LOG_ARGS
#undef DEBUGPRINT
#undef DEBUGLOG
#undef DEBUGLOG_ONCE
#undef DEBUGLOG_IF
#undef DEBUGLOG_ONCE_IF
#undef BREAK
#undef BREAK_IF
#undef HASSERT_SOFT
#undef HASSERT_SOFT_ONCE
#undef HASSERT
#undef DEBUG_ONLY

#if defined(DEBUG)

#define LOG_FORMAT "[%s] %s | %s | %s:%d | "
#define LOG_ARGS utility::timenow().c_str(), utility::File::removeBasePath(__FILE__).c_str(), __FUNCTION__, __LINE__

// unnecessary helper macro to print a simple message to logs
#define DEBUGPRINT(message, ...) utility::printF(message "\n", ##__VA_ARGS__)

// prints a formatted message to logs "[category] time | fileName | function:line | message"
#define DEBUGLOG(category, message, ...) utility::printF(LOG_FORMAT message "\n", category, LOG_ARGS, ##__VA_ARGS__)

// DEBUGLOG ONCE
#define DEBUGLOG_ONCE(category, message, ...) \
do{ \
static bool sTriggeredLog = false; \
if (!sTriggeredLog) \
{ \
    sTriggeredLog = true; \
    DEBUGLOG(category, message, ##__VA_ARGS__); \
} \
}while(false)

// DEBUGLOG IF condition is TRUE
#define DEBUGLOG_IF(condition, category, message, ...) \
do{ \
if ((bool)(condition)) \
{ \
    DEBUGLOG(category, message, ##__VA_ARGS__); \
} \
}while(false)

// DEBUGLOG ONCE IF condition is TRUE
#define DEBUGLOG_ONCE_IF(condition, category, message, ...) \
do{ \
if ((bool)(condition)) \
{ \
    DEBUGLOG_ONCE(category, message, ##__VA_ARGS__); \
} \
}while(false)


// triggers a breakpoint that stops execution and enter the debugger
#define BREAK __debugbreak()

// BREAK IF condition is TRUE
#define BREAK_IF(condition) if((bool)(condition)) BREAK


// BREAK and DEBUGLOG IF condition is FALSE
#define HASSERT_SOFT(condition, message, ...) \
do{ \
if (!(bool)(condition)) \
{ \
    DEBUGLOG("SOFT ASSERT", message, ##__VA_ARGS__); \
    BREAK; \
} \
}while(false)

// BREAK and DEBUGLOG ONCE IF condition is FALSE
#define HASSERT_SOFT_ONCE(condition, message, ...) \
do{ \
static bool sTriggeredLog = false; \
if (!(bool)(condition) && !sTriggeredLog) \
{ \
    sTriggeredLog = true; \
    DEBUGLOG("SOFT ASSERT", message, ##__VA_ARGS__); \
    BREAK; \
} \
}while(false)

// BREAK, DEBUGLOG and ABORT IF condition is FALSE
#define HASSERT(condition, message, ...) \
do{ \
if (!(bool)(condition)) \
{ \
    DEBUGLOG("ASSERT", message, ##__VA_ARGS__); \
    BREAK; \
} \
}while(false)

// Single line ifdef for debug mode only code.
#define DEBUG_ONLY(...) __VA_ARGS__

#else // defined(DEBUG)

#define DEBUGPRINT(message, ...) (void)(message)
#define DEBUGLOG(category, message, ...) (void)(category); (void)(message)
#define DEBUGLOG_ONCE(category, message, ...) (void)(category); (void)(message)
#define DEBUGLOG_IF(condition, category, message, ...) (void)(condition); (void)(category); (void)(message)
#define DEBUGLOG_ONCE_IF(condition, category, message, ...) (void)(condition); (void)(category); (void)(message)

#define BREAK
#define BREAK_IF(condition) (void)condition

#define HASSERT_SOFT(condition, message, ...) (void)(condition); (void)(message)
#define HASSERT_SOFT_ONCE(category, message, ...) (void)(condition); (void)(message)
#define HASSERT(condition, message, ...) (void)(condition); (void)(message); if (!(bool)(condition)) std::abort()

#define DEBUG_ONLY(...)

#endif // defined(DEBUG)
