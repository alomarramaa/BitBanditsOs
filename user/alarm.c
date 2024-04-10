#include <user/alarm.h>
#include <sys_req.h>
#include <mpx/pcb.h>
#include <user/time.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <user/rtc_util.h>

#define MAX_ALARMS 5

int readTimeReg(char sect);

struct alarm alarms[MAX_ALARMS];
int numAlarms = 0;

// Function to check if any alarms are triggered
void checkAlarm(void) {
    int hours = readTimeReg('h');
    int minutes = readTimeReg('m');
    int seconds = readTimeReg('s');

    // Find if any alarm is triggered at the current time
    int index = findAlarmIndex(hours, minutes, seconds);
    if (index != -1) {
        // Display the triggered alarm message
        sys_req(WRITE, COM1, "Alarm triggered at ", strlen("Alarm triggered at "));
        sys_req(WRITE, COM1, hours, 2); 
        sys_req(WRITE, COM1, ":", 1);
        sys_req(WRITE, COM1, minutes, 2); 
        sys_req(WRITE, COM1, ":", 1);
        sys_req(WRITE, COM1, seconds, 2); 
        sys_req(WRITE, COM1, ", ", 2);
        sys_req(WRITE, COM1, alarms[index].message, strlen(alarms[index].message)); 
        sys_req(WRITE, COM1, "\n", 1);

        // Remove the triggered alarm 
        removeAlarm(index);
    }

    if (numAlarms == 0) {
        sys_req(WRITE, COM1, "No alarms triggered.\n", strlen("No alarms triggered.\n"));
    }
}
// Function to create a new alarm
void createAlarm(int hour, int minute, int seconds, char* message) {
    if (numAlarms >= MAX_ALARMS) {
        sys_req(WRITE, COM1, "Error: Maximum number of alarms reached.\n", strlen("Error: Maximum number of alarms reached.\n"));
        return;
    }
    // Store the alarm details
    alarms[numAlarms].hour = hour;
    alarms[numAlarms].minute = minute;
    alarms[numAlarms].seconds = seconds;

    // Construct the message
    char timeStr[7];
    timeStr[0] = (hour / 10) + '0';
    timeStr[1] = (hour % 10) + '0';
    timeStr[2] = ':';
    timeStr[3] = (minute / 10) + '0';
    timeStr[4] = (minute % 10) + '0';
    timeStr[5] = '\0';

    sys_req(WRITE, COM1, "Alarm: ", strlen("Alarm: "));
    sys_req(WRITE, COM1, timeStr, strlen(timeStr));
    sys_req(WRITE, COM1, ", ", 2);
    sys_req(WRITE, COM1, message, strlen(message)); 
    sys_req(WRITE, COM1, "\n", 1);

    // Copy message 
    for (size_t i = 0; i < sizeof(alarms[numAlarms].message) - 1 && message[i] != '\0'; ++i) {
        alarms[numAlarms].message[i] = message[i];
    }
    alarms[numAlarms].message[sizeof(alarms[numAlarms].message) - 1] = '\0';

    numAlarms++;
}

// Function to remove an alarm
void removeAlarm(int index) {
    // Check if the index is valid
    if (index < 0 || index >= numAlarms) {
        sys_req(WRITE, COM1, "Error: Invalid alarm index.\n", strlen("Error: Invalid alarm index.\n"));
        return;
    }

    numAlarms--;
    if (index < numAlarms) {
        alarms[index] = alarms[numAlarms];
    }
}

// Function to set a new alarm
void setAlarm(char* hours, char* minutes, char* seconds) {
    // Validate hours, minutes, and seconds inputs
    int hour = atoi(hours);
    int minute = atoi(minutes);
    int second = atoi(seconds);

    if (hour < 0 || hour >= 24 || minute < 0 || minute >= 60 || second < 0 || second >= 60) {
        sys_req(WRITE, COM1, "Error: Invalid time format.\n", strlen("Error: Invalid time format.\n"));
        return;
    }

    // Construct the name string
    char name[7];
    name[0] = (hour / 10) + '0';
    name[1] = (hour % 10) + '0';
    name[2] = (minute / 10) + '0';
    name[3] = (minute % 10) + '0';
    name[4] = (second / 10) + '0';
    name[5] = (second % 10) + '0';
    name[6] = '\0';

     // Create a new PCB for the alarm process
    pcb* alarmprocess = pcb_setup(name, 2, 4);

     // Context for the alarm process
    context* context1 = (context*) alarmprocess->stackPtr;
    context1->CS = 0x08;
    context1->GS = 0x10;
    context1->FS = 0x10;
    context1->ES = 0x10;
    context1->DS = 0x10;
    context1->SS = 0x10;
    context1->EBP = (uint32_t) alarmprocess->pcb_stack;
    context1->ESP = (uint32_t) alarmprocess->stackPtr;
    context1->EIP = (uint32_t) checkAlarm;
    context1->EFLAGS = 0x0202;
    context1->EAX = 0x00;
    context1->EBX = 0x00;
    context1->ECX = 0x00;
    context1->EDX = 0x00;
    context1->EDI = 0x00;
    context1->ESI = 0x00;
    pcb_insert(alarmprocess);
}
// Function to find the index of an alarm at a specific time
int findAlarmIndex(int hour, int minute, int seconds) {
    for (int i = 0; i < numAlarms; i++) {
        if (alarms[i].hour == hour && alarms[i].minute == minute && alarms[i].seconds == seconds) {
            return i; // Return the index if alarm is found
        }
    }
    return -1; // Return -1 if alarm is not found
}
