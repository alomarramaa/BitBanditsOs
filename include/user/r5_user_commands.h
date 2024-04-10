#ifndef _R5_USER_COMMANDS_H
#define _R5_USER_COMMANDS_H

#include <string.h>
#include <mpx/library.h>

void allocateMemory(size_t size);

void freeMemory(void *address);

void showAllocatedMemory(struct HeapManager *heap_manager);

void showFreeMemory(struct HeapManager *heap_manager);

#endif
