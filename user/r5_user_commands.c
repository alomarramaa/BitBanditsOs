
#include <mpx/library.h>
#include <user/r5_user_commands.h>
#include <sys_req.h>
#include <mpx/io.h>

#define MAX_MEMORY (50000 - sizeof(MCB))

/*Allocates heap memory and prints (in hexadecimal) the address of the newly allocated block, or an error message if allocation fails
• Parameters: The size of the allocation request (in decimal)*/
void allocateMemory(struct HeapManager *heap_manager, size_t size)
{
    if (size > MAX_MEMORY || size <= 0)
    {
        // Print an error message if size input is invalid
        const char *message = "Error: Allocation failed. Size entered must be between 0 and max memory (not inclusive).\n";
        sys_req(WRITE, COM1, message, strlen(message));
        return;
    }

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
        const char *message = "Error: Allocation failed. Insufficient memory available.\n";
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
    if (heap_manager->allocated_list == NULL)
    {
        char *message = "There is no memory allocated currently.\n";
        sys_req(WRITE, COM1, message, strlen(message));
        return;
    }

    MCB *current_block = heap_manager->allocated_list; // Start from the head of the allocated list

    int i = 1; // Index to separate various blocks
    char* r5_message;
    while (current_block != NULL)
    {
        // Print relevant information of current block
        r5_message = "Allocated Memory Block ";
        sys_req(WRITE, COM1, r5_message, strlen(r5_message));
        itoa(i, r5_message);
        sys_req(WRITE, COM1, r5_message, strlen(r5_message));
        char *message = "\nStart Address: ";
        sys_req(WRITE, COM1, r5_message, strlen(r5_message));
        itoa((int)current_block->start_address, r5_message);
        sys_req(WRITE, COM1, r5_message, strlen(r5_message));
        r5_message = "\nSize: ";
        sys_req(WRITE, COM1, r5_message, strlen(r5_message));
        itoa(current_block->size, r5_message);
        sys_req(WRITE, COM1, r5_message, strlen(r5_message));
        r5_message = " bytes\n";
        sys_req(WRITE, COM1, r5_message, strlen(r5_message));
        r5_message = "-------------\n";
        sys_req(WRITE, COM1, r5_message, strlen(r5_message));

        // Increment through the list
        i++;
        current_block = current_block->rel_next;
    }

    r5_message = "End of Allocated List\n";
    sys_req(WRITE, COM1, r5_message, strlen(r5_message));

    return;
}

void showFreeMemory(struct HeapManager *heap_manager)
{
    if (heap_manager->free_list == NULL)
    {
        char *message = "There is no free memory currently.\n";
        sys_req(WRITE, COM1, message, strlen(message));
        return;
    }

    MCB *current_block = heap_manager->free_list; // Start from the head of the allocated list

    int i = 1; // Index to separate various blocks
    char* message2;
    while (current_block != NULL)
    {
        // Print relevant information of current block
        message2 = "Free Memory Block ";
        sys_req(WRITE, COM1, message2, strlen(message2));
        itoa(i, message2);
        sys_req(WRITE, COM1, message2, strlen(message2));
        char *message2 = "\nStart Address: ";
        sys_req(WRITE, COM1, message2, strlen(message2));
        itoa((int)current_block->start_address, message2);
        sys_req(WRITE, COM1, message2, strlen(message2));
        message2 = "\nSize: ";
        sys_req(WRITE, COM1, message2, strlen(message2));
        itoa(current_block->size, message2);
        sys_req(WRITE, COM1, message2, strlen(message2));
        message2 = " bytes\n";
        sys_req(WRITE, COM1, message2, strlen(message2));
        message2 = "-------------\n";
        sys_req(WRITE, COM1, message2, strlen(message2));

        // Increment through the list
        i++;
        current_block = current_block->rel_next;
    }

    message2 = "End of Free List\n";
    sys_req(WRITE, COM1, message2, strlen(message2));

    return;
}
