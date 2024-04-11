/*
 * Interface users will use to interact with the operating system.
 */

#include <string.h>
#include "sys_req.h"
#include <user/comhand.h>
#include <user/time.h>
#include <mpx/io.h>
#include <mpx/serial.h>
#include <user/date.h>
#include <stdlib.h>
#include <mpx/pcb.h>
#include <processes.h>
#include <user/alarm.h>
#include <user/help.h>
#include <user/version.h>
#include <user/shutdown.h>
#include <mpx/library.h>
#include <user/r5_user_commands.h>

// Colors
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define RESET "\x1B[0m"

//struct HeapManager hm;

/*
 * Cause the Command Handler to yield the CPU
 * Returns: void
 */

void yield(void)
{
    sys_req(IDLE);
}

/*
 * Loads the processes for R3 and initializes and
 saves the context at the top of the stack
 * Returns: void
 */
void load_r3(void)
{
    const char *loadMessage = "Loading unsuspended R3 processes into memory.\n";
    sys_req(WRITE, COM1, loadMessage, strlen(loadMessage));

    r3_load_pcb(proc1, "Process_1", 3);
    r3_load_pcb(proc2, "Process_2", 3);
    r3_load_pcb(proc3, "Process_3", 3);
    r3_load_pcb(proc4, "Process_4", 3);
    r3_load_pcb(proc5, "Process_5", 3);
}

void r3_load_pcb(void (*proc_function)(void), char *proc_name, int proc_priority)
{
    char *error_msg;

    // Attempt to create a pcb for the given function
    pcb *new_process = pcb_setup(proc_name, USER, proc_priority);
    if (new_process == NULL)
    {
        error_msg = "Could not allocate the PCB for an R3 function.\n";
        sys_req(WRITE, COM1, error_msg, strlen(error_msg));
        return;
    }

    // Sets registers of stack
    struct context *c = (struct context *)new_process->stackPtr;
    // Segment registers
    c->CS = 0x08;
    c->SS = 0x10;
    c->GS = 0x10;
    c->FS = 0x10;
    c->ES = 0x10;
    c->DS = 0x10;

    // General-purpose registers
    c->EDI = 0;
    c->ESI = 0;

    c->ESP = (int)new_process->stackPtr;  // top of pcb stack
    c->EBP = (int)new_process->pcb_stack; // bottom of pcb stack

    c->EBX = 0;
    c->EDX = 0;
    c->ECX = 0;
    c->EAX = 0;

    // Status and control registers
    c->EIP = (int)proc_function;
    c->EFLAGS = 0x0202;

    pcb_insert(new_process);
}

/*
 * Loads the processes in a suspended state for R3
 and initializes and
 saves the context at the top of the stack
 * Returns: void
 */
void load_r3_suspended(void)
{
    const char *loadMessage = "Loading suspended R3 processes into memory.\n";
    sys_req(WRITE, COM1, loadMessage, strlen(loadMessage));
}

/*
 * Retrieves and displays values based on the provided resource ("date" or "time").
 * Calls corresponding functions to display the requested information.
 * Parameters: char* resource
 * Returns: void
 */
void getval(char *resource)
{
    if (strcmp(resource, "date") == 0)
        get_date();
    else if (strcmp(resource, "time") == 0)
        get_time();
}
/*
 * Checks if a given string consists of numeric digits only.
 * Iterates through each character in the string and validates if it's a digit.
 * Parameters: const char* str
 * Returns: int - 1 if all characters are digits, 0 otherwise.
 */
int is_number(const char *str)
{ // Function to check if a string is made of numbers
    while (*str)
    {
        if (*str < '0' || *str > '9')
        {             // Checks to see if the current character is not a number.
            return 0; // Not a digit
        }
        str++;
    }
    return 1; // All characters are digits
}

/*
 * Sets values based on the provided resource ("date" or "time").
 * Reads user input, validates and processes it accordingly.
 * Parameters: char* resource
 * Returns: int - 0 on success, -1 on failure.
 */

