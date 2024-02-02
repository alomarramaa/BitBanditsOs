#ifndef USER_TIME_H
#define USER_TIME_H

// @brief Displays the system time
void get_time(void);

/// @brief Changes the system time to a time inputed by the user with the given parameters
/// @param hours 
/// @param minutes 
/// @param seconds 
void set_time(int hours, int minutes, int seconds);

#endif
