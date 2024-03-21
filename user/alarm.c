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

void displayMessage(char* message);

extern int readTimeReg(char sect);
extern void sys_req(int operation, ...);

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
            sys_req(WRITE, COM1, current->message, strlen(current->message));
            sys_req(WRITE, COM1, "\n", 1);

            if (prev == NULL) {
                alarmsHead = current->nxtAlarm;
                free(current);
                current = alarmsHead;
            } else {
                prev->nxtAlarm = current->nxtAlarm;
                free(current);
                current = prev->nxtAlarm;
            }
            continue;
        }
        prev = current;
        current = current->nxtAlarm;
    }
}

void addAlarm(int hour, int minute, int seconds, char* message) {
    if (alarmsHead != NULL) {
        int numAlarms = 1;
        struct alarm* current = alarmsHead;
        while (current->nxtAlarm != NULL) {
            current = current->nxtAlarm;
            numAlarms++;
        }
        if (numAlarms >= MAX_ALARMS) {
            displayMessage("Error: Maximum number of alarms reached.");
            return;
        }
    }

    struct alarm* newAlarm = (struct alarm*)malloc(sizeof(struct alarm));
    if (newAlarm == NULL) {
        displayMessage("Error: Unable to allocate memory for new alarm.");
        return;
    }

    newAlarm->hour = hour;
    newAlarm->minute = minute;
    newAlarm->seconds = seconds;
    strncpy(newAlarm->message, message, sizeof(newAlarm->message) - 1);
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

   /* while (current != NULL) {
        if (current->hour == hour && current->minute == minute && current->seconds == seconds) {
            if (prev == NULL) {
                alarmsHead = current->nxtAlarm;
    */          
}
