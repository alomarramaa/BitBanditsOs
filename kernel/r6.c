#include <mpx/r6.h>
#include <device.h>
#include <mpx/io.h>


serial_open(COM1, int speed){

    /* Ensure that the parameters are valid, and that the device is not currently open.
2. Initialize the DCB. In particular, this should include indicating that the device is open, setting the event flag to 0, and setting the initial device status to idle. In addition, the ring buffer parameters must be initialized.
13
3. Install the new handler in the interrupt vector.
4. Compute the required baud rate divisor.
5. Store the value 0x80 in the Line Control Register. This allows the first two port addresses to access the Baud Rate Divisor register.
6. Store the high order and low order bytes of the baud rate divisor into the MSB and LSB registers, respectively.
7. Store the value 0x03 in the Line Control Register. This sets the line characteristics to 8 data bits, 1 stop bit, and no parity. It also restores normal functioning of the first two ports.
8. Enable the appropriate level in the PIC mask register.
9. Enable overall serial port interrupts by storing the value 0x08 in the Modem Control register.
10. Enable input ready interrupts only by storing the value 0x01 in the Interrupt Enable register*/
}

serial_close(COM1){
    /*Ensure that the port is currently open.
2. Clear the open indicator in the DCB.
3. Disable the appropriate level in the PIC mask register.
4. Disable all interrupts in the ACC by loading zero values to the Modem Status register and the Interrupt Enable register.*/
}

serial_read(COM1, char* buf, size_t len){

}

serial_write(COM1, char* buf, size_t len){

}

void serial_interrupt(void){

}

void serial_input_interrupt(struct dcb *dcb){

}

void serial_output_interrupt(struct dcb *dcb){
    
}