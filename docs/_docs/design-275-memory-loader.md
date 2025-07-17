---
title: "Memory Loader"
permalink: /docs/memory-loader/
excerpt: "Manual Memory Loader for the SAP-Plus computer"
---

The Memory Loader is an alternate design that can be used in place of the [Arduino Loader/Debugger](../loader/).  Using an Arduino or other microcontroller feels a bit like cheating when designing a TTL computer, so this Memory Loader design uses rocker switches and simple logic parts for an old-school feel.

[![Memory Loader](../../assets/images/memory-loader-module.jpg "memory loader")](../../assets/images/memory-loader-module.jpg)

In addition to the larger and easier to operate switches, the SAP-Plus Memory Loader offers several improvements over the manual-entry DIP switches on the classic [Ben Eater SAP-1](https://eater.net/8bit/ram).  Although it was designed to be used with the SAP-Plus, the Memory Loader can be easily interfaced to a SAP-1 or similar build.  See the [SAP-1 interfacing](#sap-1-interfacing) section for details.

The most noticable feature of the loader is the set of eight rocker switches labeled D0..D7.  These switches are connected to the data bus through a transceiver and are used to set data values for the RAM.  These switches are also indirectly used to set the MAR address value.  

The loader contains a Loader Address Register (LAR) onboard.  Similar to the data switches, the outputs of the LAR are connected to the data bus using a 74LS245 bus transceiver.  The inputs of the LAR are connected to the eight data switches and are loaded when the SET button is pressed.

The LAR is implemented using two 4-bit counters.  If the Address Increment switch is set to ON, the LAR will increment at the end of each operation.  This feature greatly speeds up manual memory loading, because a new address does not need to be keyed in for each memory byte.

## Memory Loader Design

[![Memory Loader schematic](../../assets/images/memory-loader-schematic-v30.png "memory loader schematic")](../../assets/images/memory-loader-schematic-v30.png)

The [Ben Eater SAP-1 RAM](https://eater.net/8bit/ram) design uses multiplexers to select either the bus or a set of dip switches to load the RAM and Memory Address Register (MAR).

The SAP-Plus Memory Loader takes a different approach.  When active, the Loader disables the Microcode ROMs and generates its own control signals.  By asserting a Write MAR (WMAR) or Write RAM (WRAM) signal, the Memory Loader can put a value on the data bus and the next CLK pulse will load the bus value into the MAR or RAM.

A flip-flop toggles between the Write MAR (WMAR) and Write RAM (WRAM) state.  In the Write MAR state, the WMAR signal is asserted and the outputs of the LAR are placed in the system data bus.  When the Clock button is pressed, the WMAR signal causes the data from the LAR to be clocked into the MAR.

In the Write RAM state, the WRAM signal is asserted and the outputs of the eight Data switches are placed on the system data bus.  When the Clock button is pressed, the WRAM signal causes the data from the switches to be clocked into the RAM at the current MAR address.

The Memory Loader has two modes of operation that determine the state transitions between WMAR and WRAM.  A switch on the Memory Loader board selects between RAM Write and RAM Read modes.

### RAM Write Mode

In RAM Write mode, the flip-flop toggles with each press of the Clock button, so the Memory Loader alternates between writing the contents of the LAR to the MAR or writing the Data Switches to the RAM.  A new byte is loaded into RAM with every two clock pulses.

In RAM Write mode, the LAR increments at the conclusion of the WRAM state when Address Increment is ON.

### RAM Read Mode

In RAM Read mode, the flip-flop is held in reset, which keeps it in the WMAR state.  Each clock press writes the current LAR value to the MAR. The RAM value at the MAR address can be observed on the RAM module's LEDs.

In RAM Read mode, the LAR increments on every clock cycle when Address Increment is ON.

## Controls

### Loader Active Switch

This switch enables the Memory Loader.  In the OFF position, the Loader does not assert any control signals and does not place any values on the system Data Bus.  In the ON position, the Loader Active (LD_A) signal disables the control ROM(s) and the Loader places values for the MAR or RAM on the system Data Bus.

### Data Switches

These switches place an 8-bit value on the bus when the Loader is in the WRAM state.  The switches can also load the LAR.

### Clock Button

The Clock Button generates a single system CLK pulse on the LD_C pin.  This drives the system CLK line when the loader is active.  It also toggles the Loader state between WMAR and WRAM when in Write Mode.

### Set Address Button

A press of the Set Address Button immediately loads the value of the Data Switches into the LAR.  This can be done in any state or mode and does not require a CLK signal.

### Address Increment Switch

When the Address Increment switch is ON, the LAR counts up by one automatically after each operation.  In RAM Read Mode, the LAR is incremented on every CLK pulse.  In RAM Write Mode, the LAR is incremented on each clock pulse when in the WRAM state.

### Read/Write RAM Mode Switch

The RAM Mode switch sets the operating mode of the Loader.  In Read Mode, every press of the Clock Button writes the LAR value to the MAR and optionally increments the MAR.

In Write Mode, each press of the Clock Button alternates between writing the LAR to the MAR or writing the value of the Data Switches to the RAM.  If enabled, the MAR will increment after each RAM write.

### Reset Button

The Reset button resets the on-board LAR back to zero.  It also generates a pulse on the RST pin that acts as the master reset for the SAP-Plus.  For SAP-1 and other builds, this RST pin can be combined with a RST signal being generated elsewhere to allow this button to act as a master reset.

## Memory Loader Timing

The CLK signal is generated by the Clock button using a 555 one-shot. It produces a fixed 100ms pulse, but does not occur at regular intervals.

[![Memory Loader write mode timing](../../assets/images/timing-memory-loader-write.png "memory loader write mode timing")](../../assets/images/timing-memory-loader=write.png)

The RAM and MAR values change on the rising edge of the CLK signal.The flip-flop that controls the state is clocked on the falling edge of the CLK, so the Loader state does not change while the data is being clocked into the RAM and MAR.  The LAR uses an inverted clock, so it also changes on the falling CLK edge.

In Write Mode, each CLK pulse toggles the Loader state between WMAR and WRAM.  The LAR only increments in the WMAR state, so it changes values with every other clock press.

[![Memory Loader read mode timing](../../assets/images/timing-memory-loader-read.png "memory loader read mode timing")](../../assets/images/timing-memory-loader-read.png)

In Read Mode, the flip-flop is held in reset, so the Loader is always in the WMAR state.  The LAR increments with every clock press in Read Mode.

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

Note that the WRAM and WMAR pins connect to the ROM pins that drive the signal inverters, they should not connect to the inverter outputs.
{: .notice--warning}

The six remaining ROM output pins (other than RI and MI) need resistors to pull their signals to ground.  When the LD-ACT signal is asserted, the ROM's OE will be disabled and the ROM will not drive any of its output pins.  The six pins not connected to the loader each need a 1K resistor connected between the ROM pin and GND.  These pull-down resistors will cause the control signals that the loader is not driving to all be inactive when the loader is enabled.

The loader has its own Clock button that creates a CLK pulse, just like the button on the SAP-1.  This CLK pulse is used by the loader itself, but it is also needed by the RAM and MAR on the SAP-1.  The easiest way to interface this is with a 74LS32 OR gate.  There may already be a spare one on the clock breadboard.  The existing CLK signal from the SAP-1 Clock Module connects to one input of the OR gate.  The loader LD-CLK signal connects to the other OR input.  The output of the OR gate is the new CLK signal that connects to the rest of the SAP-1.  This allows the Loader to generate the master clock signal when it is active and allows the SAP-1 clock to generate the signal when the loader is inactive.
