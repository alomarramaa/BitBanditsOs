.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h

user/date.o: user/date.c include/user/date.h include/stdlib.h include/user/rtc_util.h

user/time.o: user/time.c include/user/time.h include/stdlib.h include/user/rtc_util.h

user/help.o: user/help.c include/user/help.h 

user/comhand.o: user/comhand.c include/user/comhand.h

kernel/pcb.o: kernel/pcb.c include/mpx/pcb.h

user/user_pcb.o: user/user_pcb.c include/mpx/pcb.h

user/alarm.o: user/alarm.c include/user/alarm.h

user/version.o: user/version.c include/user/version.h

user/shutdown.o: user/shutdown.c include/user/shutdown.h

USER_OBJECTS=\
	user/core.o \
	user/date.o \
	user/time.o \
	user/comhand.o \
	user/alarm.o \
	kernel/pcb.o \
	user/user_pcb.o\
	user/help.o\
	user/version.o\
	user/shutdown.o
	
