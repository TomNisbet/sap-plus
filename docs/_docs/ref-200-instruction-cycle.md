---
title: "Instruction Cycle"
permalink: /docs/cycle/
excerpt: "Instruction Cycle and Timing for the SAP-Plus computer"
---

The diagram below shows the execution of a Load A Immediate (LAI) instruction, followed by an OUT A instruction.

[![Instruction timing](../../assets/images/timing-lai-out.png "Timing of LAI and OUT instructions")](../../assets/images/timing-lai-out.png)

### Load A Immediate (LAI)

Load A Immediate is a two-byte instruction.  The A register is loaded with the value of the second instruction byte.

**Marker 1**: the falling edge of CLK sets the Step Counter (SC) to T0.  This step asserts the Read PC and Write MAR signals to get the address of the next instruction.  The PC Increment (PI) signal is also asserted.  At this point, the Program Counter (PC) contents are read onto the bus.

**Marker 2**: the rising edge of the clock causes the bus contents to be written into the Memory Address Register (MAR).  The PC is also incremented.

**Marker 3**: the falling CLK edge increments the Step Counter to enter step T1.  This asserts the Read RAM and Write IR signals.  At this point, the RAM contents at the MAR address are read onto the bus.

**Marker 4**: the rising CLK edge loads the contents of RAM into the Instruction Register (IR).  This is the end of the T0 and T1 instruction fetch steps.

**Markers 5..6**: the MAR is loaded from the PC in step T2, exactly as was done in step T0.  This fetches the address of the instruction operand.

**Markers 7..8**: in step T3, the RAM contents are loaded into the A register.  The Next (N) signal is also asserted in this step, which will reset the SC back to step 0 (T0) to end the instruction.  The N reset is synchronous and will occur at the falling CLK edge at **marker 9**.

### Output A (OUT)

This is a one byte instruction that loads the contents of the A register into the Output Display Register.

**Marker 9..12**: the T0 and T1 steps are the usual instruction fetch sequence.  Step T0 loads the PC into the MAR and increments the PC.  Step T1 loads the RAM value into the IR.

**Marker 13**: the A register contents are placed in the bus.

**Marker 14**: the A register contents on the bus are clocked into the Output Display Register on the rising clock edge.  The N signal is asserted to end the OUT instruction.

**Marker 15:** the N signal from the OUT instruction resets the Step Counter back to T0 at this falling clock edge, starting the next instruction fetch sequence.

The Instruction Register does not get loaded until midway through the T1 microinstruction step. During T0 and T1, the IR still has the value of the previous instruction.  For example, at **Marker 9** the T0 step of the OUT instruction is executing, but the IR still contains the LAI instruction and is therefore executing the T0 step of the LAI microcode, not the OUT microcode.  This is why all instructions **must** have identical microcode for their T0 and T1 steps.
{: .notice--info}

## Reset Timing

[![reset timing](../../assets/images/timing-reset.png "Timing of the reset condition")](../../assets/images/timing-reset.png)
