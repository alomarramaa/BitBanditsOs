#ifndef PCB_H
#define PCB_H

#include <stddef.h>

typedef enum queue_tag
{
    SUS_READY_QUEUE_TAG = 0,
    READY_QUEUE_TAG = 1,
    SUS_BLOCKED_QUEUE_TAG = 2,
    BLOCKED_QUEUE_TAG = 3,
}queue_tag;

typedef enum execution_state 
{
    READY = 0,
    RUNNING = 1,
    BLOCKED = 2,
} execution_state;

typedef enum dispatching_state 
{
    SUSPENDED = 0,
    NOT_SUSPENDED = 1,
} dispatching_state;

typedef enum class_type
{
    SYSTEM = 0,
    USER = 1,
} class_type;

typedef struct pcb 
{
    char* process_name;
    class_type process_class;
    int process_priority;
    execution_state exe_state;
    dispatching_state disp_state;
    // Stack - array all initialized to 0, at least 1024 byte
    // Not sure if I'm doing this right
    int pcb_stack[1024];
    // Stack pointer
    // Or this
    int* stackPtr;
    struct pcb* next_pcbPtr;
    struct pcb* prev_pcbPtr;
} pcb;

typedef struct process_queue 
{
    struct pcb* queue_head;
    struct pcb* queue_tail;
} process_queue;

struct pcb* pcb_allocate(void);

int pcb_free(struct pcb* to_freePtr);

struct pcb* pcb_setup(char* process_name, class_type process_class, int process_priority);

struct pcb* pcb_find(char* to_find);

int pcb_insert(struct pcb* to_insertPtr);

int pcb_remove(struct pcb* to_removePtr);

int create_pcb(void);
int delete_pcb(void);
int block_pcb(void);
int unblock_pcb(void);
int suspend_pcb(void);
int resume_pcb(void);
int set_pcb_priority(void);
int show_pcb(void);
void show_ready(void);
void show_blocked(void);
void show_all(void);



struct pcb* search_queue(char* to_find, enum queue_tag queue_sel);

void enqueue(pcb* to_add, enum queue_tag queue_sel);

int dequeue(pcb* to_remove, enum queue_tag queue_sel);

#endif
