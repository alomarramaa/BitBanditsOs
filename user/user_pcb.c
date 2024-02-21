
#include <mpx/pcb.h>
#include <stddef.h>
#include <mpx/device.h>
#include <sys_req.h>
#include <string.h>
#include <stdlib.h>

void log_info(char *message)
{
    sys_req(WRITE, COM1, message, strlen(message));
}

int create_pcb(void)
{
    // Prompt user for a name
    const char *namePrompt = "Please enter a name (8 characters max) for your PCB: \n";
    sys_req(WRITE, COM1, namePrompt, strlen(namePrompt));
    char inputName[50];
    int nread = sys_req(READ, COM1, inputName, sizeof(inputName));
    sys_req(WRITE, COM1, inputName, nread);

    // Prompt user for class
    const char *classPrompt = "\nPlease enter a class for your PCB. (0 for system, 1 for user): \n";
    sys_req(WRITE, COM1, classPrompt, strlen(classPrompt));
    char inputClass[50];
    nread = sys_req(READ, COM1, inputClass, sizeof(inputClass));
    sys_req(WRITE, COM1, inputClass, nread);

    // Prompt user for priority
    const char *priorityPrompt = "\nPlease enter a priority (0-9) for your PCB: \n";
    sys_req(WRITE, COM1, priorityPrompt, strlen(priorityPrompt));
    char priorityBuffer[50];
    nread = sys_req(READ, COM1, priorityBuffer, sizeof(priorityBuffer));
    sys_req(WRITE, COM1, priorityBuffer, nread);
    int inputPriority = atoi(priorityBuffer);

    // Validate the input parameters
    if ((strlen(inputName) > 0 && strlen(inputName) <= 8) && (strcmp(inputClass, "0") == 0 || strcmp(inputClass, "1") == 0) && (inputPriority >= 0 && inputPriority <= 9))
    {
        // Check if the name is unique
        if (pcb_find(inputName) != NULL)
        {
            log_info("\nError: PCB name must be unique.");
            return -2; // Error code for non-unique name
        }

        // Call pcb_setup to create a PCB
        struct pcb *new_pcb = pcb_setup(inputName, atoi(inputClass), inputPriority);

        // // Insert the PCB into the appropriate queue
        // if (strcmp(inputClass, "0") == 0)
        // {
        //     // For class 0, insert into blocked queue
        //     pcb_insert_blocked(new_pcb);
        // }
        // else
        // {
        //     // For class 1, insert into ready queue (assuming it's a priority queue)
        //     pcb_insert_ready(new_pcb);
        // }

        pcb_insert(new_pcb);

        log_info("\nPCB created successfully.\n");
        // Return success
        return 0;
    }
    else
    {
        // Invalid parameters, display error message
        const char *error = "Error: Please make sure you entered a valid name, class, and priority (1-9).";
        sys_req(WRITE, COM1, error, strlen(error));
        return -1; // Error code for invalid parameters
    }
}

int delete_pcb(void)
{
    // Prompt user for the name of the PCB to delete
    const char *namePrompt = "Please enter the name of the PCB that you wish to delete.\n";
    sys_req(WRITE, COM1, namePrompt, strlen(namePrompt));
    char pcbToDelete[50];
    int nread = sys_req(READ, COM1, pcbToDelete, sizeof(pcbToDelete));
    sys_req(WRITE, COM1, pcbToDelete, nread);

    // Find the PCB with the given name
    struct pcb *delete_pcb = pcb_find(pcbToDelete);

    // Check if the PCB exists
    if (delete_pcb != NULL)
    {
        // Check if the PCB is a system process (system processes cannot be deleted)
        if (delete_pcb->process_class == SYSTEM)
        {
            const char *error = "Error: System processes cannot be deleted.\n";
            sys_req(WRITE, COM1, error, strlen(error));
            return -1; // Error code for attempting to delete a system process
        }

        // Remove the PCB from the queue
        int retval = pcb_remove(delete_pcb);
        if (retval != 0)
        {
            log_info("\nError: PCB could not be removed from queue.\n");
            return 2;
        }

        // Free the associated memory
        pcb_free(delete_pcb);

        log_info("\nPCB deleted successfully.\n");
        // Return success
        return 0;
    }
    else
    {
        // PCB with the given name does not exist
        const char *error = "\nError: The PCB you are trying to delete does not exist.\n";
        sys_req(WRITE, COM1, error, strlen(error));
        return -2; // Error code for non-existent PCB
    }
}

