#include <mpx/io.h>
#include <mpx/serial.h>
#include "sys_req.h"
#include <memory.h>

// // Maximum buffer list size
#define BUFFER_LIST_MAX 5

/*
 * @brief A struct to store the previous entered commands, acts as a node in a linked list
 */
typedef struct previous_buffers
{
	/*
	 * @brief Contents of the buffer
	 */
	char *bufferText;

	/*
	 * @brief Size of the buffer
	 */
	int bufferSize;

	/*
	 * @brief Pointer to the next buffer in the list
	 */
	struct previous_buffers *nextBuffer;

	/*
	 * @brief Pointer to the previous buffer in the list
	 */
	struct previous_buffers *prevBuffer;
} previous_buffers;

// Head of the linked list
static previous_buffers *bufferHead = NULL;

// Tail of the linked list
static previous_buffers *bufferTail = NULL;

// List length to ensure limited memory use
static int bufferListLength = 0;

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

//Checks status register of serial port, collects character and adds to buffer
int serial_poll(device dev, char *buffer, size_t len)
{
	int bufferCount = 0; // Size of the buffer to be returned
	int index = 0;		 // Current location within the buffer
	int tempIndex;		 // Used when traversing through the buffer
	char tempChar;
	previous_buffers *currBuffer = NULL; // Used when traversing previous commands

	int stop = 0;
	while (bufferCount < ((int)len - 1) && !stop)
	{
		/* Checks the status register of the serial port (inb(dev + LSR) & 1) to see if there are incoming characters
*/
		if (inb(dev + LSR) & 1) //device + line status register
		{
			char charIn = inb(dev); // If a character is available, it reads one byte using inb			
			switch ( (int) charIn)
			{
				case 13:
					outb(dev, '\r');  // Carriage returns (\r)
					stop = 1;
					break;
				case 10:
					outb(dev, '\n'); // New line (\n)
					stop = 1;
					break;

				case 127:	// Backspace
					if (index > 0) {
						serial_out(COM1, "\b \b", 4);      // Move the cursor back, print a space to overwrite the previous character, and move the cursor back again
						serial_out(COM1, "\x1b[2K", 4); // Clear any future characters
						index--;
						tempIndex = index;
						for (int i = index; i < bufferCount; i++) {
							buffer[i] = buffer[i + 1];      // Shift each character in the buffer one position to the left
							//serial_out(COM1, &buffer[i], 1);
						}
						buffer[bufferCount] = '\0';      // The new end of the string
						bufferCount--;
					}
					else {
						tempIndex = 0;
					}
					break;

				case 32:				   // Space
					charIn = ' ';		   // Make input character a blank space
					bufferCount++;		   // Increase buffer size
					tempIndex = index + 1; // Save next index
					do
					{
						tempChar = buffer[index];	 // Save character at current index
						buffer[index] = charIn;		 // Replace character at current index with charIn or previous tempChar
						// outb(dev, charIn);
						charIn = tempChar;			 // Set charIn to the replaced character
					} while (++index < bufferCount); // Repeat for all remaining characters in the buffer
					break;

				case 27: // Escape character
					charIn = inb(dev); // Read the next character to determine the key
					if (charIn == '[') // Check if the next character is '['
					{
						charIn = inb(dev); // Read the next character after '['
						switch (charIn)
						{
						case '3': // Delete
							charIn = inb(dev); // Read the next character after '3'
							if (charIn == '~')
							{
								// Handle delete key
								if (index < bufferCount) {
									serial_out(COM1, "\x1b[2K", 4);      // Clear any future characters
									tempIndex = index;
									for (int i = index; i < bufferCount; i++) 
									{
										buffer[i] = buffer[i + 1];      // Shift each character in the buffer one position to the left
										//serial_out(COM1, &buffer[i], 1);
									}
									buffer[bufferCount] = '\0';      // The new end of the string
									bufferCount--;
								}
								else {
									tempIndex = index;
								}
								break;
							}
							break;

						case 'A': // Up arrow
							//If not already looking at a previous command
							if (currBuffer == NULL)
							{
								// If there are no previous commands, do nothing
								if (bufferHead == NULL)
									break;
								buffer = bufferHead->bufferText;			  // Set current buffer to the next command in the list
								bufferCount = bufferHead->bufferSize; // Set the buffer count to the count of the new buffer
								tempIndex = bufferCount;
								currBuffer = bufferHead;					  // Set the current buffer pointer to the head of the list of previous commands
							}
							else
							{
								// If there are no previous commands, do nothing
								if (currBuffer->nextBuffer == NULL)
									break;
								currBuffer = currBuffer->nextBuffer;		  // Set the current buffer pointer to the new command
								buffer = currBuffer->bufferText;			  // Set current buffer to the next command in the list
								bufferCount = currBuffer->bufferSize; // Set the buffer count to the count of the new buffer
								tempIndex = bufferCount;
							}
							break;

						case 'B': // Down arrow
							// If not currently looking at a previous command, do nothing
							if (currBuffer == NULL)
								break;
							// If there is no previous command in the list, clear everything
							// as if entering a new command
							if (currBuffer->prevBuffer == NULL)
							{
								currBuffer = NULL; // Sets the current buffer pointer to null
								index = 0;		   // Resets index for buffer traversal
								tempIndex = 0;
								// Traverses through the buffer and sets every character to the null terminator
								while (index < bufferCount)
								{
									buffer[index] = '\0';
									index++;
								}
								serial_out(dev, "\x1b[2k\r", 5); // Clear the line on the terminal
								bufferCount = 0; // Resets the buffer count and index
							}
							else
							{
								currBuffer = currBuffer->prevBuffer;		  // Set the current buffer pointer to the previous command node
								buffer = currBuffer->bufferText;			  // Set current buffer to the previous command in the list
								bufferCount = currBuffer->bufferSize; // Set the buffer count to the count of the new buffer
								tempIndex = bufferCount;
							}
							break;

						case 'C': // Right arrow
							if (index == bufferCount) // Do nothing if no characters to the right
								break;
							tempIndex++; // Increase the index (move right)
							//outb(COM1, "\x1b[C");
							break;

						case 'D': // Left arrow
							if (index == 0) // Do nothing if no characters to the left
								break;
							tempIndex--; // Decrease the index (move left)
							//serial_out(COM1, "\b", 2); // Move cursor back
							break;
						}
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
			if (stop)
			{
				break;
			}
			serial_out(dev, "\x1b[2k\r> ", 8);
			serial_out(dev, buffer, bufferCount); // Display current buffer
			index = tempIndex;					  // Restore index
			while(tempIndex < bufferCount)
			{
				serial_out(dev, "\b", 1);
				tempIndex++;
			}
			tempIndex = index;
		}
	}
	
	// Creates a new node for the submitted command
	// and initializes its values
	previous_buffers* newBuffer = sys_alloc_mem(sizeof(previous_buffers));
	newBuffer->bufferSize = bufferCount;
	newBuffer->bufferText = buffer;
	newBuffer->nextBuffer = bufferHead;
	newBuffer->prevBuffer = NULL;
	// If the list is empty, sets the new buffer as the tail of the list
	if (bufferListLength == 0)
	{
		bufferHead = newBuffer;
		bufferTail = newBuffer;
		bufferListLength++; // Updates the size of the command list
	}
	// If the list is full, moves the tail back and clears the previous tail from memory
	else if (bufferListLength == BUFFER_LIST_MAX)
	{
		previous_buffers* tempBuffer = bufferTail;
		bufferTail = bufferTail->prevBuffer;
		bufferTail->nextBuffer = NULL;
		sys_free_mem(tempBuffer);
		bufferHead->prevBuffer = newBuffer;
		bufferHead = newBuffer;
	}
	// List not full but still has free spaces
	else
	{
		bufferHead->prevBuffer = newBuffer;
		bufferHead = newBuffer;
		bufferListLength++; // Updates the size of the command list
	}

	return bufferCount;
}

