Zeus Evolution
==============
A tor based botnet from my private zeus collection.

Vulnerabilities
==============
File Upload Vulnerability example [Xyl2k](https://www.youtube.com/watch?v=KeGaXXfISY0)

Different header files:
==============
+ source\common\tor.h <- binary of torrc. Inits inside SocketHook::run_tor.
+ source\client\bank_catch.h <- matches banks to a url, adds and removes any accounts.
+ source\client\globals.h <- part of a removed windows hooker dll used in Carberp.
+ source\client\modulesystem.h <- hide dll and processes also used to manage, load and download.
+ source\client\process_keylogger.h <- keylog certain processes.
+ source\client\usbshadowcpy.h <- copies to usb.
+ todo..

Additional sources seen in unreleased:
==============
+ bin2h <- turns a binary into a byte array steam for internal execution.
+ makeprimelist <- list of prime number generator for dga.
+ findbugs_BeaEngine <- find bugs in disassemblers. Counters for reversing.
+ crc64 <- calculates crc64 of bytes and compares.
+ todo..