int block_pcb(void)
{
    // Prompt user for the name of the PCB to block
    const char *prompt = "Please enter the name of the PCB that you wish to block.\n";
    sys_req(WRITE, COM1, prompt, strlen(prompt));
    char pcbToBlock[50];
    int nread = sys_req(READ, COM1, pcbToBlock, sizeof(pcbToBlock));
    sys_req(WRITE, COM1, pcbToBlock, nread);

    // Find the PCB with the given name
    struct pcb *pcb_to_block = pcb_find(pcbToBlock);

    // Check if the PCB exists and is not already blocked
    if (pcb_to_block != NULL && pcb_to_block->exe_state != BLOCKED)
    {
        // Call kernel function to block the PCB

        // Remove pcb from previous queue and insert it into the appropriate queue
        int retval = pcb_remove(pcb_to_block);
        if (retval != 0)
        {
            log_info("\nError: PCB could not be removed from queue.\n");
            return 2;
        }

        // Set pcb's execution state
        pcb_to_block->exe_state = BLOCKED;
        pcb_to_block->disp_state = SUSPENDED;

        retval = pcb_insert(pcb_to_block);
        if (retval != 0)
        {
            log_info("\nError: PCB could not be inserted into queue.\n");
            return 3;
        }

        // Return success
        log_info("\nPCB blocked successfully.\n");
        return 0;
    }
    else
    {
        // PCB with the given name does not exist
        const char *error = "\nError: The PCB you are trying to block does not exist or is already blocked.\n";
        sys_req(WRITE, COM1, error, strlen(error));
        return -1; // Error code for non-existent PCB
    }
}

int unblock_pcb(void)
{
    // Prompt user for the name of the PCB to unblock
    const char *prompt = "Please enter the name of the PCB that you wish to unblock.\n";
    sys_req(WRITE, COM1, prompt, strlen(prompt));
    char pcbToUnblock[50];
    int nread = sys_req(READ, COM1, pcbToUnblock, sizeof(pcbToUnblock));
    sys_req(WRITE, COM1, pcbToUnblock, nread);

    // Find the PCB with the given name
    struct pcb *pcb_to_unblock = pcb_find(pcbToUnblock);

    // Check if the PCB exists and is blocked
    if (pcb_to_unblock != NULL && pcb_to_unblock->exe_state == BLOCKED)
    {
        // Remove the PCB from its current queue
        int retval = pcb_remove(pcb_to_unblock);
        if (retval != 0)
        {
            log_info("\nError: PCB could not be removed from queue.\n");
            return 2;
        }

        // Set the PCB state to ready
        pcb_to_unblock->exe_state = READY;
        pcb_to_unblock->disp_state = NOT_SUSPENDED;

        // Insert the PCB into the ready queue
        retval = pcb_insert(pcb_to_unblock);
        if (retval != 0)
        {
            log_info("\nError: PCB could not be inserted into queue.\n");
            return 3;
        }

        // Return success
        log_info("\nPCB unblocked successfully.\n");
        return 0;
    }
    else
    {
        // PCB with the given name does not exist
        const char *error = "\nError: The PCB you are trying to unblock does not exist or is not blocked.\n";
        sys_req(WRITE, COM1, error, strlen(error));
        return -1; // Error code for non-existent PCB
    }
}

