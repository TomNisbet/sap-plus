static const uint8_t pgmBounce[] = {
              //    ; Simple test program to load a series of values into the A register.
              //    ; Requires only a minimal system with no ALU, Output Register, or stack.
  0x00,       // 00         nop             ; start with a NOP to avoid reset/clock issues
  0x02, 0x01, // 01         lai     1
              //    LOOP:
  0x02, 0x02, // 03         lai     2
  0x02, 0x04, // 05         lai     4
  0x02, 0x08, // 07         lai     8
  0x02, 0x10, // 09         lai     16
  0x02, 0x20, // 0b         lai     32
  0x02, 0x40, // 0d         lai     64
  0x02, 0x80, // 0f         lai     128
  0x00,       // 11         nop
  0x02, 0x40, // 12         lai     64
  0x02, 0x20, // 14         lai     32
  0x02, 0x10, // 16         lai     16
  0x02, 0x08, // 18         lai     8
  0x02, 0x04, // 1a         lai     4
  0x02, 0x02, // 1c         lai     2
  0x02, 0x01, // 1e         lai     1
  0x00,       // 20         nop
  0x10, 0x03, // 21         jmp     LOOP
              //    
};
