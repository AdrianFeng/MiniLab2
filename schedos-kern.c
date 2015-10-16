#include "schedos-kern.h"
#include "schedos-app.h"
#include "x86.h"
#include "lib.h"

/*****************************************************************************
 * schedos-kern
 *
 *   This is the schedos's kernel.
 *   It sets up process descriptors for the 4 applications, then runs
 *   them in some schedule.
 *
 *****************************************************************************/

// The program loader loads 4 processes, starting at PROC1_START, allocating
// 1 MB to each process.
// Each process's stack grows down from the top of its memory space.
// (But note that SchedOS processes, like MiniprocOS processes, are not fully
// isolated: any process could modify any part of memory.)

#define NPROCS		5
#define PROC1_START	0x200000
#define PROC_SIZE	0x100000

// +---------+-----------------------+--------+---------------------+---------/
// | Base    | Kernel         Kernel | Shared | App 0         App 0 | App 1
// | Memory  | Code + Data     Stack | Data   | Code + Data   Stack | Code ...
// +---------+-----------------------+--------+---------------------+---------/
// 0x0    0x100000               0x198000 0x200000              0x300000
//
// The program loader puts each application's starting instruction pointer
// at the very top of its stack.
//
// System-wide global variables shared among the kernel and the four
// applications are stored in memory from 0x198000 to 0x200000.  Currently
// there is just one variable there, 'cursorpos', which occupies the four
// bytes of memory 0x198000-0x198003.  You can add more variables by defining
// their addresses in schedos-symbols.ld; make sure they do not overlap!


// A process descriptor for each process.
// Note that proc_array[0] is never used.
// The first application process descriptor is proc_array[1].
static process_t proc_array[NPROCS];
//static uint32 share_one[NPROCS];
// A pointer to the currently running process.
// This is kept up to date by the run() function, in mpos-x86.c.
process_t *current;

// The preferred scheduling algorithm.
int scheduling_algorithm;
//define two global variable for EXERCISE 7
//the random_number record the last generated random number
uint16_t random_number=0x735Fu;
//  the total ticket to recosd the overall tickets that all process have 
uint16_t total_ticket=0;
// UNCOMMENT THESE LINES IF YOU DO EXERCISE 4.A
// Use these #defines to initialize your implementation.
// Changing one of these lines should change the initialization.
//#define __PRIORITY_1__ 1
//#define __PRIORITY_2__ 2
//#define __PRIORITY_3__ 3
//#define __PRIORITY_4__ 4

// UNCOMMENT THESE LINES IF YOU DO EXERCISE 4.B
// Use these #defines to initialize your implementation.
// Changing one of these lines should change the initialization.
// #define __SHARE_1__ 1
// #define __SHARE_2__ 2
// #define __SHARE_3__ 3
// #define __SHARE_4__ 4

// USE THESE VALUES FOR SETTING THE scheduling_algorithm VARIABLE.
#define __EXERCISE_1__   0  // the initial algorithm
#define __EXERCISE_2__   2  // strict priority scheduling (exercise 2)
#define __EXERCISE_4A__ 41  // p_priority algorithm (exercise 4.a)
#define __EXERCISE_4B__ 42  // p_share algorithm (exercise 4.b)
#define __EXERCISE_7__   7  // any algorithm for exercise 7

void generate_random(uint16_t *previous ) //function to  generate a random number
{
   uint16_t initial = *previous;  
   uint16_t temp = initial;
   // just generate a random function to modify the previous to get a new random value 
   unsigned record = temp & 1;  
   temp = (temp >> 2) ^ (temp << 4 );    
   temp >>=1;
   temp = (temp >> 6) ^ (temp << 8);
   temp ^= (-record) & 0xA5C3u; 
    
   if (temp != initial)
     {
       *previous =temp;
     }
   else
     { // if it happens to be the same, just regenerate.
       generate_random(previous);
     }
}

/*****************************************************************************
 * start
 *
 *   Initialize the hardware and process descriptors, then run
 *   the first process.
 *
 *****************************************************************************/

