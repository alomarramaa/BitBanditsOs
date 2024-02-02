#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>

// // Maximum buffer list size
// #define BUFFER_LIST_MAX 5

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

// /*
//  * @brief A struct to store the previous entered commands, acts as a node in a linked list
//  */
// typedef struct previous_buffers
// {
// 	/*
// 	 * @brief Contents of the buffer
// 	 */
// 	char *bufferText;

// 	/*
// 	 * @brief Size of the buffer
// 	 */
// 	int bufferSize;

// 	/*
// 	 * @brief Pointer to the next buffer in the list
// 	 */
// 	struct previous_buffers *nextBuffer;

// 	/*
// 	 * @brief Pointer to the previous buffer in the list
// 	 */
// 	struct previous_buffers *prevBuffer;
// } previous_buffers;

// // Head of the linked list
// previous_buffers *bufferHead = NULL;

// // Tail of the linked list
// previous_buffers *bufferTail = NULL;

// // List length to ensure limited memory use
// int bufferListLength = 0;

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
	int bufferCount = 0; // Size of the buffer to be returned
	int index = 0;		 // Current location within the buffer
	int tempIndex;		 // Used when traversing through the buffer
	char tempChar;
	// previous_buffers *currBuffer = NULL; // Used when traversing previous commands

	int stop = 0;
	while (bufferCount < ((int)len - 1) && !stop)
	{
		if (inb(dev + LSR) & 1)
		{
			char charIn = inb(dev); // Read one byte			
			switch ( (int) charIn)
			{
				case 1: 
					serial_out(COM1, "1", 2);
					break;
				case 2: 
					serial_out(COM1, "2", 2);
					break;
				case 3: 
					serial_out(COM1, "3", 2);
					break;
				case 4: 
					serial_out(COM1, "4", 2);
					break;
				case 5: 
					serial_out(COM1, "5", 2);break;
				case 6: 
					serial_out(COM1, "6", 2);break;
				case 7: 
					serial_out(COM1, "7", 2);break;
				case 8: 
					serial_out(COM1, "8", 2);break;
				case 9: 
					serial_out(COM1, "9", 2);break;
				case 10: 
					serial_out(COM1, "10", 3);break;
				case 11: 
					serial_out(COM1, "11", 3);break;
				case 12: 
					serial_out(COM1, "12", 3);break;
				case 13: 
					serial_out(COM1, "13", 3);break;
				case 14: 
					serial_out(COM1, "14", 3);break;
				case 15: 
					serial_out(COM1, "15", 3);break;
				case 16: 
					serial_out(COM1, "16", 3);break;
				case 17: 
					serial_out(COM1, "17", 3);break;
				case 18: 
					serial_out(COM1, "18", 3);break;
				case 19: 
					serial_out(COM1, "19", 3);break;
				case 20: 
					serial_out(COM1, "20", 3);break;
				case 21: 
					serial_out(COM1, "21", 3);break;
				case 22: 
					serial_out(COM1, "22", 3);break;
				case 23: 
					serial_out(COM1, "23", 3);break;
				case 24: 
					serial_out(COM1, "24", 3);break;
				case 25: 
					serial_out(COM1, "25", 3);break;
				case 26: 
					serial_out(COM1, "26", 3);break;
				case 27: 
					serial_out(COM1, "27", 3);break;
				case 28: 
					serial_out(COM1, "28", 3);break;
				case 29: 
					serial_out(COM1, "29", 3);break;
				case 30: 
					serial_out(COM1, "30", 3);break;
				case 31: 
					serial_out(COM1, "31", 3);break;
				case 32: 
					serial_out(COM1, "32", 3);break;
				case 33: 
					serial_out(COM1, "33", 3);break;
				case 34: 
					serial_out(COM1, "34", 3);break;
				case 35: 
					serial_out(COM1, "35", 3);break;
				case 36: 
					serial_out(COM1, "36", 3);break;
				case 37: 
					serial_out(COM1, "37", 3);break;
				case 38: 
					serial_out(COM1, "38", 3);break;
				case 39: 
					serial_out(COM1, "39", 3);break;
				case 40: 
					serial_out(COM1, "40", 3);break;
				case 41: 
					serial_out(COM1, "41", 3);break;
				case 42: 
					serial_out(COM1, "42", 3);break;
				case 43: 
					serial_out(COM1, "43", 3);break;
				case 44: 
					serial_out(COM1, "44", 3);break;
				case 45: 
					serial_out(COM1, "45", 3);break;
				case 46: 
					serial_out(COM1, "46", 3);break;
				case 47: 
					serial_out(COM1, "47", 3);break;
				case 48: 
					serial_out(COM1, "48", 3);break;
				case 49: 
					serial_out(COM1, "49", 3);break;
				case 50: 
					serial_out(COM1, "50", 3);break;
				case 51: 
					serial_out(COM1, "51", 3);break;
				case 52: 
					serial_out(COM1, "52", 3);break;
				case 53: 
					serial_out(COM1, "53", 3);break;
				case 54: 
					serial_out(COM1, "54", 3);break;
				case 55: 
					serial_out(COM1, "55", 3);break;
				case 56: 
					serial_out(COM1, "56", 3);break;
				case 57: 
					serial_out(COM1, "57", 3);break;
				case 58: 
					serial_out(COM1, "58", 3);break;
				case 59: 
					serial_out(COM1, "59", 3);break;
				case 60: 
					serial_out(COM1, "60", 3);break;
				case 61: 
					serial_out(COM1, "61", 3);break;
				case 62: 
					serial_out(COM1, "62", 3);break;
				case 63: 
					serial_out(COM1, "63", 3);break;
				case 64: 
					serial_out(COM1, "64", 3);break;
				case 65: 
					serial_out(COM1, "65", 3);break;
				case 66: 
					serial_out(COM1, "66", 3);break;
				case 67: 
					serial_out(COM1, "67", 3);break;
				case 68: 
					serial_out(COM1, "68", 3);break;
				case 69: 
					serial_out(COM1, "69", 3);break;
				case 70: 
					serial_out(COM1, "70", 3);break;
				case 71: 
					serial_out(COM1, "71", 3);break;
				case 72: 
					serial_out(COM1, "72", 3);break;
				case 73: 
					serial_out(COM1, "73", 3);break;
				case 74: 
					serial_out(COM1, "74", 3);break;
				case 75: 
					serial_out(COM1, "75", 3);break;
				case 76: 
					serial_out(COM1, "76", 3);break;
				case 77: 
					serial_out(COM1, "77", 3);break;
				case 78: 
					serial_out(COM1, "78", 3);break;
				case 79: 
					serial_out(COM1, "79", 3);break;
				case 80: 
					serial_out(COM1, "80", 3);break;
				case 81: 
					serial_out(COM1, "81", 3);break;
				case 82: 
					serial_out(COM1, "82", 3);break;
				case 83: 
					serial_out(COM1, "83", 3);break;
				case 84: 
					serial_out(COM1, "84", 3);break;
				case 85: 
					serial_out(COM1, "85", 3);break;
				case 86: 
					serial_out(COM1, "86", 3);break;
				case 87: 
					serial_out(COM1, "87", 3);break;
				case 88: 
					serial_out(COM1, "88", 3);break;
				case 89: 
					serial_out(COM1, "89", 3);break;
				case 90: 
					serial_out(COM1, "90", 3);break;
				case 91: 
					serial_out(COM1, "91", 3);break;
				case 92: 
					serial_out(COM1, "92", 3);break;
				case 93: 
					serial_out(COM1, "93", 3);break;
				case 94: 
					serial_out(COM1, "94", 3);break;
				case 95: 
					serial_out(COM1, "95", 3);break;
				case 96: 
					serial_out(COM1, "96", 3);break;
				case 97: 
					serial_out(COM1, "97", 3);break;
				case 98: 
					serial_out(COM1, "98", 3);break;
				case 99: 
					serial_out(COM1, "99", 3);break;
				case 100: 
					serial_out(COM1, "100", 4);break;
				case 101: 
					serial_out(COM1, "101", 4);break;
				case 102: 
					serial_out(COM1, "102", 4);break;
				case 103: 
					serial_out(COM1, "103", 4);break;
				case 104: 
					serial_out(COM1, "104", 4);break;
				case 105: 
					serial_out(COM1, "105", 4);break;
				case 106: 
					serial_out(COM1, "106", 4);break;
				case 107: 
					serial_out(COM1, "107", 4);break;
				case 108: 
					serial_out(COM1, "108", 4);break;
				case 109: 
					serial_out(COM1, "109", 4);break;
				case 110: 
					serial_out(COM1, "110", 4);break;
				case 111: 
					serial_out(COM1, "111", 4);break;
				case 112: 
					serial_out(COM1, "112", 4);break;
				case 113: 
					serial_out(COM1, "113", 4);break;
				case 114: 
					serial_out(COM1, "114", 4);break;
				case 115: 
					serial_out(COM1, "115", 4);break;
				case 116: 
					serial_out(COM1, "116", 4);break;
				case 117: 
					serial_out(COM1, "117", 4);break;
				case 118: 
					serial_out(COM1, "118", 4);break;
				case 119: 
					serial_out(COM1, "119", 4);break;
				case 120: 
					serial_out(COM1, "120", 4);break;
				case 121: 
					serial_out(COM1, "121", 4);break;
				case 122: 
					serial_out(COM1, "122", 4);break;
				case 123: 
					serial_out(COM1, "123", 4);break;
				case 124: 
					serial_out(COM1, "124", 4);break;
				case 125: 
					serial_out(COM1, "125", 4);break;
				case 126: 
					serial_out(COM1, "126", 4);break;
				case 127: 
					serial_out(COM1, "127", 4);break;


			// case 13:
			// 	outb(dev, '\r');  // Carriage returns (\r)
			// 	stop = 1;
			// 	break;
			// case 10:
			// 	outb(dev, '\n'); // New line (\n)
			// 	stop = 1;
			// 	break;

			// case 8:				// Backspace
			// 	if (index > 0) {
			// 		serial_out(COM1, "\b \b", 4);      // Move the cursor back, print a space to overwrite the previous character, and move the cursor back again
			// 		index--;
			// 		tempIndex = index;
			// 		for (int i = index; i < bufferCount; i++) {
			// 			buffer[i] = buffer[i + 1];      // Shift each character in the buffer one position to the left
			// 			//serial_out(COM1, &buffer[i], 1);
			// 		}
			// 		buffer[bufferCount] = '\0';      // The new end of the string
			// 		bufferCount--;
			// 	}
			// 	else {
			// 		tempIndex = 0;
			// 	}
			// 	break;

			// case 127:	// Backspace ?? (Delete?)
			// // For some reason, when I press backspace on my keyboard it enters this case, so I put the backspace code here
			// // I thought this was the delete function?
			// // Delete and the arrow keys are being detected as other ascii values for some reason
			// 	if (index > 0) {
			// 		serial_out(COM1, "\b \b", 4);      // Move the cursor back, print a space to overwrite the previous character, and move the cursor back again
			// 		index--;
			// 		tempIndex = index;
			// 		for (int i = index; i < bufferCount; i++) {
			// 			buffer[i] = buffer[i + 1];      // Shift each character in the buffer one position to the left
			// 			//serial_out(COM1, &buffer[i], 1);
			// 		}
			// 		buffer[bufferCount] = '\0';      // The new end of the string
			// 		bufferCount--;
			// 	}
			// 	else {
			// 		tempIndex = 0;
			// 	}
			// 	break;

			// case 32:				   // Space
			// 	charIn = ' ';		   // Make input character a blank space
			// 	bufferCount++;		   // Increase buffer size
			// 	tempIndex = index + 1; // Save next index
			// 	do
			// 	{
			// 		tempChar = buffer[index];	 // Save character at current index
			// 		buffer[index] = charIn;		 // Replace character at current index with charIn or previous tempChar
			// 		// outb(dev, charIn);
			// 		charIn = tempChar;			 // Set charIn to the replaced character
			// 	} while (++index < bufferCount); // Repeat for all remaining characters in the buffer
			// 	break;

			// case 37:			// Left arrow
			// 	if (index == 0) // Do nothing if no characters to the left
			// 		break;
			// 	tempIndex--; // Decrease the index (move left)
			// 	break;

			// case 38: // Up arrow
			// 	// If not already looking at a previous command
			// 	// if (currBuffer == NULL)
			// 	// {
			// 	// 	// If there are no previous commands, do nothing
			// 	// 	if (bufferHead == NULL)
			// 	// 		break;
			// 	// 	buffer = bufferHead->bufferText;			  // Set current buffer to the next command in the list
			// 	// 	bufferCount = bufferHead->bufferSize; // Set the buffer count to the count of the new buffer
			// 	// 	tempIndex = bufferCount;
			// 	// 	currBuffer = bufferHead;					  // Set the current buffer pointer to the head of the list of previous commands
			// 	// }
			// 	// else
			// 	// {
			// 	// 	// If there are no previous commands, do nothing
			// 	// 	if (currBuffer->nextBuffer == NULL)
			// 	// 		break;
			// 	// 	currBuffer = currBuffer->nextBuffer;		  // Set the current buffer pointer to the new command
			// 	// 	buffer = currBuffer->bufferText;			  // Set current buffer to the next command in the list
			// 	// 	bufferCount = currBuffer->bufferSize; // Set the buffer count to the count of the new buffer
			// 	// 	tempIndex = bufferCount;
			// 	// }
			// 	break;

			// case 39:					  // Right arrow
			// 	if (index == bufferCount) // Do nothing if no characters to the right
			// 		break;
			// 	tempIndex++; // Increase the index (move right)
			// 	break;

			// case 40: // Down arrow
			// 	// // If not currently looking at a previous command, do nothing
			// 	// if (currBuffer == NULL)
			// 	// 	break;
			// 	// // If there is no previous command in the list, clear everything
			// 	// // as if entering a new command
			// 	// if (currBuffer->prevBuffer == NULL)
			// 	// {
			// 	// 	currBuffer = NULL; // Sets the current buffer pointer to null
			// 	// 	index = 0;		   // Resets index for buffer traversal
			// 	// 	tempIndex = 0;
			// 	// 	// Traverses through the buffer and sets every character to the null terminator
			// 	// 	while (index < bufferCount)
			// 	// 	{
			// 	// 		buffer[index] = '\0';
			// 	// 		index++;
			// 	// 	}
			// 	// 	bufferCount = 0; // Resets the buffer count and index
			// 	// }
			// 	// else
			// 	// {
			// 	// 	currBuffer = currBuffer->prevBuffer;		  // Set the current buffer pointer to the previous command node
			// 	// 	buffer = currBuffer->bufferText;			  // Set current buffer to the previous command in the list
			// 	// 	bufferCount = currBuffer->bufferSize; // Set the buffer count to the count of the new buffer
			// 	// 	tempIndex = bufferCount;
			// 	// }
			// 	break;

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
			stop = 1;
			if (stop)
			{
				break;
			}
			//serial_out(dev, "\x1b[2k\r", 5);
			//serial_out(dev, buffer, bufferCount); // Display current buffer
			// index = tempIndex;					  // Restore index
		}
	}
	// Creates a new node for the submitted command
	// and initializes its values
	// previous_buffers newBuffer;
	// newBuffer.bufferSize = bufferCount;
	// newBuffer.bufferText = buffer;
	// newBuffer.nextBuffer = bufferHead;
	// newBuffer.prevBuffer = NULL;
	// // If the list is empty, sets the new buffer as the tail of the list
	// if (bufferListLength == 0)
	// {
	// 	bufferHead = &newBuffer;
	// 	bufferTail = &newBuffer;
	// 	bufferListLength++; // Updates the size of the command list
	// }
	// // If the list is full, moves the tail back and clears the previous tail from memory
	// else if (bufferListLength == BUFFER_LIST_MAX)
	// {
	// 	// previous_buffers* tempBuffer = bufferTail;
	// 	bufferTail = bufferTail->prevBuffer;
	// 	bufferTail->nextBuffer = NULL;
	// 	// clear(tempBuffer);
	// 	bufferHead->prevBuffer = &newBuffer;
	// 	bufferHead = &newBuffer;
	// }
	// else
	// {
	// 	bufferHead->prevBuffer = &newBuffer;
	// 	bufferHead = &newBuffer;
	// 	bufferListLength++; // Updates the size of the command list
	// }

	return bufferCount;
}

