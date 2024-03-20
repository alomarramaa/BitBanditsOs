#include <user/alarm.h>
#include "sys_req.h"
#include "mpx/pcb.h"
#include "user/time.h"
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>

void checkAlarm();

void showAlarms();

void createAlarm(){
    struct pcb *alarmPCB = pcb_setup("AlarmProccess", 0, 0);
    struct context* alarmContext = (struct context*)&alarmPCB->prev_pcbPtr[1023-sizeof(struct context)];
    alarmPCB->process_priority= 1;
    alarmContext->CS = 0x08;
    alarmContext->DS = 0x10;
    alarmContext->ES = 0x10;
    alarmContext->FS = 0x10;
    alarmContext->GS = 0x10;
    alarmContext->SS = 0x10;
    alarmContext->EBP = (uint32_t)alarmPCB->prev_pcbPtr;
    alarmContext->ESP = (uint32_t)alarmPCB->next_pcbPtr;
    alarmContext->EIP = (uint32_t)checkAlarm;
    alarmContext->EFLAGS = 0x0202;
    alarmContext->EDI = 0;
    alarmContext->ESI = 0;
    alarmContext->EDX = 0;
    alarmContext->ECX = 0;
    alarmContext->EBX = 0;
    alarmContext->EAX = 0;

    pcb_insert(alarmPCB);
}

void addAlarm(int hour, int minute, int seconds, char* message);
