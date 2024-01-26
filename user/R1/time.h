/**
 @file time.h
 @brief File used for time related commands
*/
int readTimeReg(char sect);
/// @brief Displays the system time
void display_current_time();
/// @brief Changes teh system time to a time inputed by the user witht hegiven parameters
/// @param hours 
/// @param minutes 
/// @param seconds
void set_time(int hours, int minutes, int seconds);