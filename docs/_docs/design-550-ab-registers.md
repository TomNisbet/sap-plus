---
title: "A/B Registers"
permalink: /docs/a-b-registers/
excerpt: "A and B registers for the SAP-Plus computer"
---

[![A and B Registers](../../assets/images/ab-register-module-small.png "A and B Registers")](../../assets/images/ab-register-module.png)

The A and B registers are 8-bit registers that provide the inputs to the ALU.  The A register acts as an accumulator and general-purpose register that is accessible from instructions available to the user.

The B register is only used as an operand register for the ALU operations.  It is not accessible as a general-purpose register and there are no instructions to directly read or write its contents.  The microcode may also use this register as temporarly storage for non-ALU instructions.

[![A-B registers schematic](../../assets/images/ab-register-schematic.png "A-B registers schematic"){:width="720px"}](../../assets/images/ab-register-schematic.png)

The outputs of each register are sent to the ALU module using the Module Interconnects on the right-hand side of the board.  The A/B Module and ALU Module must be placed in adjacent slots of a backplane board.

The B register contains additional circuitry to invert its outputs to the data bus and to the ALU.  When the B Invert (_BI_) signal is asserted, a set of XOR gates invert the output pins of B.  For example if B contains the value 0111 1000, the value sent to the bus and ALU will be 1000 0111.  The B0..B7 LEDs will show the value of B after the inversion, not the value stored in the register.  The use of the _BI_ signal is detailed in the ALU documentation.

The A and B registers are each implemented using a 74LS377 register to store the register value and a 74LS245 8-bit bus transceiver to output the register value to the data bus.
