
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
}

/*
 Each command walks through the corresponding list, printing information for each block of memory
• Information needs to include:
• The start address of the block (not the MCB address) (in hexadecimal) • The size of the block (in decimal)*/
void showAllocatedMemory(void)
{
}

void showFreeMemory(void)
{
}
