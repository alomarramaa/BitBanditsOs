#include <stdint.h>
#include <string.h>
#include "sys_req.h"
#include <mpx/pcb.h>

// Global or static context pointer representing the initial context
static struct context *initial_context = NULL;
struct pcb *current_process;

// Define the C function for handling the ISR
struct context *sys_call(struct context *current_context)
{

    // If first time running sys_call, set initial context

    if (current_context->EAX == IDLE)
    {
        if (initial_context == NULL)
        {
            initial_context = current_context;
        }

        process_queue *ready_q = get_ready_queue();
        if (ready_q->queue_head != NULL)
        { // Update the stack ptr

            // Get first pcb in ready queue and remove it from the queue
            pcb *next_process = ready_q->queue_head;
            pcb_remove(next_process);

            // flip for R4
            if (current_process != NULL)
            {
                current_process->stackPtr = (int *)current_context;
                pcb_insert(current_process);
            }

            // Put current process back into queue and update current process variable
            current_process = next_process;
            return (struct context *)current_process->stackPtr;
        }
        else
        {
            // No remaining ready processes
            current_context = initial_context;
            initial_context = NULL;
            current_process = NULL;
            current_context->EAX = 0;
            return current_context;
        }
    }
    else if (current_context->EAX == EXIT)
    {
        // Attempt to delete current process
        pcb_free(current_process);

        process_queue *ready_q = get_ready_queue();
        if (ready_q->queue_head != NULL)
        {
            // Get first pcb in ready queue and remove it from the queue
            pcb *next_process = ready_q->queue_head;
            pcb_remove(next_process);

            // Update current process variable
            current_process = next_process;
            return (struct context *)current_process->stackPtr;
        }
        else
        {
            // No remaining ready processes
            current_context = initial_context;
            initial_context = NULL;
            current_process = NULL;
            current_context->EAX = 0;
            return current_context;
        }

        // Set return to 0
    }
    else
    {
        current_context->EAX = -1;
        return current_context;
    }
}
