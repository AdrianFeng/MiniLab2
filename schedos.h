#ifndef WEENSYOS_SCHEDOS_H
#define WEENSYOS_SCHEDOS_H
#include "types.h"

/*****************************************************************************
 * schedos.h
 *
 *   Constants and variables shared by SchedOS's kernel and applications.
 *
 *****************************************************************************/

// System call numbers.
// An application calls a system call by causing the specified interrupt.

#define INT_SYS_YIELD		   48
#define INT_SYS_EXIT		   49
#define INT_SYS_SET_PRIORITY	   50
#define INT_SYS_SET_SHARE_NUMBER   51
#define INT_SYS_WRITE_WITH_ATOMIC  52
#define INT_SYS_CHECK_THE_LOCK     53
// The current screen cursor position (stored at memory location 0x198000).

extern uint16_t * volatile cursorpos;
extern uint32_t volatile ACQUIRE_NUMBER;

#endif
