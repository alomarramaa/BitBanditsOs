#ifndef _COMHAND_H
#define _COMHAND_H

#include <string.h>
#include "sys_req.h"
#include "date.h"
#include "time.h"

void yield(void);

void r3_load_pcb(void (*proc_function)(void), char *proc_name, int proc_priority);

void create_proc(void);
void load_create_proc(void (*proc_function)(void), char *proc_name, int proc_priority);



void comhand(void);

#endif
