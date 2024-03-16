bits 32
global sys_call_isr

extern sys_call            ; Declare sys_call as an external symbol

sys_call_isr:
    ; Save context
    pusha                   ; Push all general purpose registers
    push gs                 ; Push remaining segment registers
    push fs
    push es
    push ss
    push ds

    push dword esp          ; Push ESP as a parameter for the C function call

    call sys_call           ; Call the C function

    ; Set ESP based on the return value of your function (EAX)
    mov esp, eax

    ; Restore context
    pop ds                  ; Pop segment registers
    pop ss
    pop es
    pop fs
    pop gs

    popa                    ; Pop all general purpose registers

    iret                    ; Return from ISR