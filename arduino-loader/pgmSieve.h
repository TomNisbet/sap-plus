static const uint8_t pgmSieve[] = {
              //    ; Output all 8-bit prime numbers using the Sieve of Eratosthenes.
              //    ;
              //    ; The primes are found by filling memory locations 2..255 with the values 2..255.  This uses 254 of the
              //    ; available RAM locations. The other two locations store the next number to test and the next non-prime
              //    ; number to be marked.
              //    ;
              //    ; The algoritm works as follows:
              //    ;
              //    ; start at 2, output it as a prime, and then mark all multiples of 2 (4, 6, 8, 10,...)
              //    ; as non-prime by writing a zero to their memory locations.
              //    ;
              //    ; find the next non-zero memory location, which is 3.  Output it and mark multiples (6, 9, 12, ...)
              //    ; with zero.
              //    ;
              //    ; continue until the end of memory is reached
              //    ;
              //    ; This program shows how the SP can be used as an index register with the LAX and SAX instructions.
              //    
              //            data    0
              // 00 NUM:    byte            ; number value to be tested for prime
              // 01 ZEROIX: byte            ; next location to write zero
              //    
              //            data    2
              // 02 SIEVE:  byte            ; array of numbers 2..255 stored in memory at address 2..255
              //    
              //            code
              //    START:
  0x02, 0x02, // 00         lai     2       ; store the starting NUM
  0x04, 0x00, // 02         sam     NUM
  0x05,       // 04         tas
              //    FILL:
  0x0f,       // 05         sax             ; store the values 2..255 into memory locations 2..255
  0x1d,       // 06         ins
  0x06,       // 07         tsa
  0x14, 0x05, // 08         jne     FILL
              //            ; find the next non-zero entry and print it as a prime
  0x03, 0x00, // 0a         lam     NUM     ; try the next number
              //    NXTPRM:
  0x05,       // 0c         tas             ; read from memory location [NUM]
  0x0e,       // 0d         lax             ; check for zero value
  0x0b,       // 0e         tst             ;
  0x12, 0x1e, // 0f         jeq     NEXT    ; not prime if zero at this location
  0x01,       // 11         out             ; non-zero means prime, so display
              //    ZERO:
  0x04, 0x01, // 12         sam     ZEROIX  ; zero out all multiples of NUM
  0x02, 0x00, // 14         lai     0
  0x0f,       // 16         sax
  0x03, 0x01, // 17         lam     ZEROIX
  0x21, 0x00, // 19         adm     NUM
  0x05,       // 1b         tas
  0x13, 0x12, // 1c         jnc     ZERO
              //    
              //    NEXT:
  0x03, 0x00, // 1e         lam     NUM     ; check the next highest number for prime
  0x07,       // 20         ina
  0x11, 0x27, // 21         jc      SPIN    ; all primes found
  0x04, 0x00, // 23         sam     NUM
  0x10, 0x0c, // 25         jmp     NXTPRM
              //    
              //    SPIN:
  0x02, 0x55, // 27         lai     0x55    ; Loop here forever, but at least look cool while doing it
              //    SPIN2:
  0x3f,       // 29         cyn
  0x10, 0x29, // 2a         jmp     SPIN2
};
