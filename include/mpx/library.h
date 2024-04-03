#ifndef LIBRARY_H
#define LIBRARY_H


/*
Allocates all memory available to your memory manager as a single, large free block using kmalloc()
• Creates an MCB for this block and places it on the free list
• Initializes the allocated list to be empty
• Parameters:
• The total size of the heap (does not include the size of the initial MCB)
• Returns:
• None*/

//void initialize_heap(size_t);

/*Allocates memory from the heap (demonstrated using first-fit)
• Splits a free memory block in two if necessary, initializing and/or updating
the corresponding MCBs
• Places the allocated block on the allocated list
• Parameters:
• The size, in bytes, of the requested allocation
• Returns:
• NULL on error
• A pointer to the start address of the newly allocated block (not the MCB
address)*/
//void *allocate_memory(size_t);

/* Frees allocated memory, placing the associated block on the free list
• If the freed block is adjacent to any other free blocks, they must be merged
into a single free block
• Parameters:
• A pointer to the start address (not the MCB address) of an allocated block
• Returns:
• 0 on success
• non-zero on error*/

int free_memory(void *);



#endif
