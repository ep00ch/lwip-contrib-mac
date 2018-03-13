/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__


/* Turn debugging on */
// We define with the compiler on a per-file basis
//#define LWIP_DEBUG
//#undef LWIP_DEBUG

#if defined LWIP_DEBUG
#include <stdio.h>
#endif

#define BYTE_ORDER BIG_ENDIAN

typedef unsigned char	u_char;
typedef unsigned char	u8_t;
typedef signed char		s8_t;
typedef unsigned short	u16_t;
typedef signed short	s16_t;
typedef unsigned int	u32_t;
typedef signed int		s32_t;

typedef u32_t mem_ptr_t;

#define __sio_fd_t_defined
typedef void * sio_fd_t;

/* Compiler hints for packing structures */
#define PACK_STRUCT_BEGIN //#pragma options align=mac68k
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END //#pragma options align=reset
#define PACK_STRUCT_FIELD(x) x
//#define ALIGN_STRUCT_8_BEGIN #pragma options align=power
//#define ALIGN_STRUCT_END #pragma options align=reset


/*
 * Debug printing formats
 */
#define X16_F "x"
#define U16_F "u"

#define S16_F "d"

#define X32_F "x"
#define U32_F "u"
#define S32_F "d"

/* Plaform specific diagnostic output */
#ifdef LWIP_DEBUG 
  
#if SER_DEBUG
/* Proto for the serial debug print function in test.c */
void ser_debug_print( const char *fmt, ...);

#define LWIP_PLATFORM_DIAG(x) do {ser_debug_print x;} while(0)
#else
#define LWIP_PLATFORM_DIAG(x) do {printf x;} while(0)
#endif

#define LWIP_PLATFORM_ASSERT(x) LWIP_PLATFORM_DIAG(("Assertion Failed: \"%s\" at line %d in %s\n", \
                                     x, __LINE__, __FILE__))

#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
				LWIP_PLATFORM_DIAG(("Error: %s", message)); handler;}} while(0)
  
#else

#define LWIP_PLATFORM_DIAG(x)
#define LWIP_PLATFORM_ASSERT(x)
#endif


/*
#define LWIP_PLATFORM_DIAG(x)
#define LWIP_PLATFORM_DIAG(x) lwip_debug_print x
#define LWIP_PLATFORM_DIAG(x) do {printf x;} while(0)

#define LWIP_PLATFORM_ASSERT(x) lwip_debug_print( "Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__)
*/

#endif /* __ARCH_CC_H__ */
