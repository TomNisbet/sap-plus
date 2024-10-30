---
title: "Backplane"
permalink: /docs/backplane/
excerpt: "Backplane for the SAP-Plus computer"
---

The SAP-Plus backplane and interconnect design is based on an [8-bit PCB implementation
by
djh82uk](https://www.reddit.com/r/beneater/comments/pn4j6j/finally_complete_with_all_bugs_fixed/)

The system is built from three types of boards.  The Backplane board is a passive base board with a generic 40 pin bus.  Module boards have the functional circuitry for the computer, like the registers, ALU, or Program Counter.  Module boards plug into the bus connectors on the Backplane board.  A single Backplane can hold three Module boards.  Bus Connector boards connect two adjacent Backplane boards, extending the system vertically to accomodate additional modules.  

In addition to the bus signals, which connect to all modules, the Backplane board also has interconnect headers that allow signals to be passed between two adjacent Module boards.

For example, the A/B Register module uses the interconnects to pass the A and B outputs to the ALU module.

The Backplane boards contain no active components.  Other than the ground and power connections on pins 1 and 2 and 39 and 40 of the bus, there are no other bus signals that are interpreted by the backplane.  This makes it suitable to any number of generic designs.

## Backplane and Bus

The main system bus is 40 pins using 2.54mm spacing.  This was chosen as the largest number of pins that would find on a 100mm board.  All of the 40 pin connectors on the bakplane are wired together, so pin one on a connector is wired to pin one on every other connector.

[![Backplane](../../assets/images/backplanes.jpg "backplane")](../../assets/images/backplanes.jpg)

Each Backplane board can hold three modules across in the A, B, and C positions. The module boards measure 90mm from the top row of pins to the bottom row.  The total board height is 96mm.

Module boards attach to the Backplane board using 40 pin headers along the top and bottom edge of the module.  This provides both the electrical and mechanical connection - there are no screws or fasteners to hold the modules in place.  The signals are identical on the top and bottom connectors, so the modules can pull signals from either set of pins, giving flexibility in their routing.

Backplane boards have additional 40-pin bus connectors along the top and bottom so that multiple backplane boards can be chained together to accomodate additional groups of three module boards.  A small Bus Connector board is used to connect the backplanes. Three Bus Connectors are used to provide mechanical stability between a pair of backplanes. Each one of the three is connecting the entire bus, so only one Bus Connector is technically needed to make the electrical connections.

## Backplane module interconnects

A pair of 2x8 pin header is provided between each board to make module-to-module connections.  A module board can use both sets of connectors for a total of 32 pins. Rather than being connected left-to-left and right-to-right, the interconnects have the inner pins connected and the outer pins connected.  A module needing 8 or fewer connections could populate just a 1x8 connector instead of a 2x8 to save space.

[![Backplane with modules](../../assets/images/backplanes-with-modules.jpg "backplane with module boards")](../../assets/images/backplanes-with-modules.jpg)


Note that the interconnects are not connected all the way across the backplane.  If the A module and C module need to talk, the signals will need to route through the B module.  Because the interconnects do not connect all the way across, it is possible that the A and B modules could use a different set of interconnects as the B and C modules.

There are no module-specific interconnects vertically, so related boards must share the same backplane board to leverage the horizontal interconnects.

## Bus connectors

[![Backplane](../../assets/images/backplane-with-connectors.jpg "backplane")](../../assets/images/backplane-with-connectors.jpg)

The Bus Connector boards connect Backplane boards together to expand the computernvertically. Three connectors are used between each pair of backplane boards.  The bus connectors wire all 40 lines top-to-bottom.

### Data LEDs and termination

A second version of the Bus Connector boards has space to populate 8 LEDs to show values on the data bus. 

With the board flipped and the pin headers installed on the other side, the LEDs can be replaced by jumper wires and the resistor network can provide data bus termination to ensure that the bus will ready all zeroes when no device is driving it.

Unlike the backplane and generic bus connector boards, the LED boards assume that eight specific pins on the 40-pin connector are connected to the data bus, so they are specific to the SAP-Plus design.

Note that an LED board with no LEDs or resistors installed will still function as a bus connector. There is no reason that the dedicated bus connector boards are needed other than aesthetics.
