.POSIX:

kernel/serial.o: kernel/serial.c include/mpx/io.h include/mpx/serial.h \
  include/mpx/device.h include/sys_req.h 

kernel/kmain.o: kernel/kmain.c include/mpx/gdt.h include/mpx/interrupts.h \
  include/mpx/serial.h include/mpx/device.h include/mpx/vm.h \
  include/sys_req.h include/string.h include/memory.h

kernel/core-c.o: kernel/core-c.c include/mpx/gdt.h include/mpx/panic.h \
  include/mpx/interrupts.h include/mpx/io.h include/mpx/serial.h \
  include/mpx/device.h include/sys_req.h include/string.h \
  include/mpx/vm.h

kernel/library.o: kernel/library.c include/mpx/library.h

kernel/r6.o: kernel/r6.c include/mpx/r6.h
 




KERNEL_OBJECTS=\
	kernel/core-asm.o\
  kernel/sys_call.o\
	kernel/sys_call_isr.o\
	kernel/serial.o\
	kernel/kmain.o\
	kernel/core-c.o\
  kernel/library.o\
  kernel/r6.o\
  kernel/serial_isr.o

