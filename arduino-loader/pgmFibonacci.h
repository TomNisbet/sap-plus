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
