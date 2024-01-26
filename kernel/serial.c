#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>

// Maximum buffer list size
#define BUFFER_LIST_MAX 5

enum uart_registers {
	RBR = 0,	// Receive Buffer
	THR = 0,	// Transmitter Holding
	DLL = 0,	// Divisor Latch LSB
	IER = 1,	// Interrupt Enable
	DLM = 1,	// Divisor Latch MSB
	IIR = 2,	// Interrupt Identification
	FCR = 2,	// FIFO Control
	LCR = 3,	// Line Control
	MCR = 4,	// Modem Control
	LSR = 5,	// Line Status
	MSR = 6,	// Modem Status
	SCR = 7,	// Scratch
};

/*
* @brief A struct to store the previous entered commands, acts as a node in a linked list
*/
typedef struct previous_buffers
{
	/*
	* @brief Contents of the buffer
	*/
	char* bufferText;

	/*
	* @brief Size of the buffer
	*/
	int bufferSize;

	/*
	* @brief Pointer to the next buffer in the list
	*/
	previous_buffers* nextBuffer;

	/*
	* @brief Pointer to the previous buffer in the list
	*/
	previous_buffers* prevBuffer;
} previous_buffers;

// Head of the linked list
previous_buffers* bufferHead = NULL;

// Tail of the linked list
previous_buffers* bufferTail = NULL;

// List length to ensure limited memory use
int bufferListLength = 0;

static int initialized[4] = { 0 };

static int serial_devno(device dev)
{
	switch (dev) {
	case COM1: return 0;
	case COM2: return 1;
	case COM3: return 2;
	case COM4: return 3;
	}
	return -1;
}

int serial_init(device dev)
{
	int dno = serial_devno(dev);
	if (dno == -1) {
		return -1;
	}
	outb(dev + IER, 0x00);	//disable interrupts
	outb(dev + LCR, 0x80);	//set line control register
	outb(dev + DLL, 115200 / 9600);	//set bsd least sig bit
	outb(dev + DLM, 0x00);	//brd most significant bit
	outb(dev + LCR, 0x03);	//lock divisor; 8bits, no parity, one stop
	outb(dev + FCR, 0xC7);	//enable fifo, clear, 14byte threshold
	outb(dev + MCR, 0x0B);	//enable interrupts, rts/dsr set
	(void)inb(dev);		//read bit to reset port
	initialized[dno] = 1;
	return 0;
}

int serial_out(device dev, const char *buffer, size_t len)
{
	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0) {
		return -1;
	}
	for (size_t i = 0; i < len; i++) {
		outb(dev, buffer[i]);
	}
	return (int)len;
}

