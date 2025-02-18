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
  0x00,       // 00         nop             ; start with a NOP to avoid reset/clock issues
              //    START:
  0x02, 0x00, // 01         lai     0       ; output zero as the first square
  0x01,       // 03         out
  0x04, 0x00, // 04         sam     NUM     ; save as first value of NUM
              //    NEXT:
  0x03, 0x00, // 06         lam     NUM     ; find the next square.  Don't need to check overflow
  0x07,       // 08         ina             ;   here because NUM * NUM will overflow before NUM
  0x04, 0x00, // 09         sam     NUM
  0x05,       // 0b         tas             ; Add NUM to itself NUM-1 times.  SP is the add counter
              //    MULT:
  0x1e,       // 0c         dcs             ; see if more adds are needed
  0x12, 0x15, // 0d         jz      MDONE   ; done - NUM has been added to itself NUM times
              //                            ; could check for borrow here and return 0 to check for multiply by zero
  0x21, 0x00, // 0f         adm     NUM     ; add current sum to NUM
  0x13, 0x0c, // 11         jnc     MULT    ; keep adding if no overflow
  0x10, 0x18, // 13         jmp     NOMORE  ; NUM is too big to square
              //    
              //    MDONE:
  0x01,       // 15         out             ; multiplication complete, output result
  0x10, 0x06, // 16         jmp     NEXT    ; do next value of NUM
              //    
              //    NOMORE:
  0x02, 0xf0, // 18         lai     0xf0    ; no more squares to find.  Wait here for a moment,
  0x3f,       // 1a         cyn             ;   blink the lights, then start over
  0x3f,       // 1b         cyn
  0x10, 0x01, // 1c         jmp     START
};
