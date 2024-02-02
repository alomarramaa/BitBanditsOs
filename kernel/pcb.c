#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>

typedef enum execution_state 
{
    READY = 0,
    RUNNIG = 1,
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
    // Stack
    // Stack pointer
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


    return NULL;
}

int pcb_free(struct pcb* to_freePtr)
{


    return 0;
}

struct pcb* pcb_setup(const char* process_name, int process_class, int process_priority)
{


    return NULL;
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