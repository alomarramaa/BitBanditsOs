#include <mpx/r6.h>
#include <mpx/device.h>
#include <mpx/io.h>
#include <stddef.h>
#include <mpx/serial.h>

extern void serial_isr(void *);

// Defining register addresses

#define line_control_register 0x3F8
#define baud_rate_devisor_lsb 0x3F8
#define baud_rate_devisor_msb 0x3F9
#define modem_control_register 0x3FC
#define interrupt_enable_register 0x3FA
#define pic_mask_register 0x21

int serial_open(device *dev, int speed)

{
    // Ensure that the parameters are valid, and that the device is not currently open

    if (dev == NULL || speed <= 0)
    {
        return -1; // Invalid parameters
    }
    // Initialize the DCB and ring buffer parameters
    DCB *dcb = {0};
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
    idt_install(0x23, serial_isr);

    // Compute the required baud rate divisor.
    // int baud_divisor = (int)(115200 / speed);

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
    return 0;
}

int serial_close(device *dev)
{

    if (dev == NULL)
    {
        return -1;
    }
    DCB *dcb = {0};
    if (dcb->is_open == 1) // Ensure that the port is currently open.
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

int serial_read(device *dev, char *buf, size_t len)
{

    DCB *dcb = NULL;
    if (dev == NULL || buf == NULL || len <= 0)
    {
        return -1; // Invalid parameters
    }

    if (dcb->event_flag == 1 && dcb->status_code == IDLE) // Ensure that the port is open, and the status is idle
    {

        // Initialize the input buffer variables (not the ring buffer!)

        dcb->status_code = READ; // Set the status to reading

        dcb->event_flag = 0; // Clear the caller’s event flag.

        /*Copy characters from the ring buffer to the requestor’s buffer, until the ring buffer is emptied, the requested count has been reached, or a new-line (ENTER) code has been found.
         The copied characters should, of course, be removed from the ring buffer.
         Either input interrupts or all interrupts should be disabled during the copying. */

        // If more characters are needed, return. If the block is complete, continue with step 7.

        dcb->status_code = IDLE; // Reset the DCB status to idle,
        dcb->event_flag = 1;     // Set the event flag

        // return the actual count to the requestor’s variable.
    }
    return 0; // subject to change
}

int serial_write(device *dev, char *buf, size_t len)
{
    DCB *dcb = NULL;

    if (dev == NULL)
    {
        return -1; // Invalid parameters
    }

    if (dcb->is_open == 0)
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

    if (dcb->is_open == 1 || dcb->status_code == IDLE) // Ensure that the port is currently open and idle.
    {
        // Install the buffer pointer and counters in the DCB
        dcb->outputBufferAddr = (uint8_t *)buf;
        dcb->outputBufferCounter = len;

        dcb->status_code = WRITE; // Set the current status to writing.
        dcb->event_flag = 0;      // Clear the caller’s event flag

        // Get the first character from the requestor’s buffer and store it in the output register.
        *((volatile uint8_t *)0x3F8) = *dcb->outputBufferAddr;

        // Enable write interrupts by setting bit 1 of the Interrupt Enable register
        //  This must be done by setting the register to the logical OR of its previous contents and 0x02.
        *((volatile uint8_t *)0x3FA) |= 0x02;

        return 0; // Success
    }
    return -1; // subject to change
}

void serial_interrupt(void)
{
    device i = COM1;
    if ((inb(COM1 + IIR) & 1) == 0)
        i = COM1;
    else if ((inb(COM2 + IIR) & 1) == 0)
        i = COM2;
    else if ((inb(COM3 + IIR) & 1) == 0)
        i = COM3;
    else if ((inb(COM4 + IIR) & 1) == 0)
        i = COM4;

    DCB *dcb = NULL;

    if (dcb->event_flag == 1)
    {
        return;
    }

    char stat = (inb(i + IIR) & 6);
    switch (stat)
    {
    case 0:
    case 2:
        serial_output_interrupt(dcb);
        break;
    case 4:
        serial_input_interrupt(dcb);
        break;
    case 6:
    default:
        break;
    }
    outb(0x20, 0x20);
}

void serial_input_interrupt(DCB *dcb)
{
    char character = inb(dcb->dev);
    if (dcb->status_code != READ)
    {
        if (dcb->inputIndex == dcb->outputIndex)
        {
            return;
        }
        dcb->inputRingBuffer[dcb->inputIndex] = character;
        dcb->inputIndex = (dcb->outputIndex + 1) % 128;
    }
    else
    {

        switch (character)
        {

        case '\r': // Carriage returns
            outb(dcb->dev, '\n');
            dcb->inputBufferAddr[dcb->inputBufferCounter] = '\0';
            dcb->event_flag = 1;
            dcb->status_code = IDLE;
            return;

        case '\n': // newline
            outb(dcb->dev, '\n');
            dcb->inputBufferAddr[dcb->inputBufferCounter] = '\0';
            dcb->event_flag = 1;
            dcb->status_code = IDLE;
            return;

        case 127: // backspace
            outb(COM1, "\b");
            if (dcb->inputIndex > 0)
            {
                for (uint32_t i = dcb->inputIndex; i < dcb->inputBufferCounter; i++)
                {
                    dcb->inputBufferAddr[i - 1] = dcb->inputBufferAddr[i];
                    outb(COM1, dcb->inputBufferAddr[i]);
                }
                outb(COM1, " \b"); // do not stack with for loop, clears trailing char from terminal
                for (uint32_t i = dcb->inputIndex; i < dcb->inputBufferCounter; i++)
                    outb(COM1, "\b");
                dcb->inputBufferCounter--;
                dcb->inputIndex--;
                dcb->inputBufferAddr[dcb->inputBufferCounter] = '\0';
            }
            break;

        default:
            dcb->inputBufferAddr[dcb->inputIndex] = character;
            if (dcb->inputIndex == dcb->inputBufferCounter)
                dcb->inputBufferCounter++;
            dcb->inputIndex++;
            outb(COM1, character);
            break;
        }
    }
}

void serial_output_interrupt(DCB *dcb)
{
    if (dcb->status_code != WRITE)
        return;
    if (dcb->outputBufferCounter >= dcb->outputBufferLen)
    {
        dcb->event_flag = 1;
        dcb->status_code = IDLE;

        char inten = inb(dcb->dev + IER);
        outb(dcb->dev + IER, inten & ~0x02);

        return;
    }
    outb(dcb->dev, dcb->outputBufferAddr[dcb->outputBufferCounter]);
    dcb->outputBufferCounter++;
}