int serial_poll(device dev, char *buffer, size_t len)
{
	int bufferCount = 0; // Size of the buffer to be returned
	int index = 0; // Current location within the buffer
	int tempIndex; // Used when traversing through the buffer
	char tempChar; 
	previous_buffers* currBuffer = NULL; // Used when traversing previous commands

	while (bufferCount < ((int)len-1)) {
		if(inb(dev + LSR) & 1) {
			char charIn = inb(dev); //Read one byte
			switch(charIn) {
				case 13: //Carriage returns (\r)
					bufferCount++; //Increase buffer size
					tempIndex = index + 1; //Save next index
					do {
						tempChar = buffer[index]; //Save character at current index
						buffer[index] = '\r'; //Replace character at current index with '\r' or previous tempChar
						charIn = tempChar; //Set charIn to the replaced character
					} while(++index < bufferCount); //Repeat for all remaining characters in the buffer
					break;

				case 10: //New line (\n)
					bufferCount++; //Increase buffer size
					tempIndex = index + 1; //Save next index
					do {
						tempChar = buffer[index]; //Save character at current index
						buffer[index] = charIn; //Replace character at current index with '\n' or previous tempChar
						charIn = tempChar; //Set charIn to the replaced character
					} while(++index < bufferCount); //Repeat for all remaining characters in the buffer
					break;

				case 8: //Backspace
					if (index == 0) //Do nothing if no previous characters
						break;
					index--; //Traverse to character to be removed
					tempIndex = index; //Save current index
					do {
						buffer[index] = buffer[index+1]; //Replace current character with next character in buffer
					} while (++index < bufferCount); //Repeat for each remaining character in buffer
					buffer[index] = '\0'; //Replace the ending character with a null terminator
					bufferCount--; //Update bufferCount
					break;

				case 127: //Delete
					if (index == bufferCount) //Do nothing if no future characters
						break;
					tempIndex = index; //Save current index
					do {
						buffer[index] = buffer[index+1]; //Replace current character with next character in buffer
					} while(++index < bufferCount); //Repeat for each remaining character in buffer
					buffer[index] = '\0'; //Replace the ending character with a null terminator
					bufferCount--; //Update bufferCount
					break;

				case 32: //Space
					charIn = ' '; //Make input character a blank space
					bufferCount++; //Increase buffer size
					tempIndex = index + 1; //Save next index
					do {
						tempChar = buffer[index]; //Save character at current index
						buffer[index] = charIn; //Replace character at current index with charIn or previous tempChar
						charIn = tempChar; //Set charIn to the replaced character
					} while(++index < bufferCount); //Repeat for all remaining characters in the buffer
					break;

				case 37: //Left arrow
					if (index == 0) //Do nothing if no characters to the left
						break;	
					index--; //Decrease the index (move left)
					break;

				case 38: //Up arrow
					// If not already looking at a previous command
					if (currBuffer == NULL) {
						// If there are no previous commands, do nothing
						if (bufferHead == NULL)
							break;
						buffer = bufferHead->bufferText; // Set current buffer to the next command in the list
						bufferCount = index = bufferHead->bufferSize; // Set the buffer count to the count of the new buffer
						currBuffer = bufferHead; // Set the current buffer pointer to the head of the list of previous commands
					}
					else {
						// If there are no previous commands, do nothing
						if (currBuffer->nextBuffer == NULL)
							break;
						currBuffer = currBuffer->nextBuffer; // Set the current buffer pointer to the new command
						buffer = currBuffer->bufferText; // Set current buffer to the next command in the list
						bufferCount = index = currBuffer->bufferSize; // Set the buffer count to the count of the new buffer
					}
					break;

				case 39: //Right arrow
					if(index == bufferCount) //Do nothing if no characters to the right
						break;
					index++; //Increase the index (move right)
					break;

				case 40: //Down arrow
					// If not currently looking at a previous command, do nothing
					if (currBuffer == NULL)
						break;
					// If there is no previous command in the list, clear everything
					// as if entering a new command
					if (currBuffer->prevBuffer == NULL) {
						currBuffer = NULL; // Sets the current buffer pointer to null
						index = 0; // Resets index for buffer traversal
						// Traverses through the buffer and sets every character to the null terminator
						while (index < bufferCount) {
							buffer[index] = '\0';
							index++;
						}
						bufferCount = index = 0; // Resets the buffer count and index
					}
					else {
						currBuffer = currBuffer->prevBuffer; // Set the current buffer pointer to the previous command node
						buffer = currBuffer->bufferText; // Set current buffer to the previous command in the list
						bufferCount = index = currBuffer->bufferSize; // Set the buffer count to the count of the new buffer
					}
					break;

				default: //Basic character (A-Z, a-z, 0-9)
					bufferCount++; //Increase buffer size
					tempIndex = index + 1; //Save next index
					do {
						tempChar = buffer[index]; //Save character at current index
						buffer[index] = charIn; //Replace character at current index with charIn or previous tempChar
						charIn = tempChar; //Set charIn to the replaced character
					} while(++index < bufferCount); //Repeat for all remaining characters in the buffer
			}
			
			serial_out(dev, buffer, bufferCount); //Display current buffer
			index = tempIndex; //Restore index
		}
	}
	// Creates a new node for the submitted command
	// and initializes its values
	previous_buffers newBuffer;
	newBuffer.bufferSize = bufferCount;
	newBuffer.bufferText = buffer;
	newBuffer.nextBuffer = bufferHead;
	newBuffer.prevBuffer = NULL;
	// Sets the new node to the head of the list
	bufferHead->prevBuffer = &newBuffer;
	bufferHead = &newBuffer;
	// If the list is empty, sets the new buffer as the tail of the list
	if (bufferListLength == 0) {
		bufferTail = &newBuffer;
		bufferListLength++; // Updates the size of the command list
	}
	// If the list is full, moves the tail back and clears the previous tail from memory
	else if (bufferListLength == BUFFER_LIST_MAX) {
		previous_buffers* tempBuffer = bufferTail;
		bufferTail = bufferTail->prevBuffer;
		bufferTail->nextBuffer = NULL;
		clear(tempBuffer);
	}
	else {
		bufferListLength++; // Updates the size of the command list
	}

	return bufferCount;
}
