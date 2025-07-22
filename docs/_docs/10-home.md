---
title: "SAP-Plus 8-bit TTL Computer"
permalink: /
excerpt: "SAP-Plus: Simple As Possible (SAP) computer with a modular PCB design"
---

![SAP-Plus TTL computer](assets/videos/sap-plus.gif "SAP-Plus")

Simple As Possible (SAP) Computer with modular PCB design

SAP-Plus is an extremely simple computer built from TTL logic.  There is no microprocessor used.  Instead, all of the processing is performed using logic gates, counters, decoders, and other basic chips.  A popular example of this is the [Ben Eater 8-bit breadboard computer](https://eater.net/8bit).

SAP-Plus uses some of the basic design of the Ben Eater build and adds the following extensions:

* [Modular PCB backplane](docs/backplane/) and bus connectors, providing flexible arrangement of the computing boards to easily modify and extend the functionality
* All registers are expanded to 8-bits, including the memory addressing
* [Arduino-based loader/debugger](docs/loader/) with full access to all data and microcode control lines to load programs and run interactive diagnostics
* Each instruction can have a variable number of microcode steps, between 3 and 16
* RAM can address two different 256-byte blocks, allowing for separate program, data, or stack areas

[![SAP-Plus boards](assets/images/boards1-500.jpg "SAP-Plus boards")](assets/images/boards1.jpg)

This build shares a lot of similarities with the [NQSAP-PCB TTL computer](https://tomnisbet.github.io/nqsap-pcb/), but is simplified in the following ways:

* All through-hole parts, no surface mount soldering required
* Simplified [ALU](docs/alu/) with only a few simple operations
* Simplified flags implementation with Carry and Zero flags
* No dedicated index registers or shift register (SP can be used as an index register)
