# SAP-Plus Loader and Debugger

This folder contains the Arduino sketch for the SAP-Plus Loader and Interactive Debugger.
The hardware design and features of the Loader/Debugger are listed in the
[Loader documentation](https://tomnisbet.github.io/sap-plus/docs/loader/) in this repo.

## Building

The Loader software is compiled using the Arduino IDE.  Load the ardino-loader.ino file
and all of the supporting files should be included automatically.  Be certain to select
"Arduino Nano" as the Tools->Board menu of the IDE.

## Version History

### 3.0

* change ssd1306lite code to match version 1.2 hardware module that uses 3.3V wiring with pullup resistors
* add sieve example program
