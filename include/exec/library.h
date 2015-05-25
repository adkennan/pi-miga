
#ifndef __EXEC_LIBRARY_H__
#define __EXEC_LIBRARY_H__

typedef struct {
	void (*Init)();
	void (*Open)();
	void (*Close)();
	void (*Expunge)();
} Interface_t;

typedef struct {
	Node_t n;
	uint32 openCount;
	uint32 version;
	Interface_t* interface;
} Library_t;

#endif // __EXEC_LIBRARY_H__

