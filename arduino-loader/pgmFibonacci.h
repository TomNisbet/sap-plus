static const uint8_t pgmFibonacci[] = {
              //            data
              // 00 NUM1:   byte
              // 01 NUM2:   byte
              //    
              //            code
              //    START:
  0x02, 0x01, // 00         lai     1
  0x04, 0x00, // 02         sam     NUM1
  0x01,       // 04         out
              //    LOOP:
  0x04, 0x01, // 05         sam     NUM2
  0x01,       // 07         out
  0x21, 0x00, // 08         adm     NUM1
  0x11, 0x00, // 0a         jc      START
  0x04, 0x00, // 0c         sam     NUM1
  0x01,       // 0e         out
  0x21, 0x01, // 0f         adm     NUM2
  0x11, 0x00, // 11         jc      START
  0x10, 0x05, // 13         jmp     LOOP
};
