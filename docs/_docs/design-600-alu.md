---
title: "ALU"
permalink: /docs/alu/
excerpt: "Arithmetic Logic Unit (ALU) for the SAP-Plus computer"
---

[![Arithmetic Logic Unit (ALU)](../../assets/images/alu-module-small.png "ALU")](../../assets/images/alu-module.png)

The SAP-Plus computer uses a pair of 74LS283 4-bit adders chips as a simple adder/subtractor.  The inputs to the adder are the A Register and the B register as presented over the Module Interconnects.  If the _BI_ signal is asserted, then the inverted value of the B register is fed to the adders.  The carry-in value to the adders is the value of the _CX_ signal from the Microcode Module.

All ALU instructions write their result to the A register.

The ALU result appears to the system as read-only register that is read to the bus when the Read ALU (_RL_) signal is asserted.  There is no actual hardware register that holds the ALU result, the read value is the outputs of the adders when the _RL_ is asserted.

In addition to the adders, the ALU module also contains a Flags Register and the zero detect circuit.

The ALU Module supports the following operations:

* Add A to immediate or memory
* Add A with Carry to immediate or memory
* Subtract immediate or memory from A
* Subtract immediate or memory from A with Borrow
* Compare A to immediate or memory
* NOT A (invert all bits of A)
* Increment A
* Decrement A

The Increment and Decrement operations have the B register read from the data bus with no source writing to the data bus.  The data bus terminating resistors ensure that a read  returns all zeros if no source is writing to the data bus.  Reading zero into B and adding it to A with the carry bit set gives in A+1.  Inverting the zero in B to all ones gives negative one, which can be added to A to get A-1.

### ALU Design

The SP-PLUS ALU has two subtle differences from the [Ben Eater SAP-1](https://eater.net/8bit/) that give it extra functionality.  The first is that the zero-detect circuit is connected to the data bus instead of the ALU outputs.  This lets it detect zeros from any source, so it can be used with non-ALU instructions.

The second change is that the _SU_ signal on the SAP-1 is separated into individual _BI_ and _CX_ signals that invert the B register and feed the carry-in signal of the adders.  This change allows the _NOT_, _INC_ and _DEC_ instructions.  It also enables the _ADC_ and _SBC_, that can be used to create code to add or subtract numbers larger than eight bits.

[![NQSAP ALU schematic)](../../assets/images/alu-schematic.png "ALU schematic"){:width="500px"}](../../assets/images/alu-schematic.png)


## Flags

The ALU Module contains two flags: Zero and Carry.  These are both stored in a 74LS173 4-bit register when the Flags Load (_FL_) signal is asserted, so they cannot be written invididually.  The two output bits from the Flags Register are sent to the 40-pin bus as _FZ_ and _FC_.  These bits are loaded into the _IR_ and used to address the microcode ROM, so instructions can have different microinstruction steps based on flag values.

### Zero flag

The zero flag is set if all bits on the data bus are zero.  Using the bus instead of the ALU output for zero-detect means that it can also be used with non-adder operations like NOT or register loads.

The zero flag is calculated using three 3-input OR gates feeding a 3-input NAND gate.  The output of the NAND gate is then inverted by a second NAND, making those two gates equivalent to a single 3-input AND gate.  

The original 1.0 version of the board actually used a triple 3-input AND gate for zero detect with two gates unused, but this board mistakenly connected the active-HIGH _FL_ signal to the flags register.  The 74LS173 chip used for the flags needed an active-LOW signal rather than active-HIGH, so the AND gates were replaced by NANDs with one of the spare gates used to invert the _FL_ signal and the other spare gate inverting the zero-detect NAND back to an AND.  The board photo shows the NAND gate soldered on top of the old AND gate with wires making the new connections.
{: .notice--info}

### Carry flag

The SAP-Plus addition and subtraction operations are modeled after the 6502 processor.  An addition with carry instruction will produce A+B when carry is clear and A+B+1 when carry is set.  The carry flag will be set at the completion of the operation if the addition carried into the nonexistent ninth bit.

