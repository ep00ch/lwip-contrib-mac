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
 */

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/sio.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <Errors.h>
#include <Files.h>
#include <Devices.h>
#include <Serial.h>

/**
 * SIO_DEBUG: Enable debugging for SIO.
 */
#ifndef SIO_DEBUG
#define SIO_DEBUG    LWIP_DBG_OFF
#endif

/**
 * MACOS_INPUT_BUFF_SIZE: Set the ppp input buffer. 
 * 64 or less uses driver's default 64 byte buffer.
 */
#ifndef MACOS_SIO_BUFF_SIZE
#define MACOS_SIO_BUFF_SIZE    64
#endif


/* sio_fd_t is supposed to be a void pointer, so we need to do some casting back. */
#define PORT_NUM(sd) (*(short *)sd)

#define IN_REF(sd) (int)(PORT_NUM(sd) + 3) * -2
#define OUT_REF(sd) (int)IN_REF(sd)-1

u8_t sio_setup(sio_fd_t sd);
OSErr ROMSDOpen(SPortSel whichPort);

char *sio_input_buffer;

#if NO_SYS==0
static unsigned char sio_abort = 0xFF;
#endif

#pragma segment LWUPDN
u8_t sio_setup(sio_fd_t fd)
{
	OSErr   error;	
	
	unsigned short  baud;
	short  databits;
	short  stopbits;
	short  parity;
	short  config;
	SerShk shake;

	/* Set the new data */
	baud = baud19200;		/* 19200 */
	parity = noParity;		/* None*/
	databits = data8;		/* 8 */
	stopbits = stop10;		/* 1 */

	/* do not use flow control */
	shake.fXOn = 0;
	shake.fCTS = 0;
	shake.xOn  = 0;
	shake.xOff = 0;
	shake.errs = 0;
	shake.evts = 0;
	shake.fInX = 0;
	shake.fDTR = 0;

	config = baud + parity + databits + stopbits;
	//config = (baud | parity | databits | stopbits);
	
	LWIP_DEBUGF(SIO_DEBUG, ("sio_setup(%d, %d)\n", IN_REF(fd), OUT_REF(fd))) ;

	if (error = SerReset(IN_REF(fd), config))
		LWIP_DEBUGF(SIO_DEBUG | LWIP_DBG_LEVEL_WARNING, ("sio_setup(%d) can't configure serial port due to \'SerReset\' error:%d. Continuing...\n", IN_REF(fd), error)) ;
	if (error = SerHShake(IN_REF(fd), &shake))
		LWIP_DEBUGF(SIO_DEBUG | LWIP_DBG_LEVEL_WARNING, ("sio_setup(%d) can't disable serial handshake due to \'SerHShake\' error:%d. Continuing...\n", IN_REF(fd), error)) ;
		
	if (error = SerReset(OUT_REF(fd), config))
		LWIP_DEBUGF(SIO_DEBUG | LWIP_DBG_LEVEL_WARNING, ("sio_setup(%d) can't configure serial port due to \'SerReset\' error:%d. Continuing...\n", OUT_REF(fd), error)) ;
	if (error = SerHShake(OUT_REF(fd), &shake))
		LWIP_DEBUGF(SIO_DEBUG | LWIP_DBG_LEVEL_WARNING, ("sio_setup(%d) can't disable serial handshake due to \'SerHShake\' error:%d. Continuing...\n", OUT_REF(fd), error)) ;
	
	return 1;
}


OSErr ROMSDOpen(SPortSel whichPort)
{
	OSErr error = fnfErr;
	short inRef, outRef;
	
	char *inDriver;
	char *outDriver;

	switch (whichPort)
	{
		case sPortA:
			inDriver  = ".AIn";
			outDriver = ".AOut";
			break;
		
		case sPortB:
			inDriver  = ".BIn";
			outDriver = ".BOut";
			break;

		default:
			return error;
			break;
	}
	
	error = opendriver(outDriver, &outRef);
	LWIP_DEBUGF(SIO_DEBUG, ("ROMSDOpen(%d) 'opendriver(%s, %d)\n", whichPort, outDriver, outRef )) ;
	if (error == noErr)
	{
		error = opendriver(inDriver, &inRef);
		LWIP_DEBUGF(SIO_DEBUG, ("ROMSDOpen(%d) 'opendriver(%s, %d)\n", whichPort, inDriver, inRef )) ;
	}
		
	LWIP_DEBUGF(SIO_DEBUG, ("ROMSDOpen(%d) will return: %d\n", whichPort, error));
	
	return error;
}
 