int suspend_pcb(void)
{
    // Prompt user for the name of the PCB to suspend
    const char *prompt = "Please enter the name of the PCB that you wish to suspend.\n";
    sys_req(WRITE, COM1, prompt, strlen(prompt));
    char pcbToSuspend[50];
    int nread = sys_req(READ, COM1, pcbToSuspend, sizeof(pcbToSuspend));
    sys_req(WRITE, COM1, pcbToSuspend, nread);

    // Find the PCB with the given name
    struct pcb *pcb_to_suspend = pcb_find(pcbToSuspend);

    // Check if the PCB exists and is not already suspended
    if (pcb_to_suspend != NULL && pcb_to_suspend->disp_state != SUSPENDED)
    {
        // Check if the PCB is a system process
        if (pcb_to_suspend->process_class == SYSTEM)
        {
            const char *error = "\nError: System processes cannot be suspended.\n";
            sys_req(WRITE, COM1, error, strlen(error));
            return -1; // Error code for attempting to suspend a system process
        }

        // Remove PCB from prev queue and reinsert into appropriate queue
        int retval = pcb_remove(pcb_to_suspend);
        if (retval != 0)
        {
            log_info("\nError: PCB could not be removed from queue.\n");
            return 2;
        }

        // Suspend the PCB
        pcb_to_suspend->exe_state = BLOCKED;
        pcb_to_suspend->disp_state = SUSPENDED;

        retval = pcb_insert(pcb_to_suspend);
        if (retval != 0)
        {
            log_info("\nError: PCB could not be inserted into queue.\n");
            return 3;
        }

        // Return success
        log_info("\nPCB suspended successfully\n");
        return 0;
    } else {
        // PCB with the given name does not exist
        const char *error = "\nError: The PCB you are trying to suspend does not exist or is already suspended.\n";
        sys_req(WRITE, COM1, error, strlen(error));
        return -2; // Error code for non-existent PCB
    }
}

int resume_pcb(void)
{
    // Prompt user for the name of the PCB to resume
    const char *prompt = "Please enter the name of the PCB that you wish to resume.\n";
    sys_req(WRITE, COM1, prompt, strlen(prompt));
    char pcbToResume[50];
    int nread = sys_req(READ, COM1, pcbToResume, sizeof(pcbToResume));
    sys_req(WRITE, COM1, pcbToResume, nread);

    // Find the PCB with the given name
    struct pcb *pcb_to_resume = pcb_find(pcbToResume);

    // Check if the PCB exists and is not suspended
    if (pcb_to_resume != NULL && pcb_to_resume->disp_state == SUSPENDED)
    {
        // Remove PCB from its queue and insert it into correct queue
        int retval = pcb_remove(pcb_to_resume);
        if (retval != 0)
        {
            log_info("\nError: PCB could not be removed from queue.\n");
            return 2;
        }
        
        // Set the PCB state to not suspended
        pcb_to_resume->exe_state = RUNNING;
        pcb_to_resume->disp_state = NOT_SUSPENDED;

        
        retval = pcb_insert(pcb_to_resume);
        if (retval != 0)
        {
            log_info("\nError: PCB could not be inserted into queue.\n");
            return 3;
        }

        // Return success
        log_info("\nPCB resumed successfully\n");
        return 0;
    }
    else
    {
        // PCB with the given name does not exist
        const char *error = "\nError: The PCB you are trying to resume does not exist or is not suspended.\n";
        sys_req(WRITE, COM1, error, strlen(error));
        return -1; // Error code for non-existent PCB
    }
}

int set_pcb_priority(void)
{
    // Prompt user for the name of the PCB and the new priority
    const char *prompt_name = "Please enter the name of the PCB you wish to set the priority for: \n";
    sys_req(WRITE, COM1, prompt_name, strlen(prompt_name));

    char user_input_name[50]; // Assuming a reasonable buffer size
    int nread = sys_req(READ, COM1, user_input_name, sizeof(user_input_name));
    sys_req(WRITE, COM1, user_input_name, nread);

    const char *prompt_priority = "\nPlease enter the new priority (0-9) for the PCB: \n";
    sys_req(WRITE, COM1, prompt_priority, strlen(prompt_priority));

    char user_input_priority[3]; // Assuming a reasonable buffer size
    nread = sys_req(READ, COM1, user_input_priority, sizeof(user_input_priority));
    sys_req(WRITE, COM1, user_input_priority, nread);

    // Convert user input for new priority to integer
    int new_priority = atoi(user_input_priority);

    // Find the PCB with the given name
    struct pcb *pcb_to_set_priority = pcb_find(user_input_name);

    // Check if the PCB exists
    if (pcb_to_set_priority != NULL)
    {
        // Check if the new priority is within the valid range
        if ((new_priority == 0 && pcb_to_set_priority->process_class == SYSTEM) || (new_priority >= 1 && new_priority <= 9))
        {
            // Set the new priority for the PCB
            pcb_to_set_priority->process_priority = new_priority;

            // Requeue the PCB if in a ready queue
            if (pcb_to_set_priority->exe_state == READY)
            {
                pcb_remove(pcb_to_set_priority);
                pcb_insert(pcb_to_set_priority);
            }

            // Return success
            log_info("\nPCB priority set successfully\n");
            return 0;
        }
        else
        {
            // Invalid priority
            const char *error = "Error: Priority must be between 0 and 9. Priority can only be 0 for system processes";
            sys_req(WRITE, COM1, error, strlen(error));
            return -2; // Error code for invalid priority
        }
    }
    else
    {
        // PCB with the given name does not exist
        const char *error = "Error: The PCB you are trying to set the priority for does not exist.";
        sys_req(WRITE, COM1, error, strlen(error));
        return -1; // Error code for non-existent PCB
    }
}

