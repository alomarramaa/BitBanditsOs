bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call			; The C function that sys_call_isr will call

sys_call_isr:
	; Save context
    pushad              ; Push all general purpose registers
    push ds             ; Push remaining segment registers
    push es
    push fs
    push gs

    push dword esp      ; Push ESP as a parameter for the C function call

    call sys_call      ; Call your C function

	; Restore context
    add esp, 4          ; Adjust ESP to remove the parameter pushed earlier

    pop gs              ; Pop segment registers
    pop fs
    pop es
    pop ds

    popad               ; Pop all general purpose registers

	iret ; Return from ISR
