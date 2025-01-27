static const uint8_t pgmStackTest[] = {
              //    ; Test the stack using the PHA/PLA and JSR/RTS instructions
              //    
              // ff STKTOP  equ     0xff
              // 0a N       equ     10
              //    
  0x02, 0xff, // 00         lai     STKTOP  ; initialize the stack to the top of memory
  0x05,       // 02         tas
  0x02, 0x00, // 03         lai     0       ; Test stack - load value to A and push, change A, pop
              //    
              //    LOOP:
  0x01,       // 05         out             ; output and save A
  0x15,       // 06         pha
  0x17, 0x18, // 07         jsr     ADDN    ; output and save A+N
  0x15,       // 09         pha
  0x17, 0x18, // 0a         jsr     ADDN    ; output and save A+2N
  0x15,       // 0c         pha
  0x17, 0x18, // 0d         jsr     ADDN    ; output A+3N
  0x16,       // 0f         pla
  0x01,       // 10         out             ; restore and output A+2N
  0x16,       // 11         pla
  0x01,       // 12         out             ; restore and output A+N
  0x16,       // 13         pla
  0x01,       // 14         out             ; restore and output A
  0x07,       // 15         ina             ; increment A and repeat the pattern
  0x10, 0x05, // 16         jmp     LOOP
              //    
              //    ADDN:   ; Add N to A and output the new value
  0x20, 0x0a, // 18         adi     N
  0x01,       // 1a         out
  0x18,       // 1b         rts
};
