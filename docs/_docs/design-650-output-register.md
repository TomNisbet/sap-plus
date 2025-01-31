---
title: "Output Register"
permalink: /docs/output-register/
excerpt: "Output register and display for the SAP-Plus computer"
---

[![Output register and display](../../assets/images/output-module.png "output register"){:width="720px"}](../../assets/images/output-module.png)

The SAP-Plus Output Register is similar to the design of the Ben Eater SAP-1.  The primary difference is the use of a larger ROM with more display bits.  This allows four display modes instead of just two.

The SW2 and SW1 switches select one of the display modes:

* signed decimal (-128 to 127)
* unsigned decimal (0 to 255)
* hex (00 to ff)
* binary (0000 0000 to 1111 1111)

The binary mode uses the eight vertical bars present in the four displays to indicate eight binary conditions.  A binary one is indicated by lighting the top segment of the bar and a zero is indicated by lighting the bottom segment.

[![Output Register Schematic](../../assets/images/output-schematic.png "output register"){:width="720px"}](../../assets/images/output-schematic.png)

## Hardware design

The 8-bit value to be displayed is read from the bus into a 74LS377 8-bit register when the _WO_ signal is active.  The outputs of the register drive the lowest eight _A0..A7_ address lines of the display ROM.

The 555 timer generates a free-running clock pulse that drives a 74LS161 4-bit counter.  The clock has a potentiometer to make minor adjustments to the clock speed.  This adjustment is helpful to prevent the clock speed from matching up with the framing speed when taking videos of the circuit.

Only the lower two bits of the 4-bit clock are used, so it effectively is just counting from zero to three.  The two binary counter outputs are used as the _A8..A9_ address bits for the ROM.  The clock bits are also fed into a 74LS138 3-to-8 decoder to provide the common cathode digit select signals.  Note that a 2-to-4 decoder could have been used for this, but the 74LS138 is already used elsewhere in the SAP-Plus design, so it was chosen to not add another unique chip type to the overall system.

The two display select switches drive the _A10..A11_ address lines of the ROM, so the complete inputs to the ROM are 8 bits to indicate the value to be displayed, 2 bits to indicate the display digit to be shown, and 2 bits to indicate the display mode.  The outputs of the ROM are connected to the display segment inputs.

### Support for multiple ROM types

Many of the 28-pin 8K and 32K PROMs have similar pinouts, so jumpers are provided to allow different PROM types to be used as the display ROM.  The jumpers allow pins 1 and 27 to be connected to either +5V or GND or left floating.

For a 28C256 EEPROM, pin 27 is _WE_ and pin 1 is _A14_.  To use this chip, tie _WE_ to +5V and _A14_ to GND.  The 28C64 also has _WE_ on pin 27, but pin 1 is not connected, since the chip has no _A14_ signal.  

27C257 EEPROMs were widely used for PC BIOS applications, and can often be found surplus for cheap.  These chips have _Vpp_ on pin 1 and _A14_ on pin 27, so _Vpp_ must be tied to +5V and _A14_ to GND.

The [Arduino sketch to burn the display ROM](https://github.com/TomNisbet/sap-plus/tree/main/arduino-output-rom) is based on the same TommyPROM hardware design that is used for the Microcode.  This will work with the 28C256 or 28C64.  

The 27C257 is not supported by the sketch directly.  The best way to burn that would be to modify the sketch to output a file instead and then use the same hardware with the 27C version of the TommyPROM code to burn the data.