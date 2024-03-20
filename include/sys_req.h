#ifndef MPX_SYS_REQ_H
#define MPX_SYS_REQ_H

#include <mpx/device.h>
#include <stdlib.h>

/**
 @file sys_req.h
 @brief System request function and constants
*/

// Define the context struct
// Defined using bitfields modeled after the given site
// https://flint.cs.yale.edu/cs422/doc/pc-arch.html
typedef struct context {
	// Status and control registers (also CS from Segment registers)
	int EFLAGS : 31;
	int CS : 15;
	int EIP : 31;

	// Segment registers (remaining)
	int DS : 15;
	int SS : 15;
	int ES : 15;
	int FS : 15;
	int GS : 15;

	// General-purpose registers
	int EAX : 31;
	int EBX : 31;
	int ECX : 31;
	int EDX : 31;
	int ESI : 31;
	int EDI : 31;
	int EBP : 31;
	int ESP : 31;
} context;

// Global PCB pointer representing the currently executing process
struct pcb *current_process;

typedef enum op_code{
	EXIT,
	IDLE,
	READ,
	WRITE,
} op_code;
    
// error codes
#define INVALID_OPERATION	(-1)
#define INVALID_BUFFER		(-2)
#define INVALID_COUNT		(-3)

extern struct context* sys_call(struct context *current_context);

/**
 Request an MPX kernel operation.
 @param op_code One of READ, WRITE, IDLE, or EXIT
 @param ... As required for READ or WRITE
 @return Varies by operation
*/ 
int sys_req(op_code op, ...);
 
#endif
