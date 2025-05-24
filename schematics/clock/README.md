# Clock Module schematics

created with KiCad 8.0

## Changelog

### 2.0

* Add range switch to select from two capacitor values for the astable clock
* Add 4-bit counter clock divider to fix clock reset problem and allow more astable clock frequency ranges
* Change astable potentiometer from 1M to 100K - allows for finer control while range switch and divider still allow wide range for frequencies
* RUN/STOP, RST, and LD_ACT signals now hold the divider clock in reset rather than just gating the astable clock, ensuring that the clock always starts low and the first transition is a rising edge

### 1.1

* Add missing connection on pin 4 of 555 timer
* Swap labels on RUN/STOP switch

### 1.0

* Initial version
