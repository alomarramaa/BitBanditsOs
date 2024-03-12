bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call			; The C function that sys_call_isr will call

sys_call_isr:
	; Save context
    push esp             ; Push all general purpose registers
    push ebp
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    push gs             ; Push remaining segment registers
    push fs
    push es
    push ss
    push ds

    push dword esp      ; Push ESP as a parameter for the C function call

    call sys_call      ; Call your C function

	; Set ESP based on the return value of your function (EAX)
	mov esp, eax

	; Restore context
    add esp, 4          ; Adjust ESP to remove the parameter pushed earlier

    pop ds              ; Pop segment registers
    pop ss
    pop es
    pop fs
    pop gs
    pop eax               ; Pop all general purpose registers
    pop ebx
    pop ecx
    pop edx
    pop esi
    pop edi
    pop ebp
    pop esp

	iret ; Return from ISR
