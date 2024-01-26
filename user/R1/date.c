#include "date.h"
#include "sys_req.h"
#include "string.h"
#include <mpx/io.h>
#include "stdlib.h"
#include "rtc_util.h"


// RTC register addresses for time
#define RTC_MONTH 0x07
#define RTC_DAY 0x08
#define RTC_YEAR 0x09

// Function to read a byte from RTC register
int read_rtc_register(int reg) {
    outb(0x70,reg); // Use 0x70 for RTC register selection
    return inb(0x71); // Use 0x71 for RTC data port
}

// Function to convert BCD to binary
int bcd_to_binary(int bcd) {
    int tens = (bcd >> 4) & 0x0F; // Extract the tens column
    int ones = bcd & 0x0F;       // Extract the ones column
    return (tens * 10) + ones;    // Convert and combine tens and ones
}


int readTimeReg(char sect){
    switch(sect){
        case 'h': return bcd_to_binary(read_rtc_register(RTC_HOURS));
        case 'm': return bcd_to_binary(read_rtc_register(RTC_MINUTES));
        case 's': return bcd_to_binary(read_rtc_register(RTC_SECONDS));
        default: return -1;
    }
}

// Function to get and display the current time
void display_current_time() {
    char buffer[20];
    sys_req(WRITE,COM1,"Current time: ",15);
    itoa(bcd_to_binary(read_rtc_register(RTC_HOURS)), buffer);
    sys_req(WRITE,COM1,buffer,strlen(buffer));
    sys_req(WRITE,COM1,": ",3);
    itoa(bcd_to_binary(read_rtc_register(RTC_MINUTES)), buffer);
    sys_req(WRITE,COM1,buffer,strlen(buffer));
    sys_req(WRITE,COM1,": ",3);
    itoa(bcd_to_binary(read_rtc_register(RTC_SECONDS)), buffer);
    sys_req(WRITE,COM1,buffer,strlen(buffer));
    sys_req(WRITE,COM1,"\n",2);

}

// Function to set the time in RTC
void set_time(int hours, int minutes, int seconds) {
    // Convert time values to BCD
    hours = hours % 24; // Ensure hours are in the range 0-23
    int hours_bcd = ((hours / 10) << 4) | (hours % 10);
    int minutes_bcd = ((minutes / 10) << 4) | (minutes % 10);
    int seconds_bcd = ((seconds / 10) << 4) | (seconds % 10);


    // Set date in RTC
    outb(0x70,RTC_SECONDS); // Select month register
    outb(0x71,seconds_bcd); // Set month

    outb(0x70,RTC_MINUTES); // Select day register
    outb(0x71,minutes_bcd); // Set day

    outb(0x70,RTC_HOURS);   // Select year register
    outb(0x71,hours_bcd);   // Set year

}