int setval(char *resource)
{
    if (strcmp(resource, "date") == 0)
    {
        sys_req(WRITE, COM1, "Please input the new date (mm dd yyyy): \n>", 42);
    }
    else if (strcmp(resource, "time") == 0)
    {
        sys_req(WRITE, COM1, "Please input the new time (hh mm ss): \n>", 40);
    }

    if (strcmp(resource, "date") == 0 || strcmp(resource, "time") == 0)
    { // Check if the resource is "date" or "time"
        char buff[100];

        if (sys_req(READ, COM1, buff, sizeof(buff)) <= 0)
        { // Read input
            sys_req(WRITE, COM1, "Error reading input\n", 21);
            return -1; // Indicate failure
        }

        int i = 0;
        while (buff[i] != '\0')
        {
            i++;
        }
        buff[i] = ' ';
        buff[i + 1] = '\0';

        char *token = strtok(buff, " "); // Tokenize the input using space (' ')
        int m_h = (token != NULL && strlen(token) == 2) ? atoi(token) : -1;

        token = strtok(NULL, " ");
        int d_m = (token != NULL && strlen(token) == 2) ? atoi(token) : -1;

        token = strtok(NULL, " ");
        int y_s = (strcmp(resource, "date") == 0) ? ((token != NULL && strlen(token) == 4) ? atoi(token) : -1) : ((token != NULL && strlen(token) == 2) ? atoi(token) : -1); // If date requires 4 chars, time requires 2

        // Remove newline character if present
        int len = strlen(token);
        if (len > 0 && token[len - 1] == '\n')
        {
            token[len - 1] = '\0';
        }

        if (strcmp(resource, "date") == 0)
        { // Check if the resource is "date"

            if (m_h == -1 || d_m == -1 || y_s == -1)
            { // Check for invalid format
                sys_req(WRITE, COM1, "Invalid format. Use 'mm dd yyyy'\n", 34);
                return -1; // Indicate failure
            }

            set_date(m_h, d_m, y_s); // Set the date using set_date function
            get_date();              // Display the updated date
        }
        else if (strcmp(resource, "time") == 0)
        { // Check if the resource is "time"

            if (m_h == -1 || d_m == -1 || y_s == -1)
            { // Check for invalid format
                sys_req(WRITE, COM1, "Invalid format. Use 'hh mm ss'\n", 32);
                return -1; // Indicate failure
            }

            // Set the time using set_time function
            set_time(m_h, d_m, y_s);
            get_time(); // Display the updated time
        }

        return 0; // Indicate success
    }
    else
    {
        sys_req(WRITE, COM1, "Invalid request\n", 17);
        return -1; // Indicate failure
    }
}

/*
 * Clears the terminal by blanking it.
 * Parameters: void
 * Returns: void
 */

void clear(device dev)
{
    outb(dev, "\033[2J");
}

/*
 * Writes a new line to ensure consistent formatting
 * Parameters: void
 * Returns: void
 */
void writeNewLine(void)
{
    const char *newLine = "\n";
    sys_req(WRITE, COM1, newLine, strlen(newLine));
}

/*
 * Takes in user input and evaluates what command it is, then executes the command by calling the relevant function.
 Also, prints the welcome message to the MPX in color.
 * Parameters: void
 * Returns: void
 */
