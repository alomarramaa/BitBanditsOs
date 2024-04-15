#ifndef _R6_H
#define _R6_H

/**
 @file mpx/r6.h
 @brief Functions for R6
*/

#include <string.h>
#include <mpx/library.h>
#include <mpx/device.h>

struct dcb
{

    int port_flag;   // A flag indicating whether the port is open (1 is open, 0 ic closed)
    int event_flag;  // Set to 0 at the beginning of an operation, and set to 1 to indicate when the operation is complete
    int status_code; // A status code, with possible values idle, reading and writing
    /*
    • Addresses and counters associated with the current input buffer
    • Addresses and counters associated with the current output buffer
    • An array to be used as the input ring buffer, with associated input index, output index, and counter*/
} dcb;

int serial_open(device dev, int speed);

int serial_close(device dev);

int serial_read(device dev, char *buf, size_t len);

int serial_write(device deb, char *buf, size_t len);

void serial_interrupt(void);

void serial_input_interrupt(struct dcb *dcb);

void serial_output_interrupt(struct dcb *dcb);

#endif
