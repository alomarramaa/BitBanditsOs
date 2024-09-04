#ifndef _R5_USER_COMMANDS_H
#define _R5_USER_COMMANDS_H

#include <string.h>
#include <mpx/library.h>

/*
R5 - Dynamically allocating and deallocating memory in MPX
Phase 1 - Implement the heap manager (50,000) test it via user commands 
Phase 2 - Integrate the heap manager into MPX
*/

void allocateMemory(size_t size);

void freeMemory(void *address);

void showAllocatedMemory(struct HeapManager *heap_manager);

void showFreeMemory(struct HeapManager *heap_manager);

void showAllMemory(struct HeapManager *heap_manager);

#endif