void comhand(void)
{
    /***************/
    // Welcome message
    const char *asciiArt =
        RED "\nWelcome to " RESET
            GREEN "Bit Bandits' MPX!\n" RESET
            "         _________\n"
            "        / ======= \\\n"
            "       / __________\\\n"
            "      | ___________ |\n"
            "      | | -       | |\n"
            "      | |  CS450  | |\n"
            "      | |_________| |________________________\n"
            "      \\=____________/   Bit Bandits          )\n" YELLOW "      /\"\"\"\"\"\"\"\"\"\"\"\"\"\\\n" RESET
                BLUE "     / ::::::::::::: \\\n" RESET
            "    (_________________)\n"
            "\n";

    sys_req(WRITE, COM1, asciiArt, strlen(asciiArt));

    // Constants
    const char *comhandInitializeStr = " Comhand Initialized: Please write your preferred command in all lowercase.\n";
    const char *avaliableCommandStr = " Available Commands:\n\n\t"
                                      "--- general commands --\n\t"
                                      "\n\t"
                                      "echo\n\tget time/date\n\thelp\n\tset time/date\n\tshutdown\n\tversion\n\tclear\n\t"
                                      "\n\t"
                                      "--- pcb commands--\n\t"
                                      "\n\t"
                                      "delete pcb\n\tblock pcb\n\tunblock pcb\n\tsuspend pcb\n\tresume pcb\n\tset pcb priority\n\tshow pcb\n\tshow ready\n\tshow blocked\n\tshow all\n\t"
                                      "\n\t"
                                      "-- r3 commands --\n\t"
                                      "\n\t"
                                      "load r3\n\tload r3 suspended\n"
                                      "\n\t"
                                      "-- r4 commands --\n\t"
                                      "\n\t"
                                      "set alarm\n\tremove alarm\n"
                                      "\n\t"
                                      "-- r5 commands --\n\t"
                                      "\n\t"
                                      "allocate memory\n\tfree memory\n\tshow allocated memory\n\tshow free memory\n\tshow all memory\n\n";

    sys_req(WRITE, COM1, comhandInitializeStr, strlen(comhandInitializeStr));
    sys_req(WRITE, COM1, avaliableCommandStr, strlen(avaliableCommandStr));

    // Begin loop for command handler
    for (;;)
    {
        

        char buf[100] = {0};

        sys_req(IDLE); // yield CPU before prompting for user input
        
        sys_req(WRITE, COM1, "> ", 1); // Display prompt
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        sys_req(WRITE, COM1, buf, nread);

        writeNewLine();

        if (strcmp(buf, "shutdown") == 0) // Shutdown Command
        {

            if (shutdown() == 1)
            {
                // pcb_free(); //empty pcb queue
                sys_req(EXIT);
            }
        }

        if (strcmp(buf, "version") == 0) // Version Command
        {
            version();
        }

        else if (strcmp(buf, "help") == 0) // Help Command
        {
            help();
        }
        else if (strcmp(buf, "get date") == 0) // Get Date Command
        {

            getval("date");
        }

        else if (strcmp(buf, "set date") == 0) // Set Date Command
        {
            setval("date");
        }

        else if (strcmp(buf, "get time") == 0) // Set Date Command
        {

            getval("time");
        }

        else if (strcmp(buf, "set time") == 0) // Set Date Command
        {
            setval("time");
        }
        else if (strcmp(buf, "clear") == 0)
        {
            clear(COM1);
        }
        else if (strcmp(buf, "delete pcb") == 0) // Delete PCB
        {
            delete_pcb();
        }
        else if (strcmp(buf, "block pcb") == 0) // Block PCB
        {
            block_pcb();
        }
        else if (strcmp(buf, "unblock pcb") == 0) // Unblock PCB
        {
            unblock_pcb();
        }
        else if (strcmp(buf, "suspend pcb") == 0) // Suspend PCB
        {
            suspend_pcb();
        }
        else if (strcmp(buf, "resume pcb") == 0) // Resume PCB
        {
            resume_pcb();
        }
        else if (strcmp(buf, "set pcb priority") == 0) // Resume PCB
        {
            set_pcb_priority();
        }
        else if (strcmp(buf, "show pcb") == 0) // Show PCB
        {
            show_pcb();
        }
        else if (strcmp(buf, "show ready") == 0) // Show Ready
        {
            show_ready();
        }
        else if (strcmp(buf, "show blocked") == 0) // Show Blocked
        {
            show_blocked();
        }
        else if (strcmp(buf, "show all") == 0) // Show All
        {
            show_all();
        }

        else if (strcmp(buf, "load r3") == 0)
        {
            load_r3();
        }

        else if (strcmp(buf, "load r3 suspended") == 0)
        {
            load_r3_suspended();
        }
        else if (strcmp(buf, "set alarm") == 0)
        {
            int hour, minute, seconds;
            char message[100];
            char tempBuf[100];

            sys_req(WRITE, COM1, "Enter hour: \n", 14);
            int nread = sys_req(READ, COM1, tempBuf, sizeof(tempBuf));
            tempBuf[nread] = '\0';
            hour = atoi(tempBuf);

            sys_req(WRITE, COM1, "\nEnter minute: \n", 17);
            nread = sys_req(READ, COM1, tempBuf, sizeof(tempBuf));
            tempBuf[nread] = '\0';
            minute = atoi(tempBuf);

            sys_req(WRITE, COM1, "\nEnter seconds: \n", 18);
            nread = sys_req(READ, COM1, tempBuf, sizeof(tempBuf));
            tempBuf[nread] = '\0';
            seconds = atoi(tempBuf);

            sys_req(WRITE, COM1, "\nEnter message: \n", 18);
            nread = sys_req(READ, COM1, message, sizeof(message));
            message[nread] = '\0';

            createAlarm(hour, minute, seconds, message);
            sys_req(WRITE, COM1, "\nAlarm set.\n", 13);
        }
        else if (strcmp(buf, "remove alarm") == 0)
        {
            int hour, minute, seconds;
            char tempBuf[100];

            sys_req(WRITE, COM1, "Enter hour: \n", 14);
            int nread = sys_req(READ, COM1, tempBuf, sizeof(tempBuf));
            tempBuf[nread] = '\0';
            hour = atoi(tempBuf);

            sys_req(WRITE, COM1, "\nEnter minute: \n", 17);
            nread = sys_req(READ, COM1, tempBuf, sizeof(tempBuf));
            tempBuf[nread] = '\0';
            minute = atoi(tempBuf);

            sys_req(WRITE, COM1, "\nEnter seconds: \n", 18);
            nread = sys_req(READ, COM1, tempBuf, sizeof(tempBuf));
            tempBuf[nread] = '\0';
            seconds = atoi(tempBuf);

            // Find the index of the alarm based on the time
            int index = findAlarmIndex(hour, minute, seconds);
            if (index != -1) {
                removeAlarm(index);
                sys_req(WRITE, COM1, "\nAlarm removed.\n", 17);
            } else {
                sys_req(WRITE, COM1, "\nAlarm not found.\n", 18);
            }
        }
        else if (strcmp(buf, "allocate memory") == 0)
        {
            size_t new_block_size;
            char tempBuf[100];

            sys_req(WRITE, COM1, "Enter the size of the block you wish to allocate (in bytes): \n", 63);
            int nread = sys_req(READ, COM1, tempBuf, sizeof(tempBuf));
            tempBuf[nread] = '\0';
            new_block_size = atoi(tempBuf);

            allocateMemory(new_block_size);
        }
        else if (strcmp(buf, "free memory") == 0)
        {
            char tempBuf[100];
            
            sys_req(WRITE, COM1, "Enter the memory address you wish to free: \n", 54);
            int nread = sys_req(READ, COM1, tempBuf, sizeof(tempBuf));
            tempBuf[nread] = '\0';
            void *address_to_free = (void *)atoi(tempBuf);
    
            freeMemory(address_to_free);
        }
        else if (strcmp(buf, "show allocated memory") == 0)
        {
            showAllocatedMemory(&hm);
        }
        else if (strcmp(buf, "show free memory") == 0)
        {
            showFreeMemory(&hm);
        }
        else if (strcmp(buf, "show all memory") == 0)
        {
            showAllMemory(&hm);
        }

        else // Unrecognised command
        {
            char *errorMsg = "Improper command entered. Please try again. Ensure that the command is listed and in lowercase.\n";
            sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
        }
        sys_req(IDLE); // yield CPU after prompting for user input
    }
}
