---
title: "Memory Loader"
permalink: /docs/memory-loader/
excerpt: "Manual Memory Loader for the SAP-Plus computer"
---

The Memory Loader is an alternate design that can be used in place of the [Arduino Loader/Debugger](../loader/).  Using an Arduino or other microcontroller feels a bit like cheating when designing a TTL computer, so this Memory Loader design uses rocker switches and simple logic parts for an old-school feel.

[![Memory Loader](../../assets/images/memory-loader-module.jpg "memory loader")](../../assets/images/memory-loader-module.jpg)

In addition to the larger and easier to operate switches, the SAP-Plus Memory Loader offers several improvements over the manual-entry DIP switches on the classic [Ben Eater SAP-1](https://eater.net/8bit/ram).  Although it was designed to be used with the SAP-Plus, the Memory Loader can be easily adapted to work with a SAP-1 or similar build.  See the [SAP-1 interfacing](#sap-1-interfacing) section for details.

## Memory Loader Design

The most noticable feature of the loader is the set of eight rocker switches labeled D0..D7.  These switches are connected to the data bus through a transceiver, so they can be used to set data values for the RAM.  These switches are also indirectly used to set the MAR address value.  The loader contains a Loader Address Register (LAR) onboard.  Similar to the data switches, the outputs of the LAR are connected to the data bus using a 74LS245 bus transceiver.  The inputs of the LAR are connected to the eight data switches and are loaded when the SET button is pressed.



[![Memory Loader schematic](../../assets/images/memory-loader-schematic.png "memory loader schematic")](../../assets/images/memory-loader-schematic.png)

The Ben Eater RAM design uses multiplexers to select either the bus or a set of dip switches to load the RAM and MAR.  The SAP-Plus Memory Loader takes a different approach.  When active, the Loader disables the Microcode ROMs and generates its own control signals.  With this approach, it can read or write to any register, not just the RAM and MAR. It can also generate CLK and RST signals.  It can select a register, put a value on the data bus, and then pulse the CLK line to load the bus value into the register.

## Memory Loader implementation

[![Memory Loader timing](../../assets/images/timing-memory-loader.png "memory loader timing")](../../assets/images/timing-memory-loader.png)

## SAP-Plus Interfacing

[![Memory Loader Components](../../assets/images/memory-loader-components.jpg "memory loader")](../../assets/images/memory-loader-components.jpg)

The Memory Loader connects to the SAP-Plus using a dedicated Memory Loader Interface Module.  A 20-pin ribbon cable between the two carries the power signals from the SAP-Plus to the loader and carries the data and control signals from the loader to the SAP-Plus.  The Interface Module is little more than an interface to the 40-pin bus, plus two 74LS245 bus transceivers that block the Memory Loader's control signals when the loader is not active.

[![Memory Loader Interface](../../assets/images/memory-loader-interface-module.jpg "memory loader")](../../assets/images/memory-loader-interface-module.jpg)

One of the bus transceivers drives the chip-select control signals that connect to the Microcode ROM Module.  This transceiver is hard-wired to select either the MAR or the RAM for writing, depending on the state of the WMAR and WRAM signals.  The other transceiver drives the miscellaneous control signals.  A jumper block is provided to allow each signal to be wired high or low when the loader is active.  By default, most signals should be low.  The N signal can be set high instead, so that the Step Counter is held in reset while the loader is active.  This prevents that counter's LEDs from cycling during each CLK pulse.  The CX signal can also be set high or low to select between program or data memory.

## SAP-1 Interfacing

All of the connections needed to interface the Memory Loader are present on the 20-pin J1 connector.  They can be easily connected to a SAP-1 using male-to-female Dupont breadboard jumper wires.  Make the following connections:

* Connect all Vcc signals from the loader to +5V on the SAP-1
* Connect all GND signals between the loader and SAP-1
* Connect the loader D0..D7 signals to the SAP-1 data bus

Locate the SAP-1 Microcode ROM that drives the MI and RI signals and make the following changes:

* Remove the Microcode ROM's jumper from GND to the OE signal (pin 20) and connect the OE pin to the loader's LD-ACT signal (do not use the ~LD_ACT signal, use the one next to the WMAR pin)
* Connect the loader WRAM pin to the ROM pin that drives the RI signal
* Connect the loader WMAR pin to the ROM pin that drives the MI signal

Note that the WRAM and WMAR pins connect to the ROM pins that drives the signal inverters, they should not connect to the inverter outputs.
{: .notice--warning}

The six remaining ROM output pins (other than RI and MI) need resistors to pull their signals to ground.  When the LD-ACT signal is asserted, the ROM's OE will be disabled and the ROM will not drive any of its output pins.  The six pins not connected to the loader each need a 1K resistor connected between the ROM pin and GND.  These pull-down resistors will cause the control signals that the loader is not driving to all be inactive when the loader is enabled.

The final signal to interface is the LD-CLK pin.  The loader has its own Clock button that creates a CLK pulse, just like the button on the SAP-1.  This CLK pulse is used by the loader itself, but it is also needed by the RAM and MAR on the SAP-1.  The easiest way to interface this is with a 74LS32 OR gate.  There may already be a spare one on the clock breadboard.  The existing CLK signal from the SAP-1 Clock Module connects to one input of the OR gate.  The loader LD-CLK signal connects to the other OR input.  The output of the OR gate is the new CLK signal that connects to the rest of the SAP-1.
