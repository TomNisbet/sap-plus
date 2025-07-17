# Manual Memory Loader Module schematics

created with KiCad 8.0

## Changelog

### 3.0

* Add RAM READ/WRITE switch to allow read back of RAM values by holding the flip-flop in reset and writing only to the MAR
* RST signal no longer resets the state flip-flow
* Clean up silkscreen labels with new text and positioning

### 2.0

* Switch clock phase on state flip-flop and LAR counter - state now uses inverted CLK to change on falling edge and LAR uses CLK
* LAR now increments in WRAM state instead of WMAR state - this means that the LAR LEDs will show the current MAR address while the user is setting the switches for the RAM value
* New footprint for rocker switches SW1..SW8 with larger holes
* Remove DOWN and HOLD switches and resistors
* Fix R9 and R10 labels swapped on schematic
* Add silkscreen vlues for RN1 and RN2
* RST signal now resets the state flip-flip
* ACTIVE and UP switches changed to SPDT in schematic to reflect usage on PCB
* UP switch now uses pull-up instead of pull-down resistor
* Add power input connector

### 1.0

* Initial version
