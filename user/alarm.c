#include <user/alarm.h>
#include "sys_req.h"
#include "mpx/pcb.h"
#include "user/time.h"
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <mpx/io.h> 

#define MAX_ALARMS 5

extern int readTimeReg(char sect);
extern int sys_req(op_code operation, ...);

struct alarm *alarmsHead = NULL;

void checkAlarm() {
    struct alarm* current = alarmsHead;
    struct alarm* prev = NULL;

    while (current != NULL) {
        int hours = readTimeReg('h');
        int minutes = readTimeReg('m');
        int seconds = readTimeReg('s');

        if ((hours > current->hour) || 
            (hours == current->hour && minutes > current->minute) ||
            (hours == current->hour && minutes == current->minute && seconds >= current->seconds)) {

           
            sys_req(WRITE, COM1, "Alarm: ", strlen("Alarm: "));
            sys_req(WRITE, COM1, current->hour, 2); 
            sys_req(WRITE, COM1, ":", 1);
            sys_req(WRITE, COM1, current->minute, 2); 
            sys_req(WRITE, COM1, ":", 1);
            sys_req(WRITE, COM1, current->seconds, 2); 
            sys_req(WRITE, COM1, ", ", 2);
            sys_req(WRITE, COM1, current->message, strlen(current->message)); 
            sys_req(WRITE, COM1, "\n", 1);

            if (prev == NULL) {
                alarmsHead = current->nxtAlarm;
                sys_free_mem(current);
                current = alarmsHead;
            } else {
                prev->nxtAlarm = current->nxtAlarm;
                sys_free_mem(current);
                current = prev->nxtAlarm;
            }
            continue;
        }
        prev = current;
        current = current->nxtAlarm;
    }

    sys_req(WRITE, COM1, "No alarms triggered.\n", strlen("No alarms triggered.\n"));
}

void addAlarm(int hour, int minute, int seconds, char* message) {
    // Validate hours, minutes, and seconds inputs
    if (hour < 0 || hour >= 24) {
        sys_req(WRITE, COM1, "Error: Invalid hours. Hours should be in the range 0-23.\n", strlen("Error: Invalid hours. Hours should be in the range 0-23.\n"));
        return;
    }
    if (minute < 0 || minute >= 60) {
        sys_req(WRITE, COM1, "Error: Invalid minutes. Minutes should be in the range 0-59.\n", strlen("Error: Invalid minutes. Minutes should be in the range 0-59.\n"));
        return;
    }
    if (seconds < 0 || seconds >= 60) {
        sys_req(WRITE, COM1, "Error: Invalid seconds. Seconds should be in the range 0-59.\n", strlen("Error: Invalid seconds. Seconds should be in the range 0-59.\n"));
        return;
    }

    if (alarmsHead != NULL) {
        int numAlarms = 1;
        struct alarm* current = alarmsHead;
        while (current->nxtAlarm != NULL) {
            current = current->nxtAlarm;
            numAlarms++;
        }
        if (numAlarms >= MAX_ALARMS) {
            sys_req(WRITE, COM1, "Error: Maximum number of alarms reached.\n", strlen("Error: Maximum number of alarms reached.\n"));
            return;
        }
    }

    struct alarm* newAlarm = (struct alarm*)sys_alloc_mem(sizeof(struct alarm));
    if (newAlarm == NULL) {
        sys_req(WRITE, COM1, "Error: Unable to allocate memory for new alarm.\n", strlen("Error: Unable to allocate memory for new alarm.\n"));
        return;
    }

    newAlarm->hour = hour;
    newAlarm->minute = minute;
    newAlarm->seconds = seconds;
    memcpy(newAlarm->message, message, sizeof(newAlarm->message) - 1);
    newAlarm->message[sizeof(newAlarm->message) - 1] = '\0';
    newAlarm->nxtAlarm = NULL;

    if (alarmsHead == NULL) {
        alarmsHead = newAlarm;
    } else {
        struct alarm* current = alarmsHead;
        while (current->nxtAlarm != NULL) {
            current = current->nxtAlarm;
        }
        current->nxtAlarm = newAlarm;
    }
}

void removeAlarm(int hour, int minute, int seconds) {
    struct alarm* current = alarmsHead;
    struct alarm* prev = NULL;

    while (current != NULL) {
        if (current->hour == hour && current->minute == minute && current->seconds == seconds) {
            if (prev == NULL) {
                alarmsHead = current->nxtAlarm;
                sys_free_mem(current);
                return; 
            } else {
                prev->nxtAlarm = current->nxtAlarm;
                sys_free_mem(current);
                return; 
            }
        }
        prev = current;
        current = current->nxtAlarm;
    }

    
    sys_req(WRITE, COM1, "Error: Alarm not found.\n", strlen("Error: Alarm not found.\n"));
}
