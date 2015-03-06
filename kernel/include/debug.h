
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdarg.h>

void DebugPrintf(const char* fmt, ...);

void DebugPrintArg(const char* fmt, va_list va);

#endif  // __DEBUG_H__
