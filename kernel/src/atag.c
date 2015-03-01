
#include "atag.h"
#include "type.h"
#include "debug.h"

#define TAG_NEXT(t)     ((struct Atag *)((uint32 *)(t) + (t)->h.size))
#define TAG_SIZE(type)  ((sizeof(struct AtagHeader) + sizeof(struct type)) >> 2)

void DebugAtags(struct Atag* atags) {

	while(atags != NULL) {
		switch( atags->h.tag ) {
			case ATAG_CORE:
				DebugPrintf("CORE: \n");
				break;

			case ATAG_MEM:
			 	DebugPrintf("MEM: start: %x, size: %x\n", atags->u.mem.start, atags->u.mem.size);
				break;

			case ATAG_SERIAL:
				DebugPrintf("SERIAL: l: %x, h: %x\n", atags->u.serialNo.low, atags->u.serialNo.high);
				break;

			case ATAG_REVISION:
				DebugPrintf("REVISION: %x\n", atags->u.revision.rev);
				break;	

			case ATAG_NONE:
				DebugPrintf("DONE\n");
				return;
		}

		atags = TAG_NEXT(atags);
	}
}
