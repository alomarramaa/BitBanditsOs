#ifndef _COMHAND_H
#define _COMHAND_H

#include <string.h>
#include "sys_req.h"
#include "date.h"
#include "time.h"

static struct HeapManager hm;

void yield(void);

void r3_load_pcb(void (*proc_function)(void), char *proc_name, int proc_priority);

void create_comhand_proc(void);

void create_system_idle_proc(void);

void load_proc(void (*proc_function)(void), char *proc_name, int proc_priority);

void comhand(void);

#endif
