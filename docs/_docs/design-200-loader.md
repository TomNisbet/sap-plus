---
title: "Loader / Debugger"
permalink: /docs/loader/
excerpt: "Arduino-based Loader/Debugger for the SAP-Plus computer"
---

The program memory loader uses an Arduino Nano in place of manual-entry DIP switches on the SAP-1.  The Loader/Debugger has eight of the Arduino I/O lines connected directly to the SAP-Plus host bus for read and write access.  It can drive all of the microcode control lines plus the CLK and RST lines.

Beyond the ability to simply load programs into memory, the Loader can read and write all of the SAP-Plus registers and drive any other control signals. This gives the Loader the capability to perform a comprehensive system verification.  This system test is helpful for verifying the operation of new modules.

[![Arduino Loader](../../assets/images/loader.png "loader")](../../assets/images/loader.png)

An 128x64 OLED display is included to show the status of the self test and other operations.  The display is driven by [SSD1306lite](https://github.com/TomNisbet/ssd1306lite), a simple OLED display driver with a very small RAM footprint. The OLED code was developed for this project because existing drivers either had too many configuration options or used too much memory, not leaving enough for the Loader itself.  The SSD1306lite code was written specifically for a single display type driven by an Arduino.  It is easily adapted to other projects needing a basic display. 

The Loader writes a default program to the SAP-Plus memory at power up, allowing unattended operation.  If a USB connection is established, the Loader provides a ROM monitor-like command line interface to interact with the SAP-Plus hardware.  The following features are available via the USB interface:

* Memory
  * Load memory from a set of programs or patterns that are hard-coded into the Loader
  * Dump the contents of memory in hex and ASCII
  * Edit memory values
  * Fill a section of memory with a value
* Registers
  * Read and print the value from a register
  * Write a value to a register
  * Transfer the value from one register to another
* Diagnostics
  * Test all microcode control lines with a walking LED pattern
  * Test all registers with a pattern that can be visually observed
  * Test all read/write registers by reading back the patterns written
  * Test memory with a set of patterns including walking bits
  * Test the ALU by writing operands and operations and verifying the result
  * Test the counting operations of the Stack Pointer and Program Counter

## Loader Design

[![Arduino Loader](../../assets/images/loader-schematic.png "loader/debugger"){:width="720px"}](../../assets/images/loader-schematic.png)

The Ben Eater RAM design uses multiplexers to select either the bus or a set of dip switches to load the RAM and MAR.  The SAP-PLus loader takes a different approach.  When active, the Loader disables the Microcode ROMs and generates its own control signals.  With this approach, it can read or write to any register, not just the RAM and MAR. It can also generate CLK and RST signals.  It can select a register, put a value on the data bus, and then pulse the CLK line to load the bus value into the register.

The Loader uses two 74HCT595 8-bit shift registers to drive the sixteen microcode ROM control outputs. The output enable of these registers is tied to the Loader's ACTIVE signal and the output enable of the microcode ROMs is the inverted ACTIVE signal, ensuring that only one source will be outputting a signal at any time. 

## Loader implementation

The loader uses an Arduino Nano clone.  Note that the Loader Arduino is designed to be left in the circuit even when not connected to a controlling computer through USB.  Upon power up, the loader will activate itself and then load a default program into the SAP-Plus. So even in standalone mode, the SAP-Plus will power up into a state where it has code to execute.  There is no manual dip switch loader, so there is no way to load memory without the Arduino.

The Arduino is powered from the SAP-Plus Vcc through a diode so that if the NQSAP is powered off it does not try to draw power from the Arduino's USB port.  The USB port is connected to the host computer with a hub that has individual power switched, allowing the host computer's power to be disconnected from the SAP-Plus hardware.

## Bill of Materials

This BOM includes the reset circuitry as well as the Loader.

* 74HCT00 quad 2-input NAND gate (1)
* 74HCT595 8-bit shift register (2)
* Arduino Nano
