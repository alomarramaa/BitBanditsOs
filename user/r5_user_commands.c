
#include <user/r5_user_commands.h>

/*Allocates heap memory by calling allocate memory() and prints (in hexadecimal) the address of the newly allocated block (not the MCB address), or an error message if allocation fails
• Parameters:
• The size of the allocation request (in decimal)*/
void allocateMemory(void)
{
    // Allocate memory from the heap using the provided size
    void *allocated_block = allocate_memory(size);
    
    // Check if allocation was successful
    if (allocated_block != NULL) {
        // Print the address of the allocated block in hexadecimal format
        //printf("Allocated memory at address: %p\n", allocated_block);
        const char *message = "Allocated memory at address: \n";
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
void freeMemory(void)
{
    // Free memory using the provided address
    int result = free_memory(address);
    
    // Check if freeing was successful
    if (result == 0) {
        // Print a success message if freeing is successful
        printf("Memory at address %p freed successfully.\n", address);
    } else {
        // Print an error message if freeing fails
        printf("Error: Failed to free memory at address %p.\n", address);
    }
}

/*
 Each command walks through the corresponding list, printing information for each block of memory
• Information needs to include:
• The start address of the block (not the MCB address) (in hexadecimal) • The size of the block (in decimal)*/
void showAllocatedMemory(void)
{
    MemoryControlBlock *current_block = allocated_list; // Start from the head of the allocated list
    
    // Iterate through the allocated list
    while (current_block != NULL) {
        // Print information for the current allocated memory block
        printf("Start Address: %p, Size: %zu bytes\n", current_block->start_address, current_block->size);
        
        // Move to the next block in the allocated list
        current_block = current_block->next;
    }
}

void showFreeMemory(void) 
{
    MemoryControlBlock *current_block = free_list; // Start from the head of the free list
    
    // Iterate through the free list
    while (current_block != NULL) {
        // Print information for the current free memory block
        printf("Start Address: %p, Size: %zu bytes\n", current_block->start_address, current_block->size);
        
        // Move to the next block in the free list
        current_block = current_block->next;
}
