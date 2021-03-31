/*
 * atomic.h
 *
 * Created: 31/03/2021 23:29:53
 *  Author: randy
 */ 


#ifndef ATOMIC_H_
#define ATOMIC_H_

#if defined(__GNUC__) || defined (__DOXYGEN__)

#	define ENTER_CRITICAL(UNUSED)		__asm__ __volatile__ (   \
		"in __tmp_reg__, __SREG__"                    "\n\t" \
		"cli"                                         "\n\t" \
		"push __tmp_reg__"                            "\n\t" \
		::: "memory"                                         \
	)
#	define LEAVE_CRITICAL(UNUSED)		__asm__ __volatile__ (   \
		"pop __tmp_reg__"                             "\n\t" \
		"out __SREG__, __tmp_reg__"                   "\n\t" \
		::: "memory"                                         \
	)
#	define DISABLE_INTERRUPTS()			__asm__ __volatile__ ( "cli" ::: "memory")
#	define ENABLE_INTERRUPTS()			__asm__ __volatile__ ( "sei" ::: "memory")

#elif defined(__ICCAVR__)

#	define ENTER_CRITICAL(P)			unsigned char P = __save_interrupt();__disable_interrupt();
#	define LEAVE_CRITICAL(P)			__restore_interrupt(P);
#	define DISABLE_INTERRUPTS()			__disable_interrupt();
#	define ENABLE_INTERRUPTS()			__enable_interrupt();

#else
#  error Unsupported compiler.
#endif

#endif /* ATOMIC_H_ */