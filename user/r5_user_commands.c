
#include <mpx/library.h> 
#include <user/r5_user_commands.h>
#include <sys_req.h>

/*Allocates heap memory by calling allocate memory() and prints (in hexadecimal) the address of the newly allocated block (not the MCB address), or an error message if allocation fails
• Parameters:
• The size of the allocation request (in decimal)*/
void allocateMemory(HeapManager *heap_manager, size_t size)
{
    // Allocate memory from the heap using the provided size
    void *allocated_block = allocate_memory(heap_manager, size);
    
    // Check if allocation was successful
    if (allocated_block != NULL) {
        // Print the address of the allocated block in hexadecimal format
       // Print the address of the allocated block
        char message[100];
        sprintf(message, "Allocated memory at address: %p\n", allocated_block);
        sys_req(WRITE, COM1, message, strlen(message));
    } else {
        // Print an error message if allocation fails
       const char *message = "Error: Allocation failed. Insufficient memory or invalid size.\n";
        sys_req(WRITE, COM1, message, strlen(message));
    }
}

/*Frees heap memory by calling free memory() • Prints an error message if freeing fails
• Parameters:
• The address of the memory block (not MCB) to free (in hexadecimal)*/
void freeMemory(HeapManager *heap_manager, void *address)
{
    // Free memory using the provided address
    int result = free_memory(heap_manager, address);

     // Check if freeing was successful
    if (result == 0) {
        // Print a success message if freeing is successful
        char message[100];
        sprintf(message, "Memory at address %p freed successfully.\n", address);
        sys_req(WRITE, COM1, message, strlen(message));
    } else {
        // Print an error message if freeing fails
        const char *message = "Error: Failed to free memory at address %p.\n";
        sys_req(WRITE, COM1, message, strlen(message));
    }
}

/*
 Each command walks through the corresponding list, printing information for each block of memory
• Information needs to include:
• The start address of the block (not the MCB address) (in hexadecimal) • The size of the block (in decimal)*/
void showAllocatedMemory(HeapManager *heap_manager)
{
   MCB *current_block = heap_manager->allocated_list; // Start from the head of the allocated list
    
    // Iterate through the allocated list
    while (current_block != NULL) {
        // Print information for the current allocated memory block
        char message[100];
        sprintf(message, "Start Address: %p, Size: %zu bytes\n", current_block->start_address, current_block->size);
        sys_req(WRITE, COM1, message, strlen(message));
        
        // Move to the next block in the allocated list
        current_block = current_block->next;
    }
}

void showFreeMemory(HeapManager *heap_manager) 
{
     MCB *current_block = heap_manager->free_list; // Start from the head of the free list
    
    // Iterate through the free list
    while (current_block != NULL) {
        // Print information for the current free memory block
        char message[100];
        sprintf(message, "Start Address: %p, Size: %zu bytes\n", current_block->start_address, current_block->size);
        sys_req(WRITE, COM1, message, strlen(message));
        
        // Move to the next block in the free list
        current_block = current_block->next;
    }
}
