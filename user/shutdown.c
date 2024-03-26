

#include <user/shutdown.h>
#include <sys_req.h>
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