void
start(void)
{
	int i;

	// Set up hardware (schedos-x86.c)
	segments_init();
	interrupt_controller_init(0);
	console_clear();

	// Initialize process descriptors as empty
	memset(proc_array, 0, sizeof(proc_array));
	//	memset(share_one; 0 sizeof( share_one);
	for (i = 0; i < NPROCS; i++) {
		proc_array[i].p_pid = i;
		proc_array[i].p_state = P_EMPTY;
		proc_array[i].left_to_run = 0;
		//generate_random(&random_number);
		//proc_array[i].ticket_number=random_number%total_ticket;
	}

	// Set up process descriptors (the proc_array[])
	for (i = 1; i < NPROCS; i++) {
		process_t *proc = &proc_array[i];
		uint32_t stack_ptr = PROC1_START + i * PROC_SIZE;

		// Initialize the process descriptor
		special_registers_init(proc);

		// Set ESP
		proc->p_registers.reg_esp = stack_ptr;

		// Load process and set EIP, based on ELF image
		program_loader(i - 1, &proc->p_registers.reg_eip);

		// Mark the process as runnable
		proc->p_state = P_RUNNABLE;
	}

	// Initialize the cursor-position shared variable to point to the
	// console's first character (the upper left).
	cursorpos = (uint16_t *) 0xB8000;

	// Initialize the scheduling algorithm.
	// USE THE FOLLOWING VALUES:
	//    0 = the initial algorithm
	//    2 = strict priority scheduling (exercise 2)
	//   41 = p_priority algorithm (exercise 4.a)
	//   42 = p_share algorithm (exercise 4.b)
	//    7 = any algorithm that you may implement for exercise 7
	scheduling_algorithm = 0;
	/*******************************************************
	* set the priority value for each process
        * set the share_number value for each process
        * set the lottery ticket value for each process
        *
	*******************************************************/
	/********************************************************
	* also you can use the system call that I implememnted to set
        * the priority value and the share value inside the process
        * I initlize here to make test more convineint
        *******************************************************/
	proc_array[1].p_priority = 6 ;
	proc_array[2].p_priority = 6 ;
	proc_array[3].p_priority = 9 ;
	proc_array[4].p_priority = 0 ;
	///////////////////////////////////////////////////////
	proc_array[1].share_number = 1;
	proc_array[2].share_number = 9 ;
	proc_array[3].share_number = 10 ;
	proc_array[4].share_number = 1 ;
	//////////////////////////////////////////////////////
	/*for the lottery ticket scheduling algorithm, the ticket number  
	*assigned for each process is better to be defined here since 
	*the total ticket number should be calculated here */
	//////////////////////////////////////////////////////
	proc_array[1].ticket_number = 100 ;
	proc_array[2].ticket_number = 300;
	proc_array[3].ticket_number = 700 ;
	proc_array[4].ticket_number = 2000;
	////////////////////////////////////////////////////////
	for (i=1;i< NPROCS; i++)
	  {
	    total_ticket+=proc_array[i].ticket_number;
	    proc_array[i].ticket_number=total_ticket;
	  }
	// Switch to the first process.
	  run(&proc_array[1]);

	// Should never get here!
	while (1)
		/* do nothing */;
}



/*****************************************************************************
 * interrupt
 *
 *   This is the weensy interrupt and system call handler.
 *   The current handler handles 4 different system calls (two of which
 *   do nothing), plus the clock interrupt.
 *
 *   Note that we will never receive clock interrupts while in the kernel.
 *
 *****************************************************************************/

void
interrupt(registers_t *reg)
{
	// Save the current process's register state
	// into its process descriptor
	current->p_registers = *reg;

	switch (reg->reg_intno) {

	case INT_SYS_YIELD:
		// The 'sys_yield' system call asks the kernel to schedule
		// the next process.
		schedule();

	case INT_SYS_EXIT:
		// 'sys_exit' exits the current process: it is marked as
		// non-runnable.
		// The application stored its exit status in the %eax register
		// before calling the system call.  The %eax register has
		// changed by now, but we can read the application's value
		// out of the 'reg' argument.
		// (This shows you how to transfer arguments to system calls!)
		current->p_state = P_ZOMBIE;
		current->p_exit_status = reg->reg_eax;
		schedule();

	case INT_SYS_SET_PRIORITY:
	        //actuall system call to set the priority number for the current process  
		current->p_priority = reg->reg_eax;
		run(current);

	case INT_SYS_SET_SHARE_NUMBER:
	        //actuall system call to set the share number for the current process
	        current->share_number = reg->reg_eax;
		run(current);

	case INT_CLOCK:
		// A clock interrupt occurred (so an application exhausted its
		// time quantum).
		// Switch to the next runnable process.
		schedule();
	case INT_SYS_WRITE_WITH_ATOMIC:
	        //a system call to update the value that is stored in the cursorpos
	        // and then increase cursorpos by 1
	       *cursorpos = reg->reg_eax;
	       cursorpos+=1;
	       run(current);

	case INT_SYS_CHECK_THE_LOCK :
	     if (ACQUIRE_NUMBER == 0)
	         {
	           ACQUIRE_NUMBER = 1;
	           current->p_registers.reg_eax =0;
	         }
	     else
	         {
	           current->p_registers.reg_eax =1;
	         }
	         run(current);
	default:
		while (1)
			/* do nothing */;

	}
}



