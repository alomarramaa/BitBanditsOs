
/*
 * Get and set the date for the operating system
 */

// #include <user/date.h>
#include "sys_req.h"
#include "string.h"
#include <mpx/io.h>
#include "stdlib.h"
#include <user/rtc_util.h>


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


int readDateReg(char sect){
    switch(sect){
        case 'm': return bcd_to_binary(read_rtc_register(RTC_MONTH));
        case 'd': return bcd_to_binary(read_rtc_register(RTC_DAY));
        case 'y': return bcd_to_binary(read_rtc_register(RTC_YEAR));
        default: return -1;
    }
}

// Function to get and display the current date
void get_date(void) {
    char buffer[20];
    const char* currentDate = "Current date: ";
    sys_req(WRITE, COM1, currentDate, strlen(currentDate));
    itoa(bcd_to_binary(read_rtc_register(RTC_MONTH)), buffer);
    sys_req(WRITE, COM1, buffer, strlen(buffer));
    sys_req(WRITE, COM1, "/", 2);
    itoa(bcd_to_binary(read_rtc_register(RTC_DAY)), buffer);
    sys_req(WRITE,COM1,buffer,strlen(buffer));
    sys_req(WRITE, COM1, "/", 2);
    itoa(bcd_to_binary(read_rtc_register(RTC_YEAR)), buffer);
    sys_req(WRITE, COM1, buffer, strlen(buffer));
    sys_req(WRITE, COM1, "\n", 2);

}

// Function to set the date in RTC
void set_date(int month, int day, int year) {
    // Convert date values to BCD
    month = month % 12; // Ensure the month is within the range of 0-12
    day = day % 31; //Ensure the day is within the range of 0-31
    int month_bcd = ((month / 10) << 4) | (month % 10);
    int day_bcd = ((day / 10) << 4) | (day % 10);
    int year_bcd = ((year / 10) << 4) | (year % 10);


    // Set date in RTC
    outb(0x70,RTC_MONTH); // Select month register
    outb(0x71,month_bcd); // Set month

    outb(0x70,RTC_DAY); // Select day register
    outb(0x71,day_bcd); // Set day

    outb(0x70,RTC_YEAR); // Select year register
    outb(0x71,year_bcd); // Set year
}
