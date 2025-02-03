# SAP-Plus Utilities

This folder contains utility scripts and data for the SAP-Plus.  See the  
[SAP-Plus Utilities](https://tomnisbet.github.io/sap-plus/docs/utilities/) page for the full documentation.

## Instruction Set Script

The sap-instructions.py script reads the instruction-set.cvs file with the instruction data and produces MarkDown files for the documentation.  It also creates C++ code with the instruction enums that are used by the Loader and Microcode scripts.

## World's Worst Assembler

A very basic assembler written in python.  The wwasm.py script reads a SAP-Plus  assembly language file and produces a C++ header file that can be included in the Arduino Loader script.  The assembler can optionaly produce a human readable listing file or a set of Arduino Monitor Insert commands to load the SAP-Plus machine code.
