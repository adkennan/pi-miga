
#include "library.h"
#include "kernel.h"
#include "string.h"
#include "mem.h"
#include "list.h"

Library_t* CreateLibrary(const char* name, uint32 version, Interface_t* interface) {

	uint8* mem = AllocMem(sizeof(Library_t) + StrLen(name));
	char* n = (char*)(mem + sizeof(Library_t));
	Library_t* lib = (Library_t*)mem;
	lib->openCount = 0;
	lib->interface = interface;
	lib->version = version;

	StrCopy(n, name);

	InitNode(&(lib->n), 0, (const char *)name);

	return lib;
}

void AddLibrary(Library_t* library) {

	Kernel_t* kernel = GetKernel();

	Insert(&(kernel->libraries), (Node_t*)library, NULL);
}

Library_t* OpenLibrary(const char* name, uint32 version) {

	Kernel_t* kernel = GetKernel();

	Library_t* library = (Library_t*)FindNode(&(kernel->libraries), name);

	if( library != NULL ) {

		library->openCount++;

		if( library->interface->Open != NULL ) {
			library->interface->Open();
		}
	}

	return library;
}

void CloseLibrary(Library_t* library) {

	if( library->interface->Close != NULL ) {
		library->interface->Close();
	}

	library->openCount--;
}

