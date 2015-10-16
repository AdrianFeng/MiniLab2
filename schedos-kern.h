#ifndef WEENSYOS_SCHEDOS_KERN_H
#define WEENSYOS_SCHEDOS_KERN_H
#include "schedos.h"
#include "x86.h"

/*****************************************************************************
 * schedos-kern.h
 *
 *   SchedOS kernel structures.
 *
 *****************************************************************************/

// Process state type
typedef enum procstate {
	P_EMPTY = 0,			// The process table entry is empty
					// (i.e. this is not a process)
	P_RUNNABLE,			// This process is runnable
	P_BLOCKED,			// This process is blocked
	P_ZOMBIE			// This process has exited (but note
					// that SchedOS has no sys_wait())
} procstate_t;

// Process descriptor type
typedef struct process {
	pid_t p_pid;			// Process ID

	registers_t p_registers;	// Current process state: registers,
					// stack location, EIP, etc.
					// 'registers_t' defined in x86.h

	procstate_t p_state;		// Process state; see above
	int p_exit_status;		// Process's exit status
        uint32_t p_priority;             // process' priority and the lower the number 
	                                // the higher the priority
        uint32_t share_number;          // the share value this process is assigned 
        uint32_t left_to_run;            // left run times to run

        uint16_t ticket_number;
} process_t;

// Clock frequency: the clock interrupt, if any, happens HZ times a second
#define HZ			1000

// The interrupt number corresponding to the first hardware interrupt
#define INT_HARDWARE		32
#define INT_CLOCK		(INT_HARDWARE + 0)

// Top of the kernel stack
#define KERNEL_STACK_TOP	0x180000
#define CHECK_K                 1
// Functions defined in schedos-kern.c
void interrupt(registers_t *reg);
void schedule(void);

// Functions defined in schedos-x86.c
void segments_init(void);
void interrupt_controller_init(bool_t allow_clock_interrupt);
void special_registers_init(process_t *proc);
void console_clear(void);
int console_read_digit(void);
// Function defined in schedos-loader.c
void program_loader(int programnumber, uint32_t *entry_point);

extern process_t *current;
void run(process_t *proc) __attribute__((noreturn));

#endif







		/*if (proc_array[pid].p_state == P_RUNNABLE) 
		  {
		  // skip if run more than share
		  if (proc_array[pid].left_to_run >= proc_array[pid].share_number) 
		    {
		      proc_array[pid].left_to_run = 0;
		    }
		  else 
		    {
		    proc_array[pid].left_to_run++;
		    run(&proc_array[pid]);
		    }
		  
		
		  pid = (pid + 1) % NPROCS; // don't change procs until share up
		  }
		  }*/
