#include <mpx/io.h>
#include <mpx/serial.h>
#include "sys_req.h"
#include <memory.h>
#include <string.h>
#include <mpx/pcb.h>

process_queue ready_queue;

process_queue blocked_queue;

process_queue sus_ready_queue;

process_queue sus_blocked_queue;

struct pcb* search_queue(char* to_find, enum queue_tag queue_sel)
{
    process_queue* selected_queue;
    if (queue_sel == SUS_READY_QUEUE_TAG)
    {
        selected_queue = &sus_ready_queue;
    }
    else if (queue_sel == READY_QUEUE_TAG)
    {
        selected_queue = &ready_queue;
    }
    else if (queue_sel == SUS_BLOCKED_QUEUE_TAG)
    {
        selected_queue = &sus_blocked_queue;
    }
    else if (queue_sel == BLOCKED_QUEUE_TAG)
    {
        selected_queue = &blocked_queue;
    }
    else
    {
        return NULL;
    }

    // Queue is empty
    if (selected_queue->queue_head == NULL)
    {
        return NULL;
    }
    
    pcb* index = selected_queue->queue_head;
    // Traverse through the queue until at the last item
    while (index->next_pcbPtr != NULL)
    {
        // If the index is at the pcb to find, return the pointer to the pcb
        if (strcmp(to_find, index->process_name) == 0)
        {
            return index;
        }
        index = index->next_pcbPtr;
    }

    //Check the last item of the queue
    if (strcmp(to_find, index->process_name) == 0)
    {
        return index;
    }

    // PCB not found
    return NULL;
}

void enqueue(pcb* to_add, enum queue_tag queue_sel)
{
    process_queue* selected_queue;
    if (queue_sel == SUS_READY_QUEUE_TAG)
    {
        selected_queue = &sus_ready_queue;
    }
    else if (queue_sel == READY_QUEUE_TAG)
    {
        selected_queue = &ready_queue;
    }
    else if (queue_sel == SUS_BLOCKED_QUEUE_TAG)
    {
        selected_queue = &sus_blocked_queue;
    }
    else if (queue_sel == BLOCKED_QUEUE_TAG)
    {
        selected_queue = &blocked_queue;
    }
    else
    {
        return;
    }

    // A Ready Queue (Priority Matters), Highest priority first, where processes of the same priority were FIFO
    if (queue_sel == SUS_READY_QUEUE_TAG || queue_sel == READY_QUEUE_TAG)
    {
        // Queue is empty
        if (selected_queue->queue_head == NULL)
        {
            selected_queue->queue_head = to_add;
            selected_queue->queue_tail = to_add;
            return;
        }

        // New PCB is Highest Priority in Queue
        if (to_add->process_priority < selected_queue->queue_head->process_priority)
        {
            selected_queue->queue_head->prev_pcbPtr = to_add;
            to_add->next_pcbPtr = selected_queue->queue_head;
            selected_queue->queue_head = to_add;
            return;
        }

        // New PCB is Lowest Priority in Queue
        if (to_add->process_priority >= selected_queue->queue_tail->process_priority)
        {
            selected_queue->queue_tail->next_pcbPtr = to_add;
            to_add->prev_pcbPtr = selected_queue->queue_tail;
            selected_queue->queue_tail = to_add;
            return;
        }

        pcb* index;
        // New PCB is in First Half of Queue
        if (to_add->process_priority <= 4)
        {
            index = selected_queue->queue_head;
            while (index->next_pcbPtr->process_priority <= to_add->process_priority)
            {
                index = index->next_pcbPtr;
            }
        }
        // New PCB is in Second Half of Queue
        else
        {
            index = selected_queue->queue_tail;
            while (index->prev_pcbPtr->process_priority > to_add->process_priority)
            {
                index = index->prev_pcbPtr;
            }
            index = index->prev_pcbPtr;
        }

        to_add->next_pcbPtr = index->next_pcbPtr;
        index->next_pcbPtr->prev_pcbPtr = to_add;
        index->next_pcbPtr = to_add;
        to_add->prev_pcbPtr = index;
        return;
    }
    // A Blocked Queue // FIFO
    else
    {
        if (selected_queue->queue_head == NULL)
        {
            selected_queue->queue_head = to_add;
            selected_queue->queue_tail = to_add;
            return;
        }

        else
        {
            selected_queue->queue_tail->next_pcbPtr = to_add;
            to_add->prev_pcbPtr = selected_queue->queue_tail;
            selected_queue->queue_tail = to_add;
            return;
        }
    }

    return;
}

