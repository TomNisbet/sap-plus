---
title: "Instruction Cycle"
permalink: /docs/instruction-cycle/
excerpt: "Instruction Cycle and Timing for the SAP-Plus computer"
---

The SAP-Plus instruction cycle is driven by the Instruction Register (IR) and Step Counter (SC).  These two registers provide the address bits to the microcode ROMs. By selecting an instruction and step, these registers determine which signals are asserted by the ROMs at each microcode step.

Each instruction step causes actions on both the falling and rising edge of each CLK pulse. The Step Counter changes on every falling CLK edge, so this is when each microinstruction step asserts new control signals.  Register read selects cause a register value to be read onto the bus at this time.  The registers load on a rising CLK edge, so the value from the bus is clocked into the selected write register on the rising edge.

For a concrete example, the [WaveDrom timing diagram](https://wavedrom.com/) below shows the execution of a Load A Immediate (LAI) instruction at RAM address 0, followed by an OUT A instruction at RAM address 2.

[![Instruction timing](../../assets/images/timing-lai-out.png "Timing of LAI and OUT instructions")](../../assets/images/timing-lai-out.png)

### First Instruction - Load A Immediate (LAI)

Load A Immediate is a two-byte instruction.  The A register is loaded with the value of the second instruction byte.

**Marker 1**: the falling edge of CLK sets the Step Counter (SC) to _T0_.  This step asserts the Read PC and Write MAR signals to get the address of the next instruction.  The PC Increment (PI) signal is also asserted.  At this point, the Program Counter (PC) contents are read onto the bus.

**Marker 2**: the rising edge of the clock causes the PC value on the bus to be written into the Memory Address Register (MAR).  The PC is also incremented.

**Marker 3**: the falling CLK edge increments the Step Counter to enter step _T1_.  This asserts the Read RAM and Write IR signals.  At this point, the RAM contents at the MAR address are read onto the bus.

**Marker 4**: the rising CLK edge loads the contents of RAM into the Instruction Register (IR).  This is the end of the common _T0_ and _T1_ instruction fetch steps.  The Microcode ROM is now executing the LAI instruction's microcode steps.

**Markers 5..6**: the MAR is loaded from the PC and the PC is incremented in step _T2_, exactly as was done in step T0.  This fetches the memory address of the instruction operand.

**Markers 7..8**: in step _T3_, the RAM contents are loaded into the A register.  The Next (N) signal is also asserted in this step, which will reset the SC back to step 0 (_T0_) to end the instruction.  The N reset is synchronous and will occur at the falling CLK edge at **marker 9**.

### Second Instruction - Output A (OUT)

This is a one byte instruction that loads the contents of the A register into the Output Display Register.

**Marker 9..12**: the _T0_ and _T1_ steps are the usual instruction fetch sequence.  Step _T0_ loads the PC into the MAR and increments the PC.  Step _T1_ loads the RAM value into the IR.

**Marker 13**: the A register contents are placed on the bus during the _T2_ step.

**Marker 14**: the A register contents on the bus are clocked into the Output Display Register on the rising clock edge.  The N signal is asserted to end the OUT instruction.

**Marker 15:** the N signal from the OUT instruction resets the Step Counter back to _T0_ at this falling clock edge, starting the next instruction fetch sequence.

The Instruction Register does not get loaded until midway through the _T1_ microinstruction step. During _T0_ and _T1_, the IR still has the value of the previous instruction.  For example, at **Marker 9** the _T0_ step of the OUT instruction is executing, but the IR still contains the LAI instruction and is therefore executing the T0 step of the LAI microcode, not the OUT microcode.  This is why all instructions **must** have identical microcode for their _T0_ and _T1_ steps.
{: .notice--info}

## SAP-Plus 2nd Instruction Register (IR2)

SAP-Plus uses a double-buffered Instruction Register (IR) to mitigate the [EEPROM Glitch](../eeprom-glitch/#sap-plus-design-to-avoid-the-glitch) that happens when the address lines of the microcode EEPROMs are changed.  As shown in the timing diagram of the previous section, the IR2 register is loaded from the IR on every falling clock edge. The Microcode ROM address lines are driven by IR2 instead or IR. This means that the address lines only change when the Step Counter or IR2 values change, and both of these conditions only occur on a falling clock edge.

## Reset Timing

The diagram below shows the system reset sequence.  In this example, the program at RAM address 0 starts with an OUT instruction.

[![reset timing](../../assets/images/timing-reset.png "Timing of the reset condition")](../../assets/images/timing-reset.png)

**Marker 2**: the active-low RST signal is asserted.  This resets the Step Counter (SC) back to step _T0_ and resets the Program Counter (PC) to address zero.  The _T0_ microcode asserts the Read PC (~RP), Write MAR (~WM), and PC increment (PI) signals.  Note that the the read and write select signals are active-low, but the PI signal is active-high.

**Marker 4**: the rising edge of CLK clocks the PC contents into the MAR and increments the PC.  Because the PC was cleared by the RST signal, the first _T0_ cycle after a reset event always loads zero into the MAR.

**Marker 6**: the _T1_ cycle loads the instruction opcode at address zero into the IR.  Program-specific code begins executing after this microinstruction cycle.  

In the case of the OUT instruction, the _T2_ step transers the _A_ register to the _OUT_ register and sets the _N_ signal to indicate that the instruction is complete.
