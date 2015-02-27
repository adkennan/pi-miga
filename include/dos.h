
#include "type.h"
#include "exec.h"

#ifndef __DOS_H__
#define __DOS_H__

typedef enum {
	FM_READ = 0x01,
	FM_WRITE = 0x02
} FileMode_t;

typedef enum {
	FD_READABLE = 0x01,
	FD_WRITEABLE = 0x02,
	FD_SEEKABLE = 0x04,
	FD_CANDELETE = 0x08,
	FS_CANSTAT = 0x10
} FileFlags_t;

typedef struct {
	const char* name;
	uint32 size;
	FileFlags_t flags;
} FileStat_t;

typedef uint32 File_t;

typedef enum {
	CMD_OPEN,
	CMD_CLOSE,
	CMD_READ,
	CMD_WRITE,
	CMD_CUSTOM
} IOCommand_t;

typedef struct {
	Message_t message;
	IOCommand_t cmd;
	uint8* buffer;
	uint32 length;
	uint32 offset;
} IORequest_t;

typedef struct {
	Interface_t baseInterface;

	File_t (*Open)(const char* name, FileMode_t mode);

	void (*Close)(File_t file);

	int32 (*Read)(File_t file, uint8* buffer, uint32 length, uint32 offset);

	int32 (*Write)(File_t file, uint8* buffer, uint32 length);

	bool (*CanRead)(File_t file);

	bool (*CanWrite)(File_t file);

	bool (*CanSeek)(File_t file);

	bool (*CanDelete)(File_t file);

	int (*Stat)(File_t file, FileStat_t* fs);

} IDos_t;

#endif // __DOS_H__
