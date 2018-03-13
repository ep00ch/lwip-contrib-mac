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

#include <time.h>


#include ":lwip:sys.h"
#include ":lwip:def.h"
#include ":lwip:opt.h"

#if NO_SYS
struct thread_struct_wrapper *lwip_system_threads = NULL; // a list of all threads created by lwIP

u32_t
sys_jiffies(void)
{
	return (u32_t)clock();
}

#else

static clock_t sys_start;

u32_t
sys_jiffies(void)
{
	return (u32_t)(clock() - sys_start);
}

u32_t
sys_now(void)
{
	return (u32_t)(sys_jiffies() * (1000/CLOCKS_PER_SEC) );
}

void
sys_init(void)
{
	sys_start = clock();
	return;
}

void
sys_arch_block(u16_t time)
{
  u16_t ticks;
  
  ticks = (time * (1000/CLOCKS_PER_SEC)) + clock();

  while (clock() != ticks) {}  
}

err_t 
sys_mbox_new(sys_mbox_t *mbox, int size)
{
	mbox = SYS_MBOX_NULL;
	return ERR_OK;
}

void
sys_mbox_free(sys_mbox_t *mbox)
{
  return;
}


err_t
sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  return ERR_OK;
}

err_t
sys_sem_new(sys_sem_t *sem, u8_t count)
{
	sem = 0;
	return ERR_OK;
}

u32_t
sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  sys_arch_block(timeout);
  return 0;
}

void
sys_sem_signal(sys_sem_t *sem)
{
  return;
}

void
sys_sem_free(sys_sem_t *sem)
{
  return;
}

sys_thread_t
sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    return 0;
}
#endif

u32_t
sys_now(void)
{
	return (u32_t)(sys_jiffies() * (1000/CLOCKS_PER_SEC));
}
