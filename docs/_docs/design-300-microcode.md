---
title: "Microcode ROMs"
permalink: /docs/microcode/
excerpt: "Microcode ROMs for the SAP-Plus computer"
---

[![Microcode ROMs](../../assets/images/microcode-board-500.jpg "Microcode ROMs")](../../assets/images/microcode-board.jpg)

The microcode ROMs drive the 16 control signals for the SAP-Plus computer.  This board contains the ROMs and LEDs for all control signals.

With an 6-bit Instruction Register and a 4-bit Ring Counter, the SAP-Plus can support 64 unique instructions with 16 microcode steps each. The remaining two bits of the IR contain the Carry and Zero flags. This consumes 12 of the 15 available address pins on the ROMs. The chip select uses 1 more pin, and 2 address pins are unused and tied to ground.

On the SAP-Plus Microcode Module, the ROM0 chip has A14, A13, and A12 tied to GND, so ROM0 is only accessing addresses 0000 to 0fff.  The ROM1 chip has A14 and A13 tied to GND and A12 tied to +5V, so ROM1 is always accessing addresses 1000 to 1fff.  ROM0 provides the miscellaneous control signals and ROM1 provides the seven register select signals that drive the 74LS138 selectors.

[![Microcode ROMs schematic](../../assets/images/microcode-schematic-small.png "Microcode ROMs schematic")](../../assets/images/microcode-schematic.png)

The microcode is stored in two 28C256 32Kx8 EEPROMs, giving 16 control signals.  The signals can be driven by the ROMs or by the Arduino Loader/Debugger.  When the Arduino is active, the OE signal of the ROMs is disabled.  The LEDs always show the state of the control lines, whether they are driven by the ROMs or by the Loader.

One of the ROMs provides the register read and write signals and the other provides miscellaneous control lines. The current design uses 10 write selects, 7 read selects and 6 additional control signals.

The read and write signals from one ROM are fed into 74LS138 3-to-8 multiplexers, so 4 write signals from the ROM can select up to 15 write registers and 3 read signals from the ROM can select up to 7 read registers. 

The multiplexer inputs are also tied to the module interconnect pins so they can be driven by an adjacent Loader module when the ROMs are disabled. The remaining control signals do not need to pass over the interconnects - when the ROMs are disabled, the Loader can just drive these signals directly to the bus connectors.

## Microcode creation and burning

The microcode itself is created by an [Arduino sketch](https://github.com/TomNisbet/sap-plus/tree/main/arduino-microcode) that builds the code and burns it onto 28C256 EEPROM chips.  Each chip contains the code for both ROMs, so the sketch does not differentiate between the chips when burning the code.  The complete microcode is 4K 16-bit words.  The lower 8 bits, containing the miscellaneous control bits, are burned into 4K bytes at EEPROM address 0000 to 0fff.  The upper 8 bits, containing the register select bits, are burned into 4K bytes at 1000 to 1fff.

The microcode sketch runs on [TommyPROM hardware](https://tomnisbet.github.io/TommyPROM/) and uses the low-level 28C256 code from that project to burn the ROMs.  With fast page-write support, it only takes a few seconds to burn a ROM.


