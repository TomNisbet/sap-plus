---
title: "System Clock"
permalink: /docs/clock/
excerpt: "System clock for the SAP-Plus computer"
---

The Clock Module provides the master timing signal (CLK) for the system. The clock pulse can come from one of four sources:

* adjustable frequency astable clock
* single pulse from a debounced clock pushbutton
* clock signal (LD_C) from the [Loader Module](loader/)
* clock signal from the external connector

[![System clock](../../assets/images/clock-module-v20.jpg "System clock")](../../assets/images/clock-module-v20.jpg)

## Clock Design

The initial version of the system clock was very similar in capabilities to the [Ben Eater SAP-1 clock](https://eater.net/8bit/clock).  A version 2.0 redesign was done to address the [clock-reset issue](../clock-reset-issue/).  The primary change for version 2.0 was the addition of a clock divider to synchronize the astable clock with the reset and hold signals.  The divider also allows the clock to have a number of different frequency ranges, selectable by a set of jumpers.

In addition to the divider, the clock redesign also added a wider clock frequency range, finer control of the frequency selection, and input for an external clock signal.

The SAP-1 clock uses a 1M potentiometer to select the clock frequency.  This give a range of approximately 0.3Hz to 550Hz, but the use of a single control means that can be difficult to dial in a desired setting.  Typically, the most useful setting is between 1Hz and 20Hz, but this  range is contained in just a small part of the potentiometer's travel.

The SAP-Plus clock makes three changes that allow for a larger frequency range and finer control over its selection:

* Clock divider that offers four frequency bands
* RANGE switch to select between two primary capacitor values
* Speed adjustment potentiomter reduced from 1M Ohm range to 100K to give finer control within the range

By selecting a clock divider and using the RANGE switch, the following frequency bands can be selected:

|Divider|Low Range (Hz)|High Range (Hz)|
|:---|:---:|:---:|
|base   |4.8 - 800|150 - 25500|
|DIV2   |2.5 - 400|76 - 12750|
|DIV4   |1.2 - 200|38 - 6375|
|DIV8   |0.6 - 100|19 - 3188|
|DIV16  |0.3 - 50 |9 - 1594|

Thr total selectable range of the SAP-Plus internal clock is 0.3Hz to 12,750Hz.  The SAP-1 clock has a similar minimum frequency, but the top frequency is limited to about 550Hz.  The new, higher frequencies are too fast to watch individual code steps on the LEDs, but are useful to run longer programs, like the prime number calculator, that output to the 7-segment display.


## Operation

A free-running clock is produced by the built-in astable timer or by an external source, as selected by the CLOCK SELECT jumper.  If an external clock is used, it must be a TTL-compatible signal applied on the EXT-CLK pin of the EXTERNAL CLOCK IN connector.

The RUN/STOP switch is used to disable the free-running clock.  The manual clock button and Loader clock can still be used while in the STOP position.

The Loader Active (LD_A) signal from the bus disables the free-running clock when the Loader is active.  The manual clock button can still be used while the Loader is active.

The RANGE switch selects between two different astable clock frequency ranges as shown in the frequency table above.

The SPEED control is used to adjust the astable clock frequency within the selected range.

A jumper must be placed on a single clock divider setting.  The DIV4 and DIV8 settings are good choices for general use.

The signal from the External Clock connector is fed through the clock divider to avoid the clock-reset issue when using an external clock source.  The selected divider must be accounted for when choosing an external clock frequency.
{: .notice--info}

## Clock Implementation

[![System clock](../../assets/images/clock-schematic-v20.png "System clock")](../../assets/images/clock-schematic-v20.png)

The clock uses an NE556 dual-timer for the free-running (astable) and single-pulse (one-shot) clock pulses.  The astable timer frequency is adjusted using the RANGE switch to select a capacitor value and the SPEED potentiometer to select a resistance value.

The single-pulse pushbutton generates a single positive pulse for each button press.  The pulse width is approximately 100ms.

An S-R latch is used for the RUN/STOP switch debounce, rather than using a third timer as in the SAP-1 design.  This latch is constructed from a pair of 2-input NOR gates because other gates of the same type were already used in the design.

A 74LS161 4-bit counter provides four different divided clock outputs.  The divider is driven by either the astable timer output or an external clock input signal.

The Master Reset (MR) of the clock divider forces the CLK signal low if any of the RST, Loader Active, or Stop conditions is present.  When the clock hold condition is removed, the first CLK transition will be a rising clock edge.

A pair of 3-input NOR gates act as a clock consolidator, allowing the final CLK signal to be generated by either the divider, the Loader Clock, or the single pulse pushbutton.
