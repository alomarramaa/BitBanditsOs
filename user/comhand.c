
/*
 * Brief description of the function's purpose.
 */



#include <string.h>
#include <sys_req.h>
#include <user/comhand.h>
#include <user/time.h>

// Colors
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define RESET "\x1B[0m"



/*
 * Clears the terminal by blanking it.
 * Parameters: void
 * Returns: void
 */

void clear(void)
{
    /* The “Clear” or “clear” command should simply blank the terminal,
    where the top of terminal is a new line for the user to enter their next command.
     This functionality may prove useful for other features (both bonus and required)
     in the future.  If you have a menu interface, redisplay your menu*/
}

/*
 * Prints the version of the MPX, as well as a compilation date.
 * Parameters: void
 * Returns: void
 */
void version(void) // Prints version and compile date
{
    const char *version = "MPX Version R1\n";
    const char *compileDate = "Compiled on: \n";
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
    const char *helpText = "Available Commands: \n"
                           "1. Shutdown - Shut down the system\n"
                           "2. Version - Display the current version & compilation date\n"
                           "3. Help - Display all available commands\n"
                           "4. Echo - Repeats previous message\n"
                           "5. Get Date - Display current date\n"
                           "6. Get Time -  Display current time\n"
                           "7. Set Date - Set date to desired month/day/year\n"
                           "8. Set Time -  Set time to desired hour/minute/second\n"
                           "9. Clear - Clear the terminal & redisplay menu\n";

    sys_req(WRITE, COM1, helpText, strlen(helpText));
}


/*
 * Shut down the MPX
 * Parameters: void
 * Returns: void
 */
int shutdown(void)
{
    char* shutdCheck = "Are you sure you want to shut down? (y/n)\n";
    sys_req(WRITE, COM1, shutdCheck, strlen(shutdCheck)); // Confirmation to shut down

    char confirm[50] = {0};
    int nread = sys_req(READ, COM1, confirm, sizeof(confirm));

    if (nread > 0 && confirm[0] == 'y') // Shutdown confirmed
    {
        char* confMsg = "Shutdown confirmed.\n";
        sys_req(WRITE, COM1, confMsg, strlen(confMsg));
        return 1;
    }
    else // Cancel shutdown
    {
        char* cancelMsg = "Shutdown canceled.\n";
        sys_req(WRITE, COM1, cancelMsg, strlen(cancelMsg));
        return 0;
    }
}

/*
 * Gets the system's current date
 * Parameters: void
 * Returns: void
 */
void getDate(void) 
{
    const char *date = "Current date:";
    sys_req(WRITE, COM1, date, strlen(date));
}

/*
 * Sets the current date of the system
 * Parameters: void
 * Returns: void
 */
void setDate(void) 
{
    const char *setDateMsg = "Enter the new date (MM/DD/YYYY): ";
    sys_req(WRITE, COM1, setDateMsg, strlen(setDateMsg));

    char newDate[11] = {0};
    int nread = sys_req(READ, COM1, newDate, sizeof(newDate));

    if (nread > 0)
    {
        const char *successMsg = "Date has been set successfully!\n";
        sys_req(WRITE, COM1, successMsg, strlen(successMsg));
    }
    else
    {
        const char *errorMsg = "Error reading the new date.\n";
        sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
    }
}

/*
 * Get's the current time of the system
 * Parameters: void
 * Returns: void
 */
void getTime(void) 
{
    const char *time = "Current time:";
    sys_req(WRITE, COM1, time, strlen(time));
}


/*
 * Sets the current time of the system
 * Parameters: void
 * Returns: void
 */
void setTime(void) 
{
    const char* setTimeMsg = "Enter the new time (hhmmss): ";
    sys_req(WRITE, COM1, setTimeMsg, strlen(setTimeMsg));

    char newTime[5] = {0};
    int nread = sys_req(READ, COM1, newTime, sizeof(newTime));

    if (nread > 0)
    {
        const char *successMsg = "Time has been set successfully!\n";
        sys_req(WRITE, COM1, successMsg, strlen(successMsg));
    }
    else
    {
        const char *errorMsg = "Error reading the new time.\n";
        sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
    }
}

/*
 * Writes a new line to ensure consistent formatting
 * Parameters: void
 * Returns: void
 */
void writeNewLine(void)
{
    const char* newLine = "\n";
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
            "      \\=____________/   Bit Bandits          )\n" YELLOW "      / \"\"\"\"\"\"\"\"\"\"\"\"\" \\\n" RESET
                BLUE "     / ::::::::::::: \\\n" RESET
            "    (_________________)\n"
            "\n";

    sys_req(WRITE, COM1, asciiArt, strlen(asciiArt));

    // Constants
    const char *comhandInitializeStr = " Comhand Initialized: Please write your preferred command.\n";
    const char *avaliableCommandStr = " Available Commands: \n\techo\n\tget time/date\n\thelp\n\tset time/date\n\tshutdown\n\tversion\n\tclear\n";

    sys_req(WRITE, COM1, comhandInitializeStr, strlen(comhandInitializeStr));
    sys_req(WRITE, COM1, avaliableCommandStr, strlen(avaliableCommandStr));

    // Begin loop for command handler
    for (;;)
    {
        char buf[100] = {0};
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        sys_req(WRITE, COM1, buf, nread);
        
        writeNewLine();

        if (strcmp(buf, "shutdown") == 0) // Shutdown Command
        {
            // char* shutdConf = "Shutdown confirmed.\n";
            // sys_req(WRITE, COM1, shutdConf, strlen(shutdConf));
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
            getDate();
        }

        else if (strcmp(buf, "set date") == 0) // Set Date Command
        {
            setDate();
        }

        else if (strcmp(buf, "get time") == 0) // Set Date Command
        {
            getTime();
        }

        else if (strcmp(buf, "set time") == 0) // Set Date Command
        {
            setTime();
        }

        else // Unrecognised command
        {
            char* errorMsg = "Improper command entered. Please try again.\n";
            sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
        }
    }
}
