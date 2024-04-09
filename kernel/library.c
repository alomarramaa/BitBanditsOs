#include <mpx/library.h>
#include <stddef.h>
#include <mpx/vm.h>
#include <string.h>
#include "sys_req.h"

// Function to initialize the heap manager
void initialize_heap(HeapManager *heap_manager, size_t heap_size) {
    // Allocate memory for the heap using kmalloc()
    void *heap_memory = kmalloc(heap_size, 0, NULL);
    if (heap_memory == NULL) {
        // Handle error: unable to allocate memory for the heap
        sys_req(WRITE, COM1, "Error: Unable to allocate memory for the heap.\n", strlen("Error: Unable to allocate memory for the heap.\n"));
        return;
    }
    
    // Create an MCB for the heap block
    MCB *heap_mcb = (MCB *)heap_memory;
    heap_mcb->start_address = (char*)heap_memory + sizeof(MCB); // Skip MCB itself
    heap_mcb->size = heap_size - sizeof(MCB);
    heap_mcb->next = NULL;
    heap_mcb->prev = NULL;
    heap_mcb->is_free = 1; // Initially, the entire block is free
    
    // Add the MCB to the free list
    heap_manager->free_list = heap_mcb;
    
    // Initialize the allocated list to be empty
    heap_manager->allocated_list = NULL;
}

// Function to allocate memory from the heap
void *allocate_memory(HeapManager *heap_manager, size_t size) {
    // Traverse the free list to find the first free block with sufficient size
    MCB *curr_block = heap_manager->free_list;
    while (curr_block != NULL) {
        if (curr_block->is_free && curr_block->size >= size) {
            // Allocate from this block
            if (curr_block->size > size + sizeof(MCB)) {
                // Split the block into two if necessary
                MCB *new_block = (MCB *)((char *)curr_block->start_address + size);
                new_block->start_address = (char *)curr_block->start_address + size + sizeof(MCB);
                new_block->size = curr_block->size - size - sizeof(MCB);
                new_block->next = curr_block->next;
                new_block->prev = curr_block;
                new_block->is_free = 1;
                
                if (curr_block->next != NULL) {
                    curr_block->next->prev = new_block;
                }
                curr_block->next = new_block;
                curr_block->size = size;
            }
            curr_block->is_free = 0; // Mark the block as allocated
            
            // Remove the allocated block from the free list
            if (curr_block->prev != NULL) {
                curr_block->prev->next = curr_block->next;
            } else {
                heap_manager->free_list = curr_block->next;
            }
            if (curr_block->next != NULL) {
                curr_block->next->prev = curr_block->prev;
            }
            
            // Add the allocated block to the allocated list
            curr_block->next = heap_manager->allocated_list;
            heap_manager->allocated_list = curr_block;
            
            // Return the start address of the allocated block
            return curr_block->start_address;
        }
        curr_block = curr_block->next;
    }
    
    // If no suitable block found, return NULL
    sys_req(WRITE, COM1, "Error: No suitable block found.\n", strlen("Error: No suitable block found.\n"));
    return NULL;
}

int free_memory(HeapManager *heap_manager, void *ptr) {
    // Find the MCB associated with the provided memory address
    MCB *curr_block = heap_manager->allocated_list;
    while (curr_block != NULL) {
        if (curr_block->start_address == ptr) {
            // Mark the block as free
            curr_block->is_free = 1;
            
            // Add the freed block to the free list
            curr_block->next = heap_manager->free_list;
            curr_block->prev = NULL;
            heap_manager->free_list = curr_block;
            
            // Merge adjacent free blocks if necessary
            if (curr_block->prev != NULL && curr_block->prev->is_free) {
                // Merge with previous block
                curr_block->prev->size += curr_block->size + sizeof(MCB);
                curr_block->prev->next = curr_block->next;
                
                if (curr_block->next != NULL) {
                    curr_block->next->prev = curr_block->prev;
                }
                
                curr_block = curr_block->prev;
            }
            if (curr_block->next != NULL && curr_block->next->is_free) {
                // Merge with next block
                curr_block->size += curr_block->next->size + sizeof(MCB);
                curr_block->next = curr_block->next->next;
                
                if (curr_block->next != NULL) {
                    curr_block->next->prev = curr_block;
                }
            }
            
            // Remove the freed block from the allocated list
            if (curr_block->prev != NULL) {
                curr_block->prev->next = curr_block->next;
            } else {
                heap_manager->allocated_list = curr_block->next;
            }
            if (curr_block->next != NULL) {
                curr_block->next->prev = curr_block->prev;
            }
            
            // Return success
            return 0;
        }
        curr_block = curr_block->next;
    }
    
    // If the provided memory address is not found in the allocated list, return error
    sys_req(WRITE, COM1, "Error: Provided memory address is not found.\n", strlen("Error: Provided memory address is not found.\n"));
    return -1;
}
