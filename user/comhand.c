
#include <string.h>
#include <sys_req.h>
#include <user/comhand.h>
//#include <time.h>

//Colors 
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define RESET   "\x1B[0m"


// Clears the terminal
void clear(void)
{

    /*
    The “Clear” or “clear” command should simply blank the terminal, 
    where the top of terminal is a new line for the user to enter their next command.
     This functionality may prove useful for other features (both bonus and required) 
     in the future.  If you have a menu interface, redisplay your menu*/
}
void version(void) // Prints version and compile date
{
    const char *version = "MPX Version R1\n";
    const char *compileDate = "Compiled on: \n";
    sys_req(WRITE, COM1, version, strlen(version));
    sys_req(WRITE, COM1, compileDate, strlen(compileDate));
}

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

int shutdown(void)
{
    char* shutdCheck = "Are you sure you want to shut down? (y/n)\n";
    sys_req(WRITE, COM1, shutdCheck, strlen(shutdCheck)); // Confirmation to shut down

    char confirm[50] = {0};
    int nread = sys_req(READ, COM1, confirm, sizeof(confirm));

    if (nread > 0 && confirm[0] == 'y') // shutdown confirmed
    {
        sys_req(WRITE, COM1, "Shutdown confirmed.");
        return 1;
    }
    else // Cancel shutdown
    {
        sys_req(WRITE, COM1, "Shutdown canceled.\n");
        return 0;
    }
}

void getDate(void) // Gets the systems current date
{
    const char* date = "Current date:";
    sys_req(WRITE, COM1, date, strlen(date));
}

void setDate(void) // Sets the systems current date
{
    const char* setDateMsg = "Enter the new date (MM/DD/YYYY): ";
    sys_req(WRITE, COM1, setDateMsg, strlen(setDateMsg));

    char newDate[11] = {0};
    int nread = sys_req(READ, COM1, newDate, sizeof(newDate));

    if (nread > 0) {
        const char* successMsg = "Date has been set successfully!\n";
        sys_req(WRITE, COM1, successMsg, strlen(successMsg), '\n');
    } else {
        const char* errorMsg = "Error reading the new date.\n";
        sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
    }
}

void getTime(void) // Gets the systems current time
{
    const char* time = "Current time:";
    sys_req(WRITE, COM1, time, strlen(time));
}

void setTime(void) // Sets the systems current time
{
    const char* setTimeMsg = "Enter the new time (hhmmss): ";
    sys_req(WRITE, COM1, setTimeMsg, strlen(setTimeMsg));

    char newTime[5] = {0};
    int nread = sys_req(READ, COM1, newTime, sizeof(newTime));

    

    if (nread > 0) {
        const char* successMsg = "Time has been set successfully!\n";
        sys_req(WRITE, COM1, successMsg, strlen(successMsg));
    } else {
        const char* errorMsg = "Error reading the new time.\n";
        sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
    }
}

void writeNewLine(void)
{
    const char* newLine = "\n";
    sys_req(WRITE, COM1, newLine, strlen(newLine));
}

void comhand(void)
{
    //New line
    const char* newLine = "\n";

  // Welcome message
    const char *asciiArt =
        RED     "Welcome to " RESET
        GREEN   "Bit Bandits' MPX!\n" RESET
        "         _________\n"
        "        / ======= \\\n"
        "       / __________\\\n"
        "      | ___________ |\n"
        "      | | -       | |\n"
        "      | |  CS450  | |\n"
        "      | |_________| |________________________\n"
        "      \\=____________/   Bit Bandits          )\n"
        YELLOW  "      / \"\"\"\"\"\"\"\"\"\"\"\"\" \\\n" RESET
        BLUE    "     / ::::::::::::: \\\n" RESET
        "    (_________________)\n"
        "\n";

    sys_req(WRITE, COM1, newLine, strlen(newLine));
    sys_req(WRITE, COM1, asciiArt, strlen(asciiArt));

    // Constants
    const char *comhandInitializeStr = " Comhand Initialized: Please write your preferred command.\n";
    const char *avaliableCommandStr = " Available Commands: \n\techo\n\tget time/date\n\thelp\n\tset time/date\n\tshutdown\n\tversion\n\tclear\n";

    sys_req(WRITE, COM1, comhandInitializeStr, strlen(comhandInitializeStr));
    sys_req(WRITE, COM1, avaliableCommandStr, strlen(avaliableCommandStr));

    for (;;)
    {
        char buf[100] = {0};
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        sys_req(WRITE, COM1, buf, nread);
        sys_req(WRITE, COM1, "\n");

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
