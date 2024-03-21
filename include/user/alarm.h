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

void checkAlarm(void);

void showAlarms(void);

void createAlarm(void);

void addAlarm(int hour, int minute, int seconds, char* message);

void removeAlarm(int hour, int minute, int seconds);

extern struct alarm *alarmsHead;


