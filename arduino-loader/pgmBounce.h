static const uint8_t pgmBounce[] = {
              //    ; Simple test program to load a series of values into the A register.
              //    ; Requires only a minimal system with no ALU, Output Register, or stack.
  0x02, 0x01, // 00         lai     1
              //    LOOP:
  0x02, 0x02, // 02         lai     2
  0x02, 0x04, // 04         lai     4
  0x02, 0x08, // 06         lai     8
  0x02, 0x10, // 08         lai     16
  0x02, 0x20, // 0a         lai     32
  0x02, 0x40, // 0c         lai     64
  0x02, 0x80, // 0e         lai     128
  0x00,       // 10         nop
  0x02, 0x40, // 11         lai     64
  0x02, 0x20, // 13         lai     32
  0x02, 0x10, // 15         lai     16
  0x02, 0x08, // 17         lai     8
  0x02, 0x04, // 19         lai     4
  0x02, 0x02, // 1b         lai     2
  0x02, 0x01, // 1d         lai     1
  0x00,       // 1f         nop
  0x10, 0x02, // 20         jmp     LOOP
              //    
};
