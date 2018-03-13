# ElWhip 0.07

This is a port of the lwip network stack and PPP over serial protocol for early Macintosh computers. It has been tested on a Macintosh SE running System 7.1 and a virtual Macintosh 128K with 64K ROMs. It will not be able to close the PPP session on quit on a Mac 128K due to memory limitations.

## Web server configuration:
The web server does not require the HTML files to have the HTTP headers on the top. Only files with an extension will be loaded by the server (eg: ".html").

## PPP configuration:
On Mac OS X 10.4 (and probably later):
### /etc/ppp/options file:

---
```
asyncmap 0
proxyarp
lock
```
---

### /etc/ppp/options.cu.usbserial file:

Replace cu.usbserial with the name of your serial device in /dev/
use the correct IP addresses for your setup

---
```
# Baud rate of the connection. ElWhip uses 19200.
19200
# Don't use the modem control lines.
local
# Try to reopen the connection it it is terminated.
persist
# Don't require the peer to authenticate.
noauth
# Disable negotiation of Van Jacobson style IP header compression.
novj
# IP address to be given to peer.
:192.168.11.130
# IP address of the DNS server for the peer.
ms-dns 192.168.11.1
# Enables IP forwarding on linux
ktune
# Add an entry for the peer to this system's ARP table.
proxyarp
```
---

### Running PPP from the command line:
$ pppd -d cu.usbserial
(Replace cu.usbserial with the name of your serial device)

### Forwarding requests from LAN on Mac OS X:
$ sudo sysctl -w net.inet.ip.forwarding=1

## Running ElWhip:
Just double-click the application once the modem port is connected to a computer running PPP. The application will try to negotiate a connection.  Within a few seconds you should see the PPP phase increase to 7 and see the IP address information that was negotiated. A few seconds later, the web server will start.

## Connecting to ElWhip:
From an appropriately networked computer, you can open a web browser and connect to the website. 


---
Copyrights:

ElWhip Copyright (c) 2014, 2015 Eric Pooch
 
---
```
/* lwIP is licenced under the BSD licence: 
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
*****************************************************************************
* Point to Point Protocol.
 *
 * Copyright (c) 1984-2000 Carnegie Mellon University. All rights reserved.
 *
 *    "This product includes software developed by Computing Services
 *     at Carnegie Mellon University (http://www.cmu.edu/computing/)."
 *
*
* Copyright (c) 2003 by Marc Boucher, Services Informatiques (MBSI) inc.
* portions Copyright (c) 1997 Global Election Systems Inc.
*
*
*
* THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS *AS IS* AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
* IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************
```
