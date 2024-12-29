---
title: "System Clock"
permalink: /docs/clock/
excerpt: "System clock for the SAP-Plus computer"
---

The Clock Module provides the master timing signal (CLK) for the system. The clock pulse can come from one of three sources:

* adjustable frequency astable clock
* single pulse from a debounced clock pushbutton
* external clock signal (LD_C) from the Loader Module

[![System clock](../../assets/images/clock-module-small.png "System clock")](../../assets/images/clock-module.png)

The RUN/STOP switch is used to disable the astable clock.  The manual clock button and Loader clock can still be used while in the STOP position.

The Loader Active (LD_A) signal from the bus disables the astable clock when the Loader is active.  The manual clock button can still be used while the Loader is active.

## Clock implementation

[![System clock](../../assets/images/clock-schematic-small.png "System clock")](../../assets/images/clock-schematic.png)

The clock uses an NE556 dual-timer for the astable and one-shot clock pulses.  An S-R latch is used for the run-stop switch debounce, rather than using a third timer as in the Ben Eater design.  This latch could have been constructed from a pair of 2-input NAND or 2-input NOR gates, but there was already a spare pair of 3-input gates available, so those were used instead.

