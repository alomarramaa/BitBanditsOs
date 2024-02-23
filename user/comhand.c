
/*
 * Interface users will use to interact with the operating system.
 */

#include <string.h>
#include <sys_req.h>
#include <user/comhand.h>
#include <user/time.h>
#include <mpx/io.h>
#include <mpx/serial.h>
#include <user/date.h>
#include <stdlib.h>
#include <mpx/pcb.h>

// Colors
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define RESET "\x1B[0m"

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
        buff[i+1] = '\0';

        char *token = strtok(buff, " "); // Tokenize the input using space (' ')
        int m_h = (token != NULL && strlen(token) == 2) ? atoi(token) : -1;

        token = strtok(NULL, " ");
        int d_m = (token != NULL && strlen(token) == 2) ? atoi(token) : -1;

        token = strtok(NULL, " ");
        int y_s = (strcmp(resource, "date") == 0) ? ((token != NULL && strlen(token) == 4) ? atoi(token) : -1) : 
                    ((token != NULL && strlen(token) == 2) ? atoi(token) : -1); //If date requires 4 chars, time requires 2

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
 * Prints the version of the MPX, as well as a compilation date.
 * Parameters: void
 * Returns: void
 */
void version(void) // Prints version and compile date
{
    const char *version = "MPX Version R2\n";
    const char *compileDate = "Compiled on: 2/23/24 \n";
    sys_req(WRITE, COM1, version, strlen(version));
    sys_req(WRITE, COM1, compileDate, strlen(compileDate));
}

/*
 * Prints all available commands as well as a description for the user.
 * Parameters: void
 * Returns: void
 */

void help(void) // Prints all available commands
{

    const char *helpText = "Please type your command in all lowercase only. The following are all the commands available to use: \n"
                           "Shutdown - Shut down the system\n"
                           "Version - Display the current version & compilation date\n"
                           "Help - Display all available commands\n"
                           "Echo - Repeats previous message\n"
                           "Get Date - Display current date\n"
                           "Get Time -  Display current time\n"
                           "Set Date - Set date to desired month/day/year\n"
                           "Set Time -  Set time to desired hour/minute/second\n"
                           "Clear - Clear the terminal & redisplay menu\n"
                           "Create PCB - Creates a PCB and puts it in queue\n"
                           "Delete PCB - Removes the requested process from queue\n"
                           "Block PCB - Puts the process in blocked state\n"
                           "Unblock PCB - Puts the process in the unblocked state\n"
                           "Suspend PCB - Puts the process in the suspend state\n"
                           "Resume PCB - Puts the process in the not suspended state\n"
                           "Set Priority - Changes a processes priority\n"
                           "Show PCB - Displays the process's info\n"
                           "Show Ready - Displays all process's info in ready queue\n"
                           "Show Blocked - Displays all process's info in blocked queue\n"
                           "Show All - Displays all process's info\n";
    sys_req(WRITE, COM1, helpText, strlen(helpText));
  //  sys_req(WRITE, COM1, pcbHelp, strlen(pcbHelp));
}

/*
 * Shut down the operating system, and asks for confirmation
 * Parameters: void
 * Returns: void
 */
int shutdown(void)
{
    char *shutdCheck = "Are you sure you want to shut down? (y/n)\n>";
    sys_req(WRITE, COM1, shutdCheck, strlen(shutdCheck)); // Confirmation to shut down

    char confirm[50] = {0};
    int nread = sys_req(READ, COM1, confirm, sizeof(confirm));

    if (nread > 0 && confirm[0] == 'y') // Shutdown confirmed
    {
        char *confMsg = "Shutdown confirmed.\n";
        sys_req(WRITE, COM1, confMsg, strlen(confMsg));
        return 1;
    }
    else // Cancel shutdown
    {
        char *cancelMsg = "Shutdown canceled.\n";
        sys_req(WRITE, COM1, cancelMsg, strlen(cancelMsg));
        return 0;
    }
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
    const char *avaliableCommandStr = " Available Commands:\n\n\techo\n\tget time/date\n\thelp\n\tset time/date\n\tshutdown\n\tversion\n\tclear\n";
    sys_req(WRITE, COM1, comhandInitializeStr, strlen(comhandInitializeStr));
    sys_req(WRITE, COM1, avaliableCommandStr, strlen(avaliableCommandStr));

    // Begin loop for command handler
    for (;;)
    {
        sys_req(WRITE, COM1, "> ", 1); // Display prompt

        char buf[100] = {0};
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        sys_req(WRITE, COM1, buf, nread);

        writeNewLine();

        if (strcmp(buf, "shutdown") == 0) // Shutdown Command
        {

            if (shutdown() == 1)
            {
                break; // exit the loop if shutdown is confirmed
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
        else if (strcmp(buf, "create pcb") == 0) // Create PCB Command
        {
            create_pcb();
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
        else // Unrecognised command
        {
            char *errorMsg = "Improper command entered. Please try again. Ensure that the command is listed and in lowercase.\n";
            sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
        }
    }
}
