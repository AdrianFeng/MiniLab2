#include "schedos-app.h"
#include "x86sync.h"

/*****************************************************************************
 * schedos-1
 *
 *   This tiny application prints red "1"s to the console.
 *   It yields the CPU to the kernel after each "1" using the sys_yield()
 *   system call.  This lets the kernel (schedos-kern.c) pick another
 *   application to run, if it wants.
 *
 *   The other schedos-* processes simply #include this file after defining
 *   PRINTCHAR appropriately.
 *
 *****************************************************************************/

#ifndef PRINTCHAR
#define PRINTCHAR	('1' | 0x0C00)
#endif

// UNCOMMENT THE NEXT LINE TO USE EXERCISE 8 CODE INSTEAD OF EXERCISE 6
#define __EXERCISE_8__



void
start(void)
{
	int i;
	sys_yield();
	for (i = 0; i < RUNCOUNT; i++) {
	  // Write characters to the console, yielding after each one.
	  
         // Use the following structure to choose between them:
         // #infdef __EXERCISE_8__
         // (exercise 6 code)
         // #else
         // (exercise 8 code)
	 // #endif
 
         #ifndef  __EXERCISE_8__

         ////////////EXERCISE 6 START HERE///////////////////////////////
	  /************************************************************
	   *for the exercise 6, I define a system call to acquire a lock 
	   *and also I define a global value ACQUIRE_NUMBER to store the 
           *current lock value and use the system call to modify the 
	   *ACQUIRE_NUMBER
           ***********************************************************/
	  while (check_the_lock()){}
	  atomic_swap((uint32_t *)cursorpos, (uint32_t)PRINTCHAR );
	  fetch_and_add((uint32_t *)(&cursorpos),2);
	  atomic_swap((uint32_t *)(&ACQUIRE_NUMBER), 0);
	  sys_yield();
	  ////////////EXERCISE 6 END HERE///////////////////////////////
         
          

          #else


	  ///////////////////EXERCISE 8 START HERE////////////////////////
	  /**************************************************************
	   *for the Exercise 8 I find a simpler way to make the value update 
           *become atomic, which is to update the *cursorpos and cursorpos
           *inside a system call.Therefore I implement a new system call
           *Write_with_atomic(...) to do this 
           **************************************************************/
	  Write_with_atomic(PRINTCHAR);
	  sys_yield();
	  ////////////////////EXERCISE 8 END HERE////////////////////////
         #endif
	 
	}

	//now we don't want this to be yield forever, so exit with 0 
	sys_exit(0);
}
