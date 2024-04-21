bits 32
global serial_isr

;;; io interrupt handler.
extern serial_interrupt		; The C function that serial_isr will call
extern susp, blind

serial_isr:

	cli

	call serial_interrupt

	sti
	iret