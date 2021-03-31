/*
 * compiler.h
 *
 * Created: 31/03/2021 23:45:22
 *  Author: randy
 */ 


#ifndef COMPILER_H_
#define COMPILER_H_

#if defined(__GNUC__)
#	include <avr/io.h>
#	include <avr/builtins.h>
#elif defined(__ICCAVR__)
#	define ENABLE_BIT_DEFINITIONS 1
#	include <ioavr.h>
#	include <intrinsics.h>
#	ifndef CCP_IOREG_gc
#		define CCP_IOREG_gc 0xD8 /* CPU_CCP_IOREG_gc */
#	endif
#	ifndef CCP_SPM_gc
#		define CCP_SPM_gc 0x9D /* CPU_CCP_SPM_gc */
#	endif
#else
#	error Unsupported compiler.
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#if defined(__DOXYGEN__)
#	define ISR(vect)
#elif defined(__GNUC__)
#	include <avr/interrupt.h>
#elif defined(__ICCAVR__)
#	define __ISR(x) _Pragma(#x)
#	define ISR(vect) __ISR(vector = vect) __interrupt void handler_##vect(void)
#endif

#ifdef __GNUC__
#	define cpu_irq_enable() sei()
#	define cpu_irq_disable() cli()
#else
#	define cpu_irq_enable() __enable_interrupt()
#	define cpu_irq_disable() __disable_interrupt()
#endif

#define UNUSED(v) (void)(v)

#endif /* COMPILER_H_ */