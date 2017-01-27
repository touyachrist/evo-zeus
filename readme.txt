Zeus Evolution
==============
Forgotten Zeus Evolution botnet source code from my zeus collection containing sources of various versions.

Video of exploit by friend Xylibox : https://www.youtube.com/watch?v=KeGaXXfISY0 

Different header files:
+ source\common\tor.h <- binary of torrc. Inits inside SocketHook::run_tor.
+ source\client\bank_catch.h <- matches banks to a url, adds and removes any accounts.
+ source\client\globals.h <- part of a removed windows hooker dll used in Carberp.
+ source\client\modulesystem.h <- hide dll and processes also used to manage, load and download.
+ source\client\process_keylogger.h <- keylog certain processes.
+ source\client\usbshadowcpy.h <- copies to usb.
+ todo..

Additional source files seen in unreleased sources:
+ bin2h.cpp <- turns a binary into a byte array steam.
+ makeprimelist.cpp <- list of prime number generator for dga.
+ findbugs_BeaEngine.cpp <- find bugs in disassemblers. Counters for reversing.
+ crc64.cpp <- calculates crc64 of bytes and compares.
+ todo..