A subtraction with carry operation will effectively produce A-B-1 when the carry bit is clear and A-B when the carry is set.  The carry flag will be cleared at the completion of the operation if the subtraction required a borrow from the nonexistent ninth bit.

## Subtraction with an adder

The 74LS283 adder adds two 4-bit numbers together.  There is no input to tell it to do subtraction.  But a subtraction problem can be rewritten as addition of the negative number.  For example, 5-2 is the same as 5 + -2.

To negate a number in twos-complement notation, invert all of the bits and add 1.  For example, to negate the 4-bit value 2, do the following:

``` C
0010 = 2
1101 inverted the bits
1110 added 1
```

so -2 is represented as 1110

Now negate -2

``` C
1110 = -2
0001 inverted the bits
0010 added 1
```

The result is 2

Now use this to do the subtraction problem 5 + -2 = 3

``` C
  0101
+ 1110
=10011
```

The lower 4 bits are the adder result and the 5th bit is the carry flag from the addition: result=**0011**, C=**1**

binary 0011 is 3, so 5 - 2 = 5 + -2 = 3

The calculation above still works when the 4-bit values viewed as unsigned numbers: 1110 is 14, so the problem could also be viewed as 5 + 14 = 19.  The number 19 in binary is 16+2+1 or 10011, which is the answer above for 5 + -2.

The ALU can add A + -B by inverting B and then setting the _CX_ flag to indicate carry-in to the adder.  This gives the effect of making B negative by inverting and adding one.
{: .notice--info}

### Carry bit in addition and subtraction

When doing a simple addition problem, a C=1 carry flag indicates that the result did not fit in the number of bits available.  If two adders are chained together, a carry from the lower adder indicates that the higher adder should add one more to its result.

When doing subtraction, the carry flag seems to behave backwards, but it makes sense when looking closer.  In the earlier problem, 5 - 2 produced a result of 3 with the carry flag C=1.  If two adders are chained together, the upper adder would get a C=1 carry-in flag from the lower adder.  

Here's why this works:

The value -2 when represented in 8 bits would be 1111 1110.  if the carry out from the lower adder was not considered, the two adders doing this calculation would come up with an incorrect answer:

``` C
 0000 0101     5
+1111 1110    -2
=1111 0011   243
```

When the C=1 carry out from the lower adder is added in to the upper adder, the correct result is obtained:

``` C
 0000 0101     5
+1111 1110    -2
=0000 0011     3
```

For the case where a borrow is needed, consider this problem:

``` C
 0000 0010     2
+1111 1011    -5
=1111 1101    -3
```

In this case, the C=0 carry-out from the lower adder leaves the upper 4-bits all set, so the answer is -3, which is correct.

Another way of understanding the behavior of the carry bit is to look at the way 2's complement numbers are formed.

|Binary|Unsigned|2's Complement|
|:---:|:---:|:---:|
|0000 | 0| 0|
|0001 | 1| 1|
|0010 | 2| 2|
|0011 | 3| 3|
|0100 | 4| 4|
|0101 | 5| 5|
|0110 | 6| 6|
|0111 | 7| 7|
|1000 | 8| -8|
|1001 | 9| -7|
|1010 |10| -6|
|1011 |11| -5|
|1100 |12| -4|
|1101 |13| -3|
|1110 |14| -2|
|1111 |15| -1|

Adding one to any number above gives a large number, even with the negative numbers.  For example, -4 follows -5 in the table and -4 is the larger number.  The carry flag adds one to any number, so C=1 means the result gets larger and C=0 means it does not.  If the carry flag is zero, it means that the result will be one less than if it were set.

Note that the jump from 7 to -8 seems to break this rule, but it does not because 7+1=8 and the number 8 cannot be represented in a 4-bit 2's-complement.  Adding 7+1 is an overflow condition when treating the numbers as signed values.
