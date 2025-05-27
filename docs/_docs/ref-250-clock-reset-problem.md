---
title: "Clock Reset Issue"
permalink: /docs/clock-reset-issue/
excerpt: "Clock and Reset interactions in SAP computers"
---

The [WaveDrom timing diagram](https://wavedrom.com/) below shows the system reset sequence.  In this example, the program at address 0 is starts with an OUT instruction.  

When the Reset button is pressed, the Step Counter (SC) resets back to zero, setting the microcode state to T0.  In this state, the Program Counter (PC) is read onto the bus, and writing is enabled to the Memory Address Register (MAR).  The PI signal is also asserted to increment the PC after it is read.

Execution begins when the Reset button is released and the RST signal goes high again.  At the next rising edge of the CLK, the WM signal will clock the PC value from the data bus into the MAR.  The rising clock edge will also increment the PC due to the PI signal.  The T1 state will then load the Instruction Register (IR) with the opcode from RAM at the MAR location.

[![reset timing](../../assets/images/timing-reset.png "Timing of the reset condition")](../../assets/images/timing-reset.png)

The [logic analyzer](https://www.saleae.com) trace below shows the reset condition captured on a running SAP-Plus. The system comes out of reset at **Marker 0**. The rising clock edge at **Marker 1** loads the MAR and increments the PC.

This is the ideal case for the reset sequence.

![reset timing](../../assets/images/logic-reset-low.png "Reset when CLK low")

## Reset Problem When CLK is HIGH

The previous timing and logic traces show the system coming out of reset while the clock is low. Because the RST and CLK signals are not synchronized, there is no guarantee that this will happen.  The trace below shows the system when reset is released while the clock is high.

![reset timing](../../assets/images/logic-reset-high.png "Reset when CLK high")

In this case, the next clock transition after reset occurs at **Marker 1**. This is a falling edge, causing the mirocode state to advance to T1.  There was no rising clock edge in the T0 state, meaning that the MAR will not be loaded and the PC will not be incremented.

In summary, if the system comes out of reset when the CLK signal is high, the first T0 state is effectively skipped - the MAR is not loaded and the PC is not incremented.

This condition is not a problem for the Ben Eater SAP-1 computer, but it can cause major issues for extended systems like NQSAP and SAP-Plus.  

### SAP-1 ###

When the system comes out of reset, the first T0 state always loads zero into the MAR because it is loading the MAR from the PC and the PC is cleared during the reset condition.  Missing the MAR load is not a problem with SAP-1 because the MAR is also cleared at reset and therefore already contains zero.  The following T1 state will then load the IR with the instruction opcode at RAM address zero.

The only complication for SAP-1 is that the PC did not increment in step T0, so the next instruction cycle will load and execute the instruction at address zero a second time. The first instruction in most programs is usually setting initial values for the system, so executing it twice won't cause any noticable problem.

### SAP-Plus ###

The SAP-Plus has two differences from SAP-1 that can cause issues at reset. The first is that the MAR is implemented using a 74LS377 8-bit register rather than a 74LS173 register.  The 74LS377 does not have a CLR pin, so the SAP-Plus MAR is not cleared to zero at reset.  If the rising CLK is not present in T0 to load the MAR, then the first T1 step will load the IR with a value from a random RAM address instead of from address zero.  This will cause a random instruction to be executed after reset.

In the best-case, the PC will still be at zero after the random instruction, so the next T0 and T1 will then cause the actual first instruction to be loaded and executed, just like with the SAP-1.

The second and more serious problem is due to the fact that [SAP-Plus instructions](../in-summary/#instructions-by-opcode) can be either one or two bytes in length.  

For example, the first byte of a Load A Immediate (LAI) instruction is the LAI opcode and the second byte is the value to be loaded into A.

When a two-byte instruction is executed, the following microcode steps are performed:

* _T0_ loads the PC into the MAR and increments the PC
* _T1_ loads the IR with the instruction opcode from RAM
* _T2_ loads the PC into the MAR again and increments the PC so that the instruction argument can be loaded

The important point is that a two-byte instruction causes the PC to be incremented twice, once in _T0_ and again in _T2_.

If the initial _T0_ step is skipped **and** the random instruction executed first is a two-byte instruction, the the PC will not be incremented in _T0_, but it will be incremented in _T2_.  This means that the PC will be pointing to address one after executing the random instruction.  One of two things happens in this case.

If the actual first instruction is a single-byte instruction, it is skipped and execution continues with the second instruction.

If the actual first instruction is a two-byte instruction, then the missed _T0_ step will cause the PC to be pointing to the first instruction's argument at the start of the next instruction cycle.  This data value will be fetched at the next _T0_ and loaded into the IR on the next _T1_ as it it were a valid instruction.

Once the system gets out of sync and starts executing data, the entire program is compromised.  It may get back on track after a few instructions, or it may overwrite data or execute a JMP to a completely different clock of code or data.

### Code Example After Bad Reset

The following code could be executed in two very different ways, depending on the random value in the MAR after reset.

``` cpp
02 10 // 00 LAI 16    ; load A with 16 (hex 0x10)
20 07 // 02 ADI 7     ; add 7 to A
01    // 04 OUT       ; output A
 ```

If the first value loaded into the IR is a single byte instruction, like Increment A (INA), then everything will work out fine.  After the INA is executed, the PC will still be zero, so the next instruction cycle will execute the LAI at address zero as intended.

If the first instruction executed from the random starting address is a two-byte instruction, it can cause the PC to get out of sync and will execute one or more data values as if they were legitimate instructions. For example, if the random MAR value points to a Compare Memory to A (CPM) instruction, the following actions will occur:

* The PC will not be incremented in the first _T0_ step, but it will be incremented in the _T2_ step when the CPM microcode fetches the operand of the CPM instruction.  
* When the unexpected CPM instruction is complete, the PC will contain the value 1.  
* The next instruction cycle will fetch the value at RAM address 1 and load it into the IR to be executed.  In this case, the value is the operand of the LAI instruction, which is 0x10.  
* In SAP-Plus, the value 0x10 is the opcode of the two-byte Jump instruction, so the JMP instruction that is seemingly at address 1 will cause an unconditional jump to the address stored at RAM address 2.
* The value at address 2 is the opcode of the ADI instruction, which is 0x20.  The executed instruction will be JMP 0x20.
* After the JMP, the PC will contain 0x20, so the next cycle will  fetch the value at RAM address 0x20 and attempt to execute that as an instruction.

In summary, a two-byte instruction at the first, random value of the MAR could cause this code:

``` cpp
02 10 // 00 LAI 16    ; load A with 16 (hex 0x10)
20 07 // 02 ADI 7     ; add 7 to A
 ```

to execute as if it were this code:

``` cpp
29 10 // 00 CPM 0x10  ; two-byte instruction from uninitialized MAR
 
10 20 // 01 JMP 0x20  ; bogus JMP at address 1 created from LAI data and ADI opcode
        // ...
??    // 20 ???       ; chaos continues at new address
```

Depending on the random value loaded when coming out of reset, the system may skip instructions or execute data as if it were code.

## SAP-Plus Clock Redesign

To fix the clock-reset problem, the [SAP-Plus System Clock Module](../clock/) was redesigned so that the astable timer output is fed into a J/K flip-flop used as a clock divider. The resulting astable divided by two output from the flip-flop is used as the system clock signal. Because the frequency of the new CLK is half of the astable clock frequency, the astable clock circuit was adjusted to generate a higher clock frequency range.

The flip-flop reset signal is held active during a system reset condition.  This keeps the system clock output low. The flip-flop reset is also asserted when the RUN/STOP switch is in the STOP position or when the Arduino Loader is active.  When the flip-flop reset is released, the first clock transition is always a rising edge.  This rising edge ensures that the first T0 microcode step is always executed after a system reset or clock hold.

Although the initial Clock Module redesign used a J/K flip-flop as a clock divider, the final version replaced this with a 74LS161 4-bit counter.  While still being a single-chip solution, the counter offered four different clock options (divide by 2, by 4, by 8 or divide by 16) to offer a larger range of clock speeds.
{: .notice--info}

The logic trace below shows the system coming out of reset when the astable timer signal is low. The astable output continues to count while the system is held in reset, but the CLK output from the divider is held low.

The astable timer output in the logic traces is incorrectly labeled as "ASYNC" instead of "ASTABLE"
{: .notice--warning}

![reset timing](../../assets/images/logic-clk2-low.png "CLK2 reset when CLK low")

At *Marker 0*, the reset button is released while the astable signal is low.  At *Marker 1*, the next rising edge of the astable clock causes the CLK signal to transition high.  This is the initial rising CLK edge needed in microcode step _T0_ after a system reset.

The logic trace below shows the same scenario, but the reset button is released while the astable timer output is high.  Again, the system CLK signal is held low while the reset button is pressed.

![reset timing](../../assets/images/logic-clk2-high.png "CLK2 reset when CLK high")

At *Marker 0*, the reset button is released while the astable clock output is high.  The system CLK does not go high until the astable signal goes low and then high again, as shown at *Marker 1*.

The result in either scenario is the same: the first CLK transition after a reset condition is a rising CLK edge.