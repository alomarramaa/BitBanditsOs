
#include <string.h>
#include <sys_req.h>

#define READ 1
#define WRITE 2
#define COM1 3

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
                           "1. Shutdown - Shut down the system"
                           "2. Version - Display the current version & compilation date"
                           "3. Help - Display all available commands"
                           "4. Echo - Repeats previous message"
                           "5. Get Date - Display current date"
                           "6. Get Time -  Display current time"
                           "7. Set Date - Set date to desired month/day/year"
                           "8. Set Time -  Set time to desired hour/minute/second";
                           
    sys_req(WRITE, COM1, helpText, strlen(helpText));
}

int shutdown(void) // Shutdown the program
{
    {

        sys_req(WRITE, COM1, "Are you sure you want to shut down? (y/n)"); // Confirmation to shut down

        char confirm[50] = {0};
       // int nread = sys_req(READ, COM1, confirm);
        sys_req(READ, COM1, confirm);

        if (strcmp(confirm, "y") == 0) // shutdown confirmed

        {
            sys_req(WRITE, COM1, "Shutdown confirmed.");
            return 1;
        }

        else // Cancel shutdpwn
        {
            sys_req(WRITE, COM1, "Shutdown canceled.\n");
            return 0;
        }
    }
}

void comhand(void)
{
/* THE BONUS THING FOR CREATIVE STATUP
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

    sys_req(WRITE, COM1,"Comhand Initialized: Please write your preferred command\n",56);
    sys_req(WRITE, COM1,"Available commands: \n\techo\n\tget\n\thelp\n\tset\n\tshutdown\n\tversion\n",70);
    
    for (;;)
    {
        char buf[100] = {0};
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        sys_req(WRITE, COM1, buf, nread);

        if (strcmp(buf, "shutdown") == 0) // Shutdown Command
        {
            sys_req(WRITE, COM1, "Shutdown confirmed.");
            shutdown();
        }

        if (strcmp(buf, "version") == 0) // Version Command

        {
            version();
        }

        else if (strcmp(buf, "help") == 0) // Help Command
        {
            help();
        }

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
