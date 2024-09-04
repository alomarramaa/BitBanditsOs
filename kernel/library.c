#include <mpx/library.h>
#include <stddef.h>
#include <mpx/vm.h>
#include <string.h>
#include "sys_req.h"

struct HeapManager* heap_manager = &hm;

// Function to initialize the heap manager
void initialize_heap(size_t heap_size) 
{
    // Allocate memory for the heap using kmalloc()
    void *heap_memory = kmalloc(heap_size, 0, NULL); 
    if (heap_memory == NULL)
    {
        return;
    }

    // Create an MCB for the heap block
    MCB *heap_mcb = (MCB *)heap_memory;
    heap_mcb->start_address = (char *)heap_memory + sizeof(MCB); // Skip MCB itself
    heap_mcb->size = heap_size - sizeof(MCB);
    heap_mcb->next = NULL;
    heap_mcb->prev = NULL;
    heap_mcb->rel_next = NULL;
    heap_mcb->rel_prev = NULL;
    heap_mcb->is_free = 1; // Initially, the entire block is free

    // Add the MCB to the free list
    heap_manager->free_list = heap_mcb;
    
    // Initialize the allocated list to be empty
    heap_manager->allocated_list = NULL;
}

// Function to allocate memory from the heap
void *allocate_memory(size_t size)
{
    // Traverse the free list to find the first free block with sufficient size
    MCB *curr_block = heap_manager->free_list;

    while (curr_block != NULL) 
    {
        if (curr_block->is_free && curr_block->size >= size) 
        {
            // Allocate from this block
            if (curr_block->size > size + sizeof(MCB))
            {
                // Split the block into two if necessary
                MCB *new_block = (MCB *)((char *)curr_block->start_address + size);
                new_block->start_address = (char *)curr_block->start_address + size + sizeof(MCB);
                new_block->size = curr_block->size - size - sizeof(MCB);

                // Add new block into overall list after current block
                new_block->next = curr_block->next;
                new_block->prev = curr_block;
                if (curr_block->next != NULL)
                {
                    curr_block->next->prev = new_block;
                }
                curr_block->next = new_block;
                
                // Insert the new block into the free list in place of the block
                // being allocated
                new_block->rel_next = curr_block->rel_next;
                new_block->rel_prev = curr_block->rel_prev;
                if (curr_block->rel_next != NULL)
                {
                    curr_block->rel_next->rel_prev = new_block;
                }
                if (curr_block->rel_prev != NULL)
                {
                    curr_block->rel_prev->rel_next = new_block;
                }
                else
                {
                    //Curr block is head of free list
                    heap_manager->free_list = new_block;
                }

                curr_block->size = size;
                new_block->is_free = 1;
            }
            else
            {
                // Remove block from free list
                if (curr_block->rel_prev == NULL)
                {
                    // Current block is the head of the free list
                    heap_manager->free_list = curr_block->rel_next;
                }
                else
                {
                    curr_block->rel_prev->rel_next = curr_block->rel_next;
                }
                
                if (curr_block->rel_next != NULL)
                {
                    curr_block->rel_next->rel_prev = curr_block->rel_prev;
                }
            }

            // Add the allocated block to the allocated list
            if (heap_manager->allocated_list == NULL)
            {
                heap_manager->allocated_list = curr_block;
                curr_block->rel_next = NULL;
                curr_block->rel_prev = NULL;
                curr_block->is_free = 0;
            }
            else
            {
                MCB* allocated_index = heap_manager->allocated_list;
                if (allocated_index->start_address > curr_block->start_address)
                {
                    curr_block->rel_next = allocated_index;
                    allocated_index->rel_prev = curr_block;
                    heap_manager->allocated_list = curr_block;
                    curr_block->is_free = 0;
                }
                else
                {
                    while (allocated_index->rel_next != NULL)
                    {
                        if (allocated_index->rel_next->start_address > curr_block->start_address)
                        {
                            allocated_index->rel_next->rel_prev = curr_block;
                            curr_block->rel_next = allocated_index->rel_next;
                            curr_block->rel_prev = allocated_index;
                            allocated_index->rel_next = curr_block;

                            curr_block->is_free = 0;
                            break;
                        }

                        allocated_index = allocated_index->rel_next;
                    }

                    if (curr_block->is_free)
                    {
                        allocated_index->rel_next = curr_block;
                        curr_block->rel_prev = allocated_index;
                        curr_block->rel_next = NULL;
                        curr_block->is_free = 0;
                    }
                }
            }

            // Return the start address of the allocated block
            return curr_block->start_address;
        }
        curr_block = curr_block->next;
    }

    return NULL;
}
/*
 Steps: 
1. Remove it from allocated list
2. Set it to free
3. Insert into free list
4. Merge contiguous free blocks of memory*/
int free_memory(void *ptr)
{
    // Find the MCB associated with the provided memory address
    MCB *curr_block = heap_manager->allocated_list;
    while (curr_block != NULL)
    {
        if (curr_block->start_address == ptr)
        {
            // Remove the freed block from the allocated list
            if (curr_block->rel_prev != NULL)
            {
                curr_block->rel_prev->rel_next = curr_block->rel_next;
            }
            else
            {
                heap_manager->allocated_list = curr_block->rel_next;
            }
            if (curr_block->rel_next != NULL)
            {
                curr_block->rel_next->rel_prev = curr_block->rel_prev;
            }
            curr_block->rel_next = NULL;
            curr_block->rel_prev = NULL;

            // Set block as free
            curr_block->is_free = 1;

            // Add to free list
            MCB* index = heap_manager->free_list;
            if (index == NULL || index->start_address > curr_block->start_address)
            {
                // Becomes head of free list
                heap_manager->free_list = curr_block;
                if (index != NULL)
                {
                    curr_block->rel_next = index;
                    index->rel_prev = curr_block;
                }
            }
            else
            {
                while (index != NULL)
                {
                    if (index->start_address > curr_block->start_address)
                    {
                        index->rel_prev->rel_next = curr_block;
                        curr_block->rel_prev = index->rel_prev;
                        curr_block->rel_next = index;
                        index->rel_prev = curr_block;
                        break;
                    }
                    else if (index->rel_next == NULL)
                    {
                        index->rel_next = curr_block;
                        curr_block->rel_prev = index;
                        break;
                    }

                    index = index->rel_next;
                }
            }
            
            // Merge adjacent free blocks if necessary
            if (curr_block->next != NULL && curr_block->next->is_free)
            {
                // Merge with next block
                curr_block->size += curr_block->next->size;

                curr_block->rel_next = curr_block->next->rel_next;
                if (curr_block->next->rel_next != NULL)
                {
                    curr_block->next->rel_next->rel_prev = curr_block;
                }
                curr_block->next->rel_next = NULL;
                curr_block->next->rel_prev = NULL;

                MCB* temp = curr_block->next;
                if (curr_block->next->next != NULL)
                {
                    curr_block->next->next->prev = curr_block;
                }
                curr_block->next = curr_block->next->next;
                temp->next = NULL;
                temp->prev = NULL;
            }
            if (curr_block->prev != NULL && curr_block->prev->is_free)
            {
                // Merge with previous block
                curr_block = curr_block->prev;
                curr_block->size += curr_block->next->size;

                curr_block->rel_next = curr_block->next->rel_next;
                if (curr_block->next->rel_next != NULL)
                {
                    curr_block->next->rel_next->rel_prev = curr_block;
                }
                curr_block->next->rel_next = NULL;
                curr_block->next->rel_prev = NULL;

                MCB* temp = curr_block->next;
                if (curr_block->next->next != NULL)
                {
                    curr_block->next->next->prev = curr_block;
                }
                curr_block->next = curr_block->next->next;
                temp->next = NULL;
                temp->prev = NULL;
            }

            // Return success
            return 0;
        }
        curr_block = curr_block->next;
    }
    return -1;
}
