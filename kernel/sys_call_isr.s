bits 32
global sys_call_isr

extern sys_call			; The C function that sys_call_isr will call
sys_call_isr:
	
        pusha

        push ds
        push es
        push fs
        push gs
        push ss

        push esp
        
        call sys_call
        
        mov esp, eax
        
        pop ss
        pop gs
        pop fs
        pop es
        pop ds

        popa

	iret