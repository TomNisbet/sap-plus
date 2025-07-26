---
title: "RAM and MAR"
permalink: /docs/ram-mar/
excerpt: "RAM and Memory Address Register (MAR) for the SAP-Plus computer"
---

The RAM provides storage for programs and data. The 8-bit Memory Address Register (MAR) addresses 256 bytes of RAM memory.  

[![RAM and MAR](../../assets/images/ram-mar-module-small.png "RAM and MAR")](../../assets/images/ram-mar-module.png)

The shared _CX_ signal from the Microcode Module is used as the _MX_ signal by the MAR to provide one additional address bit.  This segments the RAM into two separate areas for program, data, and stack, giving a total of 512 bytes of storage.  The current Microcode uses the MX=0 area for code storage and MX=1 for both data and stack operations.  This can be changed in the Microcode, but corresponding changes may be needed in the Loader code as well to access the code storage space.

Some SAP designs have a dedicated loader circuit that is part of the RAM and MAR design.  SAP-Plus does not include any switches or circuitry to modify the RAM and MAR.  The SAP-Plus Loader accesses the RAM and MAR using the data bus and the _WM_, _WR_, and _RR_ signals in the same way they are accessed by the microcode.

[![RAM and MAR schematic](../../assets/images/ram-mar-schematic-small.png "RAM and MAR schematic")](../../assets/images/ram-mar-schematic.png)

## Memory Addressing

The MAR is read-only, using a single 74LS377 register that is loaded from the data bus using the Write MAR (_WM_) signal.  

The use of separate data and code areas via the _MX_ signal means that the SAP-Plus does not support self-modifying code because the register load and store operations work in a RAM block that is separate from the program storage block.

## RAM Implementation

The RAM is implemented using a 32Kx8 bit static RAM.  Only a small portion of this is used and the additional address lines are tied to ground.  The 32Kx8 chip is overkill, since only 512 of the 32768 bytes are used.  The RAM chip was chosen because it is readily available and easy to wire.  In fact, it shares the same pinouts as the 32Kx8 EEPROM used in the Microcode Module.

The RAM is read and written to the data bus using the Read RAM (_RR_) and Write RAM (_WR_) signals from the Microcode.

The RAM has LEDs connected that display the data byte at the current MAR address.  The LEDs were added to allow the use of the [switch-based program memory loader](../memory-loader/).

The RAM LEDs make the design a bit more complex.  Without them, the RAM chip could connect directly to the data bus and the RAM outputs would tri-state when the RAM was not being accessed.  The [NQSAP-PCB RAM design](https://tomnisbet.github.io/nqsap-pcb/docs/ram-mar/) is an example of this simpler approach.

In SAP-Plus, a 74LS245 bus tranceiver is used to isolate the RAM outputs from the data bus so that the RAM can keep its outputs active to drive the LEDs.  The RAM and bus driver have three states:

* When the RAM is not being accessed, the RAM _OE_ is active and the bus transceiver _CE_ is not.  This lights the RAM LEDs but isolates the RAM from the data bus.
* When the RAM is being read, the RAM _OE_ and the bus tranceiver _CE_ are both active.  The transceiver _A->B_ direction is set move data from RAM to the data bus.
* When the RAM is being written, the transceiver _CE_ is active and the RAM _OE_ is not.  The _A-B_ is set to move data from the data bus to the RAM.  In addition, a signal is generated to drive the RAM _WE_ when the data is ready.

All of these functions are accomplished using one quad-NAND gate as shown below.

[![RAM decode](../../assets/images/ram-decode.png "RAM and MAR schematic")](../../assets/images/ram-mar-schematic.png)

The U3A NAND gate inverts the active-LOW _WR_ signal to produce a HIGH when a RAM write is requested.  U3B combines this signal with the CLK to produce a LOW pulse that drives the RAM _WE_ signal.  The inverted _WR_ signal from U3A is also used to drive the active-LOW RAM _OE_ signal so that the RAM is outputting in all cases except for a write operation.

The U3D NAND gate combines the _RR_ and _WR_ signals and that output is inverted by U3C so that the bus transceiver is active whenever either the _RR_ or _RW_ signal is asserted.

The bus transceiver _A->B_ direction is controlled directly by the _WR_ signal, so that it is feeding the data bus to RAM on a write and RAM to data bus in all other cases.  Note that the direction doesn't matter if neither _RR_ or _WR_ is asserted because the transceiver output will be disabled by _CE_.
