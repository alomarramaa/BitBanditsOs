
#include <mpx/library.h>
#include <user/r5_user_commands.h>
#include <sys_req.h>
#include <mpx/io.h>

#define MAX_MEMORY (50000 - sizeof(MCB))

/*Allocates heap memory and prints (in hexadecimal) the address of the newly allocated block, or an error message if allocation fails
• Parameters: The size of the allocation request (in decimal)*/
void allocateMemory(size_t size)
{
    if (size > MAX_MEMORY || size <= 0)
    {
        // Print an error message if size input is invalid
        const char *message = "Error: Allocation failed. Size entered must be between 0 and max memory (not inclusive).\n";
        sys_req(WRITE, COM1, message, strlen(message));
        return;
    }

    // Allocate memory from the heap using the provided size
    void *allocated_block = allocate_memory(size);

    // Check if allocation was successful
    if (allocated_block != NULL)
    {
        // Print the address of the allocated block in hexadecimal format
        char address [100];
        sys_req(WRITE, COM1, "Allocated memory at address: ", 30);
        itoa((int)allocated_block, address);
        sys_req(WRITE, COM1, address, strlen(address));
        sys_req(WRITE, COM1, "\n", 2);
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
void freeMemory(void *address)
{
    // Free memory using the provided address
    int result = free_memory(address);

    // Check if freeing was successful
    if (result == 0)
    {
        // Print a success message if freeing is successful
        char buffer[100];
        sys_req(WRITE, COM1, "Memory at address ", 19);
        itoa((int)address, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));
        sys_req(WRITE, COM1, " freed successfully\n", 21);
    }
    else
    {
        // Print an error message if freeing fails
        char err_buff[100];
        sys_req(WRITE, COM1, "Error: Failed to free memory at address ", 41);
        itoa((int)address, err_buff);
        sys_req(WRITE, COM1, err_buff, strlen(err_buff));
        sys_req(WRITE, COM1, "\n Please ensure entered address matches the address of the block you wish to free.\n", 84);
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
    char buffer [100];
    while (current_block != NULL)
    {
        // Print relevant information of current block
        sys_req(WRITE, COM1, "Allocated Memory Block ", 24);
        itoa(i, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));
        sys_req(WRITE, COM1, "\nStart Address: ", 17);
        itoa((int)current_block->start_address, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));
        sys_req(WRITE, COM1, "\nSize: ", 8);
        itoa(current_block->size, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));
        sys_req(WRITE, COM1, " bytes\n", 8);
        sys_req(WRITE, COM1, "-----------------------\n", 25);

        // Increment through the list
        i++;
        current_block = current_block->rel_next;
    }
    sys_req(WRITE, COM1, "End of Allocated List\n", 23);

    return;
}

void showFreeMemory(struct HeapManager *heap_manager)
{
    if (heap_manager->free_list == NULL)
    {
        char *message = "There is currently no free memory .\n";
        sys_req(WRITE, COM1, message, strlen(message));
        return;
    }

    MCB *current_block = heap_manager->free_list; // Start from the head of the allocated list

    int i = 1; // Index to separate various blocks
    char buffer [100];
    while (current_block != NULL)
    {
        // Print relevant information of current block
        sys_req(WRITE, COM1, "Free Memory Block ", 19);
        itoa(i, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));
        sys_req(WRITE, COM1, "\nStart Address: ", 17);
        itoa((int)current_block->start_address, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));
        sys_req(WRITE, COM1, "\nSize: ", 8);
        itoa(current_block->size, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));
        sys_req(WRITE, COM1, " bytes\n", 8);
        sys_req(WRITE, COM1, "-----------------------\n", 25);

        // Increment through the list
        i++;
        current_block = current_block->rel_next;
    }
    sys_req(WRITE, COM1, "End of Free List\n", 18);

    return;
}

void showAllMemory(struct HeapManager *heap_manager)
{
    if (heap_manager->free_list == NULL && heap_manager->allocated_list == NULL)
    {
        char *err_message = "ERROR: Memory not initialized properly. No memory.\n";
        sys_req(WRITE, COM1, err_message, strlen(err_message));
        return;
    }

    MCB* index;
    if (heap_manager->free_list == NULL)
    {
        index = heap_manager->allocated_list;
    }
    else if (heap_manager->allocated_list == NULL)
    {
        index = heap_manager->free_list;
    }
    else
    {
        // Set index to the first address in the list
        index = heap_manager->free_list->start_address < heap_manager->allocated_list->start_address ? heap_manager->free_list : heap_manager->allocated_list;
    }

    int i = 1; // Index to separate various blocks
    char buffer [100];
    while (index != NULL)
    {
        // Print relevant information of current block
        sys_req(WRITE, COM1, "Memory Block ", 14);
        itoa(i, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));
        if (index->is_free == 1)
        {
            sys_req(WRITE, COM1, "\nFree Memory Block", 19);
        }
        else
        {
            sys_req(WRITE, COM1, "\nAllocated Memory Block", 24);
        }
        sys_req(WRITE, COM1, "\nStart Address: ", 17);
        itoa((int)index->start_address, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));
        sys_req(WRITE, COM1, "\nSize: ", 8);
        itoa(index->size, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));
        sys_req(WRITE, COM1, " bytes\n", 8);
        sys_req(WRITE, COM1, "-----------------------\n", 25);

        // Increment through the list
        i++;
        index = index->next;
    }
    sys_req(WRITE, COM1, "End of Memory List\n", 20);

    return;
}