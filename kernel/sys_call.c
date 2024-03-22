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
            if (current_process != NULL)
            {
                current_process->stackPtr = &(next_process->pcb_stack[PCB_STACK_SIZE - ESP_OFFSET]);
            }

            // Put current process back into queue and update current process variable
            pcb_insert(current_process);
            current_process = next_process;

            current_process->stackPtr = &(next_process->pcb_stack[PCB_STACK_SIZE - ESP_OFFSET]);
            current_context->ESP = *(current_process->stackPtr);
            current_process->stackPtr -= CS_OFFSET;
            current_context->CS = *(current_process->stackPtr);
            current_process->stackPtr -= DS_OFFSET;
            current_context->DS = *(current_process->stackPtr);
            current_process->stackPtr -= SS_OFFSET;
            current_context->SS = *(current_process->stackPtr);
            current_process->stackPtr -= ES_OFFSET;
            current_context->ES = *(current_process->stackPtr);
            current_process->stackPtr -= FS_OFFSET;
            current_context->FS = *(current_process->stackPtr);
            current_process->stackPtr -= GS_OFFSET;
            current_context->GS = *(current_process->stackPtr);
            current_process->stackPtr -= EAX_OFFSET;
            current_context->EAX = *(current_process->stackPtr);
            current_process->stackPtr -= EBX_OFFSET;
            current_context->EBX = *(current_process->stackPtr);
            current_process->stackPtr -= ECX_OFFSET;
            current_context->ECX = *(current_process->stackPtr);
            current_process->stackPtr -= EDX_OFFSET;
            current_context->EDX = *(current_process->stackPtr);
            current_process->stackPtr -= ESI_OFFSET;
            current_context->ESI = *(current_process->stackPtr);
            current_process->stackPtr -= EDI_OFFSET;
            current_context->EDI = *(current_process->stackPtr);
            current_process->stackPtr -= EFLAGS_OFFSET;
            current_context->EFLAGS = *(current_process->stackPtr);
            current_process->stackPtr -= EIP_OFFSET;
            current_context->EIP = *(current_process->stackPtr);
            current_process->stackPtr -= EBP_OFFSET;
            current_context->EBP = *(current_process->stackPtr);
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

            current_process->stackPtr = &(next_process->pcb_stack[PCB_STACK_SIZE - ESP_OFFSET]);
            current_context->ESP = *(current_process->stackPtr);
            current_process->stackPtr -= CS_OFFSET;
            current_context->CS = *(current_process->stackPtr);
            current_process->stackPtr -= DS_OFFSET;
            current_context->DS = *(current_process->stackPtr);
            current_process->stackPtr -= SS_OFFSET;
            current_context->SS = *(current_process->stackPtr);
            current_process->stackPtr -= ES_OFFSET;
            current_context->ES = *(current_process->stackPtr);
            current_process->stackPtr -= FS_OFFSET;
            current_context->FS = *(current_process->stackPtr);
            current_process->stackPtr -= GS_OFFSET;
            current_context->GS = *(current_process->stackPtr);
            current_process->stackPtr -= EAX_OFFSET;
            current_context->EAX = *(current_process->stackPtr);
            current_process->stackPtr -= EBX_OFFSET;
            current_context->EBX = *(current_process->stackPtr);
            current_process->stackPtr -= ECX_OFFSET;
            current_context->ECX = *(current_process->stackPtr);
            current_process->stackPtr -= EDX_OFFSET;
            current_context->EDX = *(current_process->stackPtr);
            current_process->stackPtr -= ESI_OFFSET;
            current_context->ESI = *(current_process->stackPtr);
            current_process->stackPtr -= EDI_OFFSET;
            current_context->EDI = *(current_process->stackPtr);
            current_process->stackPtr -= EFLAGS_OFFSET;
            current_context->EFLAGS = *(current_process->stackPtr);
            current_process->stackPtr -= EIP_OFFSET;
            current_context->EIP = *(current_process->stackPtr);
            current_process->stackPtr -= EBP_OFFSET;
            current_context->EBP = *(current_process->stackPtr);
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
