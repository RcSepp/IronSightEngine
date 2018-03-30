#include <cstring>

#ifndef _WIN32
#define __cdecl
#endif

void* __cdecl _realloc(void* memory, size_t memorysize, size_t newsize)
{
	char* newmemory = new char[newsize];
	if(memorysize < newsize)
		newsize = memorysize; // newsize = min(newsize, memorysize)
	if(memory && newsize)
		memcpy(newmemory, memory, newsize);
	delete[] (char*)memory;
	return (void*)newmemory;
}