/*****************************************************************************
 * schedule
 *
 *   This is the weensy process scheduler.
 *   It picks a runnable process, then context-switches to that process.
 *   If there are no runnable processes, it spins forever.
 *
 *   This function implements multiple scheduling algorithms, depending on
 *   the value of 'scheduling_algorithm'.  We've provided one; in the problem
 *   set you will provide at least one more.
 *
 *****************************************************************************/

void
schedule(void)
{
	pid_t pid = current->p_pid;
	pid_t temp;
	uint32_t index = 0;
	pid_t next_to_run =-1;
	uint32_t next_priority = -1;
	uint32_t check =0;
	uint32_t Priority;
	uint16_t current_record;
	if (scheduling_algorithm ==  __EXERCISE_1__ )
		while (1) {
			pid = (pid + 1) % NPROCS;

			// Run the selected process, but skip
			// non-runnable processes.
			// Note that the 'run' function does not return.
			if (proc_array[pid].p_state == P_RUNNABLE)
				run(&proc_array[pid]);
		}
	else if  (scheduling_algorithm ==  __EXERCISE_2__ ) 
	  {
	    /**********************EXERCISE 2 START HERE **********************/
	    //implementation for priority scheduling the lower the pid, the higher 
	    //its priority
	    pid=1;
	    while (1)
	      {
		// search the process list from the beginning to find the first 
		// runnable process to shcedule it to run
		if (proc_array[pid].p_state == P_RUNNABLE)
		  {
		    run(&proc_array[pid]);
		  }
		else
		  {
		    pid=pid+1;
		    // if meet the end of the list and we find again from the beginning
		    if (pid == (pid_t)NPROCS )
		      {
			pid =1;
		      }
		  }
	      }
	    /************************EXERCISE 2 END HERE***********************/
	  }
	// If we get here, we are running an unknown scheduling algorithm.
	else if (scheduling_algorithm == __EXERCISE_4A__ )
	  {
	    /**********************EXERCISE 4A START HERE **********************/
	    while(1)
	      {
		index = 0; 
		next_to_run =-1;
		next_priority = -1;
		check =0;
		while (index <  (NPROCS))
		  {
		    pid=(pid+1)%NPROCS;
		    if(proc_array[pid].p_state == P_RUNNABLE)
		      {
			if (check == 0 )
			  {
			    next_priority = proc_array[pid].p_priority;
			    check = 1;
			    next_to_run = pid;
			  }
			else
			  {
			    if ( proc_array[pid].p_priority < next_priority )
			      {
				next_priority = proc_array[pid].p_priority;
				next_to_run = pid;
			      }
			  }
		      }
		    index = index+1;
		  }
		if ( check == 1)
		  {
		    run(&proc_array[next_to_run]);
		  }
	      }
	    /**********************EXERCISE 4A END HERE **********************/
	  }
	else if (scheduling_algorithm == __EXERCISE_4B__)
	  {
	     /**********************EXERCISE 4B START HERE **********************/
	    while (1)
	      {

		temp= (pid+1) %(NPROCS);
		index =0;
		check =0;
		if (proc_array[pid].p_state == P_RUNNABLE && proc_array[pid].left_to_run >0 )
		  {
		    proc_array[pid].left_to_run--;
		    run(&proc_array[pid]);
		  }
		else 
		  {
		    Priority = 0 ;
		    proc_array[pid].left_to_run= proc_array[pid].share_number;
		    for (index = 0 ; index < NPROCS-1  ; index ++)
		      {
			//if (temp == 0 )
			// {
			//  continue;
			// }
			if (proc_array[temp].p_state == P_RUNNABLE)
			  {
			    if ( proc_array[temp].share_number > Priority )
			      {
				pid = temp;
				//Priority = proc_array[temp].share_number;
				//flag=proc_array[temp].flag;
			      }
			  }
			temp = (temp +1)%( NPROCS);
		      }
		   
		  }
	      }
	     /**********************EXERCISE 4B END HERE **********************/
	  }
	else if (scheduling_algorithm == __EXERCISE_7__)
	  {
	    /**********************EXERCISE 7 START HERE **********************/
	    while(1)
	      {
		generate_random( &random_number);
		current_record = random_number%total_ticket;
		for (index =1 ; index < NPROCS; index ++)
		  {
		    if (current_record <=proc_array[index].ticket_number && proc_array[index].p_state == P_RUNNABLE) 
		      {
			run(&proc_array[index]);
			break;
		      }
		  }
		//pid = (pid + 1) % NPROCS;
	      }
	    /**********************EXERCISE 7 START HERE **********************/
	  }
	else
	cursorpos = console_printf(cursorpos, 0x100, "\nUnknown scheduling algorithm %d\n", scheduling_algorithm);
	while (1)
	  /* do nothing */;
}
