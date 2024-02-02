#include <stddef.h>

struct pcb* pcb_allocate(void);

int pcb_free(struct pcb* to_freePtr);

struct pcb* pcb_setup(const char* process_name, int process_class, int process_priority);

struct pcb* pcb_find(const char* to_find);

void pcb_insert(struct pcb* to_insertPtr);

int pcb_remove(struct pcb* to_removePtr);