
/*
 * Get and set the time for the operating system
 */

//#include "time.h"
#include "sys_req.h"
#include "string.h"
#include <mpx/io.h>
#include "stdlib.h"
#include <user/rtc_util.h>



// RTC register addresses for time
#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x02
#define RTC_HOURS 0x04

int readTimeReg(char sect){
    switch(sect){
        case 'h': return bcd_to_binary(read_rtc_register(RTC_HOURS));
        case 'm': return bcd_to_binary(read_rtc_register(RTC_MINUTES));
        case 's': return bcd_to_binary(read_rtc_register(RTC_SECONDS));
        default: return -1;
    }
}


/*
 * Gets the current time
 * Parameters: void
 * Returns: void
 */
void get_time(void) {
    char buffer[20];
    const char* currentTime = "Current time: ";
    sys_req(WRITE, COM1, currentTime, strlen(currentTime));
    itoa(bcd_to_binary(read_rtc_register(RTC_HOURS)), buffer);
    sys_req(WRITE, COM1, buffer, strlen(buffer));
    sys_req(WRITE, COM1, ":", 2);
    itoa(bcd_to_binary(read_rtc_register(RTC_MINUTES)), buffer);
    sys_req(WRITE, COM1, buffer, strlen(buffer));
    sys_req(WRITE, COM1, ":", 2);
    itoa(bcd_to_binary(read_rtc_register(RTC_SECONDS)), buffer);
    sys_req(WRITE, COM1, buffer, strlen(buffer));
    sys_req(WRITE, COM1, "\n", 2);
}

/*
 * Sets the time in RTC
 * Parameters: void
 * Returns: void
 */

void set_time(int hours, int minutes, int seconds) {
    // Convert time values to BCD
    hours = hours % 24; // Ensure hours are in the range 0-23
    minutes = minutes % 60; // Ensure minutes are in the range 0-60
    seconds = seconds % 60; // Ensures seconds are in the range 0-60
    int hours_bcd = ((hours / 10) << 4) | (hours % 10);
    int minutes_bcd = ((minutes / 10) << 4) | (minutes % 10);
    int seconds_bcd = ((seconds / 10) << 4) | (seconds % 10);

    // Set time in RTC
    outb(0x70, RTC_SECONDS); // Select seconds register
    outb(0x71, seconds_bcd); // Set seconds

    outb(0x70, RTC_MINUTES); // Select minutes register
    outb(0x71, minutes_bcd); // Set minutes

    outb(0x70, RTC_HOURS);   // Select hours register
    outb(0x71, hours_bcd);   // Set hours
}
