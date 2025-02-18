---
title: "SAP-Plus Utilities"
permalink: /docs/utilities/
excerpt: "utility scripts and data for the SAP-Plus computer"
---

The [utilities folder](https://github.com/TomNisbet/sap-plus/tree/main/utilities/) contains utility scripts and data for the SAP-Plus.  

## Instruction Set Script

The sap-instructions.py script reads the instruction-set.cvs file with the instruction data and produces MarkDown files for the documentation.  It also creates C++ code with the instruction enums that are used by the Loader and Microcode scripts.  

The script creates four output files:

### in-10-summary.md and in-20-details.md

Instruction set summary and details MarkDown files for the documentaion.  These are written directly to the docs tree and can be checked in without modification after running the script.

### in-enums.cpp

A list of C++ enumerations for the instruction set.  Used by both arduino-microcode.ino and arduino-loader.ino. The content of this file need to be manually edited into the arduino sketch files.

### in-opcodes.cpp

The instruction opcodes and names as a C++ comment.  ALso includes the number of microcode steps for each instruction.  This can be used as the comment text in the microcode table in the Arduino sketch.

## World's Worst Assembler

A very basic assembler written in python.  The wwasm.py script reads a SAP-Plus  assembly language file and produces a C++ header file that can be included in the Arduino Loader script.  The assembler can optionaly produce a human readable listing file or a set of Arduino Monitor Insert commands to load the SAP-Plus machine code.

The pgmXXX.h file produced by the assembler consists of an array of program bytes to be loaded to the target computer.  The original assembly source and program addresses are included as C++ comments, making the file a useful reference for debugging SAP-Plus programs.

An example pgmFibonacci.h file is shown below.

``` c
static const uint8_t pgmFibonacci[] = {
              //    ; Display the fibonacci sequence on the Output Register
              //            data
              // 00 NUM1:   byte
              // 01 NUM2:   byte
              //    
              //            code
              //    START:
  0x02, 0x01, // 00         lai     1       ; Start with the first number in the sequence and save as NUM1
  0x04, 0x00, // 02         sam     NUM1
  0x01,       // 04         out
              //    LOOP:
  0x04, 0x01, // 05         sam     NUM2    ; Save current sum to NUM2 and output it
  0x01,       // 07         out
  0x21, 0x00, // 08         adm     NUM1    ; Add current sum to NUM1 and save as NUM1
  0x11, 0x00, // 0a         jc      START   ; Restart if sum exceeds 255
  0x04, 0x00, // 0c         sam     NUM1
  0x01,       // 0e         out
  0x21, 0x01, // 0f         adm     NUM2    ; Add current sum to NUM2
  0x11, 0x00, // 11         jc      START   ; Restart if sum exceeds 255
  0x10, 0x05, // 13         jmp     LOOP
};
```

The wwasm --monitor option generates Insert commands for the Arduino Loader/Monitor.

``` bash
utilities$ python3 wwasm.py -m fibonacci.asm
i00 02 01 04 00 01 04 01 01
i08 21 00 11 00 04 00 01 21
i10 01 11 00 10 05
```
