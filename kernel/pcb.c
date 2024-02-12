#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>
#include <memory.h>
#include <string.h>

typedef enum execution_state 
{
    READY = 0,
    RUNNING = 1,
    BLOCKED = 2,
}execution_state;

typedef enum dispatching_state 
{
    SUSPENDED = 0,
    NOTSUS = 1,
}dispatching_state;

typedef struct pcb 
{
    char* process_name;
    char* process_class;
    int process_priority;
    execution_state exe_state;
    dispatching_state disp_state;
    // Stack - array all initialized to 0, at least 1024 byte
    // Not sure if I'm doing this right
    int pcb_stack[1024];
    // Stack pointer
    // Or this
    int* stackPtr;
    pcb* next_pcbPtr;
    pcb* prev_pcbPtr;
} pcb;

typedef struct process_queue 
{
    pcb* queue_head;
    pcb* queue_tail;
} process_queue;


struct pcb* pcb_allocate(void)
{
    pcb* new_pcbPtr = sys_alloc_mem(sizeof(pcb));
    if (new_pcbPtr == NULL)
    {
        return NULL;
    }

    new_pcbPtr->process_name = NULL;
    new_pcbPtr->process_class = NULL;
    new_pcbPtr->process_priority = -1;
    new_pcbPtr->exe_state = 2;
    new_pcbPtr->disp_state = 0;
    // new_pcbPtr->pcb_stack = {0}; Don't remember how to do this (set all to 0)
    new_pcbPtr->stackPtr = &(new_pcbPtr->pcb_stack[1022]);
    new_pcbPtr->next_pcbPtr = NULL;
    new_pcbPtr->prev_pcbPtr = NULL;


    return new_pcbPtr;
}

int pcb_free(struct pcb* to_freePtr)
{
    return sys_free_mem(to_freePtr);
}

struct pcb* pcb_setup(const char* process_name, int process_class, int process_priority)
{
    if ((process_priority < 0 || process_priority > 9) ||
        (process_priority == 0 && strcmp(process_class, "system") != 0))
    {
        return NULL;
    }

    pcb* new_pcbPtr = pcb_allocate();
    if (new_pcbPtr == NULL)
    {
        return NULL;
    }

    new_pcbPtr->process_name = process_name;
    new_pcbPtr->process_class = process_class;
    new_pcbPtr->process_priority = process_priority;
    new_pcbPtr->exe_state = 0;
    new_pcbPtr->disp_state = 1;

    return new_pcbPtr;
}

struct pcb* pcb_find(const char* to_find)
{
    

    return NULL;
}

void pcb_insert(struct pcb* to_insertPtr)
{


    return;
}

int pcb_remove(struct pcb* to_removePtr)
{


    return 0;
}