#include <mpx/gdt.h>
#include <mpx/interrupts.h>
#include <mpx/serial.h>
#include <mpx/vm.h>
#include "sys_req.h"
#include <string.h>
#include <memory.h>
#include <user/date.h>
#include <processes.h>
#include <mpx/pcb.h>

#include <user/comhand.h>

static void klogv(device dev, const char *msg)
{
	char prefix[] = "klogv: ";
	serial_out(dev, prefix, strlen(prefix));
	serial_out(dev, msg, strlen(msg));
	serial_out(dev, "\r\n", 2);
}

void kmain(void)
{
	// 0) Serial I/O -- <mpx/serial.h>
	// If we don't initialize the serial port, we have no way of
	// performing I/O. So we need to do that before anything else so we
	// can at least get some output on the screen.
	// Note that here, you should call the function *before* the output
	// via klogv(), or the message won't print. In all other cases, the
	// output should come first as it describes what is about to happen.
	klogv(COM1, "Initialized serial I/O on COM1 device...");
	serial_init(COM1);

	// 1) Global Descriptor Table (GDT) -- <mpx/gdt.h>
	// Keeps track of the various memory segments (Code, Data, Stack, etc.)
	// required by the x86 architecture. This needs to be initialized before
	// interrupts can be configured.
	klogv(COM1, "Initializing Global Descriptor Table...");
	gdt_init();

	// 2) Interrupt Descriptor Table (IDT) -- <mpx/interrupts.h>
	// Keeps track of where the various Interrupt Vectors are stored. It
	// needs to be initialized before Interrupt Service Routines (ISRs) can
	// be installed.
	klogv(COM1, "Initializing Interrupt Descriptor Table...");
	idt_init();

	// 3) Disable Interrupts -- <mpx/interrupts.h>
	// You'll be modifying how interrupts work, so disable them to avoid
	// crashing.
	klogv(COM1, "Disabling interrupts...");
	cli();

	// 4) Interrupt Request (IRQ) -- <mpx/interrupts.h>
	// The x86 architecture requires ISRs for at least the first 32
	// Interrupt Request (IRQ) lines.
	klogv(COM1, "Initializing Interrupt Request routines...");
	irq_init();

	// 5) Programmable Interrupt Controller (PIC) -- <mpx/interrupts.h>
	// The x86 architecture uses a Programmable Interrupt Controller (PIC)
	// to map hardware interrupts to software interrupts that the CPU can
	// then handle via the IDT and its list of ISRs.
	klogv(COM1, "Initializing Programmable Interrupt Controller...");
	pic_init();

	// 6) Reenable interrupts -- <mpx/interrupts.h>
	// Now that interrupt routines are set up, allow interrupts to happen
	// again.
	klogv(COM1, "Enabling Interrupts...");
	sti();

	// 7) Virtual Memory (VM) -- <mpx/vm.h>
	// Virtual Memory (VM) allows the CPU to map logical addresses used by
	// programs to physical address in RAM. This allows each process to
	// behave as though it has exclusive access to memory. It also allows,
	// in more advanced systems, the kernel to swap programs between RAM and
	// storage (such as a hard drive or SSD), and to set permissions such as
	// Read, Write, or Execute for pages of memory. VM is managed through
	// Page Tables, data structures that describe the logical-to-physical
	// mapping as well as manage permissions and other metadata.
	klogv(COM1, "Initializing Virtual Memory...");
	vm_init();

	// 8) MPX Modules -- *headers vary*
	// Module specific initialization -- not all modules require this.
	klogv(COM1, "Initializing MPX modules...");
	// R5: sys_set_heap_functions(...);
	// R4: create commhand and idle processes

	// 9) YOUR command handler -- *create and #include an appropriate .h file*
	// Pass execution to your command handler so the user can interact with
	// the system.
	klogv(COM1, "Transferring control to commhand...");

	// Create a comhand pcb with the highest priority
	create_comhand_pcb();

	// Create a system idle process running as the lowest priority (provided in sys_idle_process()
	create_system_idle_pcb();

	// pcb* comhand_pcb = pcb_setup("command_handler", SYSTEM, 0);
	// // Set the stack to contain the address of the comhand function
	// comhand_pcb->pcb_stack[PCB_STACK_SIZE] = (int) comhand;
	// pcb_insert(comhand_pcb);

	// // Create the system idle process pcb with the lowest priority
	// pcb* sys_idle_pcb = pcb_setup("system_idle_proc", SYSTEM, 9);
	// // Set the stack to contain the address of the system idle process function
	// sys_idle_pcb->pcb_stack[PCB_STACK_SIZE] = (int) sys_idle_process;
	// pcb_insert(sys_idle_pcb);

	// Give the comhand function control
	__asm__ volatile("int $0x60" ::"a"(IDLE));

	// 10) System Shutdown -- *headers to be determined by your design*
	// After your command handler returns, take care of any clean up that
	// is necessary.
	klogv(COM1, "Starting system shutdown procedure...");

	// 11) Halt CPU -- *no headers necessary, no changes necessary*
	// Execution of kmain() will complete and return to where it was called
	// in boot.s, which will then attempt to power off Qemu or halt the CPU.
	klogv(COM1, "Halting CPU...");
}

void load_comhand_pcb(void (*comhand)(void), char *proc_name, int proc_priority)
{

	// Attempt to create a pcb for the given function
	pcb *new_process = pcb_setup(proc_name, USER, proc_priority);

	// Sets registers of stack
	struct context *c = (struct context *)new_process->stackPtr;
	c->FS = 0x10;
	c->GS = 0x10;
	c->DS = 0x10;
	c->ES = 0x10;
	c->CS = 0x8;
	c->ESP = (int)(new_process->stackPtr);
	c->EBP = (int)(new_process->pcb_stack);
	c->EIP = (int)comhand;
	c->EFLAGS = 0x202;

	pcb_insert(new_process);
}

//Creates a comhand PCB with the highest priority
void create_comhand_pcb(void)
{

	load_comhand_pcb(comhand, "comhand", 0);
}

void load_system_idle_pcb(void (*sys_idle_process)(void), char *proc_name, int proc_priority)
{

	// Attempt to create a pcb for the given function
	pcb *new_process = pcb_setup(proc_name, SYSTEM, proc_priority);

	// Sets registers of stack
	struct context *c = (struct context *)new_process->stackPtr;
	c->FS = 0x10;
	c->GS = 0x10;
	c->DS = 0x10;
	c->ES = 0x10;
	c->CS = 0x8;
	c->ESP = (int)(new_process->stackPtr);
	c->EBP = (int)(new_process->pcb_stack);
	c->EIP = (int)sys_idle_process;
	c->EFLAGS = 0x202;

	pcb_insert(new_process);
}


//Creates a system idle PCB with the lowest priority
void create_system_idle_pcb(void)
{

	load_system_idle_pcb(sys_idle_process, "idle", 9);
}
