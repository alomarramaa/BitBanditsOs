
#include <user/version.h>
#include <sys_req.h>
/*
 * Prints the version of the MPX, as well as a compilation date.
 * Parameters: void
 * Returns: void
 */
void version(void) // Prints version and compile date
{
    const char *version = "\nMPX Version R4\n";
    const char *compileDate = "Compiled on: 3/22/24 \n\n";
    sys_req(WRITE, COM1, version, strlen(version));
    sys_req(WRITE, COM1, compileDate, strlen(compileDate));
}
