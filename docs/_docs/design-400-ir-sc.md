---
title: "Instruction Register and Step Counter"
permalink: /docs/ir-sc/
excerpt: "Instruction Register and Step Counter for the SAP-Plus computer"
---

The Instruction Register (IR) and Step Counter (SC) provide a 12-bit address for the microcode ROMs.  The MA0..MA11 bits are carried on the Interconnect Bus, so the Microcode Module and IR-SC Module must be placed in adjacent backplane slots.

[![Instruction Register and Step Counter](../../assets/images/ir-sc-module-small.png "Instruction Register and Step Counter")](../../assets/images/ir-sc-module.png)

## Step Counter

The Microcode Step Counter (SC) is a 4-bit binary microinstruction step counter that provides the A0..A3 addresses for the Microcode ROMs.  It counts on the falling edge of every CLK signal.

The SC has an attached 3-to-8 decoder to drive the LED display that indicates the step as T0..T7.  This is easier to follow than a simple binary display using four LEDs.

With a 4-bit counter, an instruction can have as many as sixteen steps, but most have fewer than eight.  Because the higher steps aren't typically used, there is just a single T_EXT LED to indicate that steps T8..T15 are active.  Step T8 is indicated by T-EXT and T0, step T9 by T-EXT and T1, and so on.

### Step Counter implementation and reset

The Step Counter is implemented using a 74LS161 4-bit counter.  This counter can be reset in two ways.  The SAP-Plus RST signal is connected to the reset signal of the counter.  This provides an asychronous reset of the SC whenever RST is pulled low.  This reset does not depend on the CLK signal.  Holding RST low will not only reset the counter, it will also prevent counting until the RST is released.  The Loaded uses this to prevent counting during the system self test.

Because most instructions will have fewer than 16 steps, the N signal from the microcode can be used to end an instruction without executing all sixteen microinstruction steps.  The N signal is tied to the 74LS161's Load pin and the data inputs are all tied to GND.  When the N signal is asserted, the SC will reset on the next CLK pulse.  Instructions with fewer than 16 steps include the N bit in their last microcode step.


[![Instruction Register and Step Counter schematic](../../assets/images/ir-sc-schematic-small.png "Instruction Register and Step Counter schematic")](../../assets/images/ir-sc-schematic.png)

## Instruction Register

The Instruction Register (IR) is an 8-bit, write-only register that loads on the WI signal.  Bits IR0..IR5 are connected to D0..D5 on the data bus, allowing a 6-bit opcode to be loaded from RAM.  This provides 64 unique opcodes.  Bits IR6 and IR7 are connected to the FZ and FC signals from the Flags Register on the ALU Module.  

**NOTE:** The contents of the Flags Register are not moved to the IR until the start of a new instruction, so an instruction's microcode cannot alter its behavior based on the content of a Flags Register that is changed in an earlier instruction step.
{: .notice--info}

The Instruction Register is double buffered to reduce [glitches when the EEPROM address lines change](https://tomnisbet.github.io/nqsap-pcb/docs/eeprom-glitch/).  In the non-buffered design, the EEPROM outputs will be in an unknown state for about 100ns when the Step Counter address changes on every falling edge of the system clock.  In addition, the outputs will also glitch on the rising edge of the clock at instruction step T1 when the IR is loaded with the new opcode.  In a traditional SAP design, the outputs would also glitch on any rising clock edge that changes the Flags Register contents.

The double buffer adds a second register following the IR register.  The IR loads at every the rising CLK edge of every T1 instruction step.  The second register loads the contents of the IR and presents them to the ROM address lines on every falling edge.  As a result, the EEPROM address lines only ever change on a falling clock edge, allowing logic that combines ROM signals with a positive clock signal.  Because the two flags register bits are fed through the IR, this also restricts the flag bit changes to the falling clock edge.
