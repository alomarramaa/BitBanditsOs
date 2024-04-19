#include <mpx/r6.h>
#include <mpx/device.h>
#include <mpx/io.h>
#include <stddef.h>
#include <mpx/serial.h>

// Defining register addresses

#define line_control_register 0x3F8;
#define baud_rate_devisor_lsb 0x3F8;
#define baud_rate_devisor_msb ;
#define modem_control_register ;
#define interrupt_enable_register ;
#define pic_mask_register 0x21;

int serial_open(device dev, int speed)
{

    if (dev == NULL)
    {
        return -1; // invalid device parameter
    }
    else
    {
        serial_init(dev);
    }

    if (speed <= 0)
    {
        return -2; // Invalid speed parameter
    }
    
    //Ensure that the parameters are valid, and that the device is not currently open.

    //2. Initialize the DCB. In particular, this should include indicating that the device is open, setting the event flag to 0, and setting the initial device status to idle. In addition, the ring buffer parameters must be initialized.
    //enum sys_req.h
    op_code code = IDLE;
    struct dcb dcb = {1, 0, code};

    //3. Install the new handler in the interrupt vector. 
    //idt_install(0x23, handler);



    // Compute the required baud rate divisor.
    int baud_divisor = (int)(115200 / speed);

    /*

     7. Store the value 0x03 in the Line Control Register. This sets the line characteristics to 8 data bits, 1 stop bit, and no parity. It also restores normal functioning of the first two ports.
     8. Enable the appropriate level in the PIC mask register.
     9. Enable overall serial port interrupts by storing the value 0x08 in the Modem Control register.
     10. Enable input ready interrupts only by storing the value 0x01 in the Interrupt Enable register*/
}


    int serial_close(device dev)
{

    /*Ensure that the port is currently open.
2. Clear the open indicator in the DCB.
3. Disable the appropriate level in the PIC mask register.
4. Disable all interrupts in the ACC by loading zero values to the Modem Status register and the Interrupt Enable register.*/
}

int serial_read(device dev, char *buf, size_t len)
{
}

int serial_write(device dev, char *buf, size_t len)
{
}

void serial_interrupt(void)
{
}

void serial_input_interrupt(struct dcb *dcb)
{
}

void serial_output_interrupt(struct dcb *dcb)
{
}
