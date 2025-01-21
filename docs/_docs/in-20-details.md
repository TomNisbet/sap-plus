---
title: "SAP-Plus Instructions"
permalink: /docs/in-details/
excerpt: "Instruction set for the SAP-Plus Computer"
---

## SAP-Plus Instructions

The SAP-Plus has a single user-accessable register A that functions as the accumulator for ALU operations.
If no stack operations are needed, then the SP can also be used as a general purpose register.

The memory features two 256 byte areas that are addressed by the Memory Address Register.  The Program Memory
area stores the instructions and instruction arguments.  The data area is used for the stack and for data
storage with the Load, Store, and arithmetic operations.

The stack pointer must be initialized before the stack is used.  This can be done using LAI to load a value
into A and TAS to move it to the SP.  Because the stack grows downward, it is recomended that the SP be
initialized to the value 255 so it can use the top section of the data memory.  General data storage can
then start at zero and grow upward.


|           |           |           |           |           |           |           |           |
|:---:      |:---:      |:---:      |:---:      |:---:      |:---:      |:---:      |:---:      |
|[ACI](#aci)|[ACM](#acm)|[ADI](#adi)|[ADM](#adm)|[ASL](#asl)|[CLF](#clf)|[CPI](#cpi)|[CPM](#cpm)|
|[DCA](#dca)|[INA](#ina)|[JC](#jc)|[JMP](#jmp)|[JNC](#jnc)|[JNZ](#jnz)|[JSR](#jsr)|[JZ](#jz)|
|[LAI](#lai)|[LAM](#lam)|[NOP](#nop)|[NOT](#not)|[OUT](#out)|[PHA](#pha)|[PLA](#pla)|[RC](#rc)|
|[RNC](#rnc)|[RNZ](#rnz)|[RTS](#rts)|[RZ](#rz)|[SAM](#sam)|[SBI](#sbi)|[SBM](#sbm)|[SCI](#sci)|
|[SCM](#scm)|[SEF](#sef)|[TAS](#tas)|[TSA](#tsa)|[TST](#tst)|

## ACI

**add immediate to A with carry**

Add A to immediate value and store the result in A.  The Carry flag value is included in the addition.  If Carry is set, the value in A will be A+value+1.

**Carry Flag:** set if A overflows

**Zero Flag:** set if zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|ACI|0x24|2|5|

## ACM

**add memory to A with carry**

Add A to value in data memory at the address argument and store the result in A.  The Carry flag value is included in the addition.  If Carry is set, the value in A will be A+value+1.

**Carry Flag:** set if A overflows

**Zero Flag:** set if zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|ACM|0x25|2|6|

## ADI

**add  immediate to A**

Add A to immediate value and store the result in A.  The Carry flag value is not used in the addition.

**Carry Flag:** set if A overflows

**Zero Flag:** set if zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|ADI|0x20|2|5|

## ADM

**add memory to A**

Add A to value  in data memory at the address argument and store the result in A.  The Carry flag value is not used in the addition.

**Carry Flag:** set if A overflows

**Zero Flag:** set if zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|ADM|0x21|2|6|

## ASL

**arithmetic shift left A**

Shift A one position left.  Least significant bit of A takes the value of the Carry flag and the Carry Flag is set with the most significant bit of A.

**Carry Flag:** set if A overflows

**Zero Flag:** set if zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|ASL|0x0a|1|4|

## CLF

**clear carry and zero flags**

Clear carry and zero flags.

**Carry Flag:** clear

**Zero Flag:** clear

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|CLF|0x0c|1|4|

## CPI

**compare immediate to A**

Compare A to immediate value in the next byte of program memory.  Carry flag is clear if A<value and Zero flag is set if A=value.  The value of A is not changed.

**Carry Flag:** clear if A < value

**Zero Flag:** set if equal

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|CPI|0x28|2|5|

## CPM

**compare memory to A**

Compare A to the value in data memory addressed by  the next byte of program memory.  Carry flag is clear if A<value and Zero flag is set if A=value.  The value of A is not changed.

**Carry Flag:** clear if A < value

**Zero Flag:** set if equal

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|CPM|0x29|2|6|

## DCA

**decrement A**

Decrement A.

**Carry Flag:** clear if wrap past zero

**Zero Flag:** set if A decremented to zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|DCA|0x08|1|4|

## INA

**increment A**

Increment A.

**Carry Flag:** set if wrap to zero

**Zero Flag:** set if wrap to zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|INA|0x07|1|4|

## JC

**jump on Carry**

Jump to address argument if Carry flag set

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|JC|0x11|2|4|

## JMP

**jump unconditional**

Jump to address argument

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|JMP|0x10|2|4|

## JNC

**jump on no Carry**

Jump to address argument if Carry flag clear

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|JNC|0x13|2|4|

## JNZ

**jump on no Zero**

Jump to address argument if Zero  flag clear

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|JNZ|0x14|2|4|

## JSR

**jump to subroutine**

Jump to subroutine at address argument.  PC is stored on the stack and SP is decremented.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|JSR|0x17|2|7|

## JZ

**jump on Zero**

Jump to address argument if Zero  flag set

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|JZ|0x12|2|4|

## LAI

**load A immediate**

Load A with immediate value stored in the next byte in program memory.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|LAI|0x02|2|4|

## LAM

**load A from memory**

Load A with the value from memory address specified by the next byte in program memory.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|LAM|0x03|2|5|

## NOP

**no operation**

No operation is performed.  Data registers and flags are unchanged.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|NOP|0x00|1|3|

## NOT

**NOT A**

Replace A with the logical NOT of A, meaning that all zero bits become one and all one bits become zero.

**Carry Flag:** undetermined

**Zero Flag:** set if zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|NOT|0x09|1|4|

## OUT

**output A**

Store A in the Output Register for display.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|OUT|0x01|1|3|

## PHA

**push A**

Push processor status (flags) on to the stack.  SP is decremented.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|PHA|0x15|1|5|

## PLA

**pull A**

Pull accumulator from the stack.  SP is incremented.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|PLA|0x16|1|5|

## RC

**return if carry**

Return from subroutine if Carry flag is set else no operation.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|RC|0x19|1|5|

## RNC

**return if not carry**

Return from subroutine if Carry flag is not set else no operation.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|RNC|0x1b|1|5|

## RNZ

**return if not zero**

Return from subroutine if Zero flag is not set else no operation.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|RNZ|0x1c|1|5|

## RTS

**return from subroutine**

Return from subroutine.  PC is pulled from the stack and SP is incremented.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|RTS|0x18|1|5|

## RZ

**return if zero**

Return from subroutine if Zero flag is set else no operation.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|RZ|0x1a|1|5|

## SAM

**store A to memory**

Store A to memory at the address specified by the next byte in program memory.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|SAM|0x04|2|5|

## SBI

**subtract immediate from A**

Subtract  immediate value from A and store the result in A.  The Carry flag value is not used in the addition.

**Carry Flag:** clear if A underflows

**Zero Flag:** set if zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|SBI|0x22|2|5|

## SBM

**subtract memory from A**

Subtract  value in data memory at the address argument from A and store the result in A.  The Carry flag value is not used in the compuation.

**Carry Flag:** clear if A underflows

**Zero Flag:** set if zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|SBM|0x23|2|6|

## SCI

**subtract immediate from A with carry**

Subtract  immediate value from A and store the result in A.  The Carry flag is included in the subtraction.  If Carry is clear, the value in A will be A-value-1.

**Carry Flag:** clear if A underflows

**Zero Flag:** set if zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|SCI|0x26|2|5|

## SCM

**subtract memory from A with carry**

Subtract  value in data memory at the address argument from A and store the result in A.   The Carry flag is included in the subtraction.  If Carry is clear, the value in A will be A-value-1.

**Carry Flag:** clear if A underflows

**Zero Flag:** set if zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|SCM|0x27|2|6|

## SEF

**set carry and zero flags**

Set carry and zero flags.

**Carry Flag:** set

**Zero Flag:** set

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|SEF|0x0d|1|4|

## TAS

**transfer A to SP**

Transfer A to the SP register.  A is unchanged.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|TAS|0x05|1|3|

## TSA

**transfer SP to A**

Transfer the value of the SP register to A.  SP is unchanged.

**Carry Flag:** unchanged

**Zero Flag:** unchanged

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|TSA|0x06|1|3|

## TST

**test A**

Test A for sign and zero.  Carry flag becomes a minus flag (A<0). The Zero flag is set if A=0.

**Carry Flag:** set if A is negative

**Zero Flag:** set if A is zero

|Name|Opcode|Bytes|Cycles|
:--- |:---: |:---:|:---: |
|TST|0x0b|1|4|


*this file was generated by sap-plus-instructions.py at 21-Jan-2025 09:19:43*
