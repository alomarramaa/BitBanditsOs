
#include <mpx/pcb.h>
#include <stddef.h>
#include <mpx/device.h>
#include <sys_req.h>
#include <string.h>
#include <stdlib.h>

int create_pcb(void)

{
    // Prompt user for a name
    const char *namePrompt = "Please enter a name (8 characters max) for your PCB";
    sys_req(WRITE, COM1, namePrompt, strlen(namePrompt));
    char inputName[50];
    sys_req(READ, COM1, inputName, sizeof(inputName));

    //Prompt user for class
    const char *classPrompt = "Please enter a class (user or system application) for your PCB.";
    sys_req(WRITE, COM1, classPrompt, strlen(classPrompt));
    char inputClass[50];
    sys_req(READ, COM1, inputClass, sizeof(inputClass));

    //Prompt user for priority
    const char *priorityPrompt = "Please enter a priority (0-9) for your PCB.";
    sys_req(WRITE, COM1, priorityPrompt, strlen(priorityPrompt));
    int inputPriority;
    sys_req(READ, COM1, inputPriority, sizeof(inputPriority));

    // Validate the input parameters
    if (strlen(inputName) > 0 && (strcmp(inputClass, "0") == 0 || strcmp(inputClass, "1") == 0) && (inputPriority >= 1 && inputPriority <= 9))
    {
        // Check if the name is unique
        if (pcb_find(inputName) != NULL)
        {
            const char *error = "Error: PCB name must be unique.";
            sys_req(WRITE, COM1, error, strlen(error));
            return -2; // Error code for non-unique name
        }

        // Call pcb_setup to create a PCB
        struct pcb *new_pcb = pcb_setup(inputName, inputClass, inputPriority);

        // Insert the PCB into the appropriate queue
        if (strcmp(inputClass, "0") == 0)
        {
            // For class 0, insert into blocked queue
            pcb_insert_blocked(new_pcb);
        }
        else
        {
            // For class 1, insert into ready queue (assuming it's a priority queue)
            pcb_insert_ready(new_pcb);
        }

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

int delete_pcb(char *process_name)
{
    // Prompt user for the name of the PCB to delete
    const char *namePrompt = "Please enter the name of the PCB that you wish to delete.";
    sys_req(WRITE, COM1, namePrompt, strlen(namePrompt));
    char pcbToDelete[50];
    sys_req(READ, COM1, pcbToDelete, sizeof(pcbToDelete));

    // Find the PCB with the given name
    struct pcb *delete_pcb = pcb_find(pcbToDelete);

    // Check if the PCB exists
    if (delete_pcb != NULL)
    {
        // Check if the PCB is a system process (system processes cannot be deleted)
        if (pcb_is_system_process(delete_pcb))
        {
            const char *error = "Error: System processes cannot be deleted.";
            sys_req(WRITE, COM1, error, strlen(error));
            return -1; // Error code for attempting to delete a system process
        }

        // Remove the PCB from the queue
        pcb_remove(delete_pcb);

        // Free the associated memory
        pcb_free(delete_pcb);

        // Return success
        return 0;
    }
    else
    {
        // PCB with the given name does not exist
        const char *error = "Error: The PCB you are trying to delete does not exist.";
        sys_req(WRITE, COM1, error, strlen(error));
        return -2; // Error code for non-existent PCB
    }
}

int block_pcb(char *process_name)
{
     // Prompt user for the name of the PCB to block
    const char *prompt = "Please enter the name of the PCB that you wish to block.";
    sys_req(WRITE, COM1, prompt, strlen(prompt));
    char pcbToBlock[50];
    sys_req(READ, COM1, pcbToBlock, sizeof(pcbToBlock));

    // Find the PCB with the given name
    struct pcb *pcb_to_block = pcb_find(pcbToBlock);

    // Check if the PCB exists
    if (pcb_to_block != NULL)
    {
        // Call kernel function to block the PCB
        block_process(pcb_to_block); // need to replace with kernel function

        // Return success
        return 0;
    }
    else
    {
        // PCB with the given name does not exist
        const char *error = "Error: The PCB you are trying to block does not exist.";
        sys_req(WRITE, COM1, error, strlen(error));
        return -1; // Error code for non-existent PCB
    }
}

int unblock_pcb(char *process_name)
{
   // Prompt user for the name of the PCB to unblock
    const char *prompt = "Please enter the name of the PCB that you wish to block.";
    sys_req(WRITE, COM1, prompt, strlen(prompt));
    char pcbToUnblock[50];
    sys_req(READ, COM1, pcbToUnblock, sizeof(pcbToUnblock));


    // Find the PCB with the given name
    struct pcb *pcb_to_unblock = pcb_find(pcbToUnblock);

    // Check if the PCB exists
    if (pcb_to_unblock != NULL)
    {
        // Remove the PCB from its current queue
        remove_pcb_from_queue(pcb_to_unblock);

        // Set the PCB state to ready
        pcb_to_unblock->exe_state = READY;

        // Insert the PCB into the ready queue
        insert_pcb_ready(pcb_to_unblock);

        // Return success
        return 0;
    }
    else
    {
        // PCB with the given name does not exist
        const char *error = "Error: The PCB you are trying to unblock does not exist.";
        sys_req(WRITE, COM1, error, strlen(error));
        return -1; // Error code for non-existent PCB
    }
}

int suspend_pcb(char *process_name)
{
     // Prompt user for the name of the PCB to suspend
    const char *prompt = "Please enter the name of the PCB that you wish to suspend.";
    sys_req(WRITE, COM1, prompt, strlen(prompt));
    char pcbToSuspend[50];
    sys_req(READ, COM1, pcbToSuspend, sizeof(pcbToSuspend));

    // Find the PCB with the given name
    struct pcb *pcb_to_suspend = pcb_find(pcbToSuspend);

    // Check if the PCB exists
    if (pcb_to_suspend != NULL)
    {
        // Check if the PCB is a system process
        if (pcb_is_system_process(pcb_to_suspend))
        {
            const char *error = "Error: System processes cannot be suspended.";
            sys_req(WRITE, COM1, error, strlen(error));
            return -1; // Error code for attempting to suspend a system process
        }

        // Suspend the PCB
        pcb_to_suspend->disp_state = SUSPENDED;

        // Return success
        return 0;
    }
    else
    {
        // PCB with the given name does not exist
        const char *error = "Error: The PCB you are trying to suspend does not exist.";
        sys_req(WRITE, COM1, error, strlen(error));
        return -2; // Error code for non-existent PCB
    }
}

int resume_pcb(char *process_name)
{
        // Prompt user for the name of the PCB to resume
    const char *prompt = "Please enter the name of the PCB that you wish to resume.";
    sys_req(WRITE, COM1, prompt, strlen(prompt));
    char pcbToResume[50];
    sys_req(READ, COM1, pcbToResume, sizeof(pcbToResume));
    // Find the PCB with the given name
    struct pcb *pcb_to_resume = pcb_find(pcbToResume);

    // Check if the PCB exists
    if (pcb_to_resume != NULL)
    {
        // Set the PCB state to not suspended
        pcb_to_resume->disp_state = NOT_SUSPENDED;

        // Move the PCB to the appropriate queue (assuming based on its class)
        if (pcb_to_resume->process_class == 0)
        {
            // Move to blocked queue if it's a class 0 process
            insert_pcb_blocked(pcb_to_resume);
        }
        else
        {
            // Move to ready queue if it's a class 1 process
            insert_pcb_ready(pcb_to_resume); // Need to replace this with the actual function to insert PCB into the ready queue
        }

        // Return success
        return 0;
    }
    else
    {
        // PCB with the given name does not exist
        const char *error = "Error: The PCB you are trying to resume does not exist.";
        sys_req(WRITE, COM1, error, strlen(error));
        return -1; // Error code for non-existent PCB
    }
}

int set_pcb_priority(char *process_name)
{
    // Prompt user for the name of the PCB and the new priority
    const char *prompt_name = "Please enter the name of the PCB you wish to set the priority for: ";
    sys_req(WRITE, COM1, prompt_name, strlen(prompt_name));

    char user_input_name[50]; // Assuming a reasonable buffer size
    sys_req(READ, COM1, user_input_name, sizeof(user_input_name));

    const char *prompt_priority = "Please enter the new priority (0-9) for the PCB: ";
    sys_req(WRITE, COM1, prompt_priority, strlen(prompt_priority));

    char user_input_priority[3]; // Assuming a reasonable buffer size
    sys_req(READ, COM1, user_input_priority, sizeof(user_input_priority));

    // Convert user input for new priority to integer
    int new_priority = atoi(user_input_priority);

    // Find the PCB with the given name
    struct pcb *pcb_to_set_priority = pcb_find(user_input_name);

    // Check if the PCB exists
    if (pcb_to_set_priority != NULL)
    {
        // Check if the new priority is within the valid range
        if (new_priority >= 0 && new_priority <= 9)
        {
            // Set the new priority for the PCB
            pcb_to_set_priority->process_priority = new_priority;

            // Move the PCB to the appropriate place in the appropriate queue
            if (pcb_to_set_priority->process_class == 0)
            {
                // For class 0 processes, move to the blocked queue
                move_pcb_to_blocked_queue(pcb_to_set_priority);
            }
            else
            {
                // For class 1 processes, move to the ready queue
                move_pcb_to_ready_queue(pcb_to_set_priority);
            }

            // Return success
            return 0;
        }
        else
        {
            // Invalid priority
            const char *error = "Error: Priority must be between 0 and 9.";
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

    /*
    Displays a process’s: • Name
• Class
• State
• Suspended Status • Priority
• Parameters:
• Process Name
• Error Checking:
• Name must be valid*/
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