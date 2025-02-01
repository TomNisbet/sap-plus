static const uint8_t pgmSquares[] = {
              //    ; Find all squares between 0.255 and display on the Output Register
              //    ;
              //    ; This does not require any stack operations, so SP is used as a
              //    ; general-purpose register.
              //    
              //            data
              // 00 NUM:    byte
              //    
              //            code
              //    START:
  0x02, 0x00, // 00         lai     0       ; output zero as the first square
  0x01,       // 02         out
  0x04, 0x00, // 03         sam     NUM     ; save as first value of NUM
              //    NEXT:
  0x03, 0x00, // 05         lam     NUM     ; find the next square.  Don't need to check overflow
  0x07,       // 07         ina             ;   here because NUM * NUM will overflow before NUM
  0x04, 0x00, // 08         sam     NUM
  0x05,       // 0a         tas             ; Add NUM to itself NUM times.  SP is the add counter
              //    MULT:
  0x1e,       // 0b         dcs             ; see if more adds are needed
  0x12, 0x14, // 0c         jz      MDONE   ; done - NUM has been added to itself NUM times
              //                            ; could check for borrow here and return 0 to check for multiply by zero
  0x21, 0x00, // 0e         adm     NUM     ; add current sum to NUM
  0x13, 0x0b, // 10         jnc     MULT    ; keep adding if no overflow
  0x10, 0x17, // 12         jmp     NOMORE  ; NUM is too big to square
              //    
              //    MDONE:
  0x01,       // 14         out             ; multiplication complete, output result
  0x10, 0x05, // 15         jmp     NEXT    ; do next value of NUM
              //    
              //    NOMORE:
  0x02, 0xf0, // 17         lai     0xf0    ; no more squares to find.  Wait here for a moment,
  0x3f,       // 19         cyn             ;   blink the lights, then start over
  0x3f,       // 1a         cyn
  0x10, 0x00, // 1b         jmp     START
};
