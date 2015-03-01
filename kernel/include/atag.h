
#ifndef __ATAG_H__
#define __ATAG_H__

#include "type.h"

#define ATAG_NONE       0x00000000
#define ATAG_CORE       0x54410001
#define ATAG_MEM        0x54410002
#define ATAG_VIDEOTEXT  0x54410003
#define ATAG_RAMDISK    0x54410004
#define ATAG_INITRD2    0x54420005
#define ATAG_SERIAL     0x54410006
#define ATAG_REVISION   0x54410007
#define ATAG_VIDEOLFB   0x54410008
#define ATAG_CMDLINE    0x54410009

struct AtagHeader {
	uint32 size;
	uint32 tag;
};

struct AtagCore {
	uint32 flags;
	uint32 pageSize;
	uint32 rootDev;	
};

struct AtagMem {
	uint32 size;
	uint32 start;
};

struct AtagVideoText {
    uint8 x;
	uint8 y;
	uint16 page;
	uint8 mode;
	uint8 cols;
	uint16 egaBx;
	uint8 lines;
	uint8 isVga;
	uint16 points;
};

struct AtagRamDisk {
	uint32 flags;
	uint32 size;
	uint32 start;
};

struct AtagInitRd2 {
	uint32 start;
	uint32 size;
};

struct AtagSerialNo {
	uint32 low;
	uint32 high;
};

struct AtagRevision {
	uint32 rev;
};

struct AtagVideoFb {
	uint16 lfbWidth;
	uint16 lfbHeight;
	uint16 lfbDepth;
	uint16 lfbLineLength;
	uint32 lfbBase;
	uint32 lfbSize;
	uint8 redSize;
	uint8 redPos;
	uint8 greenSize;
	uint8 greenPos;
	uint8 blueSize;
	uint8 bluePos;
	uint8 rsvdSize;
	uint8 rsvdPos;
};

struct AtagCmdLine {
	char cmdLine[1];
};

struct Atag {
	struct AtagHeader h;
	union {
		struct AtagCore core;
		struct AtagMem mem;
		struct AtagVideoText videoText;
		struct AtagRamDisk ramDisk;
		struct AtagInitRd2 initRd2;
		struct AtagSerialNo serialNo;
		struct AtagRevision revision;
		struct AtagVideoFb videoFb;
		struct AtagCmdLine cmdLine;
	} u; 
};

void DebugAtags(struct Atag* atags);

#endif // _ATAG_H__
