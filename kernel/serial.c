#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>

enum uart_registers
{
	RBR = 0, // Receive Buffer
	THR = 0, // Transmitter Holding
	DLL = 0, // Divisor Latch LSB
	IER = 1, // Interrupt Enable
	DLM = 1, // Divisor Latch MSB
	IIR = 2, // Interrupt Identification
	FCR = 2, // FIFO Control
	LCR = 3, // Line Control
	MCR = 4, // Modem Control
	LSR = 5, // Line Status
	MSR = 6, // Modem Status
	SCR = 7, // Scratch
};

typedef struct previous_buffers
{
	char *bufferText;
	int bufferSize;
	struct previous_buffers *nextBuffer;
	struct previous_buffers *prevBuffer;
} previous_buffers;

previous_buffers *bufferHead = NULL;
previous_buffers *bufferTail = NULL;
int bufferListLength = 0;

static int initialized[4] = {0};

static int serial_devno(device dev)
{
	switch (dev)
	{
	case COM1:
		return 0;
	case COM2:
		return 1;
	case COM3:
		return 2;
	case COM4:
		return 3;
	}
	return -1;
}

int serial_init(device dev)
{
	int dno = serial_devno(dev);
	if (dno == -1)
	{
		return -1;
	}
	outb(dev + IER, 0x00);			// disable interrupts
	outb(dev + LCR, 0x80);			// set line control register
	outb(dev + DLL, 115200 / 9600); // set bsd least sig bit
	outb(dev + DLM, 0x00);			// brd most significant bit
	outb(dev + LCR, 0x03);			// lock divisor; 8bits, no parity, one stop
	outb(dev + FCR, 0xC7);			// enable fifo, clear, 14byte threshold
	outb(dev + MCR, 0x0B);			// enable interrupts, rts/dsr set
	(void)inb(dev);					// read bit to reset port
	initialized[dno] = 1;
	return 0;
}

int serial_out(device dev, const char *buffer, size_t len)
{
	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0)
	{
		return -1;
	}
	for (size_t i = 0; i < len; i++)
	{
		outb(dev, buffer[i]);
	}
	return (int)len;
}

int serial_poll(device dev, char *buffer, size_t len)
{
	int bufferCount = 0;
	int index = 0;
	int tempIndex;
	char tempChar;
	previous_buffers *currBuffer = NULL;

	while (bufferCount < ((int)len - 1))
	{
		if (inb(dev + LSR) & 1)
		{
			char charIn = inb(dev); // Read one byte
			switch (charIn)
			{
			case 13: // Carriage returns (\r)
				break;
			case 10: // New line (\n)
				break;
			case 8:				// Backspace
				if (index == 0) // Do nothing if no previous characters
					break;
				index--;		   // Traverse to character to be removed
				tempIndex = index; // Save current index
				do
				{
					buffer[index] = buffer[index + 1]; // Replace current character with next character in buffer
				} while (++index < bufferCount);	   // Repeat for each remaining character in buffer
				buffer[index] = '\0';				   // Replace the ending character with a null terminator
				bufferCount--;						   // Update bufferCount
				break;
			case 127:					  // Delete
				if (index == bufferCount) // Do nothing if no future characters
					break;
				tempIndex = index; // Save current index
				do
				{
					buffer[index] = buffer[index + 1]; // Replace current character with next character in buffer
				} while (++index < bufferCount);	   // Repeat for each remaining character in buffer
				buffer[index] = '\0';				   // Replace the ending character with a null terminator
				bufferCount--;						   // Update bufferCount
				break;
			case 32:				   // Space
				charIn = ' ';		   // Make input character a blank space
				bufferCount++;		   // Increase buffer size
				tempIndex = index + 1; // Save next index
				do
				{
					tempChar = buffer[index];	 // Save character at current index
					buffer[index] = charIn;		 // Replace character at current index with charIn or previous tempChar
					charIn = tempChar;			 // Set charIn to the replaced character
				} while (++index < bufferCount); // Repeat for all remaining characters in the buffer
				break;
			case 37:			// Left arrow
				if (index == 0) // Do nothing if no characters to the left
					break;
				index--; // Decrease the index (move left)
				break;
			case 38: // Up arrow
				if (currBuffer == NULL)
				{
					if (bufferHead == NULL)
						break;
					buffer = bufferHead->bufferText;
					bufferCount = bufferHead->bufferSize;
					index = bufferCount;
					currBuffer = bufferHead;
				}
				else
				{
					if (currBuffer->nextBuffer == NULL)
						break;
					currBuffer = currBuffer->nextBuffer;
					buffer = currBuffer->bufferText;
					bufferCount = currBuffer->bufferSize;
					index = bufferCount;
				}
				break;
			case 39:					  // Right arrow
				if (index == bufferCount) // Do nothing if no characters to the right
					break;
				index++; // Increase the index (move right)
				break;
			case 40: // Down arrow
				if (currBuffer == NULL)
					break;
				if (currBuffer->prevBuffer == NULL)
				{
					currBuffer = NULL;
					index = 0;
					while (index < bufferCount)
					{
						buffer[index] = '\0';
						index++;
					}
					bufferCount = index = 0;
				}
				else
				{
					currBuffer = currBuffer->prevBuffer;
					buffer = currBuffer->bufferText;
					bufferCount = currBuffer->bufferSize;
					index = bufferCount;
				}
				break;
			default:				   // Basic character (A-Z, a-z, 0-9)
				bufferCount++;		   // Increase buffer size
				tempIndex = index + 1; // Save next index
				do
				{
					tempChar = buffer[index];	 // Save character at current index
					buffer[index] = charIn;		 // Replace character at current index with charIn or previous tempChar
					charIn = tempChar;			 // Set charIn to the replaced character
				} while (++index < bufferCount); // Repeat for all remaining characters in the buffer
			}

			serial_out(dev, buffer, bufferCount); // Display current buffer
			index = tempIndex;					  // Restore index
		}
	}
	previous_buffers newBuffer;
	newBuffer.bufferSize = bufferCount;
	newBuffer.bufferText = buffer;
	newBuffer.nextBuffer = bufferHead;
	newBuffer.prevBuffer = NULL;
	bufferHead->prevBuffer = &newBuffer;
	bufferHead = &newBuffer;
	if (bufferListLength == 0)
	{
		bufferTail = &newBuffer;
		bufferListLength++;
	}
	else if (bufferListLength == 10)
	{
		previous_buffers *tempBuffer = bufferTail;
		bufferTail = bufferTail->prevBuffer;
		bufferTail->nextBuffer = NULL;
		clear(tempBuffer);
	}
	else
	{
		bufferListLength++;
	}

	return bufferCount;
}