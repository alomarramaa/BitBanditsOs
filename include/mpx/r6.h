
#ifndef _R6_H
#define _R6_H

/**
 @file mpx/r6.h
 @brief Functions for R6
*/

/*
- Implement an interrupt-driven serial port driver to enable communication between our computer and console
- Sse COM1 port
*/

#include <string.h>
#include <mpx/library.h>
#include <mpx/device.h>
#include <sys_req.h>
#include <mpx/interrupts.h>
#include <stdint.h>
#include <mpx/io.h>
#include <mpx/serial.h>

typedef struct ring_buffer
{
    void *buffer;     // data buffer
    void *buffer_end; // end of data buffer
    size_t capacity;  // maximum number of items in the buffer
    size_t count;     // number of items in the buffer
    size_t size;        // size of each item in the buffer
    void *head;       // pointer to head
    void *tail;       // pointer to tail
} ring_buffer;

/*
IOCB
- Allows higher level software to access the device driver
- Represents active or pending IO requests
- Event flag must be provided
*/
typedef struct
{

    int is_open;              // A flag indicating whether the port is open (1 is open, 0 ic closed)
    int event_flag;           // Set to 0 at the beginning of an operation, and set to 1 to indicate when the operation is complete
    op_code status_code;       // A status code, with possible values idle, reading and writing
    uint8_t *inputBufferAddr; // Addresses and counters associated with the current input buffer
    uint32_t inputBufferCounter;
    
    uint8_t *outputBufferAddr; //Addresses and counters associated with the current output buffer
    uint32_t outputBufferCounter;
    uint32_t outputBufferLen;
   
   // Input ring buffer and associated indices/counters
   /*
 - Handle input devices that may deliver characters before a program is ready to receive them
 - it is a fixed size array, managed as a circular list
   */
    uint8_t inputRingBuffer[500];  // Assuming a buffer size of 500 bytes
    uint32_t inputIndex;
    uint32_t outputIndex;
    uint32_t ringBufferCounter;

    device dev;

} DCB;



int serial_open(device *dev, int speed);

int serial_close(device *dev);

int serial_read(device *dev, char *buf, size_t len);

int serial_write(device *dev, char *buf, size_t len);

void serial_interrupt(void);

void serial_input_interrupt(DCB *dcb);

void serial_output_interrupt(DCB *dcb);

#endif
