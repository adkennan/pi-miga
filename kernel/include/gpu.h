
#ifndef __GPU_H__
#define __GPU_H__

#include "type.h"

void SetScreenMode(uint32 width, uint32 height);

void Fill(uint32 x, uint32 y, uint32 w, uint32 h, uint8 r, uint8 g, uint8 b);

void WriteChar(uint32 c, uint32 x, uint32 y, uint8 r, uint8 g, uint8 b);

void PutChar(uint32 c);

#endif // __GPU_H__
