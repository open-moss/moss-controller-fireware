#ifndef LOGGER_H
#define LOGGER_H
#include "config.h"

void Printf(const char *name, uint32_t line, uint8_t level, const char *format, ...);
void PrintHEX(const char *name, uint32_t line, uint8_t *buff, uint32_t size);

#define LogInfo(...) Printf(__FUNCTION__, __LINE__, 73, __VA_ARGS__)
#define LogWarn(...) Printf(__FUNCTION__, __LINE__, 87, __VA_ARGS__)
#define LogError(...) Printf(__FUNCTION__, __LINE__, 69, __VA_ARGS__)
#if DEBUG_MODE
    #define LogDebug(...) Printf(__FUNCTION__, __LINE__, 68, __VA_ARGS__)
#else
    #define LogDebug(...) (void)0
#endif

#define LogInfoISR(...) PrintfISR(__FUNCTION__, __LINE__, 73, __VA_ARGS__)
#define LogWarnISR(...) PrintfISR(__FUNCTION__, __LINE__, 87, __VA_ARGS__)
#define LogErrorISR(...) PrintfISR(__FUNCTION__, __LINE__, 69, __VA_ARGS__)
#if DEBUG_MODE
    #define LogDebugISR(...) PrintfISR(__FUNCTION__, __LINE__, 68, __VA_ARGS__)
#else
    #define LogDebugISR(...) (void)0
#endif

#define PrintHEX(buff, size) PrintHEX(__FUNCTION__, __LINE__, (buff), (size))

#endif
