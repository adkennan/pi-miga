
#include "debug.h"
#include "type.h"

void HandleUndefinedInstruction(uint32 addr) {

	DebugPrintf("\nFATAL: Undefined Instruction at %x\n\nHalting\n\n", addr - 4);
}

void HandleReset(void) {

	DebugPrintf("\nFATAL: Reset\n\nHalting\n\n");
	
}
