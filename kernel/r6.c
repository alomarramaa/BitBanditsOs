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
    // Ensure that the parameters are valid, and that the device is not currently open

    if (dev == NULL || speed <= 0)
    {
        return -1; // Invalid parameters
    }
    // Initialize the DCB and ring buffer parameters
    DCB *dcb;
    dcb->is_open = 1;        // Port is open initially
    dcb->event_flag = 0;     // Event flag set to 0 initially
    dcb->status_code = IDLE; // Status code set to idle initially
    dcb->inputBufferAddr = NULL;
    dcb->inputBufferCounter = 0;
    dcb->outputBufferAddr = NULL;
    dcb->outputBufferCounter = 0;
    dcb->inputIndex = 0;
    dcb->outputIndex = 0;
    dcb->ringBufferCounter = 0;

    //  Install the new handler in the interrupt vector.
    idt_install(0x23, handler);

    // Compute the required baud rate divisor.
    int baud_divisor = (int)(115200 / speed);

    // Store 0x80 in the Line Control Register
    *((volatile uint16_t *)0x3FB) = 0x80;

    // Store the high order and low order bytes of the baud rate divisor into the MSB and LSB registers

    //  Store the value 0x03 in the Line Control Register.
    *((volatile uint16_t *)0x3FB) = 0x03;

    // Enable the appropriate level in the PIC mask register. Not sure if this is right, from the doc
    int mask = inb(0x21);
    mask &= ~(1 << 3); // (enable level 3)
    outb(0x21, mask);

    // Enable overall serial port interrupts by storing the value 0x08 in the Modem Control register
    *((volatile uint16_t *)0x3FC) |= 0x08;

    // Enable input ready interrupts only by storing the value 0x01 in the Interrupt Enable register
    *((volatile uint16_t *)0x3FA) |= 0x01;
}

int serial_close(device dev) // Not sure if this is right!
{
    DCB *dcb;
    if (dcb->is_open = 1) // Ensure that the port is currently open.
    {
        // Clear the open indicator in the DCB.
        dcb->is_open = 0;

        // Disable the appropriate level in the PIC mask register
        cli();
        int mask = inb(0x21);
        mask |= (1 << 3); // Mask (disable) IRQ level 3
        outb(0x21, mask);
        sti();
        // Disable all interrupts in the ACC by loading zero values to the Modem Status register and the Interrupt Enable register.*/
        *((volatile uint16_t *)0x3FC) = 0x00; // Modem Status register
        *((volatile uint16_t *)0x3FA) = 0x00; // Interrupt Enable register

        return 0; // Success
    }
    return -201; // Serial port not open
}

int serial_read(device dev, char *buf, size_t len)
{

    DCB *dcb;
    if (dev == NULL || buf == NULL || len <= 0)
    {
        return -1; // Invalid parameters
    }

    if (dcb->event_flag = 1 && dcb->status_code == IDLE) // Ensure that the port is open, and the status is idle
    {

        // Initialize the input buffer variables (not the ring buffer!)

        dcb->status_code == READING; // Set the status to reading

        dcb->event_flag = 0; // Clear the caller’s event flag.

        /*Copy characters from the ring buffer to the requestor’s buffer, until the ring buffer is emptied, the requested count has been reached, or a new-line (ENTER) code has been found.
         The copied characters should, of course, be removed from the ring buffer.
         Either input interrupts or all interrupts should be disabled during the copying. */

        // If more characters are needed, return. If the block is complete, continue with step 7.

        dcb->status_code == IDLE; // Reset the DCB status to idle,
        dcb->event_flag = 1;      // Set the event flag

        // return the actual count to the requestor’s variable.
    }
}

int serial_write(device dev, char *buf, size_t len)
{
    DCB *dcb;

    if (dev == NULL)
    {
        return -1; // Invalid parameters
    }

    if (dcb->is_open = 0)
    {
        return -401; // Serial port is not open
    }

    if (buf == NULL)
    {
        return -402; // Invalid buffer address
    }

    if (len <= 0)
    {
        return -403; // Invalid count
    }

    if (dcb->status_code != IDLE)
    {
        return -404; // Device is busy
    }

    if (dcb->is_open = 1 || dcb->status_code == IDLE) // Ensure that the port is currently open and idle.
    {
        // Install the buffer pointer and counters in the DCB
        dcb->outputBufferAddr = (uint8_t *)buf;
        dcb->outputBufferCounter = len;

        dcb->status_code == WRITING; // Set the current status to writing.
        dcb->event_flag = 0;         // Clear the caller’s event flag

        // Get the first character from the requestor’s buffer and store it in the output register.
        *((volatile uint8_t *)0x3F8) = *dcb->outputBufferAddr;

        // Enable write interrupts by setting bit 1 of the Interrupt Enable register
        //  This must be done by setting the register to the logical OR of its previous contents and 0x02.
        *((volatile uint8_t *)0x3FA) |= 0x02;

        return 0; // Success
    }
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
