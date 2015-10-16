#ifndef WEENSYOS_SCHEDOS_APP_H
#define WEENSYOS_SCHEDOS_APP_H
#include "schedos.h"

/*****************************************************************************
 * schedos-app.h
 *
 *   System call functions and constants used by SchedOS applications.
 *
 *****************************************************************************/


// The number of times each application should run
#define RUNCOUNT	320
/*****************************************************************************
 * check_the_lock(void)
 *
 *  System call exercise 6
 *
 *****************************************************************************/
static inline uint32_t
check_the_lock(void)
{
	uint32_t result;
	asm volatile("int %1\n"
		     : "=a" (result)
	             : "i" (INT_SYS_CHECK_THE_LOCK)
		     : "cc", "memory");
	return result;
}
/*****************************************************************************
 * Write_with_atomic(uint16_t input)
 *
 *  System call exercise 8
 *
 *****************************************************************************/

static inline void
Write_with_atomic(uint16_t input)
{
	
	asm volatile("int %0\n"
		     : : "i" (INT_SYS_WRITE_WITH_ATOMIC),
		       "a" (input)
		     : "cc", "memory");
}

/*****************************************************************************
 * sys_yield
 *
 *   Yield control of the CPU to the kernel, which will pick another
 *   process to run.  (It might run this process again, depending on the
 *   scheduling policy.)
 *
 *****************************************************************************/

static inline void
sys_yield(void)
{
	// We call a system call by causing an interrupt with the 'int'
	// instruction.  In weensyos, the type of system call is indicated
	// by the interrupt number -- here, INT_SYS_YIELD.
	asm volatile("int %0\n"
		     : : "i" (INT_SYS_YIELD)
		     : "cc", "memory");
}


/*****************************************************************************
 * sys_exit(status)
 *
 *   Exit the current process with exit status 'status'.
 *
 *****************************************************************************/

static inline void sys_exit(int status) __attribute__ ((noreturn));
static inline void
sys_exit(int status)
{
	// Different system call, different interrupt number (INT_SYS_EXIT).
	// This time, we also pass an argument to the system call.
	// We do this by loading the argument into a known register; then
	// the kernel can look up that register value to read the argument.
	// Here, the status is loaded into register %eax.
	// You can load other registers with similar syntax; specifically:
	//	"a" = %eax, "b" = %ebx, "c" = %ecx, "d" = %edx,
	//	"S" = %esi, "D" = %edi.
	asm volatile("int %0\n"
		     : : "i" (INT_SYS_EXIT),
		         "a" (status)
		     : "cc", "memory");
    loop: goto loop; // Convince GCC that function truly does not return.
}




/*****************************************************************************
 * Set_Priority(int priority_number)
 *
 *   System cal for exercise 4a.
 *
 *****************************************************************************/

static inline void
Set_Priority(int priority_number)
{
	
	asm volatile("int %0\n"
		     : : "i" (INT_SYS_SET_PRIORITY),
		       "a" (priority_number)
		     : "cc", "memory");
}


/*****************************************************************************
 * Set_Share_Number(int share_number)
 *
 *  System call exercise 4b
 *
 *****************************************************************************/


static inline void
Set_Share_Number(int share_number)
{
	
	asm volatile("int %0\n"
		     : : "i" (INT_SYS_SET_SHARE_NUMBER),
		     "a" (share_number)
		     : "cc", "memory");
}



#endif
