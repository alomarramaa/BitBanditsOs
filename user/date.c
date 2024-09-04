
/*
 * Get and set the date for the operating system
 */

// #include <user/date.h>
#include "sys_req.h"
#include "string.h"
#include <mpx/io.h>
#include "stdlib.h"
#include <user/rtc_util.h>
#include <mpx/interrupts.h>


// RTC register addresses for time
#define RTC_DAY 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09

// Function to read a byte from RTC register
int read_rtc_register(int reg) {
    outb(0x70, reg); // Use 0x70 for RTC register selection
    return inb(0x71); // Use 0x71 for RTC data port
}

// Function to convert BCD to binary
int bcd_to_binary(int bcd) {
    int thousands = (bcd >> 12) & 0x0F; // Extract the thousands column
    int hundreds = (bcd >> 8) & 0x0F;   // Extract the hundreds column
    int tens = (bcd >> 4) & 0x0F;       // Extract the tens column
    int ones = bcd & 0x0F;              // Extract the ones column

    return thousands * 1000 + hundreds * 100 + tens * 10 + ones;
}

// Function to convert Binary to BCD (Binary Coded Decimal)
int binary_to_bcd(int binary) {
    int thousands = binary / 1000;
    int hundreds = (binary / 100) % 10;
    int tens = (binary / 10) % 10;
    int ones = binary % 10;

    return (thousands << 12) | (hundreds << 8) | (tens << 4) | ones;
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
/*
Writes the appropriate value to the RTC index register (0x70) using outb()
+ read the result from 0x71 using inb()
*/
void get_date(void) {
    char mbuffer[20];
    char dbuffer[20];
    char ybuffer[20];
    const char* currentDate = "Current date: ";
    sys_req(WRITE, COM1, currentDate, strlen(currentDate));
    itoa(readDateReg('m'), mbuffer);
    sys_req(WRITE, COM1, mbuffer, strlen(mbuffer));
    sys_req(WRITE, COM1, "/", 2);
    itoa(readDateReg('d'), dbuffer);
    sys_req(WRITE,COM1, dbuffer, strlen(dbuffer));
    sys_req(WRITE, COM1, "/", 2);
    itoa(readDateReg('y'), ybuffer);
    // Ensures consistent output for years ending in 00 to 09
    if (atoi(ybuffer) < 10)
    {
        sys_req(WRITE, COM1, "0", strlen("0"));
    }
    sys_req(WRITE, COM1, ybuffer, strlen(ybuffer));
    sys_req(WRITE, COM1, "\n", 2);
}

// Function to get the number of days in a month
int days_in_month(int month, int year) {
    switch(month) {
        case 2:
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
                return 29; // February has 29 days in a leap year
            else
                return 28; // February has 28 days in a non-leap year
        case 4:
        case 6:
        case 9:
        case 11:
            return 30; // April, June, September, November have 30 days
        default:
            return 31; // All other months have 31 days
    }
}

// Function to set the date in RTC with data validation
/*
Disable interrupts (sti())
  + write the appropriate value to the RTC index register using outb
  + write a new value to 0x71 using outb, enable interrupts (cli())
*/
void set_date(int month, int day, int year) {
    // Validate month
    if (month < 1 || month > 12) {
        // Invalid month
        const char* errorMsg = "Error: Invalid month.\n";
        sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
        return;
    }

    // Validate day
    int max_days = days_in_month(month, year);
    if (day < 1 || day > max_days) {
        // Invalid day
        const char* errorMsg = "Error: Invalid day for this month.\n";
        sys_req(WRITE, COM1, errorMsg, strlen(errorMsg));
        return;
    }

    // Convert date values to BCD
    int month_bcd = binary_to_bcd(month);
    int day_bcd = binary_to_bcd(day);

    // Convert year to BCD and ensure it's a 4-digit year
    year = year % 9999;  // Assuming the RTC uses a two-digit year representation
    int year_bcd = binary_to_bcd(year);

    // Disable interrupts
    cli();

    // Set date in RTC
    outb(0x70, RTC_MONTH); // Select month register
    outb(0x71, month_bcd); // Set month

    outb(0x70, RTC_DAY);   // Select day register
    outb(0x71, day_bcd);   // Set day

    outb(0x70, RTC_YEAR);  // Select year register
    outb(0x71, year_bcd);  // Set year

    // Enable interrupts
    sti();
}
