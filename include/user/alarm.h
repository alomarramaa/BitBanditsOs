/**
@file alarm.h
@brief File used for alarm related functions
*/

/*
- Contains a message and a time
- Process idles when a time has not yet occurred
- At time, prints message and exits
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

void createAlarm(int hour, int minute, int seconds, char* message);

void addAlarm(int hour, int minute, int seconds, char* message);

//void removeAlarm(int hour, int minute, int seconds);

extern struct alarm *alarmsHead;

void removeAlarm(int index);

void setAlarm(char* hours, char* minutes, char* seconds);

int findAlarmIndex(int hour, int minute, int seconds);
