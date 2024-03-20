/**
@file alarm.h
@brief File used for alarm related functions
*/

struct alarm {
int hour;
int minute;
int seconds;
char message[50];
struct alarm* nxtAlarm;
};

void checkAlarm();

void showAlarms();

void createAlarm();

void addAlarm(int hour, int minute, int seconds, char* message);

