// Function to read a byte from RTC register
int read_rtc_register(int reg) {
    outb(0x70,reg); // Use 0x70 for RTC register selection
    return inb(0x71); // Use 0x71 for RTC data port
}

// Function to convert BCD to binary
int bcd_to_binary(int bcd) {
    int tens = (bcd >> 4) & 0x0F; // Extract the tens column
    int ones = bcd & 0x0F;       // Extract the ones column
    return (tens * 10) + ones;    // Convert and combine tens and ones
}
