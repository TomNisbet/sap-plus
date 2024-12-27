---
title: "Module Template and Prototype"
permalink: /docs/template/
excerpt: "Module template and prototype module of the SAP-Plus computer"
---

The template schematic and PCB serve as a starting point for new module boards.  The KiCad files are included in the repo, but the gerber files are not present because this board does not need to be ordered to build a complete SAP-Plus system.

The PCB layout is a blank module board with the two 40-pin bus and all four 2x8-pin interconnect headers in place. The schematic has all bus pins labelled and connected.

## Module Template

To design a new module, copy the template files to a new directory.  Edit the schematic file to add the new module functionality.  Any unused interconnect headers can be removed or replaced with smaller connectors to reclaim the board space in the layout.  For example. a 2x8 interconnect may be replaced with a 1x8 or 1x4 if a smaller number of signals is needed.

When the schematic is complete, delete andyunused signal labels on the bus connectors and mark them as no-connect.  This will prevent errors from being flagged in KiCad's Electrical Rules Check (ERC).


## Prototype Module

A template can be assembled using longer pin connectors. By inserting this template board in place of a module and then using the top pins for breadboard connections, temporary boards can be created that can be plugged in and removed from the system while keeping all of the bus connections to the breadboard intact.

