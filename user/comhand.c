
#include <stdio.h>
#include <string.h>

#define READ 1
#define WRITE 2
#define COM1 3

// Prints version and compile date
void version()
{
    const char *version = "MPX Version R1";
    const char *compileDate = "Compiled on: ";
    sys_req(WRITE, COM1, version, strlen(version));
    sys_req(WRITE, COM1, compileDate, strlen(compileDate));
}

void help()
{
    const char *helpText = "Avaliable Commands: \n"
                           "1. Shutdown - Shut down the system"
                           "2. Version - Display the current version & compilation date"
                           "3. Help - Display all avaliable commands"
                           "4. Get Date - Display current date"
                           "5. Get Time -  Display current time"
                           "6. Set Date - Set date to desired month/day/year"
                           "7. Set Time -  Set time to desired hour/minute/second";
                           
    sys_req(WRITE, COM1, helpText, strlen(helpText));
}

void shutdown()
{
    {
        // Confirmation to shut down
        sys_req(WRITE, COM1, "Are you sure you want to shut down? (y/n)");

        char confirm[50] = {0};
        int nread = sys_req(READ, COM1, confirm);

        if (strcmp(confirm), "y" == 0)
        {
            sys_req(WRITE, COM1, "Shutdown confirmed.");
            return;
        }

        else
        {
            sys_req(WRITE, COM1, "Shutdown canceled.\n");
        }
    }
}

void getDate(){

}

void setDate(){

}

void getTime(){

}

void setTime(){
    
}

void comhand(void)
{

    // Basic command handler format
    // Big if/else statement, split/check input, match and run method for command
    // Delete and arrow keys use ANSI codes
    // Menu style
    for (;;)
    {
        char buf[100] = {0};
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        sys_req(WRITE, COM1, buf, nread);

        // Shutdown Command
        if (strcmp(buf, "shutdown") == 0)
        {
            // Needs to exit the command handler loop
            // Execution will return to kmain() & system will halt
            // Must ask for confirmation

            sys_req(WRITE, COM1, "Shutdown confirmed.");
            shutdown();
            return 0;
        }
        // Version Command
        //  Prints current version of MPX & compilation date (in this case, R1)

        else if (strcmp(buf, "version" == 0))
        {
            version();
        }

        // Help Command
        //  Provides usage instructions for all commands
        //  Strings need to be hardcoded

        else if (strcmp(buf, "help") == 0)
        {
            help();
        }

        // Get/Set Date Command

        // Get/Set Time Command
    }
}
