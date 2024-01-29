.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h

user/comhand.o: user/comhand.c include/user/comhand.h

USER_OBJECTS=\
	user/core.o \
	user/comhand.o

