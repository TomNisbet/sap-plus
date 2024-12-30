---
title: "Microcode ROMs"
permalink: /docs/microcode/
excerpt: "Microcode ROMs for the SAP-Plus computer"
---

The Microcode Module contains two 8-bit wide ROMs to drive the 16 control signals for the SAP-Plus computer.  This board contains the ROMs and LEDs for all control signals.

[![Microcode ROMs](../../assets/images/microcode-module-small.png "Microcode ROMs")](../../assets/images/microcode-module.png)

The 16-bit wide microcode words are stored in two 28C256 32Kx8 EEPROMs.  ROM0 contains the lower 8 bits of the word and ROM1 contains the upper 8 bits.

With an 6-bit Instruction Register and a 4-bit Step Counter, the SAP-Plus can support 64 unique instructions with 16 microcode steps each. The remaining two bits of the IR contain the Carry and Zero flags. This consumes 12 of the 15 available address pins on the ROMs. The chip select uses one more pin, and two address pins are unused and tied to ground.

On the SAP-Plus Microcode Module, the ROM0 chip has _A14_, _A13_, and _A12_ tied to GND, so ROM0 is only accessing addresses 0000 to 0fff.  The ROM1 chip also has _A14_ and _A13_ tied to GND, but _A12_ is tied to +5V, so ROM1 is always accessing addresses 1000 to 1fff.  ROM0 provides the miscellaneous control signals and ROM1 provides the seven register select signals that drive the 74LS138 selectors.

[![Microcode ROMs schematic](../../assets/images/microcode-schematic-small.png "Microcode ROMs schematic")](../../assets/images/microcode-schematic.png)


The _LD-A_ signal from the Loader Module is tied to the _OE_ pins of the microcode ROMs.  When the Loader is active, the ROM chip outputs are disables and the Loader can then drive all of the control signals.  When the Loader is not active, its outputs are disabled to not interfere with the ROM signals.  The LEDs on the Microcode Module always show the state of the control lines, whether they are driven by the ROMs or by the Loader.

ROM0 provides miscellaneous control lines. ROM1 provides the register read and write signals. The current design uses 10 write selects, 7 read selects and 6 additional control signals.

The read and write signals from ROM1 are fed into the inputs of 74LS138 3-to-8 multiplexers, so 4 write signals from the ROM can select up to 15 write registers and 3 read signals from the ROM can select up to 7 read registers.

The multiplexer inputs are also tied to the module interconnect pins so they can be driven by an adjacent Loader Module when the microcode ROMs are disabled. The ROM0 control signals do not need to pass over the interconnects - when the ROMs are disabled, the Loader can just drive these signals directly to the bus connectors.

## ROM0 control signals

ROM0 drives the following six control signals:

|Signal|Name|Description|
|:---|:---|:---|
|PI|Program Increment|Advance the Program Counter|
|N |Next Instruction |Reset the Step Counter|
|FL|Flags Load       |Load the Flags Register|
|BI|B Invert         |Invert the B Register Output|
|CX|Carry and Misc   |see note below|
|SC|Stack Count      |Enable the Stack Pointer Counter|

The main 40-pin connector used throughout the system was chosen because it would fit on a 100x100mm board, which is a size that can be manufactured cheaply.  After the data bus, chip selects, and other required signals, only six pins remained for control signals.  ROM0 has eight outputs, but two are unused because there was no room on the connector for them.
{: .notice--info}

Due to the lack of available signals, the _CX_ signal is shared by multiple modules and has the following functions:

On the ALU Module, the _CX_ signal is used to indicate Carry-In to the adders.

On the RAM Module, the _CX_ signal is used as _MX_, a memory address extension bit allowing two separate blocks of memory.

On the PC-SP Module, the _CX_ bit indicates the up/down direction to count the Stack Pointer when the _SC_ signal is active.

## Microcode creation and burning

The microcode itself is created by an [Arduino sketch](https://github.com/TomNisbet/sap-plus/tree/main/arduino-microcode) that builds the code and burns it onto 28C256 EEPROM chips.  Each chip contains the complete code for both ROMs, so the sketch does not differentiate between the chips when burning the code.  

The complete microcode is 4K 16-bit words.  The lower 8 bits, containing the miscellaneous control bits, are burned into 4K bytes at EEPROM address 0000 to 0fff.  The upper 8 bits, containing the register select bits, are burned into 4K bytes at 1000 to 1fff.  The remaining 24K bytes at addresses 2000 to 7fff are not used.

After burning two copies of the ROMs, one ROM is placed in each socket on the Microcode Module and the _A12_ pin determines whether that ROM accesses the ROM0 or ROM1 code.

The microcode sketch runs on [TommyPROM hardware](https://tomnisbet.github.io/TommyPROM/) and uses the low-level 28C256 code from that project to burn the ROMs.  With fast page-write support, it only takes a few seconds to burn a ROM.


