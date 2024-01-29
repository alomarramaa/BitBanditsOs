/**
 @file date.h
 @brief File used for date related commands
*/
int readDateReg(char sect);
/// @brief Displays the system date
void display_current_date(void);
/// @brief Changes the system date to a date inputed by the user with the given parameters
/// @param month 
/// @param day
/// @param year
void set_date(int month, int day, int year);