int show_pcb(char *process_name)
{
    // Not necessary, dummy line to use input so I can run the mpx
    if (process_name == NULL)
    {
        return -1;
    }

    // Find the PCB with the given name.
    struct pcb *pcb = pcb_find(process_name);

    // Check if the PCB exists
    if (pcb != NULL)
    {
        // Display PCB information
        char buffer[50];

        // Display Name
        const char *name_prompt = "\nName: ";
        sys_req(WRITE, COM1, name_prompt, strlen(name_prompt));
        sys_req(WRITE, COM1, pcb->process_name, strlen(pcb->process_name));

        // Display Class
        const char *class_prompt = "\nClass: ";
        sys_req(WRITE, COM1, class_prompt, strlen(class_prompt));
        itoa(pcb->process_class, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));

        // Display Priority
        const char *priority_prompt = "\nPriority: ";
        sys_req(WRITE, COM1, priority_prompt, strlen(priority_prompt));
        itoa(pcb->process_priority, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));

        // Display State
        const char *state_prompt = "\nState: ";
        sys_req(WRITE, COM1, state_prompt, strlen(state_prompt));
        itoa(pcb->exe_state, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));

        // Display Suspended state
        const char *suspended_prompt = "\nSuspended state: ";
        sys_req(WRITE, COM1, suspended_prompt, strlen(suspended_prompt));
        itoa(pcb->disp_state, buffer);
        sys_req(WRITE, COM1, buffer, strlen(buffer));

        const char *newline = "\n";
        sys_req(WRITE, COM1, newline, strlen(newline));

        return 0;
    }

    // PCB with the given name does not exist
    const char *not_found_error = "\nProcess not found.\n";
    sys_req(WRITE, COM1, not_found_error, strlen(not_found_error));
    /*
    Displays a process’s: • Name
• Class
• State
• Suspended Status • Priority
• Parameters:
• Process Name
• Error Checking:
• Name must be valid*/
    return 0;
}

void show_ready(void)
{
    // Prompt user for a name for PCB
    const char *prompt = "Below are all the PCBs in a ready state.";
    sys_req(WRITE, COM1, prompt, strlen(prompt));
    /*
    For all processes in the Ready state, display the process’s: • Name
• Class
• State
• Suspended Status • Priority
• Parameters: • None
• Error Checking: • None*/
}

void show_blocked(void)
{
    // Prompt user for a name for PCB
    const char *prompt = "Below are all the PCBs in a blocked state.";
    sys_req(WRITE, COM1, prompt, strlen(prompt));
    /*
     For all processes in the Blocked state, display the process’s: • Name
• Class
• State
• Suspended Status • Priority
• Parameters: • None
• Error Checking: • None*/
}

void show_all(void)
{
    // Prompt user for a name for PCB
    const char *prompt = "Below are all the PCBs, regardless of state.";
    sys_req(WRITE, COM1, prompt, strlen(prompt));
    /*
    For all processes (in any state), display the process’s:
    • Name
• Class
• State
• Suspended Status • Priority
• Parameters: • None
• Error Checking: • None*/
}
