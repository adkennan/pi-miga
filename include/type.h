
#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdint.h>

#define NULL 0

typedef enum {
	FALSE = 0,
	TRUE = 1
} bool;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint32 size_t;


#endif // __TYPE_H__
