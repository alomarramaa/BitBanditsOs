bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call			; The C function that sys_call_isr will call

sys_call_isr:
	; Save context
    pusha             ; Push all general purpose registers

    push gs             ; Push remaining segment registers
    push fs
    push es
    push ss
    push ds

    push dword esp      ; Push ESP as a parameter for the C function call

    mov eax sys_call

    call eax      ; Call your C function

	; Set ESP based on the return value of your function (EAX)
	mov esp, eax

	; Restore context
    add esp, 4          ; Adjust ESP to remove the parameter pushed earlier

    pop ds              ; Pop segment registers
    pop ss
    pop es
    pop fs
    pop gs

    popa               ; Pop all general purpose registers

	iret ; Return from ISR
