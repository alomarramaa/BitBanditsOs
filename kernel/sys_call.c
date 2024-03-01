#include <stdint.h>

// Define the context struct
struct context {
    // Define the members of your context struct here
    // For example:
    // int eax;
    // int ebx;
    // ...
};

// Global PCB pointer representing the currently executing process
struct context *current_pcb = NULL;

// Global or static context pointer representing the initial context
struct context *initial_context = NULL;

// Define the C function for handling the ISR
struct context* sys_call(uint32_t op_code, struct context *pcb) {
    // Check the operation code
    switch(op_code) {
        case IDLE:
            // Implement IDLE operation
            // If there are ready, non-suspended PCBs in the queue, remove the first
            // Save the context of the current PCB
            // Add the current PCB back to the queue
            // Return the context of the next process
            // If the PCB queue is empty, or only consists of blocked or suspended PCBs, continue with the current process
            // Ensure that the return value seen by sys req() is 0
            break;
        case EXIT:
            // Implement EXIT operation
            // Delete the currently running PCB
            // If there are any ready, non-suspended PCBs in the queue, load the first as in IDLE
            // If the PCB queue is empty, or only consists of blocked or suspended PCBs, load the original context
            // Ensure that the return value seen by sys req() is 0
            break;
        default:
            // If the operation code is anything but IDLE or EXIT, do not load any new context and set the return value to -1
            break;
    }

    // Return the appropriate context based on the operation code
    return NULL;
}