int dequeue(pcb* to_remove, enum queue_tag queue_sel)
{
    process_queue* selected_queue;
    if (queue_sel == SUS_READY_QUEUE_TAG)
    {
        selected_queue = &sus_ready_queue;
    }
    else if (queue_sel == READY_QUEUE_TAG)
    {
        selected_queue = &ready_queue;
    }
    else if (queue_sel == SUS_BLOCKED_QUEUE_TAG)
    {
        selected_queue = &sus_blocked_queue;
    }
    else if (queue_sel == BLOCKED_QUEUE_TAG)
    {
        selected_queue = &blocked_queue;
    }
    else
    {
        const char* queueMessage = "Dequeue: Invalid queue selection";
        sys_req(WRITE, COM1, queueMessage, strlen(queueMessage));
        return 1;
    }

    // PCB Not in the Queue
    if (search_queue(to_remove->process_name, queue_sel) == NULL)
    {
        const char* searchMessage = "Dequeue: PCB not in the queue";
        sys_req(WRITE, COM1, searchMessage, strlen(searchMessage));
        return 1;
    }

    // PCB is Only Item in Queue
    if (selected_queue->queue_head == to_remove && selected_queue->queue_tail == to_remove)
    {
        selected_queue->queue_head = NULL;
        selected_queue->queue_tail = NULL;
        to_remove->prev_pcbPtr = NULL;
        to_remove->next_pcbPtr = NULL;
        return 0;
    }

    // PCB is Head of Queue
    if (selected_queue->queue_head == to_remove)
    {
        selected_queue->queue_head = to_remove->next_pcbPtr;
        to_remove->next_pcbPtr->prev_pcbPtr = NULL;
        to_remove->next_pcbPtr = NULL;
        return 0;
    }

    // PCB is Tail of Queue
    if (selected_queue->queue_tail == to_remove)
    {
        selected_queue->queue_tail = to_remove->prev_pcbPtr;
        to_remove->prev_pcbPtr->next_pcbPtr = NULL;
        to_remove->prev_pcbPtr = NULL;
        return 0;
    }

    // PCB is Elsewhere in Queue
    to_remove->prev_pcbPtr->next_pcbPtr = to_remove->next_pcbPtr;
    to_remove->next_pcbPtr->prev_pcbPtr = to_remove->prev_pcbPtr;
    to_remove->prev_pcbPtr = NULL;
    to_remove->next_pcbPtr = NULL;
    return 0;
}

struct pcb* pcb_allocate(void)
{
    // Allocate memory and ensure it is properly allocated
    pcb* new_pcbPtr = sys_alloc_mem(sizeof(pcb));
    if (new_pcbPtr == NULL)
    {
        return NULL;
    }

    memset(new_pcbPtr->pcb_stack, 0, sizeof(new_pcbPtr->pcb_stack));
    new_pcbPtr->stackPtr = new_pcbPtr->pcb_stack+PCB_STACK_SIZE-sizeof(struct context);
    new_pcbPtr->next_pcbPtr = NULL;
    new_pcbPtr->prev_pcbPtr = NULL;

    return new_pcbPtr;
}

int pcb_free(struct pcb* to_freePtr)
{
    int frees_name = sys_free_mem(to_freePtr->process_name);
    if (frees_name != 0)
    {
        return frees_name;
    }
    return sys_free_mem(to_freePtr);
}

struct pcb* pcb_setup(char* new_process_name, class_type new_process_class, int new_process_priority)
{
    // Allocate memory for the pcb and ensure proper allocation
    pcb* new_pcbPtr = pcb_allocate();
    if (new_pcbPtr == NULL)
    {
        return NULL;
    }

    // Allocate memory for the process name
    new_pcbPtr->process_name = (char*)sys_alloc_mem(strlen(new_process_name) + 1);
    if (new_pcbPtr->process_name == NULL)
    {
        sys_free_mem(new_pcbPtr);
        return NULL;
    }

    // Copy the process name into the allocated memory
    int i = 0;
    while (new_process_name[i] != '\0')
    {
        new_pcbPtr->process_name[i] = new_process_name[i];
        i++;
    }
    new_pcbPtr->process_name[i] = '\0'; // Null-terminate the string

    // Finalize PCB initialization
    new_pcbPtr->process_class = new_process_class;
    new_pcbPtr->process_priority = new_process_priority;
    new_pcbPtr->exe_state = READY;
    new_pcbPtr->disp_state = NOT_SUSPENDED;

    return new_pcbPtr;
}

struct pcb* pcb_find(char* to_find)
{
    // Ensure given a valid name
    if (to_find == NULL || strlen(to_find) == 0)
    {
        return NULL;
    }

    pcb* found_pcb;

    // Traverse through the four queues
    for(int i = 0; i <= 3; i++)
    {
        // Check if PCB is in current queue
        found_pcb = search_queue(to_find, (enum queue_tag) i);
        if (found_pcb != NULL)
        {
            return found_pcb;
        }
    }

    // PCB not found in any queue
    return NULL;
}

int pcb_insert(struct pcb* to_insertPtr)
{
    // Ensure given valid PCB
    if (to_insertPtr == NULL)
    {
        return 1;
    }

    // Check which queue to add to
    queue_tag queue_sel = to_insertPtr->disp_state + to_insertPtr->exe_state;

    // Add to appropriate queue
    enqueue(to_insertPtr, queue_sel);
    
    return 0;
}

int pcb_remove(struct pcb* to_removePtr)
{
    // Ensure given valid PCB
    if (to_removePtr == NULL)
    {
        return 1;
    }

    // Check which queue to remove from
    queue_tag queue_sel = to_removePtr->disp_state + to_removePtr->exe_state;

    // Attempt to remove the PCB
    if (dequeue(to_removePtr, queue_sel) == 1)
    {
        // Unable to remove PCB
        return 1;
    }

    // PCB removed successfully
    return 0;
}

struct process_queue* get_ready_queue(void)
{
    return &ready_queue;
}

struct process_queue* get_blocked_queue(void)
{
    return &blocked_queue;
}

struct process_queue* get_sus_ready_queue(void)
{
    return &sus_ready_queue;
}

struct process_queue* get_sus_blocked_queue(void)
{
    return &sus_blocked_queue;
}
