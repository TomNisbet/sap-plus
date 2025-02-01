---
title: "SAP-Plus Instructions"
permalink: /docs/in-summary/
excerpt: "Instruction set summaries for the SAP-Plus Computer"
---


## Instructions by Name

|Name|Opcode|Description|
|:---|:---|:---|
|[ACI](../in-details#aci)|0x24|add immediate to A with carry|
|[ACM](../in-details#acm)|0x25|add memory to A with carry|
|[ADI](../in-details#adi)|0x20|add immediate to A|
|[ADM](../in-details#adm)|0x21|add memory to A|
|[ASL](../in-details#asl)|0x0a|arithmetic shift left A|
|[CLF](../in-details#clf)|0x0c|clear carry and zero flags|
|[CPI](../in-details#cpi)|0x28|compare immediate to A|
|[CPM](../in-details#cpm)|0x29|compare memory to A|
|[CYN](../in-details#cyn)|0x3f|can you not|
|[DCA](../in-details#dca)|0x08|decrement A|
|[DCS](../in-details#dcs)|0x1e|decrement SP|
|[INA](../in-details#ina)|0x07|increment A|
|[INS](../in-details#ins)|0x1d|increment SP|
|[JC](../in-details#jc)|0x11|jump on Carry|
|[JEQ](../in-details#jz)|0x12|jump if equal|
|[JGE](../in-details#jc)|0x11|jump if greater or equal|
|[JLT](../in-details#jnc)|0x13|jump if less than|
|[JMP](../in-details#jmp)|0x10|jump unconditional|
|[JNC](../in-details#jnc)|0x13|jump on no Carry|
|[JNE](../in-details#jnz)|0x14|jump if not equal|
|[JNZ](../in-details#jnz)|0x14|jump on no Zero|
|[JSR](../in-details#jsr)|0x17|jump to subroutine|
|[JZ](../in-details#jz)|0x12|jump on Zero|
|[LAI](../in-details#lai)|0x02|load A immediate|
|[LAM](../in-details#lam)|0x03|load A from memory|
|[LAX](../in-details#lax)|0x0e|load A indexed by SP|
|[NOP](../in-details#nop)|0x00|no operation|
|[NOT](../in-details#not)|0x09|NOT A|
|[OUT](../in-details#out)|0x01|output A|
|[PHA](../in-details#pha)|0x15|push A|
|[PLA](../in-details#pla)|0x16|pull A|
|[RC](../in-details#rc)|0x19|return if carry|
|[REQ](../in-details#rz)|0x1a|return if equal|
|[RGE](../in-details#rc)|0x19|return if greater or equal|
|[RLT](../in-details#rnc)|0x1b|return if less than|
|[RNC](../in-details#rnc)|0x1b|return if not carry|
|[RNE](../in-details#rnz)|0x1c|return if not equal|
|[RNZ](../in-details#rnz)|0x1c|return if not zero|
|[RTS](../in-details#rts)|0x18|return from subroutine|
|[RZ](../in-details#rz)|0x1a|return if zero|
|[SAM](../in-details#sam)|0x04|store A to memory|
|[SAX](../in-details#sax)|0x0f|store A indexed by SP|
|[SBI](../in-details#sbi)|0x22|subtract immediate from A|
|[SBM](../in-details#sbm)|0x23|subtract memory from A|
|[SCI](../in-details#sci)|0x26|subtract immediate from A with carry|
|[SCM](../in-details#scm)|0x27|subtract memory from A with carry|
|[SEF](../in-details#sef)|0x0d|set carry and zero flags|
|[TAS](../in-details#tas)|0x05|transfer A to SP|
|[TSA](../in-details#tsa)|0x06|transfer SP to A|
|[TST](../in-details#tst)|0x0b|test A|


## Instructions by Opcode

|Opcode|Name|Description|
|:---|:---|:---|
|0x00|[NOP](../in-details#nop)|no operation|
|0x01|[OUT](../in-details#out)|output A|
|0x02|[LAI](../in-details#lai)|load A immediate|
|0x03|[LAM](../in-details#lam)|load A from memory|
|0x04|[SAM](../in-details#sam)|store A to memory|
|0x05|[TAS](../in-details#tas)|transfer A to SP|
|0x06|[TSA](../in-details#tsa)|transfer SP to A|
|0x07|[INA](../in-details#ina)|increment A|
|0x08|[DCA](../in-details#dca)|decrement A|
|0x09|[NOT](../in-details#not)|NOT A|
|0x0a|[ASL](../in-details#asl)|arithmetic shift left A|
|0x0b|[TST](../in-details#tst)|test A|
|0x0c|[CLF](../in-details#clf)|clear carry and zero flags|
|0x0d|[SEF](../in-details#sef)|set carry and zero flags|
|0x0e|[LAX](../in-details#lax)|load A indexed by SP|
|0x0f|[SAX](../in-details#sax)|store A indexed by SP|
|0x10|[JMP](../in-details#jmp)|jump unconditional|
|0x11|[JC](../in-details#jc)|jump on Carry|
|0x11|[JGE](../in-details#jc)|jump if greater or equal|
|0x12|[JZ](../in-details#jz)|jump on Zero|
|0x12|[JEQ](../in-details#jz)|jump if equal|
|0x13|[JNC](../in-details#jnc)|jump on no Carry|
|0x13|[JLT](../in-details#jnc)|jump if less than|
|0x14|[JNZ](../in-details#jnz)|jump on no Zero|
|0x14|[JNE](../in-details#jnz)|jump if not equal|
|0x15|[PHA](../in-details#pha)|push A|
|0x16|[PLA](../in-details#pla)|pull A|
|0x17|[JSR](../in-details#jsr)|jump to subroutine|
|0x18|[RTS](../in-details#rts)|return from subroutine|
|0x19|[RC](../in-details#rc)|return if carry|
|0x19|[RGE](../in-details#rc)|return if greater or equal|
|0x1a|[RZ](../in-details#rz)|return if zero|
|0x1a|[REQ](../in-details#rz)|return if equal|
|0x1b|[RNC](../in-details#rnc)|return if not carry|
|0x1b|[RLT](../in-details#rnc)|return if less than|
|0x1c|[RNZ](../in-details#rnz)|return if not zero|
|0x1c|[RNE](../in-details#rnz)|return if not equal|
|0x1d|[INS](../in-details#ins)|increment SP|
|0x1e|[DCS](../in-details#dcs)|decrement SP|
|0x20|[ADI](../in-details#adi)|add immediate to A|
|0x21|[ADM](../in-details#adm)|add memory to A|
|0x22|[SBI](../in-details#sbi)|subtract immediate from A|
|0x23|[SBM](../in-details#sbm)|subtract memory from A|
|0x24|[ACI](../in-details#aci)|add immediate to A with carry|
|0x25|[ACM](../in-details#acm)|add memory to A with carry|
|0x26|[SCI](../in-details#sci)|subtract immediate from A with carry|
|0x27|[SCM](../in-details#scm)|subtract memory from A with carry|
|0x28|[CPI](../in-details#cpi)|compare immediate to A|
|0x29|[CPM](../in-details#cpm)|compare memory to A|
|0x3f|[CYN](../in-details#cyn)|can you not|


*this file was generated by sap-plus-instructions.py at 01-Feb-2025 12:32:01*
