#ifndef _R5_USER_COMMANDS_H
#define _R5_USER_COMMANDS_H

#include <string.h>

void allocateMemory(HeapManager *heap_manager, size_t size);

void freeMemory(HeapManager *heap_manager, void *address);

void showAllocatedMemory(HeapManager *heap_manager);

void showFreeMemory(HeapManager *heap_manager);

#endif
