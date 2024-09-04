bits 32
global sys_call_isr

extern sys_call			; The C function that sys_call_isr will call
sys_call_isr:
	
        pusha ; push all general-purpose CPU register values onto the stack

        push ds ; push the value given by the operand onto the stack
        push es
        push fs
        push gs
        push ss

        push esp
        
        call sys_call ; call a subroutine
        
        mov esp, eax ; move the contents of one location into another
        
        pop ss ; pop the value off of the top of the stack into the location described by the operand
        pop gs
        pop fs
        pop es
        pop ds

        popa ; pop values off of the stack into all general purpose CPU registers

	iret ; return from interrupt