static const uint8_t pgmFibonacci[] = {
              //    ; Display the fibonacci sequence on the Output Register
              //            data
              // 00 NUM1:   byte
              // 01 NUM2:   byte
              //    
              //            code
  0x00,       // 00         nop             ; start with a NOP to avoid reset/clock issues
              //    START:
  0x02, 0x01, // 01         lai     1       ; Start with the first number in the sequence and save as NUM1
  0x04, 0x00, // 03         sam     NUM1
  0x01,       // 05         out
              //    LOOP:
  0x04, 0x01, // 06         sam     NUM2    ; Save current sum to NUM2 and output it
  0x01,       // 08         out
  0x21, 0x00, // 09         adm     NUM1    ; Add current sum to NUM1 and save as NUM1
  0x11, 0x01, // 0b         jc      START   ; Restart if sum exceeds 255
  0x04, 0x00, // 0d         sam     NUM1
  0x01,       // 0f         out
  0x21, 0x01, // 10         adm     NUM2    ; Add current sum to NUM2
  0x11, 0x01, // 12         jc      START   ; Restart if sum exceeds 255
  0x10, 0x06, // 14         jmp     LOOP
};
