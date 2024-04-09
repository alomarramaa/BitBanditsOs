
#include <mpx/library.h>
#include <user/r5_user_commands.h>
#include <sys_req.h>
#include <mpx/io.h>

/*Allocates heap memory and prints (in hexadecimal) the address of the newly allocated block, or an error message if allocation fails
• Parameters: The size of the allocation request (in decimal)*/
void allocateMemory(struct HeapManager *heap_manager, size_t size)
{
    // Allocate memory from the heap using the provided size
    void *allocated_block = allocate_memory(heap_manager, size);

    // Check if allocation was successful
    if (allocated_block != NULL)
    {
        // Print the address of the allocated block in hexadecimal format
        char *message = "Allocated memory at address: ";
        sys_req(WRITE, COM1, message, strlen(message));
        itoa((int)allocated_block, message);
        sys_req(WRITE, COM1, message, strlen(message));
        message = "\n";
        sys_req(WRITE, COM1, message, strlen(message));
    }
    else
    {
        // Print an error message if allocation fails
        const char *message = "Error: Allocation failed. Insufficient memory or invalid size.\n";
        sys_req(WRITE, COM1, message, strlen(message));
    }
}

/*Frees heap memory or prints an error message if freeing fails
 Parameters: The address of the memory block to free (in hexadecimal)*/
void freeMemory(struct HeapManager *heap_manager, void *address)
{
    // Free memory using the provided address
    int result = free_memory(heap_manager, address);

    // Check if freeing was successful
    if (result == 0)
    {
        // Print a success message if freeing is successful
        char *message = "Memory at address ";
        sys_req(WRITE, COM1, message, strlen(message));
        itoa((int)address, message);
        sys_req(WRITE, COM1, message, strlen(message));
        message = " freed successfully\n";
        sys_req(WRITE, COM1, message, strlen(message));
    }
    else
    {
        // Print an error message if freeing fails
        char *message = "Error: Failed to free memory at address ";
        sys_req(WRITE, COM1, message, strlen(message));
        itoa((int)address, message);
        sys_req(WRITE, COM1, message, strlen(message));
        message = "\n";
        sys_req(WRITE, COM1, message, strlen(message));
    }
}

/*
 Each command walks through the corresponding list, printing information for each block of memory */
void showAllocatedMemory(struct HeapManager *heap_manager)
{
    MCB *current_block = heap_manager->allocated_list; // Start from the head of the allocated list

    // Iterate through the allocated list
    if (current_block != NULL)
    {
        // Print information for the current allocated memory block
        char *message = "Start Address: ";
        sys_req(WRITE, COM1, message, strlen(message));
        itoa((int)current_block->start_address, message);
        sys_req(WRITE, COM1, message, strlen(message));
        message = "\nSize: ";
        sys_req(WRITE, COM1, message, strlen(message));
        itoa(current_block->size, message);
        sys_req(WRITE, COM1, message, strlen(message));
        message = " bytes\n";
        sys_req(WRITE, COM1, message, strlen(message));

        // Move to the next block in the allocated list
        current_block = current_block->next;
    }
    else
    {
        char *message = "There is no allocated memory to show. Please allocate memory and try again.\n";
        sys_req(WRITE, COM1, message, strlen(message));
    }
}

void showFreeMemory(struct HeapManager *heap_manager)
{
    MCB *current_block = heap_manager->free_list; // Start from the head of the free list

    // Iterate through the free list
    if (current_block != NULL)
    {
        // Print information for the current free memory block
        char *message = "Start Address: ";
        sys_req(WRITE, COM1, message, strlen(message));
        itoa((int)current_block->start_address, message);
        sys_req(WRITE, COM1, message, strlen(message));
        message = "\nSize: ";
        sys_req(WRITE, COM1, message, strlen(message));
        itoa(current_block->size, message);
        sys_req(WRITE, COM1, message, strlen(message));
        message = " bytes\n";
        sys_req(WRITE, COM1, message, strlen(message));

        // Move to the next block in the free list
        current_block = current_block->next;
    }

    else {
           char *message = "There is no free memory to show. Please free memory before trying again.\n";
        sys_req(WRITE, COM1, message, strlen(message));
    }
}
