static const uint8_t pgmStackTest[] = {
              //    ; Test the stack using the PHA/PLA and JSR/RTS instructions
              //    
              // ff STKTOP  equ     0xff
              // 0a N       equ     10
              //    
  0x00,       // 00         nop             ; start with a NOP to avoid reset/clock issues
  0x02, 0xff, // 01         lai     STKTOP  ; initialize the stack to the top of memory
  0x05,       // 03         tas
  0x02, 0x00, // 04         lai     0       ; Test stack - load value to A and push, change A, pop
              //    
              //    LOOP:
  0x01,       // 06         out             ; output and save A
  0x15,       // 07         pha
  0x17, 0x19, // 08         jsr     ADDN    ; output and save A+N
  0x15,       // 0a         pha
  0x17, 0x19, // 0b         jsr     ADDN    ; output and save A+2N
  0x15,       // 0d         pha
  0x17, 0x19, // 0e         jsr     ADDN    ; output A+3N
  0x16,       // 10         pla
  0x01,       // 11         out             ; restore and output A+2N
  0x16,       // 12         pla
  0x01,       // 13         out             ; restore and output A+N
  0x16,       // 14         pla
  0x01,       // 15         out             ; restore and output A
  0x07,       // 16         ina             ; increment A and repeat the pattern
  0x10, 0x06, // 17         jmp     LOOP
              //    
              //    ADDN:   ; Add N to A and output the new value
  0x20, 0x0a, // 19         adi     N
  0x01,       // 1b         out
  0x18,       // 1c         rts
};
