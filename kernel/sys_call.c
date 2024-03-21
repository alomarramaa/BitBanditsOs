#include <stdint.h>
#include <string.h>
#include "sys_req.h"
#include <mpx/pcb.h>

// Global or static context pointer representing the initial context
static struct context *initial_context = NULL;
struct pcb *current_process;
extern struct context* sys_call(struct context *current_context);

// Define the C function for handling the ISR
struct context* sys_call(struct context *current_context) 
{

    // If first time running sys_call, set initial context
    if (initial_context == NULL)
    {
        initial_context = current_context;
    }

    if (current_context->EAX == IDLE)
    {
        process_queue* ready_q = get_ready_queue();
        if (ready_q->queue_head != NULL)
        {
            // Get first pcb in ready queue and remove it from the queue
            pcb* next_process = ready_q->queue_head;
            pcb_remove(next_process);

            // Update the stack ptr
            // Sets registers of stack starting with ESP and ending with EBP
            next_process->stackPtr = &(next_process->pcb_stack[PCB_STACK_SIZE - ESP_OFFSET]);
            *(next_process->stackPtr) = current_context->ESP;
            next_process->stackPtr -= CS_OFFSET;
            *(next_process->stackPtr) = current_context->CS;
            next_process->stackPtr -= DS_OFFSET;
            *(next_process->stackPtr) = current_context->DS;
            next_process->stackPtr -= SS_OFFSET;
            *(next_process->stackPtr) = current_context->SS;
            next_process->stackPtr -= ES_OFFSET;
            *(next_process->stackPtr) = current_context->ES;
            next_process->stackPtr -= FS_OFFSET;
            *(next_process->stackPtr) = current_context->FS;
            next_process->stackPtr -= GS_OFFSET;
            *(next_process->stackPtr) = current_context->GS;
            next_process->stackPtr -= EAX_OFFSET;
            *(next_process->stackPtr) = current_context->EAX;
            next_process->stackPtr -= EBX_OFFSET;
            *(next_process->stackPtr) = current_context->EBX;
            next_process->stackPtr -= ECX_OFFSET;
            *(next_process->stackPtr) = current_context->ECX;
            next_process->stackPtr -= EDX_OFFSET;
            *(next_process->stackPtr) = current_context->EDX;
            next_process->stackPtr -= ESI_OFFSET;
            *(next_process->stackPtr) = current_context->ESI;
            next_process->stackPtr -= EDI_OFFSET;
            *(next_process->stackPtr) = current_context->EDI;
            next_process->stackPtr -= EFLAGS_OFFSET;
            *(next_process->stackPtr) = current_context->EFLAGS;
            next_process->stackPtr -= EIP_OFFSET;
            *(next_process->stackPtr) = current_context->EIP;
            next_process->stackPtr -= EBP_OFFSET;
            *(next_process->stackPtr) = current_context->EBP;

            // Put current process back into queue and update current process variable
            pcb_insert(current_process);
            current_process = next_process;
        }

        current_context->EAX = 0;
        return current_context;
    }
    else if (current_context->EAX == EXIT)
    {
        // Attempt to delete current process
        if (pcb_free(current_process) != 0)
        {
            // Error freeing memory
            current_context->EAX = -1;
            return current_context;
        }

        process_queue* ready_q = get_ready_queue();
        if (ready_q->queue_head != NULL)
        {
            // Get first pcb in ready queue and remove it from the queue
            pcb* next_process = ready_q->queue_head;
            pcb_remove(next_process);

            // Update current process variable
            current_process = next_process;
        }
        else
        {
            // No remaining ready processes
            current_context = initial_context;
        }

        // Set return to 0
        current_context->EAX = 0;
        return current_context;
    }
    else
    {
        current_context->EAX = -1;
        return current_context;
    }
}
