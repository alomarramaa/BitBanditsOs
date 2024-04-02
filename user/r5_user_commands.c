
#include <user/r5_user_commands.h>

/*Allocates heap memory by calling allocate memory() and prints (in hexadecimal) the address of the newly allocated block (not the MCB address), or an error message if allocation fails
• Parameters:
• The size of the allocation request (in decimal)*/
void allocate_memory(void)
{
}

/*Frees heap memory by calling free memory() • Prints an error message if freeing fails
• Parameters:
• The address of the memory block (not MCB) to free (in hexadecimal)*/
void free_memory(void)
{
}

/*
 Each command walks through the corresponding list, printing information for each block of memory
• Information needs to include:
• The start address of the block (not the MCB address) (in hexadecimal) • The size of the block (in decimal)*/
void show_allocated_memory(void)
{
}

void show_free_memory(void)
{
}
