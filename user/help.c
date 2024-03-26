#include <sys_req.h>
#include <user/help.h>

/*
 * Prints all available commands as well as a description for the user.
 * Parameters: void
 * Returns: void
 */

void help(void) // Prints all available commands
{

    const char *helpText = "Please type your command in all lowercase only. The following are all the commands available to use: \n"
                           "\n"
                           "---General Commands---\n\n"
                           "Shutdown - Shut down the system\n"
                           "Version - Display the current version & compilation date\n"
                           "Help - Display all available commands\n"
                           "Echo - Repeats previous message\n"
                           "Get Date - Display current date\n"
                           "Get Time -  Display current time\n"
                           "Set Date - Set date to desired month/day/year\n"
                           "Set Time -  Set time to desired hour/minute/second\n"
                           "Clear - Clear the terminal & redisplay menu\n"
                           "\n"
                           "---PCB Commands---\n\n"
                           "Delete PCB - Removes the requested process from queue\n"
                           "Block PCB - Puts the process in blocked state\n"
                           "Unblock PCB - Puts the process in the unblocked state\n"
                           "Suspend PCB - Puts the process in the suspend state\n"
                           "Resume PCB - Puts the process in the not suspended state\n"
                           "Set PCB Priority - Changes a processes priority\n"
                           "Show PCB - Displays the process's info\n"
                           "Show Ready - Displays all process's info in ready queue\n"
                           "Show Blocked - Displays all process's info in blocked queue\n"
                           "Show All - Displays all process's info\n"
                           "\n"
                           "---R3 Commands---\n\n"
                           "Load R3 - Loads the R3 test processes in a non-suspended state\n"
                           "Load R3 Suspended - Loads the R3 test processes in a suspended state\n"
                           "\n"
                           "---R4 Commands---\n\n"
                           "Set Alarm - Creates an alarm prompted by the user\n"
                           "Remove Alarm - Deletes an alarm selected by the user\n"
                           "\n";

    sys_req(WRITE, COM1, helpText, strlen(helpText));
    //  sys_req(WRITE, COM1, pcbHelp, strlen(pcbHelp));
}
