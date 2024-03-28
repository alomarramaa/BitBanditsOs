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
typedef struct context
{
	// Segment registers (remaining)
	int SS;
	int GS;
	int FS;
	int ES;
	int DS;

	// General-purpose registers
	int EDI;
	int ESI;
	int EBP;
	int ESP;
	int EBX;
	int EDX;
	int ECX;
	int EAX;

	// Status and control registers (also CS from Segment registers)
	int EIP;
	int CS;
	int EFLAGS;

} context;

// Global PCB pointer representing the currently executing process
//extern struct pcb *current_process;

typedef enum op_code
{
	EXIT,
	IDLE,
	READ,
	WRITE,
} op_code;

// error codes
#define INVALID_OPERATION (-1)
#define INVALID_BUFFER (-2)
#define INVALID_COUNT (-3)

struct context *sys_call(struct context *current_context);

/**
 Request an MPX kernel operation.
 @param op_code One of READ, WRITE, IDLE, or EXIT
 @param ... As required for READ or WRITE
 @return Varies by operation
*/
int sys_req(op_code op, ...);

#endif
