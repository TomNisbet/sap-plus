---
title: "Program Counter and Stack Pointer"
permalink: /docs/program-counter-stack-pointer/
excerpt: "Program Counter and Stack Pointer for the SAP-Plus computer"
---

The Program Counter (PC) and Stack Pointer (SP) Module implements two unrelated registers.  There are no interconnections or dependencies between the PC and SP, so they are co-located only because each design is simple and both could be implemented together on a single board.

[![Program Counter and Stack Pointer](../../assets/images/pc-sp-module-small.png "program counter and stack pointer module")](../../assets/images/pc-sp-module.png)

## Program Counter

The Program Counter (PC) is implemented using two 74LS161 4-bit counters.  It increments on any clock cycle that has the Program Increment (_PI_) signal asserted.

New values are clocked into the PC from the data bus when the Write Program (_WP_) signal is asserted.  The _WP_ signal is produced by the register selects to perform an unconditional jump.

The value of the PC is read to the bus when the Read Program (_RP_) signal is asserted.  This operation is done as the first step of every instruction to read the value of the PC into the MAR.

The PC is reset back to zero whenever _RST_ is asserted.  After a reset, program execution starts at address zero.

## Stack Pointer

The Stack Pointer (SP) is an 8-bit register implemented using a pair of 74LS193 4-bit up/down counters.  New values are clocked into the SP from the data bus when the Write Stack (_WS_) signal is asserted.  The value of the SP is read to the bus when the Read Stack (_RS_) signal is asserted.  

The SP enables new instructions to push and pull registers (_PHA_, _PLA_) and to call and return from subroutines (_JSR_, _RTS_).  There are also instructions to load the SP to and from the A register using the _TAS_ and _TSA_ instructions.  If no stack operations are needed, the SP can also be used as a general-purpose register.

The _CLR_ signal is not used to reset the SP at system reset, so the SP always starts in an unknown state.  This isn't a problem, because any programs that use the stack will want to initialize it to point to a free memory area before any stack operations are performed.

[![Program Counter and Stack Pointer Schematic](../../assets/images/pc-sp-schematic-small.png "program counter and stack pointer")](../../assets/images/pc-sp-schematic.png)

### Stack Pointer implementation

Unlike the 74LS161, the 74LS193 counters do not have a master clock or a count enable control.  Counting is performed by pulsing the _UP_ or _DOWN_ pins and loading a new value is performed by pulsing the _LOAD_ signal.  Additional logic is used to AND these signals with the master system clock.  To load a new value, the _CLK_ and _WS_ signals are combined to produce a pulse for the _LOAD_ pin.

Spare bus and microcode signals are in short supply.  Rather than adding two new signals for the SP count up and down operations, a single Stack Count (_SC_) signal was added.  This is used in conjunction with the shared _CX_ carry control signal to control counting in the SP. Asserting both _SC_ and _CX_ will cause the SP to count down and _SC_ without _CX_ will to cause it to count up.  The _SC_ and _CX_ signals are combined with the _CLK_ signal to produce the pulses for the _DOWN_ and _UP_ lines.

The shared use of the _CX_ signal means that the carry flag cannot be modified in the same microinstruction step where the SP is counting.  The normal use of the _CX_ signal will not cause the SP to count because the _SC_ signal will not be asserted.  Note that the _CX_ signals is also shared by the RAM, so SP and RAM access operations also cannot happen in a single microinstruction step.
{: .notice--info}

The combined _SC_ and _CX_ signals are also used to drive the UP and DN indicator LEDs when _SC_ is active.  This is preferable to just adding LEDs to the raw _SC_ and _CX_ signals because _CX_ would give a misleading indication on the Stack Pointer Module when the _CX_ signal was used elsewhere in the system.

### Stack Pointer microcode

The SP counts up when pushing values to the stack and counts down when pulling values.  There is no overflow protection, so the counter will quietly wrap if it hits the top or bottom of memory.  The push operation uses a post-increment, so the value is stored and then the SP is bumped to point to the next available stack address.  In other words, the SP always points to the address after the location of the value on the top of the stack. The pull operations pre-decrement, so the SP is moved to point to the value and then the
value is retrieved.

Note that the Jump to Subroutine (_JSR_) uses a few tricks in the microcode.  The value of the PC is needed to retrieve the subroutine address, but the PC also needs to be saved onto the stack before being overwritten with the new address.  The ALU B register is used for temporary storage to save the subroutine address.  The B register is not directly accessible by the user, so it is a good candidate for the microcode to use.  The example below shows a JSR instruction at memory address 20.

| Address | Contents    |
|:---:    | :---        |
| 20      | JSR opcode  |
| 21      | JSR address |
| 22      | next opcode |
|====

After the instruction fetch, the PC will have the value 21 and JSR microcode performs the following steps:

1. move the PC value to the MAR and increment the PC.  MAR contains 21 and PC contains 22.
1. read the subroutine address from RAM[21] and place it in B for temp storage
1. move the SP value into the MAR and increment the SP.
1. store the PC value (which points to the next instruction) in memory, i.e. push the _JSR_ return address on the stack.
1. move the B register value into the PC, effectively jumping to the subroutine

Note that the B register is needed because the PC value that is pushed to the stack is one address past the location that holds the subroutine address.  If the PC value was incremented and pushed before fetching the subroutine address, there would be no way to fetch the address because the PC has already moved past it.

An alternate approach that would not require temporary storage would be to push the PC while it was still pointing to the subroutine address instead of pointing to the next opcode. The RTS instruction would then need to increment the PC after fetching it from the stack. The downside to this is that the PC address on the stack would not be the true return address.


## Bill of Materials

* 74HCT00 quad 2-input NAND gate(1)
* 74HCT32 quad 2-input OR gate(1)
* 74LS161 4-bit counter (2)
* 74LS193 4-bit up/down counter (2)
* 74HCT245 8-bit bus transceiver (2)
