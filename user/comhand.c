
#include <string.h>
#include <sys_req.h>

#include <user/comhand.h>



void version(void) // Prints version and compile date
{
    const char *version = "MPX Version R1";
    const char *compileDate = "Compiled on: ";
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
                           "8. Set Time -  Set time to desired hour/minute/second\n";


    sys_req(WRITE, COM1, helpText, strlen(helpText));
}

int shutdown(void)
{
    sys_req(WRITE, COM1, "Are you sure you want to shut down? (y/n)\n"); // Confirmation to shut down

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


void comhand(void)
{
    /* THE BONUS THING FOR CREATIVE STARTUP
         _________
        / ======= \
       / __________\
      | ___________ |
      | | -       | |
      | |  CS450  | |
      | |_________| |________________________
      \=____________/   Bit Bandits          )
      / """"""""""" \                       /
     / ::::::::::::: \                  =D-'
    (_________________)
     */

    //Constants
    const char* comhandInitializeStr = "Comhand Initialized: Please write your preferred command\n";
    const char* avaliableCommandStr = "Available commands: \n\techo\n\tget\n\thelp\n\tset\n\tshutdown\n\tversion\n";

    sys_req(WRITE, COM1, comhandInitializeStr, strlen(comhandInitializeStr));
    sys_req(WRITE, COM1, avaliableCommandStr, strlen(avaliableCommandStr));

    for (;;)
    {
        char buf[100] = {0};
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        sys_req(WRITE, COM1, buf, nread);

        if (strcmp(buf, "shutdown") == 0) // Shutdown Command
        {
            sys_req(WRITE, COM1, "Shutdown confirmed.");
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
        // Add other commands here if needed

        /*

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
                */
    }
}
