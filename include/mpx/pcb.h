#ifndef PCB_H
#define PCB_H

#include <stddef.h>
#include "sys_req.h"

// Area of 1024 bytes and keeps track of PCB stack top and base
#define PCB_STACK_SIZE (1024)

typedef enum queue_tag
{
    SUS_READY_QUEUE_TAG = 0,
    READY_QUEUE_TAG = 1,
    SUS_BLOCKED_QUEUE_TAG = 2,
    BLOCKED_QUEUE_TAG = 3,
} queue_tag;

// Process states (Execution)
typedef enum execution_state
{
    READY = 0,
    RUNNING = 1,
    BLOCKED = 2,
} execution_state;

// Process states (Dispatching)
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
    char *process_name;       //(has to be at least 8 bytes)
    class_type process_class; // user (1) or system (0)
    int process_priority;     // 0 - 9
    execution_state exe_state;
    dispatching_state disp_state;
    // Stack - Saves the process's state when it is interrupted
    int pcb_stack[PCB_STACK_SIZE];
    int *stackPtr;
    struct pcb *next_pcbPtr;
    struct pcb *prev_pcbPtr;
} pcb;

typedef struct process_queue
{
    struct pcb *queue_head;
    struct pcb *queue_tail;
} process_queue;

struct pcb *pcb_allocate(void);

int pcb_free(struct pcb *to_freePtr);

void pcb_stack_init(struct pcb *to_init);

struct pcb *pcb_setup(char *new_process_name, class_type new_process_class, int new_process_priority);

struct pcb *pcb_find(char *to_find);

int pcb_insert(struct pcb *to_insertPtr);

int pcb_remove(struct pcb *to_removePtr);

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

struct pcb *search_queue(char *to_find, enum queue_tag queue_sel);

void enqueue(pcb *to_add, enum queue_tag queue_sel);

int dequeue(pcb *to_remove, enum queue_tag queue_sel);

struct process_queue *get_ready_queue(void);

struct process_queue *get_blocked_queue(void);

struct process_queue *get_sus_ready_queue(void);

struct process_queue *get_sus_blocked_queue(void);

#endif
