
#include <stdio.h>
#include <string.h>

#define READ 1
#define WRITE 2
#define COM1 3

void comhand(void)
{

    //Basic command handler format
    for (;;)
    {
        char buf[100] = {0};
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        sys_req(WRITE, COM1, buf, nread);

        if (strcmp(buf, "shutdown") == 0)
        {
            sys_req(WRITE, COM1, "Shutdown confirmed.");
            return 0;
        }
    }
}
