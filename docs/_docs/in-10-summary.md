---
title: "SAP-Plus Instructions"
permalink: /docs/in-summary/
excerpt: "Instruction set summaries for the SAP-Plus Computer"
---


## Instructions by Name

|Name|Opcode|Bytes|Description|
|:---|:---|:---|:---|
|[ACI](../in-details#aci)|0x24|2|add immediate to A with carry|
|[ACM](../in-details#acm)|0x25|2|add memory to A with carry|
|[ADI](../in-details#adi)|0x20|2|add immediate to A|
|[ADM](../in-details#adm)|0x21|2|add memory to A|
|[ASL](../in-details#asl)|0x0a|1|arithmetic shift left A|
|[CLF](../in-details#clf)|0x0c|1|clear carry and zero flags|
|[CPI](../in-details#cpi)|0x28|2|compare immediate to A|
|[CPM](../in-details#cpm)|0x29|2|compare memory to A|
|[CYN](../in-details#cyn)|0x3f|1|can you not|
|[DCA](../in-details#dca)|0x08|1|decrement A|
|[DCS](../in-details#dcs)|0x1e|1|decrement SP|
|[INA](../in-details#ina)|0x07|1|increment A|
|[INS](../in-details#ins)|0x1d|1|increment SP|
|[JC](../in-details#jc)|0x11|2|jump on Carry|
|[JEQ](../in-details#jz)|0x12|2|jump if equal|
|[JGE](../in-details#jc)|0x11|2|jump if greater or equal|
|[JLT](../in-details#jnc)|0x13|2|jump if less than|
|[JMP](../in-details#jmp)|0x10|2|jump unconditional|
|[JNC](../in-details#jnc)|0x13|2|jump on no Carry|
|[JNE](../in-details#jnz)|0x14|2|jump if not equal|
|[JNZ](../in-details#jnz)|0x14|2|jump on no Zero|
|[JSR](../in-details#jsr)|0x17|2|jump to subroutine|
|[JZ](../in-details#jz)|0x12|2|jump on Zero|
|[LAI](../in-details#lai)|0x02|2|load A immediate|
|[LAM](../in-details#lam)|0x03|2|load A from memory|
|[LAX](../in-details#lax)|0x0e|1|load A indexed by SP|
|[NOP](../in-details#nop)|0x00|1|no operation|
|[NOT](../in-details#not)|0x09|1|NOT A|
|[OUT](../in-details#out)|0x01|1|output A|
|[PHA](../in-details#pha)|0x15|1|push A|
|[PLA](../in-details#pla)|0x16|1|pull A|
|[RC](../in-details#rc)|0x19|1|return if carry|
|[REQ](../in-details#rz)|0x1a|1|return if equal|
|[RGE](../in-details#rc)|0x19|1|return if greater or equal|
|[RLT](../in-details#rnc)|0x1b|1|return if less than|
|[RNC](../in-details#rnc)|0x1b|1|return if not carry|
|[RNE](../in-details#rnz)|0x1c|1|return if not equal|
|[RNZ](../in-details#rnz)|0x1c|1|return if not zero|
|[RTS](../in-details#rts)|0x18|1|return from subroutine|
|[RZ](../in-details#rz)|0x1a|1|return if zero|
|[SAM](../in-details#sam)|0x04|2|store A to memory|
|[SAX](../in-details#sax)|0x0f|1|store A indexed by SP|
|[SBI](../in-details#sbi)|0x22|2|subtract immediate from A|
|[SBM](../in-details#sbm)|0x23|2|subtract memory from A|
|[SCI](../in-details#sci)|0x26|2|subtract immediate from A with carry|
|[SCM](../in-details#scm)|0x27|2|subtract memory from A with carry|
|[SEF](../in-details#sef)|0x0d|1|set carry and zero flags|
|[TAS](../in-details#tas)|0x05|1|transfer A to SP|
|[TSA](../in-details#tsa)|0x06|1|transfer SP to A|
|[TST](../in-details#tst)|0x0b|1|test A|


## Instructions by Opcode

|Opcode|Name|Bytes|Description|
|:---|:---|:---|:---|
|0x00|[NOP](../in-details#nop)|1|no operation|
|0x01|[OUT](../in-details#out)|1|output A|
|0x02|[LAI](../in-details#lai)|2|load A immediate|
|0x03|[LAM](../in-details#lam)|2|load A from memory|
|0x04|[SAM](../in-details#sam)|2|store A to memory|
|0x05|[TAS](../in-details#tas)|1|transfer A to SP|
|0x06|[TSA](../in-details#tsa)|1|transfer SP to A|
|0x07|[INA](../in-details#ina)|1|increment A|
|0x08|[DCA](../in-details#dca)|1|decrement A|
|0x09|[NOT](../in-details#not)|1|NOT A|
|0x0a|[ASL](../in-details#asl)|1|arithmetic shift left A|
|0x0b|[TST](../in-details#tst)|1|test A|
|0x0c|[CLF](../in-details#clf)|1|clear carry and zero flags|
|0x0d|[SEF](../in-details#sef)|1|set carry and zero flags|
|0x0e|[LAX](../in-details#lax)|1|load A indexed by SP|
|0x0f|[SAX](../in-details#sax)|1|store A indexed by SP|
|0x10|[JMP](../in-details#jmp)|2|jump unconditional|
|0x11|[JC](../in-details#jc)|2|jump on Carry|
|0x11|[JGE](../in-details#jc)|2|jump if greater or equal|
|0x12|[JZ](../in-details#jz)|2|jump on Zero|
|0x12|[JEQ](../in-details#jz)|2|jump if equal|
|0x13|[JNC](../in-details#jnc)|2|jump on no Carry|
|0x13|[JLT](../in-details#jnc)|2|jump if less than|
|0x14|[JNZ](../in-details#jnz)|2|jump on no Zero|
|0x14|[JNE](../in-details#jnz)|2|jump if not equal|
|0x15|[PHA](../in-details#pha)|1|push A|
|0x16|[PLA](../in-details#pla)|1|pull A|
|0x17|[JSR](../in-details#jsr)|2|jump to subroutine|
|0x18|[RTS](../in-details#rts)|1|return from subroutine|
|0x19|[RC](../in-details#rc)|1|return if carry|
|0x19|[RGE](../in-details#rc)|1|return if greater or equal|
|0x1a|[RZ](../in-details#rz)|1|return if zero|
|0x1a|[REQ](../in-details#rz)|1|return if equal|
|0x1b|[RNC](../in-details#rnc)|1|return if not carry|
|0x1b|[RLT](../in-details#rnc)|1|return if less than|
|0x1c|[RNZ](../in-details#rnz)|1|return if not zero|
|0x1c|[RNE](../in-details#rnz)|1|return if not equal|
|0x1d|[INS](../in-details#ins)|1|increment SP|
|0x1e|[DCS](../in-details#dcs)|1|decrement SP|
|0x20|[ADI](../in-details#adi)|2|add immediate to A|
|0x21|[ADM](../in-details#adm)|2|add memory to A|
|0x22|[SBI](../in-details#sbi)|2|subtract immediate from A|
|0x23|[SBM](../in-details#sbm)|2|subtract memory from A|
|0x24|[ACI](../in-details#aci)|2|add immediate to A with carry|
|0x25|[ACM](../in-details#acm)|2|add memory to A with carry|
|0x26|[SCI](../in-details#sci)|2|subtract immediate from A with carry|
|0x27|[SCM](../in-details#scm)|2|subtract memory from A with carry|
|0x28|[CPI](../in-details#cpi)|2|compare immediate to A|
|0x29|[CPM](../in-details#cpm)|2|compare memory to A|
|0x3f|[CYN](../in-details#cyn)|1|can you not|


*this file was generated by sap-plus-instructions.py at 27-May-2025 08:30:55*
