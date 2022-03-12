What is this?
===
This is a parser for the Victron Energy HEX protocol. Implemented from the VE.Direct protocol rev 16 PDF, available on the victron energy website.

What hardware do I need to use this?
---
From a linux computer, you need a USB-Serial TTL cable. Connect the RX/TX/GND pins to your victron MPPT. There is information how to do this on the victron website. I use this with a ESP32 board to talk with 4 MPPTs via a Atlas Scientific serial multiplexer.

How do I build this?
---
    mkdir build
    cmake ..
    make

TODO/Issues
---

* Still have a mix of C code lying around.
* Add an API for the SET commands
* Better parsing when sentences are mixed with async messages
* High level API to determine capablilities
* Probably leaks memory
* Increase test coverage
* Transform into a library, add executables as examples
* Fix the various TODOs in the code
