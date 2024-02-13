#include <stddef.h>

struct pcb* search_queue(char* to_find, enum queue_tag queue_sel);

void enqueue(pcb* to_add, enum queue_tag queue_sel);

int dequeue(pcb* to_remove, enum queue_tag queue_sel);

struct pcb* pcb_allocate(void);

int pcb_free(struct pcb* to_freePtr);

struct pcb* pcb_setup(const char* process_name, int process_class, int process_priority);

struct pcb* pcb_find(const char* to_find);

void pcb_insert(struct pcb* to_insertPtr);

int pcb_remove(struct pcb* to_removePtr);