/**
 * Opens a serial device for communication.
 * 
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{
	OSErr error = fnfErr;
	/* These are the the serial port numbers that 'sio_fd_t sd' points to. */
	static const short modemPort = sPortA;
	static const short printerPort = sPortB;
	/* The serial port device is just a pointer to one of the constant port numbers. */ 
	sio_fd_t sd;

	/* Keep track of the port name for debugging and warnings. */
	char* port_name;

	/* We don't have devices, but sPortA (0) is Modem and sPortB (1) is Printer. */ 
	switch (devnum)
	{
		case 'P' :
		case 'p' :
		case 'b' :
		case 'B' :
		case sPortB :
			port_name = "Printer";
			sd = (sio_fd_t)&printerPort;
			error = ROMSDOpen(sPortB);
			break;
		
		case 'M' :
		case 'm' :
		case 'a' :
		case 'A' :				
		case sPortA :
		default:
			port_name = "Modem";
			sd = (sio_fd_t)&modemPort;
			error = ROMSDOpen(sPortA);
			
			/* PPP was overflowing the input buffer, so make it bigger.*/
			if (MACOS_SIO_BUFF_SIZE > 64 && sio_input_buffer == NULL)
			{
				sio_input_buffer = (char *)malloc(MACOS_SIO_BUFF_SIZE);
				LWIP_ASSERT("sio_open() can't allocate memory", sio_input_buffer != NULL);
			}
			if ( error == noErr && sio_input_buffer)
				SerSetBuf(IN_REF(sd), sio_input_buffer, MACOS_SIO_BUFF_SIZE);
			break;
	}
	
	switch (error)
	{	
		case (int)noErr: //0
			LWIP_DEBUGF(SIO_TRACE, ("Serial Port successfully opened.\n", devnum));
		break;
		case (int)badUnitErr: //-21
			LWIP_ERROR("Bad Reference Number for Serial Port.\n", (error == noErr ), return NULL;);
		break;		
		case (int)dInstErr: //-26
			LWIP_ERROR("Couldn't Find Serial Driver in Resource File.\n", (error == noErr ), return NULL;);
		break;
		case (int)openErr: //-23
			LWIP_ERROR("Driver can't perform the requested reading or writing.\n", (error == noErr ), return NULL;);
		break;
		case (int)unitEmptyErr: //-22
			LWIP_ERROR("Bad Reference Number for Serial Port.\n", (error == noErr ), return NULL;);
		break;
		case (int)fnfErr: //-43
			LWIP_ERROR(("Serial Port Not Found.\n"), (error == noErr ), return NULL;);
		break;
		default:
			//printf("%d", (int)error);
			LWIP_ERROR("Unknown Error opening Serial Port.\n", (error == noErr ), return NULL;);
		break;
	}

	sio_setup(sd);
		
	LWIP_DEBUGF(SIO_DEBUG, ("%s Port Successfully Opened.\n", (PORT_NUM(sd) == (int)sPortA)? "Modem":"Printer"));

	return sd;
}


/**
 * Closes a serial device.
 * 
 * @param fd device
 */
void
sio_close(sio_fd_t fd)
{
	if (PORT_NUM(fd) == (int)sPortA && sio_input_buffer)
		SerSetBuf(IN_REF(fd), sio_input_buffer, 0);

	/* Don't Close the ROM Serial Driver */
}

/**
 * Sends a single character to the serial device.
 * 
 * @param c character to send
 * @param fd serial device handle
 * 
 * @note This function will block until the character can be sent.
 */
#pragma segment LWPPP
void sio_send(u8_t c, sio_fd_t fd)
{
	OSErr   error;
	long   count = 1;

	error = FSWrite(OUT_REF(fd), &count, &(char)c);

	if (error)
	    LWIP_DEBUGF(SIO_DEBUG | LWIP_DBG_LEVEL_WARNING, ("sio_send(%c, %d) failed due to \'FSWrite\' error: %d.\n", c, PORT_NUM(fd), error));

	return;
}

/**
 * Receives a single character from the serial device.
 * 
 * @param fd serial device handle
 * 
 * @note This function will block until a character is received. The blocking
 * can be cancelled by calling sio_read_abort().
 */

#if NO_SYS==0	
u8_t sio_recv(sio_fd_t fd)
{
	static unsigned char cbuff;
	
	sio_read(fd, &(char)cbuff, 1);
	
	return cbuff;
}
#endif


/**
 * Reads from the serial device.
 * 
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0 if aborted by sio_read_abort
 * 
 * @note If NO_SYS is > 0, this function will block until data can be received.
 * The blocking can be cancelled by calling sio_read_abort().
 */
 
u32_t sio_read(sio_fd_t fd, u8_t* data, u32_t len)
{
	long count = 0;

#if NO_SYS==0
	while (sio_abort != IN_REF(fd) && count < len)
#endif
	{
		count = sio_tryread(fd, data, len);
	}
#if NO_SYS==0
	if (sio_abort == IN_REF(fd))
		sio_abort = 0xFF;
#endif

	return count;
}


/**
 * Non-Blocking Read from the serial device.
 * 
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0
 * 
 */
u32_t sio_tryread(sio_fd_t fd, u8_t* data, u32_t len)
{
	OSErr error;
	long count = 0;
	
	(void)SerGetBuf(IN_REF(fd), &count);
	
	if (count)
	{
		/* only get up to the length asked for. */
		if (count > len) count = (long)len;
		
		if ((error = FSRead(IN_REF(fd), &count, (char *)data)))
			LWIP_DEBUGF(SIO_DEBUG | LWIP_DBG_LEVEL_WARNING, ("sio_read(%d, %d) failed due to \'FSRead\' error: %d.\n", IN_REF(fd), len,  error));
	}
	return count;
}


/**
 * Writes to the serial device.
 * 
 * @param fd serial device handle
 * @param data pointer to data to send
 * @param len length (in bytes) of data to send
 * @return number of bytes actually sent
 * 
 * @note This function will block until all data can be sent.
 */
u32_t sio_write(sio_fd_t fd, u8_t* data, u32_t len)
{
	OSErr   error;
	long	count = (long)len;
		
	error = FSWrite(OUT_REF(fd), &count, (char *)data);
		
	// The ppp module will log the error for us.
	if (error)
		return error;

	return len;
}

#if NO_SYS==0
/**
 * Aborts a blocking sio_read() call.
 * 
 * @param fd serial device handle
 */
void sio_read_abort(sio_fd_t fd)
{
  LWIP_DEBUGF(SIO_DEBUG, ("sio_read_abort(%d)\n", IN_REF(fd) ));
  sio_abort = IN_REF(fd);
  return;
}

#endif

