---
title: "Module Template and Prototype"
permalink: /docs/template/
excerpt: "Module template and prototype module of the SAP-Plus computer"
---

The template schematic and PCB serve as a starting point for new module boards.  The KiCad files are included in the repo, but the gerber files are not present because this board does not need to be ordered to build a complete SAP-Plus system.

[![Module Template](../../assets/images/template-schematic-small.png "module template schematic")](../../assets/images/template-schematic.png)

The PCB layout is a blank module board with the two 40-pin bus and all four 2x8-pin interconnect headers in place. The schematic has all bus pins labelled and connected.

[![Module Template](../../assets/images/template-pcb-small.png "module template schematic")](../../assets/images/template-pcb.png)

## Module Template

To design a new module, copy the template files to a new directory.  Edit the schematic file to add the new module functionality.  Any unused interconnect headers can be removed or replaced with smaller connectors to reclaim the board space in the layout.  For example. a 2x8 interconnect may be replaced with a 1x8 or 1x4 if a smaller number of signals is needed.  Be sure to read the important note below about the interconnect pin ordering.

When the schematic is complete, delete any unused signal labels on the bus connectors and mark them as no-connect.  This will prevent errors from being flagged in KiCad's Electrical Rules Check (ERC).

### Module Interconnects

Pay careful attention to the way the interconnects are routed through the backplane. The pins on the interconnects are connected inner-to-inner and outer-to-outer.

When two modules are placed in adjactent backplane slots, J5 on the left board will connect to J3 on the right board. J5 pin 1 connects to J3 pin 2 and J5-2 connects to J3-1.

Smaller connectors can be used if fewer pins are needed.  For example, the left board can populate a 1x8 connector at its outer edge, using the spots occupied by the odd pins of J5. The right board would put a 1x8 at its outer edge, using the even pins of J3.

## Prototype Module

When designing new modules, it can be useful to order a set of blank template boards to be used for prototyping.  The tepmlate module board can is assembled using longer pin connectors that leave room for connections on top of the board. The prototype board is inserted into the backplane and the top pins are jumpered to a breadboard.  With this method, temporary boards can be created that can be plugged in and removed from the system while keeping all of the connections to the breadboard intact.

[![Module Prototyping](../../assets/images/template-prototype-small.png "module protoytype using template board")](../../assets/images/template-prototype.png) [![Module Prototyping](../../assets/images/template-pins-small.png "module protoytype using template board")](../../assets/images/template-pins.png)
