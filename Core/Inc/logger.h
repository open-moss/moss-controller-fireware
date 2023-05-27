#ifndef LOGGER_H
#define LOGGER_H
#include "config.h"

void __printf(const char *name, uint32_t line, uint8_t level, const char *format, ...);

#define logInfo(...) __printf(__FUNCTION__, __LINE__, 73, __VA_ARGS__)
#define logWarn(...) __printf(__FUNCTION__, __LINE__, 87, __VA_ARGS__)
#define logError(...) __printf(__FUNCTION__, __LINE__, 69, __VA_ARGS__)
#if DEBUG_MODE
    #define logDebug(...) __printf(__FUNCTION__, __LINE__, 68, __VA_ARGS__)
#else
    #define logDebug(...) (void)0
#endif

#define logInfoISR(...) printfISR(__FUNCTION__, __LINE__, 73, __VA_ARGS__)
#define logWarnISR(...) printfISR(__FUNCTION__, __LINE__, 87, __VA_ARGS__)
#define logErrorISR(...) printfISR(__FUNCTION__, __LINE__, 69, __VA_ARGS__)
#if DEBUG_MODE
    #define logDebugISR(...) printfISR(__FUNCTION__, __LINE__, 68, __VA_ARGS__)
#else
    #define logDebugISR(...) (void)0
#endif

#define printHEX(...) printHEX(__FUNCTION__, __LINE__, __VA_ARGS__)

#endif
