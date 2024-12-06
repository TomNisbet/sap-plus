---
title: "System Design"
permalink: /docs/system/
excerpt: "System-level design of the SAP-Plus computer"
---

## Layout and Interconnects

The module interconnect headers on the backplane are used to connect signals that are common to a small number of boards.  These interconnects keep the bus connectors limited to signals that are typically used system-wide.  This does force the placement of some modules in the final assembled computer.  The current design has two groups of related modules that need to occupy the same backplane:

**Loader, Microcode, IR/SC**

The Loader provides register select signals to the 3-to-8 decoers on the Microcode board when the loader is active and the Microcode ROMs are disabled.  The outputs of the Instruction Register and Step Counter are sent to the Microcode module to be used as address inputs to the EEPROMs.  

**A/B Registers and ALU**

The A/B Register Module passes the outputs of its registers to the ALU as the as the operands for arithmetic calculations.

## Bus Signal List

|Pin|Name|Description|
|:---|:---|:---|
|1|GND|Ground|
|2|+5V|+5V power|
|3|RST|System Reset|
|4|D7|Data Bus D7|
|5|D6|Data Bus D6|
|6|D5|Data Bus D5|
|7|D4|Data Bus D4|
|8|D3|Data Bus D3|
|9|D2|Data Bus D2|
|10|D1|Data Bus D1|
|11|D0|Data Bus D0|
|12|WA|Write A Register|
|13|WB|Write B Register|
|14|WO|Write Output Register|
|15|WP|Write Program Counter (PC)|
|16|WR|Write RAM|
|17|WS|Write Stack Pointer (SP)|
|18|WD|Write D Register (not implemented)|
|19|WM|Write Memory Address Register (MAR)|
|20|WI|Write Instruction Register (IR)|
|21|RA|Read A Register|
|22|RB|Read B Register|
|23|RL|Read ALU|
|24|RP|Read Program Counter (PC)|
|25|RR|Read RAM|
|26|RS|Read Stack Pointer (SP)|
|27|RD|Read D Register (not implemented)|
|28|FC|Flag Carry - Carry Flag from Flags Register|
|29|FZ|Flag Zero - Zero Flag from Flags Register|
|30|SC|Stack Count enable, direction determined by CX|
|31|CX|multi use - Carry-In to ALU, SP count direction, MAR extend|
|32|BI|B Register invert to bus and ALU|
|33|FL|Flags Load - clock values into Flags Register|
|34|N|Next instruction - resets Step Counter|
|35|PI|Program Counter Increment|
|36|CLK|Master Clock pulse|
|37|LD-C|Loader Clock Output|
|38|LD-A|Loader Active|
|39|GND|Ground|
|40|+5V|+5V Power|
|====
