.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h

user/date.o: user/date.c include/user/date.h include/stdlib.h include/user/rtc_util.h

user/time.o: user/time.c include/user/time.h include/stdlib.h include/user/rtc_util.h

user/comhand.o: user/comhand.c include/user/comhand.h

kernel/pcb.o: kernel/pcb.c include/mpx/pcb.h

user/user_pcb.o: user/user_pcb.c

USER_OBJECTS=\
	user/core.o \
	user/date.o \
	user/time.o \
	user/comhand.o \
	kernel/pcb.o \
	user/user_pcb.o

