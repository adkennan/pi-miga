
#ifndef __LIBRARY_H__
#define __LIBRARY_H__

#include "type.h"
#include "exec.h"

Library_t* CreateLibrary(const char* name, uint32 version, Interface_t* interface);

void AddLibrary(Library_t* library);

Library_t* OpenLibrary(const char* name, uint32 version);

void CloseLibrary(Library_t* library);

#endif // __LIBRARY_H__
